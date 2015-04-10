#include <stdio.h>
#include <regex.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <stdint.h>
#include <signal.h>

#define SA struct sockaddr

#include <stdio.h>
#include <errno.h>
#include <sys/time.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>

#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#define SA struct sockaddr
#define h_addr h_addr_list[0]

#define MAX_LEN  1024
#define MAXHOSTNAME 80
#define BUFSIZE 1024
#define DEFAULT_MULTICAST_TTL_VALUE  2

char buf[BUFSIZE];
char rbuf[BUFSIZE];

char multicast_ip[20];
unsigned short multicast_port;

void createRecv();
void createSndr();
void CreateMCast(char *ipaddress, char *port);

int ppid = 0;
int sock;
static int sockfdtcp;
static int RUNNING = 1;
struct sockaddr_in client;
	
int main(int argc, char *argv[])
{		
	struct sockaddr_in servaddr, cliaddr;
    int psd, len;
	struct hostent *hp, *gethostbyname();
    int i;
	int so_reuseaddr, sockopt;
	int portNo;
	char buffer[MAX_LEN];
	int pid = 0;
	char mcastIPAddr[20];
    char mcastPortNo[10];
	
	if(argc != 3)
	{
		printf("Usage: ChatClient <shost> <sport>\n");
		exit(1);
	}	
	
	sockfdtcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	
    so_reuseaddr = 1;
    sockopt = setsockopt(sockfdtcp, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));
	
	hp = gethostbyname(argv[1]);
	memcpy(&(servaddr.sin_addr), hp->h_addr, hp->h_length);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));	
  
	if ((connect(sockfdtcp, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) && i == 2)
	{
		perror("connect error");
		exit(1);
	}
	
	int length = sizeof(client);
	client.sin_family = AF_INET;
	client.sin_addr.s_addr = htonl(INADDR_ANY);
	if ( getsockname (sockfdtcp, (SA *)&client,&length)  && client.sin_family == AF_INET && length == sizeof(client)) 
	{
	perror("getting socket name");
	exit(0);
	}	
	printf("Client Port : %d\n", ntohs(client.sin_port));
    
	
	len = recv(sockfdtcp, buffer, MAX_LEN, 0);
	strcpy(mcastIPAddr, buffer);
	memset(buffer, 0, MAX_LEN);
	
	
	len = recv(sockfdtcp, buffer, MAX_LEN, 0);
	strcpy(mcastPortNo, buffer);
	
	printf("Received  IP   : %s\n", mcastIPAddr);
	printf("Received  Port : %s\n", mcastPortNo);

	CreateMCast(mcastIPAddr, mcastPortNo);
	
	exit(0);
}	
	
void CreateMCast(char *ipaddress, char *port)
{	
	int pid = 0;
	
	strcpy(multicast_ip, ipaddress);
	multicast_port = atoi(port);

	ppid = getpid();
	
	pid = fork();	
	if(pid == 0)	
		createSndr();		
	else
		createRecv();
		
	exit(0);
}


