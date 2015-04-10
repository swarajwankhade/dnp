
#include "def"


void generateRand();

#define MAXHOSTNAME 80
#define MAX_CLIENTS 10
#define BUFSIZE 1024



void interruptHandle();
void quitHandle();
void PIPEhandle();

void reusePort(int sock);
void joinGroup(int s, char *group);
void groupExit(int recvSock, char *group);
void setTTLvalue(int s, u_char * ttl_value);
void setLoopback(int s, u_char loop);

void sendMessage(int inSock, int outSock);
void getMessage(int inSock, int outSock);
void interruptHandle();
void quitHandle();
void PIPEhandle();

void printLists();
void startSelect();

void sendUTOM(char* msg);
void sendMTOU(char* msg);
void dataTransfer(int clientSD, int falg);

char L[1024],E[1024];
char MIP[15];
char mport[6];



struct profile{
          char username[1024];
          char ip[15];
          int port;
} *mList,*uList;

struct counter{
		int uClientsCount;
		int mClientsCount;
		int serverUnicastSD;
} *count;

fd_set active_fd_set, read_fd_set;


int seg1,seg2,seg3;

struct profile EmptyStruct;
struct profile tempStruct;

struct sockaddr_in serverSocketAddress;
struct sockaddr_in serverUDPSocketAddress;
struct sockaddr_in serverUnicastUDPSocketAddress;
struct sockaddr_in clientUnicastUDPSocketAddress;
struct sockaddr_in clientSocketAddress;
struct sockaddr_in clientInSocketAddress;
struct sockaddr_in GroupAddress;


char buf[BUFSIZE];
char rbuf[BUFSIZE];
char recvBuf[BUFSIZE];

int i,c,j;
int rc,cc;
int bytes = -1;

int serverSD,serverUDPSD,clientSD,serverUNIUDPSD,clientListenerSD,uniClientListenerSD;
int serverPort;
int tcpChildID,unicastChildID;



void generateRand()
{
	int i,ip1,ip2,ip3,ip4,low,high,pHigh,pLow;
	
	low=224;
	high=239;
	pHigh=11001;
	pLow=9999;
	
	srand((unsigned)getpid());
	ip1=(rand()%(high-low+1)+low);
	ip2=rand()%255;
	ip3=rand()%255;
	ip4=rand()%255;
	sprintf(mport,"%d",((rand()%(pHigh-pLow+1))+pLow));
	sprintf(MIP,"%d.%d.%d.%d",ip1,ip2,ip3,ip4);
	
	sprintf(L,"%d",rand()%1000000+1000000);
	sprintf(E,"%d",rand()%1000000+1000000);
	
}

