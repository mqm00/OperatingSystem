// Shell.
//pmanager의 구동 설명을 보면 shell과 비슷하게 작동한다.
//pmanager 실행 후 한줄씩 명령어를 입력받아 실행하고 종료 명령이 나오면 종료되는 매커니즘
//따라서 sh.c에서 필요한 부분을 가져와 변형한다.
#include "param.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"
#include "syscall.h"
#include "traps.h"
#include "memlayout.h"


#define MAXARGS 55



int fork1(void);  // Fork but panics on failure.
void panic(char*);

// Execute cmd.  Never returns.
int
runcmd(char* buf)
{
    char command[10] = {0};
    int buf_index = 0;

    while(buf[buf_index] != ' ' && buf[buf_index] != '\n'){ //먼저 buf에서 command를 잘라낸다.
        //printf(2, "buffer \n");
        command[buf_index] = buf[buf_index];
        buf_index++;
    }

    //printf(1, "command: %s", command);
    if(strcmp(command, "list") == 0){ //list면 뒤에 오는 인자가 없으므로 list 명령만 수행
        list_proc();
        //printf(2, "Hi\n");
        printf(2, "> ");
        return 0;
    }
    else if(strcmp(command, "kill") == 0){ //kill이면 pid 받아와야하므로, 다음 buf부터 가져온다.
        buf_index += 1; //띄어쓰기, 즉 스페이스바를 담고있는 인덱스일 것이므로 +1 해준다.
        char* pid = "";
        for(int i=0; buf[buf_index] != '\n'; i++, buf_index++){ //pid 배열에는 pid로 입력한 숫자들이 들어간다.
            pid[i] = buf[buf_index];
        }
        int target = atoi(pid);
        if(kill(target) == 0){
            printf(1, "kill %d is success\n", target);
        }
        else{
            printf(1, "kill %d is not success\n", target);
        }
        printf(2, "> ");
        return 0;
        
    }
    else if(strcmp(command, "execute") == 0){
        char path[50];
        char* stacksize = "";
        char** argv = 0;
        buf_index += 1;
        for(int i=0; buf[buf_index] != ' '; i++, buf_index++){ //path 배열에는 path로 입력한 것들이 들어간다.
            path[i] = buf[buf_index];
        }

        buf_index += 1;
        for(int i=0; buf[buf_index] != '\n'; i++, buf_index++){ //stacksize 배열에는 stacksize로 입력한 숫자들이 들어간다.
            stacksize[i] = buf[buf_index];
        }
        int size = atoi(stacksize);

        if(fork() != 0)  wait();
        else
        {
        if(fork()==0){
        if(exec2(path, argv ,size) == -1){
            printf(1, "exec %s is failed !! \n", path);
            exit();
            };
        }
        else{
            printf(2, "> ");
            exit();
        }
        exit();
        }
    }
    else if(strcmp(command, "memlim") == 0){
        //char limit;
        //pid char 배열 만들기
        char* pid = "";
        char* limit = "";
        buf_index += 1;
        for(int i=0; buf[buf_index] != ' '; i++, buf_index++){ //pid 배열에는 pid로 입력한 숫자들이 들어간다.
            pid[i] = buf[buf_index];
        }
        int target = atoi(pid);
        
        //limit char 배열 만들기
        buf_index += 1;
        for(int i=0; buf[buf_index] != '\n'; i++, buf_index++){ //pid 배열에는 pid로 입력한 숫자들이 들어간다.
            limit[i] = buf[buf_index];
        }
        int memlim = atoi(limit);

        if(setmemorylimit(target, memlim) == 0){
            printf(1, "Process %d's memory limit : %d\n", target, memlim);
        }
        else{
            printf(2, "set memory limit to is incomplete!\n");
        }
        printf(2, "> ");
        return 0;
    }
  //exit();
  return 0;
}


int
getcmd(char *buf, int nbuf)
{
  //printf(2, "pmanager: ");
  memset(buf, 0, nbuf); //이전에 채워져있던 buf의 memory 공간을 0으로 초기화한다.
  gets(buf, nbuf); //buf배열에 있는 문자들을 읽는다.
  if(buf[0] == 0) return -1; //EOF
  if(buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't') {
    printf(2, "Pmanager has been exited\n");
    exit();
}
  return 0;
}

int
main(void)
{
  //printf(2, "main\n");
  static char buf[100];
  int fd;
  printf(2, "Now Pmanager has been run\n");

  // Ensure that three file descriptors are open.
  while((fd = open("console", O_RDWR)) >= 0){
    //printf(2, "fd\n");
    if(fd >= 3){
      close(fd);
      break;
    }
  }

  // Read and run input commands.
  printf(2, "> ");
  while(getcmd(buf, sizeof(buf)) >= 0){
    runcmd(buf);
    // if(fork1() == 0) //질문할 것!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //   runcmd(buf);
    //wait();
  }
  exit();
}

void
panic(char *s)
{
  printf(2, "%s\n", s);
  exit();
}

int
fork1(void)
{
  int pid;

  pid = fork();
  if(pid == -1)
    panic("fork");
  return pid;
}