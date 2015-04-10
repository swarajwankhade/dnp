#include "def"

#define MAXHOSTNAME 80
#define BUFSIZE 1024

char buf[BUFSIZE];
char rbuf[BUFSIZE];
char recvBuf[BUFSIZE];


void joinGroup(int s, char *group);
void sendMessage(int inSock, int outSock);
void getMessage(int inSock, int outSock);
void startSelect();
void GetUserInput();
void reusePort(int s);
void setTTLvalue(int s, u_char * ttl_value);
void setLoopback(int s, u_char loop);


 
void interruptHandle();
void quitHandle();
void quitHandleParent();
void PIPEhandle();

char L[1024],E[1024];
char *username;
char mcastIP[15];
char mport[6];

int TCPPort;
int UDPPort;
int serverPortArgument;
int childPID;

int flag = 0;
u_char TTL;
u_char loop;

fd_set active_fd_set, read_fd_set;

struct sockaddr_in serverSocketAddress;
struct sockaddr_in clientListenerUDPSocketAddress;
struct sockaddr_in clientSenderUDPSocketAddress;
struct sockaddr_in clientTCPSocketAddress;
struct sockaddr_in serverUnicastUDPSocketAddress;
struct sockaddr_in GroupAddress;

int serverSD,cltLSD,cltSSD,cltUSD;

int i,c;