void createRecv()
{
	int flag_on = 1;
	struct sockaddr_in multicast_addr, groupStruct;
	char message_received[MAX_LEN+1];
	int msgrecv_len;
	struct ip_mreq mc_req;
	struct sockaddr_in from_addr;
	unsigned int from_len;
	u_char TimeToLive, loop;
	char buf[MAX_LEN];
    int fromlen, rc;
    struct sockaddr_in from;
	
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket() failed");
		exit(1);
	}

	
	
	
	if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &flag_on, sizeof(flag_on))) < 0)
	{
		perror("receiver setsockopt() failed");
		exit(1);
	}

	
	
	
	memset(&multicast_addr, 0, sizeof(multicast_addr));
	multicast_addr.sin_family      = AF_INET;
	multicast_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	multicast_addr.sin_port        = htons(multicast_port);

	
	if ((bind(sock, (struct sockaddr *) &multicast_addr, sizeof(multicast_addr))) < 0)
	{
		perror("bind() failed");
		exit(1);
	}
	
	
	
	
	TimeToLive = DEFAULT_MULTICAST_TTL_VALUE;	
	if( setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL,(char *) &TimeToLive, sizeof(u_char)) == -1 )
    {
		printf("error setting loopback time \n");
		exit(1);
    }
	
	
		
	
	mc_req.imr_multiaddr.s_addr = inet_addr(multicast_ip);
	mc_req.imr_interface.s_addr = INADDR_ANY;

	
	if ((setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mc_req, sizeof(mc_req))) < 0)
	{
		perror("error joining group");
		exit(1);
	}	
	
	while(RUNNING)
    {
	  fromlen = sizeof(from);
      if ((rc=recvfrom(sock, buf, sizeof(buf), 0, (SA *) &from, &fromlen)) < 0)
         perror("receiving datagram  message");

      if (rc > 0){
         buf[rc] = '\0';
         printf("----------------------------------------------\n");
         printf("From %s:%d\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));
		 printf("Received: %s", buf);
		 printf("----------------------------------------------\n");
       }
    }

	
	if ((setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*) &mc_req, sizeof(mc_req))) < 0)
	{
		perror("Drop membership failed\n");
		exit(1);
	}
	
	printf("Exiting client\n");
	close(sock);
	close(sockfdtcp);
	exit(0);
}



void createSndr()
{
   int   sd;
   struct   sockaddr_in server;
   struct  hostent *hp, *gethostbyname();
   struct  servent *sp;
   struct sockaddr_in from;
   struct sockaddr_in addr;
   int fromlen;
   int length;
   int rc, cc;
   int bytes=0;
   char ThisHost[80];

   sp = getservbyname("echo", "udp");

   gethostname(ThisHost, MAXHOSTNAME);

   
   if  ( (hp = gethostbyname(ThisHost)) == NULL ) {
      fprintf(stderr, "Can't find host %s\n", multicast_ip);
      exit(-1);
   }
   bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
   

   if  ( (hp = gethostbyname(multicast_ip)) == NULL ) 
   {
      addr.sin_addr.s_addr = inet_addr(multicast_ip);
      if ((hp = gethostbyaddr((char *)&addr.sin_addr.s_addr, sizeof(addr.sin_addr.s_addr),AF_INET)) == NULL) {
         fprintf(stderr, "Can't find host %s\n", multicast_ip);
         exit(-1);
      }
   }
   
   printf("----Sender running at host NAME: %s\n", ThisHost);
   bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
   printf("(Sender INET ADDRESS is: %s )\n", inet_ntoa(server.sin_addr));

   
   server.sin_family = AF_INET; 
   server.sin_port = htons(multicast_port);

    
    sd = socket (hp->h_addrtype,SOCK_DGRAM,IPPROTO_UDP);
    if (sd<0) {
       perror("opening datagram socket");
       exit(-1);
    }   

	
	if ((bind(sd, (struct sockaddr *) &client, sizeof(client))) < 0)
	{
		perror("bind() failed");
		exit(1);
	}	

   
   while(RUNNING)
   {
	  fflush(stdout);
      memset(buf, '\0', BUFSIZE);
	  fflush(0);

	  rc = read(0,buf,sizeof(buf));
	  if (rc < 0)
	  {
		printf("error reading from multicast socket\n");
		exit(-1);
	  }
	  else if(rc == 0)
	  {		 
		 RUNNING = 0;
		 printf("Exiting ChatClient\n");
		 kill(getppid(), 9);
		 kill(getpid(), 9);
		 raise(SIGKILL);
	  }
	  
      if (sendto(sd, buf, rc, 0, (SA *)&server, sizeof(server)) <0 )
         perror("sending datagram message");
   }
   
   printf("Exiting client\n");
   close(sockfdtcp);
   close(sd);
   close(sock);
   close(sd);
   exit (0);
}

