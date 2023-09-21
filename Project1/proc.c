#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;
struct proc* MLFQ[NPROC]; //MLFQ 큐로 사용할 큐 
                          //논리적 MLFQ를 구현할 것이기 때문에 큐가 하나만 있어도 괜찮다.



int nextpid = 1;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
//cprintf("pinit\n");
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
//    cprintf("cpuid\n");
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
   // cprintf("allocproc\n");
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  p->cur_queue = 0;   //p의 default queue
  p->proc_tick = 0;   //p가 큐 내에서 사용하는 Tick, L0에서는 4, L1에서는 6, L2에서는 8 
  p->real_tick = ticks; //p가 ptable에 alloc 됐을 때의 time
  p->priority = 3;    //처음 할당될 때에는 priority가 3
  p->flag_Lock = 0;   //lock이 잡혔는지 확인하는 flag
  return p;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  // cprintf("userinit\n");
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
    // cprintf("growproc\n");
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
    // cprintf("fork\n");
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE; //fork 됐을 때 프로세스에게 기본 정보를 할당해주어야 한다.
  np->cur_queue = 0;  //처음 큐로 할당
  np->real_tick = ticks; //현재 tick 할당 
  np->proc_tick = 0; //큐 내부에서의 time quantum
  np->priority = 3; //priority
  np->flag_Lock = 0; //lock

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
    // cprintf("exit\n");
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc){
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  curproc->flag_Lock = 0;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
    // cprintf("wait\n");
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  // cprintf("scheduler\n");
struct proc *p;
struct cpu *c = mycpu();
c->proc = 0;

for (;;) {
    // Enable interrupts on this processor.
    sti();

    // Select the highest priority process in the highest priority queue.
    acquire(&ptable.lock);
    init_mlfq(); //스케줄러가 한번 스케줄링을 할 때마다 큐를 초기화시켜준다. 새로 들어온 프로세스가 있을 수도 있기 때문에 ptable을 활용한 mlfq를 구현하기 위해선 매번 ptable을
    p = 0;      //순회해서 큐에 다시 할당을 해야한다. mlfq 구현을 위해선 사용한 큐를 초기화해주어야 한다.
    int qsize = 0; //p에도 0일 할당해준다. for문을 돌 때마다 p에 들어간 프로세스가 있는지 없는지 확인하면서 돌기 때문에

    for (int i = 0; i < NPROC; i++) { //ptable을 순회하며 각 프로세스의 current queue를 조사한다.
    //먼저 schedulerLock이 잡혀있는 지 확인한다. lock이 잡혀있으면 그 프로세스만 계속 받아야하기 때문이다.
    //여기서 flag_lock은 0일 때 아무것도 아닌 상태, 1일 때가 lock이 잡혀있는 상태, 2일 때가 unlock을 호출한 상태이다.
    //RUNNABLE조건을 처리하지 않으면 lock이 잡혀있는 상태로 죽은 프로세스가 cpu를 받기 때문에 zombie panic이 뜬다.
        if ((ptable.proc[i].flag_Lock == 1 && ptable.proc[i].state == RUNNABLE)){ 
          p = &ptable.proc[i];
        }
        //unlock을 호출하면 L0에 가장 첫번 째 프로세스로 할당해주어야 하기 때문에 이를 따로 다룬다.
        if(ptable.proc[i].flag_Lock == 2 && ptable.proc[i].state == RUNNABLE){
          ptable.proc[i].flag_Lock = 0;
          ptable.proc[i].priority = 3;
          ptable.proc[i].proc_tick = 0;
          MLFQ[qsize] = &ptable.proc[i];
          qsize = qsize+1;
        }
    }
    //schedulerLock에 관련된 프로세스가 없을 시 L0에 있는 프로세스를 찾는다.
    //현재 큐 값이 L0인 프로세스를 탐색한다.
    if(!p){
    for (int i = 0; i < NPROC; i++) { //ptable을 순회하며 각 프로세스의 current queue를 조사한다.
        if ((ptable.proc[i].state == RUNNABLE) && (ptable.proc[i].cur_queue == 0)){ 
          //RUNNABLE한 프로세스만을 뽑아 //L0에 들어가야할 프로세스들을 L0큐에 넣는다.
          MLFQ[qsize] = &ptable.proc[i];
          qsize = qsize+1;
        }
    }
    //만약 위의 Loop에서 L0에 들어갈 프로세스를 찾는다면 아래의 코드를 실행해 스케줄러에게 프로세스를 준다.
      if(qsize != 0){
        int fcfs = FCFS(qsize); //가장 작은 real_tick 값을 갖는 인덱스를 찾는다. 이를 통해 FCFS를 구현할 수 있다.
        p = MLFQ[fcfs]; //L0에 들어온 프로세스 사이에 FCFS를 구현하기 위해 ptable에 할당된 시간을 비교해 가장 작은 프로세스를 스케줄러에게 넘긴다.
      }
    }

    if (!p) {         // L0에서 탐색된 프로세스가 없다면 L1을 탐색한다. 과정은 L0와 같다.
        for (int i = 0; i < NPROC; i++) {
            if (ptable.proc[i].state == RUNNABLE && ptable.proc[i].cur_queue == 1){
              MLFQ[qsize] = &ptable.proc[i];
              qsize = qsize+1;
            }
        }
        if(qsize != 0){ //Queue에 들어간 것이 있다면
        int fcfs = FCFS(qsize); //가장 작은 real_tick 값을 갖는 인덱스를 찾는다.
        p = MLFQ[fcfs]; //L0에 들어온 프로세스 사이에 FCFS를 구현하기 위해 ptable에 할당된 시간을 비교해 가장 작은 프로세스를 스케줄러에게 넘긴다.
      }
    }
    if (!p) {           //L2
        /*int next_proc = FindMin(); //큐에서 가장 작은 인덱스를 찾아주는 함수를 만든다.
        if(next_proc == -1){ //에러가 났을 때의 예외처리, lock을 안풀어주면 panic lock이 뜬다.
          release(&ptable.lock);
          continue;
        }
        p = &ptable.proc[next_proc];
            }*/
        for (int i = 0; i < NPROC; i++) {
          if (ptable.proc[i].state == RUNNABLE && ptable.proc[i].cur_queue == 2){
            MLFQ[qsize] = &ptable.proc[i];
            qsize = qsize+1;
          }
        }
        if(qsize != 0){ //Queue에 들어간 것이 있다면
        int next_proc = FindMin();
        p = &ptable.proc[next_proc];
      }
    }

    if (p) { //cpu를 p에게 준다.
        // Run the selected process.
        c->proc = p;
        switchuvm(p);
        p->state = RUNNING;
        swtch(&(c->scheduler), p->context);
        switchkvm();
        c->proc = 0;
    }
    release(&ptable.lock);
  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
    // cprintf("sched\n");
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
    // cprintf("yield\n");
  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
    // cprintf("forkret\n");
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
    // cprintf("sleep\n");
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    // cprintf("acquire ptable.lock\n");
    // cprintf("lk : %s", lk->name);
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
  // cprintf("release ptable.lock");
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
    // cprintf("wakeup1\n");
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  // cprintf("wakeup\n");
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
    // cprintf("kill\n");
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
    // cprintf("procdump\n");
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}


int
FindMin()
{
  int next = 0;
  for (; next < NPROC; next++) {
          if(ptable.proc[next].state != RUNNABLE || ptable.proc[next].cur_queue != 2) continue;
          int comp = next+1;
          for(; comp<NPROC; comp++){
            if(ptable.proc[comp].state != RUNNABLE || ptable.proc[comp].cur_queue != 2) continue;

            if(ptable.proc[next].priority <= ptable.proc[comp].priority){
              continue;
            }else{
              next = comp;
            }
          }
          return next;
          }
          return -1;
}

void 
boosting()
{
  struct proc* p;   // 모든 프로세스의 값을 바꿔준다.
  for(p=ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p != 0){
    p->cur_queue = 0;
    p->priority = 3;
    p->proc_tick = 0;
    p->flag_queue = 0;
    }
  }  
}

