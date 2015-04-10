#include "def.h"

void            sendMessage(int inSock, int outSock);
void            getMessage(int inSock, int outSock);

int             UDPPort;
int             TCPPort;
char           *MulticastIPAddress;

u_char          TimeToLive;
u_char          loop;

char MIP[100];
char MPort[10];
char listNumber[10];
char exitNumber[10];
char userBuf[30];
char buf[10];
char check;

char *userName;

void QUIThandler(int sig);

	struct sockaddr_in LocalHostR;
	struct sockaddr_in LocalHostS;
	int             UDPsocketR;
	int             UDPsocketS;
    int rc;
    int sd;

    struct   sockaddr_in server;
    struct   sockaddr_in client;
    struct   sockaddr_in addr;
    struct  hostent *hp, *gethostbyname();
	int length;


main(int argc, char *argv[])
{

	//checking the arguments 
	if (argc != 4 || argc == 2 ) 
	{
		printf("Usage: ChatClient <shost> <sport> <u/m>\n");
		printf("e.g.: ChatClient atria 10123 u \n");
		exit(1);
	}
  	if  ( (hp = gethostbyname(argv[1])) == NULL ) {
    		addr.sin_addr.s_addr = inet_addr(argv[1]);
    		if ((hp = gethostbyaddr((char *) &addr.sin_addr.s_addr,
       		sizeof(addr.sin_addr.s_addr),AF_INET)) == NULL) {
      			fprintf(stderr, "Can't find host %s\n", argv[1]);
      			exit(-1);
    		}
  	}
  	printf("\n>> UMChatServer :\t %s\n", hp->h_name);
  	bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
  	printf(">> UMChatServer IP:\t %s \n", inet_ntoa(server.sin_addr));

  	server.sin_family = AF_INET;
  	server.sin_port = htons(atoi(argv[2]));
  	sd = socket (AF_INET,SOCK_STREAM,0);


	if ( connect(sd, (SA *) &server, sizeof(server)) < 0 ) {
    		close(sd);
    		perror("connecting to server socket");
    		exit(0);
	}


	length = sizeof(client);
	if ( getsockname (sd, (SA *)&client,&length) ) {
		perror("getting socket name");
		exit(0);
  	}
    
	TCPPort = ntohs(client.sin_port);
	userName=getlogin();
	
	if(strcmp(argv[3],"m" )== 0)	
	{
		//send logic
		char octet='0';
		
		sprintf(buf, "%c", octet);
		sprintf(userBuf, "%s", userName);
		if (send(sd,userBuf, sizeof(userBuf), 0) <0 )
			perror("sending user name");
		usleep(1000);
		if (send(sd,buf , sizeof(buf), 0) <0 )
			perror("sending MIP message");
			
		
		
	}
	if(strcmp(argv[3],"u" )== 0)	
	{
		//send logic
		char octet='1';
		sprintf(buf, "%c", octet);
		sprintf(userBuf, "%s", userName);
		if (send(sd,userBuf, sizeof(userBuf), 0) <0 )
			perror("sending user name");
			usleep(1000);
		if (send(sd,buf , sizeof(buf), 0) <0 )
			perror("sending MIP message");
		
		
	
	}
	

	printf(">> ChatClient TCP Port:\t %d\n", TCPPort);

	//multicast
	if(strcmp(argv[3],"m" )== 0)
	{
    	if( (rc=recv(sd, MIP, sizeof(MIP), 0)) < 0){
      		perror("receiving MIP  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		MIP[rc]='\0';
      		printf(">> MCAST IP:\t %s", MIP);
    	}

    	if( (rc=recv(sd, MPort, sizeof(MPort), 0)) < 0){
      		perror("receiving MPort  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		MPort[rc]='\0';
      		printf(">> MCAST Port:\t %s\n", MPort);
    	}
		
		if( (rc=recv(sd, listNumber, sizeof(listNumber), 0)) < 0){
      		perror("receiving list  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		listNumber[rc]='\0';
      		printf(">> LIST Code:\t %s\n", listNumber);
    	}
		
		if( (rc=recv(sd, exitNumber, sizeof(exitNumber), 0)) < 0){
      		perror("receiving exit  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		exitNumber[rc]='\0';
      		printf(">> END Code:\t %s\n", exitNumber);
    	}
		//multicast
		
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

		if (fork() == 0)
		sendMessage(0, UDPsocketS);
		getMessage(UDPsocketR, 1);
		
		
	}
	//unicast
	if(strcmp(argv[3],"u" )== 0)
	{
		if( (rc=recv(sd, MPort, sizeof(MPort), 0)) < 0){
      		perror("receiving MPort  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		MPort[rc]='\0';
      		printf(">> MCAST Port:\t %s\n", MPort);
    	}
		
		if( (rc=recv(sd, listNumber, sizeof(listNumber), 0)) < 0){
      		perror("receiving list  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		listNumber[rc]='\0';
      		printf(">> LIST Code:\t %s\n", listNumber);
    	}
		
		if( (rc=recv(sd, exitNumber, sizeof(exitNumber), 0)) < 0){
      		perror("receiving exit  message");
      		exit(-1);
    	}
    	if (rc > 0){
      		exitNumber[rc]='\0';
      		printf(">> END Code:\t %s\n", exitNumber);
    	}
		//creates udp socket
		UDPPort = htons(atoi( MPort ) );
		
		LocalHostS.sin_family = AF_INET;
		LocalHostS.sin_port =UDPPort;
		LocalHostS.sin_addr.s_addr = htonl(INADDR_ANY);

		if ((UDPsocketS = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			printf("can't create UDP socket S: \n");
			exit(-1);
		}

		if (bind(UDPsocketS, (SA *) & LocalHostS, sizeof(LocalHostS)) < 0) {
			printf("error in bind S\n");
			exit(-1);
		}
		
		
		if (fork() == 0)
		sendMessage(0, UDPsocketS);
		getMessage(UDPsocketS, 1);
		
	}
	
}


void 
sendMessage(int inSock, int outSock)
{
	char            sendBuf[MAX_LEN];
	int             bytes = 0;
	struct sockaddr_in GroupAddress;

	GroupAddress.sin_family = AF_INET;
	GroupAddress.sin_port = UDPPort;
	GroupAddress.sin_addr.s_addr = inet_addr(MulticastIPAddress);

	
	printf("Type CTRL-D to END\n");

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
			printf("error in sendto \n");
			exit(-1);
		}
	}
}

void 
getMessage(int inSock, int outSock)
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

void 
joinGroup(int s, char *group)
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

void 
reusePort(int s)
{
	int             one = 1;

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &one, sizeof(one)) == -1) {
		printf("error in setsockopt,SO_REUSEPORT \n");
		exit(-1);
	}
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

void QUIThandler(int sig){
        printf("\nChatClient Terminated\n");
        exit(0);
}
