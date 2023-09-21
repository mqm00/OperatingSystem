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
struct proc* L0[NPROC]; //큐를 proc* 배열로 만든다.
struct proc* L1[NPROC]; 
struct proc* L2[NPROC];


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

  p->cur_queue = 0; //현재 큐를 나타내는 것으로 새로운 프로세스가 alloc 될 땐, 0으로 할당해준다.
  p->priority = 3; // priority도 3을 기본으로 설정
  p->flag_queue = 0; //아래 스케줄러에서 ptable을 계속 돌면서 큐를 업데이트한다. 이때 같은 프로세스가 하나의 큐에 여러개 들어갈 수도 있기에 flag를 하나 만든다.
  p->real_tick = ticks; //FCFS를 위한 시간 정보 
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

   np->state = RUNNABLE;
   np->cur_queue = 0;
   np->flag_queue = 0;
   np->proc_tick = 0;
   np->real_tick = ticks;

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
  int front0 = 0; //각 큐에 프로세스를 넣을 때 위치를 알려주는 변수,
  int front1 = 0; //circular queue를 생각하며 스케줄러를 구현하였다.
  int front2 = 0;
  int back0 = 0; //각 큐에서 cpu를 프로세스에 할당할 때 사용하는 변수
  int back1 = 0;

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    //Enqueue
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;
        //ptable을 순회하며 Runnable하고 아직 어떤 큐에도 존재하지 않는 프로세스를 찾는다.
        //cur_queue를 바꿔주는 건 trap.c의 time interrupt 함수에서 수행된다. 
        //cur_queue값에 따라 각각 맞는 큐에 넣어준다. front는 큐에 들어갈 위치를 포인팅한다.
      if(p->cur_queue == 0 && p->flag_queue == 0){ 
        p->flag_queue = 1;
        p->real_tick = ticks;
        L0[front0] = p;
        front0 += 1;
      }
      else if(p->cur_queue == 1 && p->flag_queue == 0){
        p->flag_queue = 1;
        p->real_tick = ticks;
        L1[front1] = p;
        front1 += 1;
      }
      else if(p->cur_queue == 2 && p->flag_queue == 0){
        p->flag_queue = 1;
        p->real_tick = ticks;
        L2[front2] = p;
        front2 += 1;
      }
    }
      //각 큐에 대한 FCFS를 tick을 기준으로 sorting함으로써 구현되도록 하였다.
      if(front0 > 1) front0 = sorting(0, front0, L0);
      if(front1 > 1) front1 = sorting(1, front1, L1);
      if(front2 > 1) front2 = sorting(2, front2, L2);
      p = 0;
      
      //실행할 프로세스 탐색
      if(front0 > 0){ //L0큐에 들어간 프로세스가 있다면
      for(; back0 < front0; back0++){ //조건문은 NPROC보다 작다했을 때 오류 -> 아예 0인 곳에서 proc 구조체의 변수를 접근하려해서 그런 것 같다.
        if(L0[back0] == 0 || L0[back0]->state != RUNNABLE){
          //L0[back0] = 0;
          continue;
        }
      p = L0[back0++];
      //이상하게 다음줄에서 back++하면 오류가 난다. 
      //현재 back을 p에 할당하고 다음으로 넘긴다. -> 
      
      //아예 enqueue할 때 전체 tick으로 sorting을 하면서 넣을까. alloc했을 때 tick이 높은 순대로 집어넣으면 스케줄러는 앞에서부터 할당만 해주면 될텐데..
      //순서대로 넣을 때의 sorting 방식은? insertion sorting으로 비교하면서

      if(back0 >= front0) back0 = 0;
      break;
        }
      if(back0 >= front0) back0 = 0; //프로세스가 없다면 back을 다시 0으로 초기화 해주어야한다.
      }
      
      if(!p){ //L0에서 스케줄링이 일어나지 않았을 때
      if(front1 > 0){
      for(; back1 < front1; back1++){ 
        if(L1[back1] == 0 || L1[back1]->state != RUNNABLE)

          continue;
      p = L1[back1++];
      //back1++;
      if(back1 >= front1) back1 = 0;
      break;
          }
      if(back1 >= front1) back1 = 0;
        }
      }

      if (!p) {                         //L2
      if(front2 > 0){
        int next_proc = FindMin(L2, front2);
        if(L2[next_proc]->state == RUNNABLE){
          p = L2[next_proc];
        }
        }
      }
      
      if(p){
        c->proc = p;
        switchuvm(p);
        p->state = RUNNING;

        swtch(&(c->scheduler), p->context);
        
        switchkvm();
        
        c->proc = 0;
      }


      release(&ptable.lock); //릴리즈는 for(;;)문 안에 있어야함을 주의
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
sorting(int level, int front, struct proc* queue[])
{
  struct proc* p;
  int i, j; 
  int count = 1; //count는 해당 큐에 있는 프로세스의 갯수를 세어준다.
  //큐 별로 FCFS를 프로세스의 tick을 기준으로 insertion sorting을 사용해 정렬함으로써 구현한다.
  // if(level == 0){
    for(i=1; i < front; i++){
      if(queue[i-1] != 0) count += 1;
      
      p = queue[i];
      for(j=i-1; j>=0 && ((queue[j] == 0) || (queue[j]->real_tick > p->real_tick)); j--){ //dequeue => 0
        queue[j+1] = queue[j];
      }
      queue[j+1] = p;
    }
    return count;
}

