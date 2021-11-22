#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <sys/types.h>
#include <pthread.h>

#define PORT 8080

void aux();
void lssort();
void grep();
void mkdir();
void lswc();

void* ThreadFunction(void *);
int main(int argc,char* argv[]){
  int sock1, sock2, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  // Creating socket file descriptor
  if ((sock1 = socket(AF_INET, SOCK_STREAM, 0)) == 0){
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Forcefully attaching socket to the port 8080
  if (bind(sock1, (struct sockaddr *)&address,sizeof(address)) < 0){
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
 while (1){
    if (listen(sock1, 3) < 0){
      perror("Listen Failed");
      exit(EXIT_FAILURE);
    }
    if ((sock2 = accept(sock1, (struct sockaddr *)&address,(socklen_t *)&addrlen)) < 0){
      perror("accept");
      exit(EXIT_FAILURE);
    }
        pthread_t th;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&th,&attr,ThreadFunction,&sock2);
        }
    close(sock1);
    return 0;
    }
 void* ThreadFunction(void * param){
    int *sock = (int*) param;
    int new_sock = *sock;
    while(1){
        char input[1024]={0};
        int exits=0;
        read(new_sock,input,sizeof(input));
        int fd10[2];
        pipe(fd10);
        pid_t pid =fork();
        if(pid == 0){
            close(fd10[0]);
            dup2(fd10[1],1);
            close(fd10[1]);
            if(strcmp(input,"date")==0){
                execl("/bin/date", "date", NULL);
            }else if(strcmp(input, "ls -l") == 0){
                execlp("ls", "ls", "-l", "/", NULL);
            }else if(strcmp(input, "pwd") == 0){
                execl("/bin/pwd","pwd",NULL);
            }else if(strcmp(input, "ps -r") == 0){
                execlp("ps","ps","-r", NULL);
            }else if(strcmp(input, "netstat") == 0){
		        execl("/bin/netstat", "netstat", NULL);
            }else if(strcmp(input, "ls | wc -l") == 0){
                lswc();
            }else if(strcmp(input,"ls -1 | sort -n") == 0){
                lssort();
            }else if(strcmp(input, "ps aux | wc -l") == 0){
                aux();
            }else if(strcmp(input, "ls -lR | grep drwx | wc -l") == 0){
                grep();
            }else if(strcmp(input, "mkdir dir | mkdir dir1 | ls | sort")){
                mkdir();
            }else if(strcmp(input, "exit")){
                close(new_sock);
            }else if(strcmp(input, "exit") == 0){
		        close(fd10[0]);
		        close(fd10[1]);
                exits = 1;
		        break;
            }else{
                perror("No available command");
            }
        }else if(pid > 0){
            waitpid(pid,NULL,0);
            close(fd10[1]);
            char output[20000]={0};
            read(fd10[0],output,sizeof(output));
            send(new_sock,output,strlen(output),0);
            if(exits == 1){
                close(new_sock);
                break;
            }
        }
        close(fd10[0]);
        close(fd10[1]);
    }
    close(new_sock);
}
void aux(){
    int fd[2];
    if(pipe(fd) == -1) {
        perror("ERROR creating a pipe\n");
        exit(1);
    }
    //ps aux | wc -l
    if(!fork())  {
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        execlp("ps", "ps","aux",(char *) NULL);
    }else {
        close(0);
        dup(fd[0]);
        close(fd[1]);
        close(fd[0]);
        execlp("wc","wc","-l",NULL);
    }
}

void lswc(){
    int fd[2];
    if(pipe(fd) == -1) {
        perror("ERROR creating a pipe\n");
        exit(1);
    }
    if(!fork())  {
        close(1);
        dup(fd[1]);
        close(fd[0]);
        execlp("ls", "ls", NULL);
    }else {
        close(0);
        dup(fd[0]);
        close(fd[1]);
        close(fd[0]);
        execlp("wc","wc","-w",NULL);
    }
}

void lssort(){
    int fd[2];
    if(pipe(fd) == -1) {
        perror("ERROR creating a pipe\n");
        exit(1);
    }
    if(!fork())  {
        close(1);
        dup(fd[1]);
        close(fd[0]);
        execlp("ls", "ls", "-1", "/", NULL);
    }else {
        close(0);
        dup(fd[0]);
        close(fd[1]);
        execlp("sort", "sort", "-n", NULL);
    }
}

void grep(){
    //ls -lR | grep drwx | wc -l
    int fd3[2];
    pipe(fd3);
    pid_t pid3 =fork();
    if(pid3 == 0) {
        int fd4[2];
        pipe(fd4);
        pid_t pid4 = fork();
        if(pid4== 0){
            close(1);
            close(fd4[0]);
            dup2(fd4[1],1);
            close(fd4[1]);
            execlp("ls","ls", "-lR","/",NULL);
         }
   else if(pid4>0){
            wait(NULL);
            close(0);
            close(fd4[1]);
            dup2(fd4[0],0);
            close(fd4[0]);
            close(fd3[0]);
            dup2(fd3[1],1);
            close(fd3[1]);
            execl("/bin/grep","grep","drwx",NULL);
          }
    }else if(pid3 > 0){
        wait(NULL);
        close(0);
        close(fd3[1]);
        dup2(fd3[0],0);
        close(fd3[0]);
        execlp("wc","wc","-l",NULL);
        execl("/bin/wc","wc","-l",NULL);
    }
}

void mkdir(){
    int fd4[2];
    int fd5[2];
    int fd6[2];
    
    pipe(fd4);
  if(fork()==0){
	  close(fd4[0]);
	  dup2(fd4[1],1);
	  execlp("/bin/mkdir", "mkdir", "dir", NULL);
  }else{
	  pipe(fd5);
	  if(fork()==0){
	    close(fd4[1]);
	    close(fd5[0]);
	    dup2(fd4[0],0);
	    dup2(fd5[1],1);
	    execlp("/bin/mkdir", "mkdir", "dir1", NULL);
	  }else{
	    pipe(fd6);
	    if(fork()==0){
	      close(fd4[0]);
	      close(fd4[1]);
	      close(fd5[1]);
	      close(fd6[0]);
	      dup2(fd5[0],0);
	      dup2(fd6[1],1);
	      execl("/bin/ls","ls",NULL);
	    }else{
	      close(fd4[0]);
	      close(fd4[1]);
	      close(fd5[0]);
	      close(fd5[1]);
	      close(fd6[1]);
	      dup2(fd6[0], 0);
	      execlp("sort","sort", NULL);
	    }
	    
	  }
  }
}
