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

int nextpid = 1;
int nexttid = 1;
extern void forkret(void);
extern void trapret(void);
int hasThread(struct proc*);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
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
  struct proc *p;
  char *sp;

  acquire(&ptable.lock); //락을 잡는다.

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++) //ptable에 접근해 UNUSED인 프로세스를 찾는다.
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;  //state를 바꾸고
  p->pid = nextpid++; //프로세스 id를 현재 nextpid로 할당한 후 nextpid를 +1 해준다. 

  release(&ptable.lock);  //pid를 설정한 후 lock를 푼다.

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){ // Allocate one 4096-byte page of physical memory. -> 메모리공간에 프로세스가 사용할 페이지 공간을 할당해준다.
    p->state = UNUSED;    //실패하면 UNUSED로 바꾸고 종료.
    return 0;
  }
  sp = p->kstack + KSTACKSIZE; //sp는 stackpointer일 것이다. 프로세스의 kernel stack의 바닥에서 stack크기만큼 더해준다. 
  //                             -> sp가 스택의 가장 윗부분을 가리키고 있을 것이다.

  // Leave room for trap frame.
  sp -= sizeof *p->tf; //거기서 trap frame이 들어갈 자리만큼 sp를 내려준다.
  p->tf = (struct trapframe*)sp; //프로세스의 tf에 현재 스택포인터의 주소값을 넣어준다.

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret; 

  sp -= sizeof *p->context; //스택포인터를 p의 context크기만큼 내린다.
  p->context = (struct context*)sp; //p의 context에 현재 스택포인터가 가리키는 주소를 넣어준다.
  memset(p->context, 0, sizeof *p->context); //memoryset을 한다. 
  //메모리를 할당받으려는 포인터, p->context. 각 메모리 블락에 넣을 값, 0. 메모리 블락을 차지할 크기, context의 크기.
  p->context->eip = (uint)forkret; //eip는 프로그램 카운터 레지스터이다.

  p->sz_limit = 0; //제한 없음. 처음 프로세스가 생길 땐 memeory limit이 없다.
  p->mainthread = 1; //메인스레드이다.
  p->tid = 0; //프로세스로 할당된 애들은 메인 스레드가 될 것이고 이는 tid에 0을 할당하는 것으로 표현한다.
  p->page_counter = 1;
  return p; //그렇게 메모리를 할당받고 스택에 데이터를 다 넣은 프로세스가 return 된다.
}

