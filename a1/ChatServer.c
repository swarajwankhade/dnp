#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
 
#define IP_FIRST_MIN 224
#define IP_FIRST_MAX 239

#define IP_REST_MIN 100
#define IP_REST_MAX 255

#define PORT_MIN 9999
#define PORT_MAX 11001

void genRandomIP(char *buf);  
void genRandomPort(char *buf);
void SHandler(int sig);

int cltPort[30], cltds[30];
char cltaddr[30][20];
int cltCount = 0;


int main(int argc, char *argv[])
{
    int sfd, cfd, j, cnt;
    socklen_t len;
    char ch, buff[INET_ADDRSTRLEN];
    struct sockaddr_in saddr, caddr;
	
	char mcastIPAddr[20];
    char mcastPortNo[10];
 
	if(argc != 2)
	{
        printf("Usage: ChatServer <sport>\n");
        exit(1);
    }

    for(j=0; j<30; j++)
	{
        cltds[j]=-1;
    }
	
    sfd= socket(AF_INET, SOCK_STREAM, 0);
 
    saddr.sin_family=AF_INET;
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    saddr.sin_port=htons(atoi(argv[1]));
 
    bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr));
 
    listen(sfd, 5);
    signal(SIGCHLD, SIG_IGN);
	
	genRandomIP(mcastIPAddr);
    genRandomPort(mcastPortNo);
	
    printf("THE IP   :  %s\n", mcastIPAddr);
	printf("THE Port :  %s\n", mcastPortNo);
 
 	
    signal(SIGINT, SHandler);
    signal(SIGQUIT, SHandler);
    signal(SIGPIPE, SHandler);
    signal(SIGCHLD, SHandler);
    signal(SIGTERM, SIG_IGN);
	
	cltCount = 0;
	 
    while(1) 
	{
        printf("waiting for response from the Client....\n");
        len = sizeof(caddr);
        cfd = accept(sfd, (struct sockaddr *)&caddr, &len);
		if(cfd < 0)
		{
			printf("Error accepting \n");
			exit(0);
		}
							
		strcpy(cltaddr[cltCount], inet_ntoa(caddr.sin_addr));
        cltPort[cltCount] = ntohs(caddr.sin_port);
	    cltds[cltCount] = cfd;
		
		printf("Connected Clients -----> %s : %d\n", cltaddr[cltCount], cltPort[cltCount]);
		
		cnt = send(cfd, mcastIPAddr, strlen(mcastIPAddr), 0);		
		if(cnt<0) 
			perror("write 1 ");
		
		sleep(1);

		cnt = send(cfd, mcastPortNo, strlen(mcastPortNo), 0);
		if(cnt<0) 
			perror("write 1 ");
			
		cltCount++;
    }
}



void genRandomIP(char *buf)
{	
	int num, i;

	
	num = getRandomNum(IP_FIRST_MIN, IP_FIRST_MAX);
	sprintf(buf, "%d", num);
	
	for(i=0; i<3; i++)
	{
		num = getRandomNum(IP_REST_MIN, IP_REST_MAX);
		sprintf(buf, "%s.%d", buf, num);
	}
}

void genRandomPort(char *buf)
{
	
	int num = getRandomNum(PORT_MIN, PORT_MAX);
	sprintf(buf, "%d", num);
}

int getRandomNum(int min, int max)
{
	int result = 0, lnum = 0, hnum = 0;
	if(min < max)
	{
		lnum = min;
		hnum = max + 1; 
	}
	else
	{
		lnum = max + 1; 
		hnum = min;
	}
	
	srand(time(NULL));
	result = (rand() % (hnum - lnum)) + lnum;

	return result;
}



void SHandler(int sig)
{   
	int i, j;
	signal(sig, SHandler);
	
    if(sig == SIGINT)
	{
        printf("Connected Clients ----->\n========================================================\n");	
        
		j = 0;
        for(i=0; i<30; i++)
		{
            if( cltds[i] != -1)
			{
				if(send(cltds[i], "probe", strlen("probe"), 0) < 0 ||
				   send(cltds[i], "probe", strlen("probe"), 0) < 0)
				{
					cltds[i] = -1;
				}
				
				if(cltds[i] != -1)
				{
					j++;
					printf("Client[%d] => %s:%d\n", j, cltaddr[i], cltPort[i]);
				}
			}
        }
   } 
   else if(sig == SIGQUIT)
   {
        for(i=0; i<30; i++)
		{
            if(cltds[i] != -1) 
				kill(cltds[i], SIGTERM);
        }
		
        exit(0);
   } 
   else if(sig == SIGTERM)
   {
	   printf("\n");
       exit(1);
   }
   else if(sig == SIGCHLD || sig == SIGPIPE)
   {
       pid_t pid = wait(NULL);
       for(i=0; i<30; i++)
	   {
           if(cltds[i]==pid)
		   {
               cltds[i]=-1;
               break;
           }
       }
   }
}