main( argc, argv )
int argc;
char *argv[];
{
	
	struct sockaddr_in addr;
	struct hostent *hp, *gethostbyname();
	int length;
	
	int bytes = -1;
		
	if (argc < 4) 
	{
         printf("Usage: UMChatClient <shost> <sport> <u|m>\n");
         exit(0);
   	} 
   	
   	signal( SIGINT, interruptHandle );
	signal( SIGQUIT, quitHandle );
   	
	 
     FD_ZERO (&active_fd_set);
   	
   	if  ( (hp = gethostbyname(argv[1])) == NULL ) 
   	{
    		addr.sin_addr.s_addr = inet_addr(argv[1]);
    		if ((hp = gethostbyaddr((char *) &addr.sin_addr.s_addr,sizeof(addr.sin_addr.s_addr),AF_INET)) == NULL) 
       		{
      			fprintf(stderr, "Can't find host %s\n", argv[1]);
      			exit(-1);
    		}
  	}
  	
  	printf("ChatServer host NAME:\t %s\n", hp->h_name);
  	bcopy ( hp->h_addr, &(serverSocketAddress.sin_addr), hp->h_length);
  	printf("ChatServer IP address:\t %s \n", inet_ntoa(serverSocketAddress.sin_addr));

  	serverSocketAddress.sin_family = AF_INET;
  	serverSocketAddress.sin_port = htons(atoi(argv[2]));
  	serverSD = socket (AF_INET,SOCK_STREAM,0);
  	
  	if ( connect(serverSD, (SA *) &serverSocketAddress, sizeof(serverSocketAddress)) < 0 )
  	{
    	close(serverSD);
    	perror("connecting to server socket");
    	exit(0);
	}
	
	printf("serverSD - %d, argv[3] : %s \n", serverSD, argv[3]);
	
	length = sizeof(clientTCPSocketAddress);
	if ( getsockname (serverSD, (SA *)&clientTCPSocketAddress,&length) )
	{
		perror("getting socket name");
		exit(0);
  	}

   	TCPPort = ntohs(clientTCPSocketAddress.sin_port);

	printf("Client TCP Port:\t %d\n", TCPPort);
	
	username=getenv("USER");
	printf("UserName --> %s\n", username);
	
	serverPortArgument = atoi(argv[2]);
	
	if(strcmp(argv[3],"u")==0) 
	{
		if (send(serverSD, "1", 1, 0) <0 )
        perror("sending stream message\n");
        
        usleep(2000);
        if (send(serverSD, username, strlen(username), 0) <0 )
        perror("sending stream message\n");
        
        flag = 0;
	}
	else if(strcmp(argv[3],"m")==0)
	{
		if (send(serverSD, "0", 1, 0) <0 )
        perror("sending stream message\n");
		
		usleep(1000);
        if (send(serverSD, username, strlen(username), 0) <0 )
        perror("sending stream message\n");
        
        flag = 1;
	}
	
	while(1)
	{
		memset(recvBuf, '\0', BUFSIZE);
		bytes = recv(serverSD, recvBuf, BUFSIZE, 0);
		
		if(bytes > 0)
		{
			if(flag)
			{
				if(strlen(mcastIP) == 0)
				{
					strcpy(mcastIP,recvBuf);
					continue;
				} 
			}
			if(strlen(mport) == 0)
			{
				strcpy(mport,recvBuf);
				
			}
			else if(strlen(L) == 0)
			{
				strcpy(L,recvBuf);
			}
			else if(strlen(E) == 0)
			{
				strcpy(E,recvBuf);
				break;	
			}					
		}
	}
	
	
  	serverUnicastUDPSocketAddress.sin_family = AF_INET;
 	serverUnicastUDPSocketAddress.sin_port = htons(TCPPort);
 	serverUnicastUDPSocketAddress.sin_addr.s_addr  =  htonl(INADDR_ANY);
 	
 	
  	cltUSD = socket (AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
  	reusePort(cltUSD);
	
	
	if(bind (cltUSD,  (SA *) &serverUnicastUDPSocketAddress, sizeof(serverUnicastUDPSocketAddress)) < 0)
	{
		perror("Getting UNI UDP SENDER Socket name");
    	exit(0);
	}
	
	FD_SET (cltUSD, &active_fd_set);
	
	
	FD_SET (0, &active_fd_set);
	
	if(flag)
	{
		printf("Received:\n McastIP : %s \n Mport : %s \n List Code(L) : %s \n End Code(E) : %s \n",mcastIP,mport,L,E);
		
		clientListenerUDPSocketAddress.sin_family = AF_INET;
		clientListenerUDPSocketAddress.sin_port = htons(atoi(mport));
		clientListenerUDPSocketAddress.sin_addr.s_addr  =  inet_addr(mcastIP);
		
		
		cltLSD = socket (AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
		reusePort(cltLSD);
		
		
		if(bind (cltLSD,  (SA *) &clientListenerUDPSocketAddress, sizeof(clientListenerUDPSocketAddress)) < 0)
		{
			perror("Getting UDP Socket name");
			exit(0);
		}
		
		
		TTL = 2;
		setTTLvalue(cltLSD, &TTL);
		
		loop = 1;		
		setLoopback(cltLSD, loop);
			
		
		joinGroup (cltLSD, mcastIP);
		
		FD_SET (cltLSD, &active_fd_set);
	}
	
	printf("Type a message to be sent followed by ENTER\n");
	printf("hit CTRL-\\ to quit \n");
	
	
	GroupAddress.sin_port = htons(serverPortArgument);
	GroupAddress.sin_addr.s_addr = serverSocketAddress.sin_addr.s_addr;
	
	if ((c=sendto(cltUSD, L, strlen(L), 0, (SA *) & GroupAddress, sizeof(GroupAddress))) < 0)
	{
		printf("error in sendto \n");
		exit(-1);
	}
	
	startSelect();
}

void startSelect()
{
	while (1)
    {
		
		read_fd_set = active_fd_set;
		memcpy(&read_fd_set, &active_fd_set, sizeof(active_fd_set));
		if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
			perror ("select");
			exit (EXIT_FAILURE);
        }
    	char inputBuf[BUFSIZE];
		char sendBuf[BUFSIZE];
		int c,bytes=0;
		
		for (i = 0; i < FD_SETSIZE; ++i)
		{
			if (FD_ISSET (i, &read_fd_set))
			{
				if(i == 0)
				{
					GroupAddress.sin_family = AF_INET;
					if(flag)
					{
						GroupAddress.sin_port = htons(atoi(mport));
						GroupAddress.sin_addr.s_addr = inet_addr(mcastIP);
					}
					else
					{
						
							GroupAddress.sin_port = htons(serverPortArgument);
							GroupAddress.sin_addr.s_addr = serverSocketAddress.sin_addr.s_addr;
					}
						
					memset(inputBuf, '\0', BUFSIZE);
					bytes = read(0, inputBuf, BUFSIZE);
					if (bytes < 0) 
					{
						printf("error in reading from STDIN \n");
						exit(-1);
					}
					else if (bytes == 0) 
					{
						quitHandle(9);
					}
					else if(bytes > 0)
					{
						memset(sendBuf, '\0', BUFSIZE);
						if(flag)
						{
							sprintf(sendBuf,"(Multicast: %s : %d) : %s",inet_ntoa(clientTCPSocketAddress.sin_addr),TCPPort,inputBuf);
						}
						else
						{
							sprintf(sendBuf,"(Unicast: %s : %d) : %s",inet_ntoa(clientTCPSocketAddress.sin_addr),TCPPort,inputBuf);
						}
						
						if ((c=sendto(cltUSD, sendBuf, strlen(sendBuf), 0, (SA *) & GroupAddress, sizeof(GroupAddress))) < 0)
						{
							printf("error in sendto \n");
							exit(-1);
						}
					}
					fflush(stdout);
				}
				else if (i == cltUSD)
				{
					memset(recvBuf, '\0', BUFSIZE);
					int fromlength=sizeof(clientListenerUDPSocketAddress);
					
					bytes = recvfrom(cltUSD, recvBuf, BUFSIZE, 0,(SA*)&clientListenerUDPSocketAddress,&fromlength);
					
					if (bytes < 0)
					{
						printf("error in reading from multicast/unicast socket\n");
						exit(-1);
					} 
					else 
					{		
						if(strncmp(L,recvBuf,7) == 0)
						{
							printf("End of List \n \n");
						}
						else if(strncmp(E,recvBuf,6) == 0)
						{
							printf("Termination Complete \n");
							sleep(1);
							kill(childPID,9);
							exit(-1);
						}
						else
						{
							printf("%s \n",recvBuf);
						}
					}		
				}
				else if(i == cltLSD)
				{
					memset(recvBuf, '\0', BUFSIZE);
					int fromlength=sizeof(clientListenerUDPSocketAddress);
					bytes = recvfrom(cltLSD, recvBuf, BUFSIZE, 0,(SA*)&clientListenerUDPSocketAddress,&fromlength);
					
					if (bytes < 0)
					{
						printf("error in reading from multicast/unicast socket\n");
						exit(-1);
					} 
					else 
					{		
						if(strncmp(L,recvBuf,6) == 0)
						{
							printf("End of List \n");
						}
						else if(strncmp(E,recvBuf,6) == 0)
						{
							printf("Termination Complete \n");
							sleep(1);
							kill(childPID,9);
							exit(-1);
						}
						else
						{
							printf("%s \n",recvBuf);
						}
					}		
				}	
			}
		}
	}
}


void interruptHandle( int sig ) 
{
	int             bytes = 0;
	char            recvBuf[BUFSIZE];
	int fromlength = sizeof(clientListenerUDPSocketAddress);;
	
	struct sockaddr_in serverUnicastAddress;
	serverUnicastAddress.sin_family = AF_INET;

	serverUnicastAddress.sin_port = htons(serverPortArgument);
	serverUnicastAddress.sin_addr.s_addr = serverSocketAddress.sin_addr.s_addr;
	
	signal( sig, interruptHandle );
	
	if ((c=sendto(cltUSD, L, strlen(L), 0, (SA *) & serverUnicastAddress, sizeof(serverUnicastAddress))) < 0) 
	{
		perror("error in interrupt sendto \n");
		exit(-1);
	}
	
	startSelect();
}

void PIPEhandle(int sig)
{

}

void quitHandle(int sig)
{
	int             bytes = 0;
	char            recvBuf[BUFSIZE];
	int fromlength = sizeof(clientListenerUDPSocketAddress);
	int trap = 0;
	
	signal( sig, quitHandle );
	
	struct sockaddr_in serverUnicastAddress;
	serverUnicastAddress.sin_family = AF_INET;

	serverUnicastAddress.sin_port = htons(serverPortArgument);
	serverUnicastAddress.sin_addr.s_addr = serverSocketAddress.sin_addr.s_addr;
	
	if ((c=sendto(cltUSD, E, strlen(E), 0, (SA *) & serverUnicastAddress, sizeof(serverUnicastAddress))) < 0) 
	{
		perror("error in interrupt sendto \n");
		exit(-1);
	}
	
	startSelect();
}

void joinGroup(int s, char *group)
{
	struct sockaddr_in groupStruct;
	struct ip_mreq  mreq;	

	if ((groupStruct.sin_addr.s_addr = inet_addr(group)) == -1)
		printf("error in inet_addr\n");

	
	mreq.imr_multiaddr = groupStruct.sin_addr;
	mreq.imr_interface.s_addr = INADDR_ANY;

	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq,
		       sizeof(mreq)) == -1) {
		printf("error in joining group \n");
		exit(-1);
	}
}