//thread를 위한 allocproc
static struct proc*
allocproc2(void)
{
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
  p->pid = 0; //thread의 pid는 메인 쓰레드의 pid를 따른다. 이는 thread_create에서 초기화해준다.
  release(&ptable.lock);  //pid를 설정한 후 lock를 푼다.

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){ 
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE; 

  // Leave room for trap frame.
  sp -= sizeof *p->tf; //거기서 trap frame이 들어갈 자리만큼 sp를 내려준다.
  p->tf = (struct trapframe*)sp; //프로세스의 tf에 현재 스택포인터의 주소값을 넣어준다.

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret; 

  sp -= sizeof *p->context; //스택포인터를 p의 context크기만큼 내린다.
  p->context = (struct context*)sp; //p의 context에 현재 스택포인터가 가리키는 주소를 넣어준다.
  memset(p->context, 0, sizeof *p->context); //memoryset을 한다. 
  //메모리를 할당받으려는 포인터, p->context. 각 메모리 블락에 넣을 값, 0. 메모리 블락을 차지할 크기, context의 크기.
  p->context->eip = (uint)forkret; //eip는 프로그램 카운터 레지스터이다.

  p->sz_limit = 0; //제한 없음. 처음 프로세스가 생길 땐 memeory limit이 없다.
  p->mainthread = 0; //메인스레드가 아니다.
  p->retval = 0;

  return p; //그렇게 메모리를 할당받고 스택에 데이터를 다 넣은 프로세스가 return 된다.
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
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
  uint sz;
  struct proc *curproc = myproc();
  struct proc* p;


  acquire(&ptable.lock);
  sz = curproc->sz;
  //memorylimit이 있는지 확인
  //limit이 0이거나 sz+n이 limit보다 작다면 메모리를 원하는 만큼 늘려줘도 된다. 
  if((curproc->sz_limit == 0) || ((sz+n) <= (curproc->sz_limit))){
    if(n > 0){
      if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
        return -1;
    } else if(n < 0){
      if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
        return -1;
    }
    
    for(p=ptable.proc ; p<&ptable.proc[NPROC] ; p++){
        if(p->pid == curproc->pid)
            p->sz = sz;
    }

	  release(&ptable.lock);
    curproc->sz = sz;
    curproc->page_counter += n;
    switchuvm(curproc);
    return 0;
  }
  else{ //0이 아니라면 제한이 있으니, sz_limit과 sz+n 비교
        //limit보다 큰 메모리를 할당하려고 한다면
      cprintf("You can't grow memory size more than limit !!! \n");
      return -1;
  }
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np; //new process
  struct proc *curproc = myproc(); //curproc에는 현재 프로세스를 넣는다.

  // Allocate process.
  if((np = allocproc()) == 0){ //새로운 프로세스를 할당한다. -> 물리적인 메모리 공간을 할당받고, 알맞은 스택을 갖는다.
    return -1;
  }

  //Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){ //새로운 프로세스의 페이지 테이블에 현재 프로세스의 페이지 테이블을 크기만큼 복사한다.
    kfree(np->kstack); //제대로 페이지 테이블이 복사되지 않으면 np를 날린다.
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }

  //페이지테이블이 제대로 복사가 되었다면 np의 속성들을 초기화해준다.
  np->sz = curproc->sz; //메모리 사이즈를 curproc과 똑같이 만들고,
  np->sz_limit = curproc->sz_limit; //부모의 meme limit을 따라간다.
  np->parent = curproc; //parent에는 현재 프로세스가 들어간다.
  *np->tf = *curproc->tf; //trap frame의 내용도 부모의 것을 갖는다. 
  np->mainthread = 1; //fork시에는 새로운 프로세스가 생성되는 것이고 이는 메인 스레드가 된다.

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]); //부모 프로세스가 오픈하고 있는 file이 있다면 자식 프로세스도 그를 복사한다.
  np->cwd = idup(curproc->cwd); //current working directory도 복사한다.

  safestrcpy(np->name, curproc->name, sizeof(curproc->name)); //자식 프로세스의 이름은 부모 프로세스의 이름을 따라간다.

  pid = np->pid; //pid를 설정하고

  acquire(&ptable.lock);

  np->state = RUNNABLE; //state를 RUNNABLE로 바꾼 후 fork를 종료한다.

  release(&ptable.lock);

  return pid; 
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  struct proc *ancestor;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // if(curproc->mainthread == 0){
  //   cprintf("thread should be exited by thread_exit\n");
  //   return;
  // }
  ancestor = curproc->parent->parent;
  int flag = hasThread(curproc);
  if(flag == 1){
    thread_cleanup(curproc->pid, curproc->tid, 1);
    if(curproc->mainthread == 0) curproc->parent = ancestor; //trouble shooting
  }

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
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
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
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
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
  struct proc *p;
  // struct proc *ancestor;
  // int flag;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);

      // flag = hasThread(p);
      // if(flag == 1){
      //   ancestor = p->parent->parent;
      //   thread_cleanup(p->pid, p->tid, 0);
      //   p->parent = ancestor;
      // }
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
setmemorylimit(int pid, int limit)
{
  if(limit < 0){ 
    cprintf("limit should be positive!\n");
    //cprintf("limit is %d\n", limit);
    return -1; //Limit이 음수이면 바로 종료
  }

  struct proc* p = 0;
  acquire(&ptable.lock);
  for(int i=0; i<NPROC; i++){
    if(ptable.proc[i].pid == pid){ //ptable에서 pid가 같은 것을 찾으면 P에 할당한다.
      p = &ptable.proc[i];
      break;
      } 
  }
  if(!p){
    cprintf("The process is not exist!\n");
     return -1; //pid가 존재하지 않는 경우
    }
  if(p->sz > limit){
    cprintf("limit should be bigger than before!\n");
    return -1; 
  }

  if(limit > 0){
    p->sz_limit = limit;
  }
  else{
    p->sz_limit = 0; //제한 없음 표시.
    cprintf("No limit setting\n");
  }
  release(&ptable.lock);
  return 0;
}