//setPriority
void
setPriority(int pid, int priority) //특정 프로세스를 찾아서 priority값을 바꾼다.
{
    for(int i=0; i<NPROC; i++){
      if(ptable.proc[i].pid == pid){
        ptable.proc[i].priority = priority;
      }
    }
}

int
getLevel(void) //level은 cur_queue 값에 있다.
{
  return myproc()->cur_queue;
}

void
schedulerLock(int password) //스케줄러락
{
  if(password == 2019034702){
  int i;
  for(i=0; i<NPROC; i++){
    if(&ptable.proc[i] != myproc()) continue; // 현재 프로세스의 ptable 인덱스를 찾는다.
    else{
      myproc()->flag_Lock = 1;    //스케줄러락이 걸리면 flag_lock값을 1로 바꿈으로써 스케줄러에서 확인할 수 있도록 한다.
      ticks = 0;                  //ticks도 초기화한다.
      break;
    }
    if(i == NPROC){ //기존에 존재하는 프로세스가 아닐 때
    cprintf("schedulerLock can be run by already exist process!!!"); //끝까지 갔는데도 없다면 현재 존재하는 프로세스가 아니므로 예외처리를 해준다.
    exit(); 
     }
    }
  }
  else{
    cprintf("PID: %d\n", myproc()->pid); //비밀번호가 맞지 않다면 명세 조건대로 값을 출력해준다.
    cprintf("Ticks: %d\n", myproc()->proc_tick);
    cprintf("Queue: %d\n", myproc()->cur_queue);
    exit();
  }
}

void
schedulerUnlock(int password) //Unlock은 flag_lock을 2로 바꿈으로써 스케줄러에서 알 수 있게 한다.
{
  if(password == 2019034702){ 
  if(myproc()->flag_Lock == 1){
    myproc()->flag_Lock = 2;
    exit();
    }
    else{
      cprintf("schedulerUnlock can be called by scheduledLocked process\n");
      exit();
    }
  }
  else{ // 비밀번호가 틀렸을 때의 예외처리
    cprintf("PID: %d\n", myproc()->pid);
    cprintf("Ticks: %d\n", myproc()->proc_tick);
    cprintf("Queue: %d\n", myproc()->cur_queue);
    exit();
  }
}

void tickinfo(void) //현재 tick값을 보기위해 
{
  cprintf("Global tick : %d\n", ticks);
  exit();
}

int 
FCFS(int size) //인자로 받은 큐 사이즈 만큼 반복문을 돌면서 프로세스가 큐에 할당된 시간 기준으로 큐에 가장 처음 들어온 놈을 찾는다.
{
  int fcfs = 0;
  for(int k=1; k < size; k++){
    if(MLFQ[fcfs]->real_tick > MLFQ[k]->real_tick) 
    fcfs = k;
  }
  return fcfs;
}

void
init_mlfq(void) //mlfq를 위해 사용할 큐를 초기화해준다.
{
  for(int i = 0; i<NPROC; i++){
    MLFQ[i] = 0;
  }
}