void leaveGroup(int recvSock, char *group)
{
	struct sockaddr_in groupStruct;
	struct ip_mreq  dreq;	

	if ((groupStruct.sin_addr.s_addr = inet_addr(group)) == -1)
		printf("error in inet_addr\n");

	dreq.imr_multiaddr = groupStruct.sin_addr;
	dreq.imr_interface.s_addr = INADDR_ANY;

	if (setsockopt(recvSock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		       (char *) &dreq, sizeof(dreq)) == -1) {
		printf("error in leaving group \n");
		exit(-1);
	}
	printf("process quitting multicast group %s \n", group);
}




void reusePort(int s)
{
	int  one = 1;

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one)) == -1) {
		printf("error in setsockopt,SO_REUSEPORT \n");
		exit(-1);
	}
}



void displayDaddr(int s)
{
	int             one = 1;


        if (setsockopt(s, IPPROTO_IP, IP_PKTINFO, (char *) &one, sizeof(one)) < 0)
                perror("IP_RECVDSTADDR setsockopt error");


}



void setTTLvalue(int s, u_char * ttl_value)
{
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char *) ttl_value,
		       sizeof(u_char)) == -1) {
		perror("error in setting TTL value\n");
	}
}



void setLoopback(int s, u_char loop)
{
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &loop,
		       sizeof(u_char)) == -1) {
		perror("error in Setting loopback\n");
	}
}

cleanup(buf)
char *buf;
{
  int i;
  for(i=0; i<BUFSIZE; i++) buf[i]='\0';
}