void
list_proc(void)
{
  struct proc* p;
  cprintf("\n");
  cprintf("Process        |     PID     |     Page     |     Memory     |     Limit     |\n");
  cprintf("--------------------------------------------------------------------------\n");
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){ 
    if(p->state == RUNNABLE || p->state == RUNNING || p->state == SLEEPING){
      if(p->mainthread == 0) continue; //thread는 출력하지 않는다.
      //int count_page=0;
      //for(int i=0; i<1024; i++){    //xv6 공식문서 참조 결과 Page table의 valid bit은 PTE_P로 선언되어있다고 한다. 이를 통해 페이지 갯수를 찾는다.
        //if(p->pgdir[i] && PTE_P) count_page += 1;
      //}
    cprintf("%s        |      %d      |      %d      |     %d     |        %d        |\n", p->name, p->pid, p->page_counter, p->sz, p->sz_limit);
    }
  }
  release(&ptable.lock);
}


int
thread_create(thread_t* thread, void* (*start_routine)(void*), void* arg)
{
  //인자로 들어온 thread 구조체에 필요한 정보들을 담는다.
  //필요한 정보라 하면, 쓰레드 각각이 갖는 것. stack, code?
  //그럼 공유를 한다는 것은 create를 부른 쓰레드의 address space의 주소값을 넘겨주면 되는건가
  //그렇다면 process에서 자원 공유할 건 공유하고 분리할 건 분리한 상태의 프로세스를 스레드로 설정하면 되겠다.
  //한마디로 스레드와 같이 특정 프로세스와 자원을 공유하는 프로세스를 스레드라고 생각하자. 그렇다면 스케줄러와 시스템콜도 이전 것을 최대한 활용할 수 있을 것 같다.
  //공유하는 자원은?
  //분리되는 자원은?
  //fork와 exec을 적절히 변형하여 구현해보자.
  struct proc* lwp; //쓰레드가 될 프로세스
  struct proc* main;
  struct proc* curproc = myproc(); //쓰레드의 메인 쓰레드가 될 프로세스
  uint sz;
  uint sp;
  uint userstack[2];


  if((lwp = allocproc2()) == 0){ //쓰레드가 사용할 physical 공간을 할당한다.
    cprintf("allocproc error\n");
    return -1;
  }

  //공간이 할당되었으면 페이지 테이블을 공유함으로써 자원을 공유하도록 한다. 스레드인 것처럼
  //fork에선 이 부분을 copyuvm으로 그대로 카피하지만 스레드의 경우는 공유를 하면 된다.

  acquire(&ptable.lock);
  main = curproc->mainthread ? curproc : curproc->parent;
  //lwp->sz = curproc->mainthread ? curproc->sz : curproc->parent->sz;
  lwp->parent = main;
  lwp->pgdir = main->pgdir;
  lwp->pid = main->pid;
  *lwp->tf = *curproc->tf; //trap frame의 내용도 일단 부모의 것을 갖는다.

  for(int i = 0; i < NOFILE; i++)
    if(main->ofile[i])
      lwp->ofile[i] = filedup(main->ofile[i]); //부모 프로세스가 오픈하고 있는 file이 있다면 쓰레드도 가져온다.
  lwp->cwd = idup(main->cwd); //current working directory도 복사한다.

  safestrcpy(lwp->name, main->name, sizeof(main->name)); //스레드의 이름은 부모 프로세스의 이름을 따라간다.

  //pid는 allocproc2()에서 메인 스레드의 pid를 할당하는 것으로 바꾸었다.
  //스레드를 구분할 tid를 설정해준다.
  lwp->tid = nexttid++; 
  //lwp->pid = curproc->pid;
  lwp->tf->eax = 0;

  sz = main->sz;

  //exec에서 pgdir=seyupkvm()을 실행하는데 Lwp에서는 pgdir을 공유한다고 위에 구현했기에 필요없다.
  if((sz = allocuvm(main->pgdir, sz, sz + 2*PGSIZE)) == 0){
    cprintf("allocuvm error\n");
    return -1;
  }
  clearpteu(main->pgdir, (char*)(sz - 2*PGSIZE));
  sp = sz;
  lwp->my_sp = sz;

  userstack[0] = 0xFFFFFFFF;
  userstack[1] = (uint)arg;
  sp -= sizeof(userstack);
  if(copyout(main->pgdir, sp, userstack, sizeof(userstack)) < 0){
    cprintf("copyout error\n");
    return -1;
  }
  
  curproc->sz = sz;
  lwp->sz = curproc->sz;
  lwp->retval = 0;
  lwp->tf->eip = (uint)start_routine;  // main
  lwp->tf->esp = sp;

  struct proc* p;

  for(p=ptable.proc ; p<&ptable.proc[NPROC] ; p++){ //같은 pid를 갖는 스레드들의 sz도 다시 한번 초기화 해준다.
        if(p->pid == curproc->pid)
            p->sz = sz;
    }

  //acquire(&ptable.lock);

  lwp->state = RUNNABLE; //state를 RUNNABLE로 바꾼 후 fork를 종료한다.

  release(&ptable.lock);

  *thread = lwp->tid;

  return 0; 
}

