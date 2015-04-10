#include "def.h"
#define MAXLINE 1024
#define U 1
#define M 0
#define S 2

void SendMessage(int inSock, int outSock);
void RecvMessage(int inSock, int outSock);
void QUIThandler(int sig);
void INThandler(int sig);
void selectHandle();  

int SCTPPort;     
int  UDPPort;
int  TCPPort;
char *MulticastIPAddress;

u_char TimeToLive;
u_char loop;

char MIP[100];
char ServerAddr[100];
char MPort[10];
char RandomCODE1[10];
char RandomCODE2[10];
char *LISTmsg = RandomCODE1;
char *ENDmsg = RandomCODE2;
char UChat[2]="1";
char MChat[2]="0";
int TYPE;


struct   hostent *hp, *gethostbyname();
struct   sockaddr_in TCPserver;
struct   sockaddr_in UDPserver;

struct   sockaddr_in GroupAddress;
struct   sockaddr_in SCTP_Qserver;    
struct   sockaddr_in from;
struct   sockaddr_in LocalHostUS;    
struct   sockaddr_in client;
struct	 sockaddr_in LocalHostUR;    
struct   sockaddr_in addr;
struct   sockaddr_in LocalHostSCTP_Q;    

char *EchoTest="!E!T!";
char record[100];
char *username;

int UDPsocket;
int length;
int fromlen;
int bytes;
int	TCP_socket, SCTPsocket, SCTP_Qsocket;    
int MCASTsocket;
int rc;


fd_set full_fd, read_fd;  

