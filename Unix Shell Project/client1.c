#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#define PORT 8080

int main(int argc, char const *argv[]) 
{ 
        int sock = 0; 
        struct sockaddr_in serv_addr;  
        char buffer[1024] = {0}; 
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        { 
                printf("\n Socket creation error \n"); 
                return -1; 
        } 

        serv_addr.sin_family = AF_INET; 
        serv_addr.sin_port = htons(PORT); 

        // Convert IPv4 and IPv6 addresses from text to binary form 
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
        { 
                printf("\nInvalid address/ Address not supported \n"); 
                return -1; 
        } 

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        { 
                printf("\nConnection Failed \n"); 
                return -1; 
        }
   while(1){
	printf("Enter a Command of the following:\ndate: to show the date and time of the system\nls -l: displays a detailed explaination of files in the directory\npwd: prints the path of the current working directory\nps -r: displays the process status\nnetstat:to show Active Internet Connections and Active Unix\n$ ls | wc -l: prints the number of files in the directory\nls -1 | sort -n: prints the files of the directory in alphabetical order\nps aux | wc -l: prints the number of processes\nls -lR | grep drwx | wc -l:prints the number of times drwx was found in the processes' information. "); 
        printf("Enter a command:");
	char input[500];
	fgets(input,sizeof(input),stdin);
	strtok(input,"\n");
        if(strcmp(input,"exit") == 0){
          close(sock);
          break;
        }
	send(sock , input , strlen(input) , 0 );  
       // printf("%s\n",input ); 
        char buff[9000];
        recv(sock,buff,sizeof(buff), 0);
        printf("%s\n",buff);
    }
        close(sock);       
	   return 0; 
} 
