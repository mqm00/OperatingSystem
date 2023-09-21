#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);//0=kernel mode
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);
  //DPL_USER = 3
  SETGATE(idt[128], 1, SEG_KCODE<<3, vectors[128], DPL_USER);
  SETGATE(idt[129], 1, SEG_KCODE<<3, vectors[129], DPL_USER); //shedulerLock
  SETGATE(idt[130], 1, SEG_KCODE<<3, vectors[130], DPL_USER); //schedulerUnlock

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall(); //eax register
    if(myproc()->killed)
      exit();
    return;
  }

  if(tf->trapno == 129){
    ticks = 0;
    myproc()->tf = tf;
    schedulerLock(2019034702);
    return;
  }
  
  if(tf->trapno == 130){
    myproc()->tf = tf;
    schedulerUnlock(2019034702);
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      cprintf("global tick : %d", ticks);
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER){

      if(ticks != 0 && ticks%100 == 0){
        if(myproc()->flag_Lock == 1) myproc()->flag_Lock = 2; //schedulerLock을 잡고있는 경우, 100tick에 L0 제일 앞으로 이동해야한다.
        boosting();
        yield();
      }

      myproc()->proc_tick += 1;

      if(myproc()->cur_queue == 0 && myproc()->proc_tick > 4){ //큐 레벨에 따른 세스의 time quantum별로 조건이 충족되면 큐를 이동시킨다.
        myproc()->cur_queue = 1;
        myproc()->proc_tick = 0;
        myproc()->real_tick = ticks;
        yield();
      }
      if(myproc()->cur_queue == 1 && myproc()->proc_tick > 6){
        myproc()->cur_queue = 2;
        myproc()->proc_tick = 0;
        myproc()->real_tick = ticks;
        yield();
      }
      if(myproc()->cur_queue == 2 && myproc()->proc_tick > 8){
        if(myproc()->priority != 0) myproc()->priority--;
        myproc()->proc_tick = 0;
        yield();
      }
      yield();
     }

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
