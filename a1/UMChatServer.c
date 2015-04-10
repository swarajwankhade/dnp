#include "def.h"
void RandGen();

#define MAXHOSTNAME 80
void reusePort(int sock);   

char MIP[100]="224.0.0.1";
char MPort[10];
char L[10];
char E[10];
char *MulticastIPAddress;
u_char TimeToLive;
u_char loop;
int UDPPort;
int TCPPort;

void INThandler( int sig );
void PIPEhandler(int sig);
void QUIThandler(int sig);

int i;


int cli_num = 3;


main( argc, argv )
int argc;
char *argv[];
{
  int   sd, psd;
  struct   sockaddr_in server;
  struct  hostent *hp, *gethostbyname();
  struct  servent *sp;
  struct sockaddr_in from;
  struct sockaddr_in LocalHostR;
	struct sockaddr_in LocalHostS;
  int fromlen;
  int length;
  char ThisHost[80];
  int pn;
  int childpid;
  int UDPsocketR;
  int UDPsocketS;

  signal( SIGINT, INThandler );
  signal( SIGQUIT, QUIThandler );


  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  pn = htons(atoi(argv[1])); 
  server.sin_port =  pn;

  sd = socket (AF_INET,SOCK_STREAM,IPPROTO_TCP); 
  reusePort(sd);
  if ( bind( sd, (SA *) &server, sizeof(server) ) < 0 ) {
    close(sd);
    perror("binding name to stream socket");
    exit(-1);
  }

  length = sizeof(server);
  if ( getsockname (sd, (SA *)&server,&length) ) {
    perror("getting socket name");
    exit(0);
  }
  printf("\nServer Port:\t %d\n", ntohs(server.sin_port));
  //UPD Socket creation
  MulticastIPAddress = MIP;
	UDPPort = htons(atoi( MPort ) );

	LocalHostR.sin_family = AF_INET;
	LocalHostR.sin_port = UDPPort;
	LocalHostR.sin_addr.s_addr = htonl(INADDR_ANY);


	if ((UDPsocketR = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("can't create UDP socket R: \n");
		exit(-1);
	}
	reusePort(UDPsocketR);

        if (bind(UDPsocketR, (SA *) & LocalHostR, sizeof(LocalHostR)) < 0) {
                printf("error in bind R\n");
                exit(-1);
        }


	LocalHostS.sin_family = AF_INET;
	LocalHostS.sin_port =  htons(TCPPort);
	LocalHostS.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((UDPsocketS = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("can't create UDP socket S: \n");
		exit(-1);
	}

	if (bind(UDPsocketS, (SA *) & LocalHostS, sizeof(LocalHostS)) < 0) {
		printf("error in bind S\n");
		exit(-1);
	}
	TimeToLive = 2;
	setTTLvalue(UDPsocketR, &TimeToLive);

	loop = 1;		/* enable loopback */
	setLoopback(UDPsocketR, loop);

	joinGroup(UDPsocketR, MIP);

        
	signal( SIGQUIT, QUIThandler );
	listen(sd,4);
    fromlen = sizeof(from);
    RandGen();

	if (fork() == 0)
	{
    sendMessage(0, UDPsocketS);
	}
	else
	{
	
	
    for(;;){
    psd  = accept(sd, (SA *)&from, &fromlen);
	getMessage(UDPsocketR, 1);
	/*if( (rc=recv(sd, MIP, sizeof(MIP), 0)) < 0){
      		perror("receiving MIP  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		MIP[rc]='\0';
      		//printf("\nReceived Multicast IP:\t %s", MIP);
    	}

    	if( (rc=recv(sd, MPort, sizeof(MPort), 0)) < 0){
      		perror("receiving MPort  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		MPort[rc]='\0';
      		//printf("Received Multicast Port:\t %s\n", MPort);
    	}
		if( (rc=recv(sd, L, sizeof(L), 0)) < 0){
      		perror("receiving MPort  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		L[rc]='\0';
      		//printf("Received Multicast Port:\t %s\n", MPort);
    	}
		if( (rc=recv(sd, E, sizeof(E), 0)) < 0){
      		perror("receiving MPort  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		E[rc]='\0';
      		//printf("Received Multicast Port:\t %s\n", MPort);
    	}*/
	
    if (psd > 1) {
    	cli_num++;
    	printf("\nNew ChatClient:\t (%s:%d)\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));
		
    	if (send(psd, MIP, strlen(MIP), 0) <0 )
        	perror("sending MIP message");
	
    	if (send(psd, MPort, strlen(MPort), 0) <0 )
       		 perror("sending MPort message");
			 
		if (send(psd, L, strlen(L), 0) <0 )
       		 perror("sending L message");
			 
		if (send(psd, E, strlen(E), 0) <0 )
       		 perror("sending E message");		 
    }

  }
	}
}

void reusePort(int s)
{
  int one=1;

  if ( setsockopt(s,SOL_SOCKET,SO_REUSEADDR,(char *) &one,sizeof(one)) == -1 )
  {
    printf("error in setsockopt,SO_REUSEPORT \n");
    exit(-1);
  }
}      

void RandGen(){
        int no = time(NULL);
	int randomPort,randomL,randomE;
	int p1, p2, p3, p4;
        srand(no);
        p1 = rand() % 15 + 224;
        p2 = rand() % 255;
        p3 = rand() % 255;
        p4 = rand() % 255;
	printf("Multicast IP:\t %d.%d.%d.%d\n",p1,p2,p3,p4);
	sprintf(MIP, "%d.%d.%d.%d\n",p1,p2,p3,p4);
        randomPort = rand() % 1000 + 10000;
        printf("Multicast Port:\t %d\n", randomPort);
        sprintf(MPort,"%d", randomPort);
		randomL = rand() % 1000000 + 1000000;
        printf("L:\t %d\n", randomL);
        sprintf(L,"%d", randomL); 
		randomE = rand() % 1000000 + 1000000;
        printf("E:\t %d\n", randomE);
        sprintf(E,"%d", randomE); 
}


void INThandler( int sig ) {
        struct sockaddr_in client;
        int clilen, port;
        char str[128];
        int dia;

        clilen = sizeof (client);

        signal(SIGINT, INThandler);
        signal(SIGPIPE, PIPEhandler);

	printf("\n\nChatClients List:\n");
	printf("--------------------\n");
        for ( i = 4; i <= cli_num; i++ ) {
                dia = write(i, "p", 1);
                usleep(50000);
                dia = write(i, "p", 1);
                usleep(50000);
                if(dia){
                        if((getpeername(i, (SA *)&client, &clilen))==0) {
                                inet_ntop(AF_INET, &(client.sin_addr), str, sizeof str);
                                port = ntohs(client.sin_port);
                                printf ( "(%s:%d)\n", str, port );
                        }
                }
        }
	printf("--------------------\n");

}
void joinGroup(int s, char *group)
{
	struct sockaddr_in groupStruct;
	struct ip_mreq  mreq;	/* multicast group info structure */

	if ((groupStruct.sin_addr.s_addr = inet_addr(group)) == -1)
		printf("error in inet_addr\n");

	/* check if group address is indeed a Class D address */
	mreq.imr_multiaddr = groupStruct.sin_addr;
	mreq.imr_interface.s_addr = INADDR_ANY;

	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &mreq,
		       sizeof(mreq)) == -1) {
		printf("error in joining group \n");
		exit(-1);
	}
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
void sendMessage(int inSock, int outSock)
{
	char            sendBuf[MAX_LEN];
	int             bytes = 0;
	struct sockaddr_in GroupAddress;

	GroupAddress.sin_family = AF_INET;
	GroupAddress.sin_port = UDPPort;
	GroupAddress.sin_addr.s_addr = inet_addr(MulticastIPAddress);

	//printf("\nType messages to be sent to the group followed by ENTER\n");
	//printf("Type CTRL-D to Quit\n");

	for (;;) {
		memset(sendBuf, '\0', MAX_LEN);
		bytes = read(inSock, sendBuf, MAX_LEN);
		if (bytes < 0) {
			printf("error in reading from STDIN \n");
			exit(-1);
		} else if (bytes == 0) {
			kill(getppid(), SIGQUIT);
			exit(0);
		} else if (sendto(outSock, sendBuf, bytes, 0, (SA *) & GroupAddress, sizeof(GroupAddress)) < 0) {
			perror("error in sendto \n");
			exit(-1);
		}
	}
}

void PIPEhandler(int sig){
        signal(SIGPIPE, PIPEhandler);
        close(i);
}

void QUIThandler(int sig){
	printf("\nChatServer Terminated\n");
	exit(0);
}

void getMessage(int inSock, int outSock)
{
	int             bytes = 0;
	char            recvBuf[MAX_LEN];
	char            sender[100];
	struct sockaddr_in from;
	int fromlen;


	for (;;) {
		memset(recvBuf, '\0', MAX_LEN);
		fromlen = sizeof(from);
		bytes = recvfrom(inSock, recvBuf, MAX_LEN, 0, (SA *) &from, &fromlen)  ;
		if (bytes < 0) {
			printf("error in reading from multicast socket\n");
			exit(-1);
		} 
/*
                else if (bytes == 0)
			printf("zero bytes read\n");
*/
		else {		/* print the message to STDOUT */
         		sprintf( sender, "(%s:%d): ", inet_ntoa(from.sin_addr), ntohs(from.sin_port));

			if (write(outSock, sender, strlen(sender)) < 0) {
				printf("error in write to STDOUT \n");
				exit(-1);
			}
			if (write(outSock, recvBuf, bytes) < 0) {
				printf("error in write to STDOUT \n");
				exit(-1);
			}
		}
	}
}
