#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//setPriority_system call
int 
sys_setPriority(void)
{
	int pid;
  int priority;
  if(argint(0, &pid) < 0 && argint(1, &priority) < 0)
    return -1;
  if(priority >= 0 || priority <= 3) 
    setPriority(pid, priority);
    else{
      cprintf("priority must be between 0 to 3!!!\n");
      return -1;
    }
	return 0;
}

//yiels_system call
int
sys_yield(void)
{
  yield();
  return 0;
}

int
sys_getLevel(void)
{
  int level = getLevel();
  return level;
}

int
sys_schedulerLock(void)  //argint가 제대로 안받아지면 pid, timequantum, queue 출력하고 끝낸다.
{                       //schedulerLock의 wrapper function
  int password;
  if(argint(0, &password) < 0){
    cprintf("password is wrong!!!\n");
    cprintf("process's PID : %d\n", &myproc()->pid);
    cprintf("process's time quantum : %d\n", &myproc()->proc_tick);
    cprintf("process's queue : %d\n", &myproc()->cur_queue);
    return -1;
  }
  schedulerLock(password);
  return 0;
}

int
sys_schedulerUnlock(void) //argint가 제대로 안받아지면 pid, timequantum, queue 출력하고 끝낸다.
{
  int password;
  if(argint(0, &password) < 0){
    cprintf("password is wrong!!!\n");
    cprintf("process's PID : %d\n", &myproc()->pid);
    cprintf("process's time quantum : %d\n", &myproc()->proc_tick);
    cprintf("process's queue : %d\n", &myproc()->cur_queue);
    return -1;
  }
  schedulerUnlock(password);
  return 0;
}


int
sys_tickinfo(void) //현재 tick 출력
{
  tickinfo();
  return 0;
}