main( int argc, char **argv )
{
	
	u_char TTL;
	u_char loop;
	
	int sLen;
	int cLen;
	
	
	if (argc < 2 || argc > 2) 
	{
         printf("Usage: UMChatServer <sport> \n");
         exit(0);
   	}
   	
	signal( SIGINT, interruptHandle );
  	signal( SIGQUIT, quitHandle );

	
	serverSocketAddress.sin_family = AF_INET;
  	serverSocketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  	serverPort = htons(atoi(argv[1])); 
  	serverSocketAddress.sin_port =  serverPort;
  	
  	
  	serverSD = socket (AF_INET,SOCK_STREAM,IPPROTO_TCP); 
  	reusePort(serverSD);
  	
  	
  	if ( bind( serverSD, (SA *) &serverSocketAddress, sizeof(serverSocketAddress) ) < 0 ) 
  	{
    	close(serverSD);
    	perror("binding name to stream socket");
    	exit(-1);
  	}
  	
  	sLen = sizeof(serverSocketAddress);  
	
  	if ( getsockname (serverSD, (SA *)&serverSocketAddress,&sLen) ) 
  	{
    	perror("Getting TCP Socket name\n");
    	exit(0);
  	}
	
  	listen(serverSD,MAX_CLIENTS);
	FD_SET (serverSD, &active_fd_set);
  	
  	generateRand();
  	
  	printf("The Multicast IP and Port : %s  : %s \n\n",MIP,mport);
	printf("The ListCode(L) and EndCode(E) : %s , %s \n",L,E);
	
 	serverUnicastUDPSocketAddress.sin_family = AF_INET;
 	serverUnicastUDPSocketAddress.sin_port = htons(atoi(argv[1]));
 	serverUnicastUDPSocketAddress.sin_addr.s_addr  =  htonl (INADDR_ANY);
  		
 	
  	serverUNIUDPSD = socket (AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
  	reusePort(serverUNIUDPSD);
			
	
	if(bind (serverUNIUDPSD,  (SA *) &serverUnicastUDPSocketAddress, sizeof(serverUnicastUDPSocketAddress)) < 0)
	{
		perror("Getting Unicast UDP Socket name");
    	exit(0);
	}
 	
	FD_SET (serverUNIUDPSD, &active_fd_set);
	
	
	
  	serverUDPSocketAddress.sin_family = AF_INET;
 	serverUDPSocketAddress.sin_port = htons(atoi(mport));
 	serverUDPSocketAddress.sin_addr.s_addr  =  inet_addr (MIP);
 	
 	
  	serverUDPSD = socket (AF_INET,SOCK_DGRAM,IPPROTO_UDP); 
  	reusePort(serverUDPSD);
	
	
	if(bind (serverUDPSD,  (SA *) &serverUDPSocketAddress, sizeof(serverUDPSocketAddress)) < 0)
	{
		perror("Getting UDP Socket name");
    	exit(0);
	}
	
 	 
	TTL = 2;
	setTTLvalue(serverUDPSD, &TTL);

	loop = 1;		
	setLoopback(serverUDPSD, loop);
	
 	
 	joinGroup (serverUDPSD, MIP);
 	

	
	FD_SET (serverUDPSD, &active_fd_set);
	
 	 
	GroupAddress.sin_family = AF_INET;
	GroupAddress.sin_port = htons(atoi(mport));
	GroupAddress.sin_addr.s_addr = inet_addr(MIP);
	
 	
  	seg1 = shmget(IPC_PRIVATE, sizeof(uList), S_IRUSR | S_IWUSR);
	uList = (struct profile *) shmat(seg1, NULL, 0);
	
	seg2 = shmget(IPC_PRIVATE, sizeof(mList), S_IRUSR | S_IWUSR);
	mList = (struct profile *) shmat(seg2, NULL, 0);
	
	seg3 = shmget(IPC_PRIVATE, sizeof(count), S_IRUSR | S_IWUSR);
	count = (struct counter *) shmat(seg3, NULL, 0);
	
	memset(uList,0,sizeof(uList));
	memset(mList,0,sizeof(mList));
	
	count[0].uClientsCount = 0;
  	count[0].mClientsCount = 0;
  	count[0].serverUnicastSD = serverUNIUDPSD;
	
  	printf("\nTCP Server Port:\t %d\n", ntohs(serverSocketAddress.sin_port));
  	printf("\nUDP Server Port:\t %d\n", ntohs(serverUDPSocketAddress.sin_port));
  	printf("Waiting for clients.....\n");
	startSelect();
}

void startSelect()
{
	while (1)
    {
		
		memcpy(&read_fd_set, &active_fd_set, sizeof(active_fd_set));
		if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
			perror ("select");
			exit (EXIT_FAILURE);
        }
		
		for (j = 0; j < 10; j++)
		{
			if (FD_ISSET (j, &read_fd_set))
			{
				if(j == serverSD)
				{
					
					int cLen = sizeof(clientSocketAddress);
					clientSD = accept(serverSD, (SA *)&clientSocketAddress, &cLen);
					
					if(clientSD > 0)
					{
						
						for(;;)
						{
							memset(recvBuf, '\0', BUFSIZE);
							recv(clientSD, recvBuf, BUFSIZE, 0);
							
							printf("(From: %s:%d):%s\n", inet_ntoa(clientSocketAddress.sin_addr),ntohs(clientSocketAddress.sin_port),recvBuf);
							
							if(atoi(recvBuf) == 1)
							{
								
								printf("Unicast Client Identified \n");
								while(1)
								{
									memset(recvBuf, '\0', BUFSIZE);
									bytes = recv(clientSD, recvBuf, BUFSIZE, 0);
									
									if(bytes > 0)
									{
										int track = count[0].uClientsCount;
										strcpy(uList[track].username,recvBuf);
										strcpy(uList[track].ip,inet_ntoa(clientSocketAddress.sin_addr));
										uList[track].port = ntohs(clientSocketAddress.sin_port);
										count[0].uClientsCount = ++track;

										usleep(500);
										
										dataTransfer(clientSD,0);
									}
									break;
								}							
								break;
							}
							else if(atoi(recvBuf) == 0)
							{
								
								printf("Muticast Client Identified \n");
								while(1)
								{
									memset(recvBuf, '\0', BUFSIZE);
									bytes = recv(clientSD, recvBuf, BUFSIZE, 0);

									if(bytes > 0)
									{
										int track = count[0].mClientsCount;
										strcpy(mList[track].username,recvBuf);
										strcpy(mList[track].ip,inet_ntoa(clientSocketAddress.sin_addr));
										mList[track].port = ntohs(clientSocketAddress.sin_port);
										count[0].mClientsCount=++track;
								
										usleep(500);
										
										dataTransfer(clientSD,1);
									}
									break;
								}
								break;
							}
						}
					}
				}
				else if(j == serverUDPSD)
				{
					
					int             bytes = 0;
					char            recvBuf[BUFSIZE];
					int fromlength;
					
					memset(recvBuf, '\0', BUFSIZE);
					fromlength=sizeof(clientInSocketAddress);
					bytes = recvfrom(serverUDPSD, recvBuf, BUFSIZE, 0,(SA*)&clientInSocketAddress,&fromlength);
					if (bytes < 0) 
					{
						printf("error in reading from multicast socket\n");
						exit(-1);
					} 
					else if (bytes > 0)
					{	
						 char msg[1024] = "";
						 sprintf(msg,"%s",recvBuf);
						 sendMTOU(msg);	
					}
				}
				else if(j == serverUNIUDPSD)
				{
					
					int  ubytes = 0;
					char urecvBuf[BUFSIZE];
					int ufromlength;
			
					ufromlength=sizeof(clientInSocketAddress);
					
					memset(urecvBuf, '\0', BUFSIZE);
					
					ubytes = recvfrom(serverUNIUDPSD, urecvBuf, BUFSIZE, 0,(SA*)&clientInSocketAddress,&ufromlength);

					if (ubytes < 0) 
					{
						printf("error in reading from Unicast socket\n");
						exit(-1);
					} 
					else if (ubytes > 0)
					{	
						if(strncmp(L,urecvBuf,sizeof(L)) == 0)
						{
							char msg[1024];
							
							clientUnicastUDPSocketAddress.sin_family = AF_INET;
							clientUnicastUDPSocketAddress.sin_port = clientInSocketAddress.sin_port;
							clientUnicastUDPSocketAddress.sin_addr.s_addr = clientInSocketAddress.sin_addr.s_addr;
				
							if(count[0].uClientsCount > 0)
							{
								sprintf(msg," \nConnected Unicast Clients : \n ---------------------------");
								if ((c=sendto(serverUNIUDPSD, msg, strlen(msg), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
								{
									perror("error in UList sendto \n");
									exit(-1);
								}
								
								for(i=0;i< count[0].uClientsCount ;i++)
								{
									if("" != (uList[i].username))
									{
										sprintf(msg,"%s @ %s  :  %d \n",uList[i].username,uList[i].ip,uList[i].port);
										if ((c=sendto(serverUNIUDPSD, msg, strlen(msg), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
										{
											perror("error in UList sendto \n");
											exit(-1);
										}
									}
								}
							}
							else
							{
								sprintf(msg," No Connected Unicast Clients  ");
								if ((c=sendto(serverUNIUDPSD, msg, strlen(msg), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
								{
									perror("error in 1 UList sendto \n");
									exit(-1);
								}
							}
	
							if(count[0].mClientsCount > 0)
							{
								sprintf(msg," Connected Multicast Clients : \n ------------------------- ");
								if ((c=sendto(serverUNIUDPSD, msg, strlen(msg), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
								{
									perror("error in UList sendto \n");
									exit(-1);
								}
								for(i=0;i< count[0].mClientsCount ;i++)
								{
									if("" != (mList[i].username))
									{
										sprintf(msg,"%s @ %s  :  %d \n",mList[i].username,mList[i].ip,mList[i].port);
										if ((c=sendto(serverUNIUDPSD, msg, strlen(msg), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
										{
											perror("error in MList sendto \n");
											exit(-1);
										}
									}
								}
								
							}
							else
							{
								sprintf(msg," No Connected Multicast Clients ");
								if ((c=sendto(serverUNIUDPSD, msg, strlen(msg), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
								{
									perror("error in 1 MList  sendto \n");
									exit(-1);
								}
							}
							
							if ((c=sendto(serverUNIUDPSD, L, strlen(L), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
							{
								perror("error in UList sendto \n");
								exit(-1);
							}
						}
						else if(strncmp(E,urecvBuf,6)==0)
						{
							char msg[1024];
							
							
							clientUnicastUDPSocketAddress.sin_family = AF_INET;
							clientUnicastUDPSocketAddress.sin_port = clientInSocketAddress.sin_port;
							clientUnicastUDPSocketAddress.sin_addr.s_addr = clientInSocketAddress.sin_addr.s_addr;
							
							if(count[0].uClientsCount > 0)
							{
								for(i=0;i< count[0].uClientsCount ;i++)
								{
									
									if( (strcmp(inet_ntoa(clientInSocketAddress.sin_addr),uList[i].ip)==0) && ntohs(clientInSocketAddress.sin_port) == uList[i].port)
									{
										if(count[0].uClientsCount == 1)
										{
											uList[0] = EmptyStruct;
										}
										else if(count[0].uClientsCount-1 == i)
										{
											uList[i] = EmptyStruct;
										}
										else
										{
											for(c = i; c < count[0].uClientsCount-1;c++)
											{
												uList[c] = uList[c+1];
											}
										}
										count[0].uClientsCount--;
									}			
								}
								if ((c=sendto(serverUNIUDPSD, E, strlen(E), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
								{
									perror("error in UList E sendto \n");
									exit(-1);
								}
							}
	
							if(count[0].mClientsCount > 0)
							{			
								for(i=0;i< count[0].mClientsCount ;i++)
								{
									if( (strcmp(inet_ntoa(clientInSocketAddress.sin_addr),mList[i].ip)==0) && ntohs(clientInSocketAddress.sin_port) == mList[i].port)
									{
										if(count[0].mClientsCount == 1)
										{
											mList[1] = EmptyStruct;
										}
										else if(count[0].mClientsCount-1 == i)
										{
											mList[i] = EmptyStruct;
										}
										else
										{
											for(c = i; c < count[0].mClientsCount-1;c++)
											{
												mList[c] = mList[c+1];
											}
										}
										count[0].mClientsCount--;
									}		
								}
								if ((c=sendto(serverUNIUDPSD, E, strlen(E), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0) 
								{
									perror("error in MList E sendto \n");
									exit(-1);
								}
							}
							
							printf("Client at %s : %d Left the group.\n",inet_ntoa(clientInSocketAddress.sin_addr),ntohs(clientInSocketAddress.sin_port));
							printf("Updated Client List : \n");
							printLists();
						}
						else
						{
							char msg[1000] = "";
							sprintf(msg,"%s", urecvBuf);
							sendUTOM(msg);
						}
					}
				}
			}
		}
	}
}





void sendUTOM(char* msg)
{
	if ((c=sendto(serverUDPSD, msg, strlen(msg), 0, (SA *) & GroupAddress, sizeof(GroupAddress))) < 0) 
	{
		printf("error in UTOM sendto \n");
		exit(-1);
	}
}


void sendMTOU(char* msg)
{
	if(count[0].uClientsCount > 0)
	{
		for(i=0;i<count[0].uClientsCount;i++)
		{
		 	
			clientUnicastUDPSocketAddress.sin_family = AF_INET;
			clientUnicastUDPSocketAddress.sin_port = htons(uList[i].port);
			clientUnicastUDPSocketAddress.sin_addr.s_addr = inet_addr(uList[i].ip);
			
			if ((c=sendto(serverUNIUDPSD, msg, strlen(msg), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0)
			{
				perror("error in MTOU sendto \n");
				exit(-1);
			}
		}
	} 
	else
	{
		printf("NO UNICAST-MULTICAST CONNECTION......\n");
	}
}


void dataTransfer(int clientSD, int flag)
{
	
	if(flag)
	{
		if (send(clientSD, MIP, strlen(MIP), 0) <0 )
        perror("sending stream message\n");
        usleep(500);
	}
	
	
	if (send(clientSD, mport, strlen(mport), 0) <0 )
        perror("sending stream message\n");
   usleep(500);
     
     
    if (send(clientSD, L, 7, 0) <0 )
        perror("sending stream message\n");
    usleep(500);
    
       
    if (send(clientSD, E, 7, 0) <0 )
        perror("sending stream message\n");
    usleep(500);
}


void interruptHandle( int sig ) 
{
	printLists();
	signal( sig, interruptHandle );
	startSelect();
}

void PIPEhandle(int sig)
{

}




void printLists()
{	
	if(count[0].uClientsCount > 0)
	{
		printf("\nUnicast Clients : \n");
		printf("------------------------------\n");
		for(i=0;i< count[0].uClientsCount ;i++)
		{
			if("" != (uList[i].username))
			{
				printf("%s @ %s  :  %d \n",uList[i].username,uList[i].ip,uList[i].port);
			}
			fflush(stdout);
		}
	}
	else
	{
		printf("\nNo Connected Unicast Clients. \n");
	}

	if(count[0].mClientsCount > 0)
	{
		printf("\n Multicast Clients : \n");
		printf("-------------------------------\n");
		for(i=0;i< count[0].mClientsCount ;i++)
		{
			if("" != (mList[i].username))
			{
				
				printf("%s @ %s  :  %d \n",mList[i].username,mList[i].ip,mList[i].port);
			}
		}
	}
	else
	{
		printf("No Connected Multicast Clients. \n");
	}

	printf("\n");
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

void quitHandle(int sig)
{
	if(count[0].uClientsCount > 0)
	{
		for(i=0;i<count[0].uClientsCount;i++)
		{
		 	
			clientUnicastUDPSocketAddress.sin_family = AF_INET;
			clientUnicastUDPSocketAddress.sin_port = htons(uList[i].port);
			clientUnicastUDPSocketAddress.sin_addr.s_addr = inet_addr(uList[i].ip);
			
			if ((c=sendto(serverUNIUDPSD, E, strlen(E), 0, (SA *) & clientUnicastUDPSocketAddress, sizeof(clientUnicastUDPSocketAddress))) < 0)
			{
				perror("error in MTOU sendto \n");
				exit(-1);
			}
		}
	} 
	
	
	
	if ((c=sendto(serverUDPSD, E, strlen(E), 0, (SA *) & GroupAddress, sizeof(GroupAddress))) < 0) 
	{
		printf("error in UTOM sendto \n");
		exit(-1);
	}
	sleep(1);

	shmdt(uList);
	shmctl(seg1, IPC_RMID, NULL);
	shmdt(mList);
	shmctl(seg2, IPC_RMID, NULL);
	shmdt(count);
	shmctl(seg3, IPC_RMID, NULL);
	printf("Termination Complete");
	
	exit(-1);
}



void groupExit(int recvSock, char *group)
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


void setTTLvalue(int s, u_char * ttl_value)
{
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char *) ttl_value,
		       sizeof(u_char)) == -1) {
		printf("error in setting loopback value\n");
	}
}



void setLoopback(int s, u_char loop)
{
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &loop,
		       sizeof(u_char)) == -1) {
		printf("error in disabling loopback\n");
	}
}



void reusePort(int s)
{
	int  one = 1;

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one)) == -1) {
		printf("error in setsockopt,SO_REUSEPORT \n");
		exit(-1);
	}
}