main(int argc, char *argv[])
{
	FD_SET(0, &full_fd);    
	
	if (argc != 4) {
		printf("Usage: ChatClient <host> <port> <m|u|s>\n");
		printf("e.g.: ChatClient atria 10123 u \n");
		exit(1);
	}

        if ((username = getlogin()) == NULL) perror("Who are you?\n");

  	if  ( (hp = gethostbyname(argv[1])) == NULL ) {
    		addr.sin_addr.s_addr = inet_addr(argv[1]);
    		if ((hp = gethostbyaddr((char *) &addr.sin_addr.s_addr,
       		sizeof(addr.sin_addr.s_addr),AF_INET)) == NULL) {
      			fprintf(stderr, "Can't find host %s\n", argv[1]);
      			exit(-1);
    		}
  	}
  	printf("\n UMChatServer --> %s\n", hp->h_name);

  	bcopy ( hp->h_addr, &(TCPserver.sin_addr), hp->h_length);
  	bcopy ( hp->h_addr, &(UDPserver.sin_addr), hp->h_length);
	bcopy ( hp->h_addr, &(SCTP_Qserver.sin_addr), hp->h_length);
  	printf(" UMChatServer IP: %s \n", inet_ntoa(TCPserver.sin_addr));

	SCTP_Qserver.sin_family = AF_INET; 
  	TCPserver.sin_family = AF_INET;
  	UDPserver.sin_family = AF_INET;
  	TCPserver.sin_port = htons(atoi(argv[2]));

	
        if (strcmp(argv[3], "s") == 0) TYPE=S;
        if (strcmp(argv[3], "m") == 0) TYPE=M;
		if (strcmp(argv[3], "u") == 0) TYPE=U;	    

if (TYPE == S)	    
{
	SCTPsocket = socket (AF_INET,SOCK_STREAM,IPPROTO_SCTP);	
	
		if ( connect(SCTPsocket, (SA *) &TCPserver, sizeof(TCPserver)) < 0 ) {
				close(SCTPsocket);
				perror("connecting to UMChatServer socket");
				exit(0);
		}

			if ( send(SCTPsocket, username, strlen(username), 0) < 0) 
				perror("send username message");

		length = sizeof(client);
		if ( getsockname (SCTPsocket, (SA *)&client,&length) ) {
			perror("getting socket name");
			exit(0);
		}
			SCTPPort = ntohs(client.sin_port);	
}
else    
	{
		TCP_socket = socket (AF_INET,SOCK_STREAM,0);
		if ( connect(TCP_socket, (SA *) &TCPserver, sizeof(TCPserver)) < 0 ) {
				close(TCP_socket);
				perror("connecting to UMChatServer socket.....");
				exit(0);
		}

			if ( send(TCP_socket, username, strlen(username), 0) < 0) 
				perror("send username message");

		length = sizeof(client);
		if ( getsockname (TCP_socket, (SA *)&client,&length) ) {
			perror("getting socket name");
			exit(0);
		}
			TCPPort = ntohs(client.sin_port);
	}


	if (TYPE == M) 
	{
       		if ( send(TCP_socket, MChat, strlen(MChat), 0) < 0) 
			perror("send MChat  message");
	        if( (rc=recv(TCP_socket, MIP, sizeof(MIP), 0)) < 0) 
			perror("receiving MIP  message");
        	if (rc > 0)
			{
               		MIP[rc]='\0';
                	printf("THE MCAST IP --> %s", MIP);
        	}
    }
	if (TYPE == U)     
	{
		if ( send(TCP_socket, UChat, strlen(UChat), 0) < 0)  
			perror("send UChat  message");
    }
	if (TYPE == S)    
	{
		if( (rc=recv(SCTPsocket, MPort, sizeof(MPort), 0)) < 0) 	
			perror("receiving MPort message 01");
		if (rc > 0)
		{
			MPort[rc]='\0';
			printf(" THE MCAST Port --> %s\n", MPort);
		}
			strcat(record,inet_ntoa(SCTP_Qserver.sin_addr));
		strcat(record,":");
		strcat(record,MPort);
			sprintf(ServerAddr, "[%s] ", record);
		UDPPort = htons(atoi( MPort ) );
		SCTP_Qserver.sin_port = UDPPort;

			if( (rc=recv(SCTPsocket, RandomCODE1, sizeof(RandomCODE1), 0)) < 0) 
			perror("receiving RandomCODE1  message");
			if (rc > 0){
				RandomCODE1[rc]='\0';
				printf(" THE LIST Code --> %s\n", RandomCODE1);
			}

			if( (rc=recv(SCTPsocket, RandomCODE2, sizeof(RandomCODE2), 0)) < 0) 
			perror("receiving RandomCODE2  message");
			if (rc > 0){
				RandomCODE2[rc]='\0';
				printf("THE END Code --> %s\n\n", RandomCODE2);
			}		
	}
	else    
	{
		if( (rc=recv(TCP_socket, MPort, sizeof(MPort), 0)) < 0) 	
			perror("receiving MPort  message");
		if (rc > 0)
		{ 
			MPort[rc]='\0';
			printf(" THE MCAST Port --> %s\n", MPort);
		}
			strcat(record,inet_ntoa(UDPserver.sin_addr));
		strcat(record,":");
		strcat(record,MPort);
			sprintf(ServerAddr, "[%s] ", record);
		UDPPort = htons(atoi( MPort ) );
		UDPserver.sin_port = UDPPort;

			if( (rc=recv(TCP_socket, RandomCODE1, sizeof(RandomCODE1), 0)) < 0) 
			perror("receiving RandomCODE1  message");
			if (rc > 0){
				RandomCODE1[rc]='\0';
				printf(" THE LIST Code --> %s\n", RandomCODE1);
			}

			if( (rc=recv(TCP_socket, RandomCODE2, sizeof(RandomCODE2), 0)) < 0) 
			perror("receiving RandomCODE2  message");
			if (rc > 0){
				RandomCODE2[rc]='\0';
				printf(" THE END Code --> %s\n\n", RandomCODE2);
			}
	}
		
	LocalHostUS.sin_family = AF_INET;
	LocalHostUS.sin_port =  htons(TCPPort);
	LocalHostUS.sin_addr.s_addr = htonl(INADDR_ANY);
	
	    
	LocalHostSCTP_Q.sin_family = AF_INET;
	LocalHostSCTP_Q.sin_port =  htons(client.sin_port)+1;
	LocalHostSCTP_Q.sin_addr.s_addr = htonl(INADDR_ANY);	

		    
        MulticastIPAddress = MIP;
        UDPPort = htons(atoi( MPort ) );
        LocalHostUR.sin_family = AF_INET;
        LocalHostUR.sin_port = UDPPort;
        LocalHostUR.sin_addr.s_addr = htonl(INADDR_ANY);

        GroupAddress.sin_family = AF_INET;
        GroupAddress.sin_port = UDPPort;
        GroupAddress.sin_addr.s_addr = inet_addr(MulticastIPAddress);

	if ((UDPsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
		perror("can't create UDP socket S: \n");
	if (bind(UDPsocket, (SA *) & LocalHostUS, sizeof(LocalHostUS)) < 0) printf("error in bind UDP S\n");
	
	FD_SET(UDPsocket, &full_fd);
	
	    
	if ((SCTP_Qsocket = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0) 
		perror("can't create SCTP-Q socket S: \n");
		
	reusePort(SCTP_Qsocket);		
	
	if (bind(SCTP_Qsocket, (SA *) & LocalHostSCTP_Q, sizeof(LocalHostSCTP_Q)) < 0) printf("error in bind SCTP_Q S\n");
	
	FD_SET(SCTP_Qsocket, &full_fd);
	
	
	if (TYPE == M) {
        	if ((MCASTsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
			perror("can't create UDP MCASTsocket R: \n");
        	reusePort(MCASTsocket);
        	if (bind(MCASTsocket, (SA *) & LocalHostUR, sizeof(LocalHostUR)) < 0)  
			perror("error in bind R\n");
        	joinGroup(MCASTsocket, MIP);
			
			FD_SET(MCASTsocket, &full_fd); 
	}
        TimeToLive = 2;
        setTTLvalue(UDPsocket, &TimeToLive);
        loop = 1;               
        setLoopback(UDPsocket, loop);
	 

	signal( SIGQUIT, QUIThandler );
	signal( SIGINT, INThandler);
	

        char            recvBuf[MAXLINE];
        int             bytes = 0;

        if (TYPE == U) {
		if (sendto(UDPsocket,EchoTest,strlen(EchoTest),0,(SA *)&UDPserver,sizeof(UDPserver)) < 0) 
			perror("error in sendto EchoTest UDPsocket \n");
	}
	
	    
        if (TYPE == S) {
		char EchoTest[100];
		strcpy(EchoTest,"WELCOME, New SCTP: ");
		
		strcat(EchoTest,username);
		strcat(EchoTest,"\n");
		
		
		if (sendto(SCTP_Qsocket,EchoTest,strlen(EchoTest),0,(SA *)&SCTP_Qserver,sizeof(SCTP_Qserver)) < 0) 
			perror("error in send to EchoTest SCTP_Qsocket \n");
	}	
        if (TYPE == M) {
		 if (sendto(UDPsocket,EchoTest,strlen(EchoTest),0,(SA *)&UDPserver,sizeof(UDPserver)) < 0) 
			 perror("error in send to EchoTest UDPsocket \n");
		if (sendto(MCASTsocket,EchoTest,strlen(EchoTest),0,(SA *)&UDPserver,sizeof(UDPserver)) < 0) 
			perror("error in send to EchoTest MCASTsocket \n");
                 if ( (bytes = recvfrom(UDPsocket, recvBuf, MAXLINE, 0, (SA *) &from, &fromlen) ) < 0)   
			 perror("error in rec from EchoTest UDPsocket \n");
	
	}	
	        sleep(1);
	
	selectHandle();
}




void selectHandle()
{
	while(1)
	{
		read_fd = full_fd;
		memcpy(&read_fd, &full_fd, sizeof(full_fd));
		
		select(FD_SETSIZE, &read_fd, NULL, NULL, NULL);
		
		if (FD_ISSET(0, &read_fd) && (TYPE != S) )
			SendMessage(0, UDPsocket);
		else if (FD_ISSET(0, &read_fd) && (TYPE == S) )
			SendMessage(0, SCTP_Qsocket);
		else if (FD_ISSET(UDPsocket, &read_fd) && (TYPE == U) )
			RecvMessage(UDPsocket, 1);
		else if (FD_ISSET(SCTP_Qsocket, &read_fd) && (TYPE == S) )
			RecvMessage(SCTP_Qsocket, 1);			
		else
			RecvMessage(MCASTsocket, 1);
	}

}

void
SendMessage(int inSock, int outSock)
{
        char            sendBuf[MAXLINE];
		int             bytes = 0;

        printf("\n Enter messages (To END press CTRL-D)\n");
        //for (;;)   
		{
                memset(sendBuf, '\0', MAXLINE);
                bytes = read(inSock, sendBuf, MAXLINE);
                if (bytes < 0) perror("error in reading from STDIN \n");
                if (bytes == 0) {
			kill(getppid(), SIGQUIT);
			exit(0);
		}
		
		if (TYPE == S) {
               		if (sendto(outSock,sendBuf,bytes,0,(SA *)&SCTP_Qserver,sizeof(SCTP_Qserver)) < 0)
                                perror("error in sendto (SCTPsocket) \n");								
		}
		if (TYPE == U) {
                	if (sendto(outSock,sendBuf,bytes,0,(SA *)&UDPserver,sizeof(UDPserver)) < 0) 
                                perror("error in sendto (UDPsocket) \n");
		}
		if (TYPE == M) {
               		if (sendto(outSock,sendBuf,bytes,0,(SA *)&GroupAddress,sizeof(GroupAddress)) < 0)
                                perror("error in sendto (UDPsocket) \n");
		}
		    		
		}
}

void
RecvMessage(int inSock, int outSock)
{
        int             bytes = 0;
        char            recvBuf[MAXLINE];
        char            sender[100];


        //for (;;) 
		{
                memset(recvBuf, '\0', MAXLINE);
                fromlen = sizeof(from);
                bytes = recvfrom(inSock, recvBuf, MAXLINE, 0, (SA *) &from, &fromlen)  ;
                if (bytes < 0) {
                        printf("error in reading from UDP socket\n");
                        exit(-1);
                }
                else {
			if (strcmp(recvBuf,EchoTest) == 0) {
                             return;
                        }
                        sprintf( sender, "[%s:%d] ", inet_ntoa(from.sin_addr), ntohs(from.sin_port));
                        if (strcmp(sender,ServerAddr)!=0) {
                                if (write(outSock, sender, strlen(sender)) < 0) {
                                        printf("error in write to STDOUT \n");
                                        exit(-1);
                                }
                        }
                        if (write(outSock, recvBuf, bytes) < 0) {
                                printf("error in write to STDOUT \n");
                                exit(-1);
                        }
                        fflush(stdout);
                        usleep(1000);
                        if (strcmp(recvBuf,ENDmsg) == 0){
                                printf(" :...Recieved (ENDmsg)...\n");
                                kill(getppid(), SIGQUIT);
                                exit(0);
                        }
                }
        }
}

void 
joinGroup(int s, char *group)
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

void INThandler(int sig){
        char LISTING[MAXLINE];
	signal( SIGINT, INThandler );
        memset(LISTING, '\0', MAXLINE);

	printf("\n\nClients LIST --> \n");

        if (TYPE == U) 
		{
			if (sendto(UDPsocket,LISTmsg,strlen(LISTmsg),0,(SA *)&UDPserver,sizeof(UDPserver)) < 0) 
				perror("error in sendto LIST \n");
		}
		if (TYPE == S)    
		{
			if (sendto(SCTP_Qsocket,LISTmsg,strlen(LISTmsg),0,(SA *)&SCTP_Qserver,sizeof(SCTP_Qserver)) < 0)
				perror("error in sendto ENDmsg \n");			
		}
        if (TYPE == M) {
		if (sendto(UDPsocket,LISTmsg,strlen(LISTmsg),0,(SA *)&UDPserver,sizeof(UDPserver)) < 0) 
			perror("error in sendto LIST \n");
                if ( (bytes = recvfrom(UDPsocket, LISTING, MAXLINE, 0, (SA *) &from, &fromlen) ) < 0)   
			perror("error in recfrom EchoTest UDPsocket \n");
		printf("%s\n", LISTING);
	}
	selectHandle();   
}

void 
reusePort(int s)
{
	int             one = 1;

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one)) == -1) {
		printf("error in setsockopt,SO_REUSEPORT \n");
		exit(-1);
	}
}

void QUIThandler(int sig)
{
    
	if (TYPE == S)
	{	if (sendto(SCTP_Qsocket,ENDmsg,strlen(ENDmsg),0,(SA *)&SCTP_Qserver,sizeof(SCTP_Qserver)) < 0)
			perror("error in sendto ENDmsg \n");	
	}			
	else    
	{
		if (sendto(UDPsocket,ENDmsg,strlen(ENDmsg),0,(SA *)&UDPserver,sizeof(UDPserver)) < 0) 
			perror("error in sendto ENDmsg \n");
	}
	usleep(1000);
        
        printf("\n... ChatClient Termination Complete ...\n");
        exit(0);
}

void 
setTTLvalue(int s, u_char * ttl_value)
{
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char *) ttl_value,
		       sizeof(u_char)) == -1) {
		printf("error in setting loopback value\n");
	}
}

void 
setLoopback(int s, u_char loop)
{
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &loop,
		       sizeof(u_char)) == -1) {
		printf("error in disabling loopback\n");
	}
}