void
thread_exit(void* retval)
{
  struct proc *curproc = myproc();
  int fd;

  curproc->retval = retval; //thread가 실행되는 함수에서의 리턴값을 메인으로 전달해주기 위함이다.
  //cprintf("retval : %d\n", *(int*)retval);
  //cprintf("curproc->retval : %d\n", *(int*)curproc->retval);
  //한마디로 스레드로 실행되는 함수의 리턴값을 스레드를 통해 메인으로 전달하는 것 같다.
  //그럼 이 리턴값을 join 함수에서 curproc->retval로 접근할 수 있을 것이다.

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

  wakeup1(curproc->parent);
  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;

  sched();
  panic("zombie exit");
}

int
thread_join(thread_t thread, void **retval)
{
  struct proc *p;
  int havekids;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->tid != thread)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one
        *retval = p->retval; //스레드의 retval을 스레드를 create한, 예를 들면 main함수로 넘겨주기 위한 작업
        kfree(p->kstack); //할당받은 물리주소 free
        p->kstack = 0;
        p->pid = 0;
        p->tid = -1;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        //deallocuvm()
        release(&ptable.lock);
        return 0;
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

void
thread_cleanup(int pid, int tid, int flag)
{
  //flag는 exec과 exit을 나누어주는 역할을 한다.
  //exec의 경우는 thread_cleanup을 부른 프로세스는 종료되어선 안되고
  //exit의 경우는 thread_cleanup을 부른 프로세스 또한 종료되어야한다.
  struct proc* p;

  acquire(&ptable.lock);
  //if(flag == 0){ //exec, 본인 빼고 다 죽인다.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid && p->tid != tid){
        kfree(p->kstack);
        p->kstack = 0;
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        p->mainthread = 0;
        p->sz = 0;
    }
  //}
  }
  // else{ //exit, kill. 부모는 죽이지 않는다. 
  //   for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
  //   if(p->pid == pid && p->tid != tid){
  //     if(p->mainthread == 1) continue; //부모는 죽이지 않는다.
  //       kfree(p->kstack);
  //       p->pid = 0;
  //       p->parent = 0;
  //       p->name[0] = 0;
  //       p->killed = 0;
  //       p->state = UNUSED;
  //       p->mainthread = 0;
  //       p->sz = 0;
  //   }
  // }
  // }

  release(&ptable.lock);
}

int
hasThread(struct proc* main) //특정 프로세스가 스레드를 갖고 있는지 확인하는 함수
{
  int flag = 0;
  struct proc* p;
  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if((p->pid == main->pid) && (p->tid != 0)){
      flag = 1;
      release(&ptable.lock);
      return flag;
    }
  }
  release(&ptable.lock);
  return flag;
}