void
Dequeue(int pid, int qlevel) //trap에서 큐 조정할 때 호출할 것
{
  if( qlevel == 0){
  for(int j=0; j < NPROC; j++){
    if(L0[j]->pid != myproc()->pid) continue;

    // myproc()->flag_queue = 0;
    // myproc()->cur_queue = 1;
    // myproc()->proc_tick = 0;
    L0[j]->flag_queue = 0;
    L0[j]->cur_queue = 1;
    L0[j]->proc_tick = 0;
    L0[j] = 0;
    
    }
  } else if( qlevel== 1){
  for(int j=0; j < NPROC; j++){
    if(L1[j]->pid != myproc()->pid) continue;

    // myproc()->flag_queue = 0;
    // myproc()->cur_queue = 2;
    // myproc()->proc_tick = 0;
    L1[j]->flag_queue = 0;
    L1[j]->cur_queue = 2;
    L1[j]->proc_tick = 0;
    L1[j] = 0;
    }
  }
}

void
cleanUp()
{
  for(int i=0; i<NPROC; i++){
    if(L0[i]->state == SLEEPING || L0[i]->state == ZOMBIE) L0[i] = 0;
    if(L1[i]->state == SLEEPING || L1[i]->state == ZOMBIE) L1[i] = 0;
    if(L2[i]->state == SLEEPING || L2[i]->state == ZOMBIE) L2[i] = 0;
    }
}

int
FindMin(struct proc* L2[], int front2)
{  
  int i;
  int min = 0;
  for (i = 1; i<front2; i++)
  {
    if (L2[min]->priority > L2[i]->priority)     
      min = i;
  }
  return min;
}

void 
boosting()
{
  struct proc* p;
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
setPriority(int pid, int priority)
{
    myproc()->priority = priority;
}

int
getLevel(void)
{
  return myproc()->cur_queue;
}

void
schedulerLock(int password)
{
  int i;
  for(i=0; i<NPROC; i++){
    if(&ptable.proc[i] != myproc()) continue;
    else{
      myproc()->flag_Lock = 1;
      break;
    }
  }
  if(i == NPROC){ //기존에 존재하는 프로세스가 아닐 때
    cprintf("schedulerLock can be run by already exist process!!!");
    exit();
  }
}

void tickinfo(void)
{
  cprintf("Global tick : %d", ticks);
  exit();
}

void procinfo(void)
{
  struct proc *p;
  sti();

  cprintf("########################################################################################\n");
  cprintf("Name \t\t PID \t\t State \t\t Priority \t\t Level \t\t Ticks\n");
  for (p = ptable.proc; p < &ptable.proc[NPROC]; p++)
  {
    if (p->state == SLEEPING)
      cprintf("%s \t\t %d \t\t SLEEPING \t\t %d \t\t %d \t\t %d\n", p->name, p->pid, p->priority, p->cur_queue, p->proc_tick);
    else if (p->state == RUNNING)
      cprintf("%s \t\t %d \t\t RUNNING \t\t %d \t\t %d \t\t %d\n", p->name, p->pid, p->priority, p->cur_queue, p->proc_tick);
    else if (p->state == RUNNABLE)
      cprintf("%s \t\t %d \t\t RUNNABLE \t\t %d \t\t %d \t\t %d\n", p->name, p->pid, p->priority, p->cur_queue, p->proc_tick);
    else if (p->state == UNUSED)
      cprintf("%s \t\t %d \t\t UNUSED \t\t %d \t\t %d \t\t %d\n", p->name, p->pid, p->priority, p->cur_queue, p->proc_tick);
    else if (p->state == ZOMBIE)
      cprintf("%s \t\t %d \t\t ZOMBIE \t\t %d \t\t %d \t\t %d \n", p->name, p->pid, p->priority, p->cur_queue, p->proc_tick);
  }

  cprintf("\n");

  cprintf("########################################################################################\n");
  
  struct proc **q = L0;
  for (int i = 0; i < NPROC; i++)
  {
    if (q[i] != 0)
      cprintf("%d l0: %d, %d, %d\n", i, q[i]->pid, q[i]->flag_queue, q[i]->cur_queue);
    else
      cprintf("%d l0: 0\n", i);
  }
  q = L1;
  for (int i = 0; i < NPROC; i++)
  {
    if (q[i] != 0)
      cprintf("%d l1: %d, %d, %d\n", i, q[i]->pid, q[i]->flag_queue, q[i]->cur_queue);
    else
      cprintf("%d l1: 0\n", i);
  }
  q = L2;
  for (int i = 0; i < NPROC; i++)
  {
    if (q[i] != 0)
      cprintf("%d l2: %d, %d, %d\n", i, q[i]->pid, q[i]->flag_queue, q[i]->proc_tick);
    else
      cprintf("%d l2: 0\n", i);
  }

  cprintf("\n\n");
  exit();
}
