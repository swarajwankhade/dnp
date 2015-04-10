#include "def.h"
void RandGen();
void getIPaddress ();
char *UnicastIPaddress;

#define MAXHOSTNAME 80
void reusePort(int sock);   

char MIP[100]="224.0.0.1";
char MPort[10];
char listNumber[10];
char exitNumber[10];
char userBuf[30];
char buf[10];


void INThandler( int sig );
void PIPEhandler(int sig);
void QUIThandler(int sig);

int i;
int randomPort;


int cli_num = 3;


main( argc, argv )
int argc;
char *argv[];
{
	int   sd, psd,UDPsocket;
	struct   sockaddr_in server;
	struct   sockaddr_in LocalHostR;
	struct  hostent *hp, *gethostbyname();
	struct  servent *sp;
	struct sockaddr_in from;
	int fromlen;
	int length;
	char ThisHost[80];
	int pn,rc;
	int childpid;
	int multiport;
	
	signal( SIGINT, INThandler );
	signal( SIGQUIT, QUIThandler );
	  
	//checking for the arguments are provided or not 
	if (argc != 2) 
	{
		printf("Usage: ChatServer <sport>\n");
		printf("e.g.: ChatServer 10123\n");
		exit(1);
	}

	//tcp socket to get the connections 
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);//OS allocated IPaddress
	pn = htons(atoi(argv[1])); //binding to the port given
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
	  
	
	getIPaddress();
	listen(sd,4);
	fromlen = sizeof(from);
	RandGen();
	
	
	//creating the udp socket
	
	LocalHostR.sin_family = AF_INET;
	LocalHostR.sin_port = randomPort;
	LocalHostR.sin_addr.s_addr = htonl(INADDR_ANY);
	  
	if ((UDPsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("can't create UDP socket : \n");
		exit(-1);
	}
	reusePort(UDPsocket);
	//binding it to the mport 
	if (bind(UDPsocket, (SA *) & LocalHostR, sizeof(LocalHostR)) < 0) {
        printf("error in bind R\n");
        exit(-1);
    }

	 
	for(;;){
		psd  = accept(sd, (SA *)&from, &fromlen);
		
		if (psd > 1) {
			cli_num++;
			
			//receiving the user name
			memset(userBuf, '\0',sizeof(userBuf));
			recv(psd, userBuf, sizeof(userBuf), 0);
			
			//receiving
			memset(buf, '\0',sizeof(buf));
			if( (rc=recv(psd, buf, sizeof(buf), 0)) < 0){
				perror("receiving stream  message");
				exit(-1);
			}
			
			if (rc > 0)
			{
				if(buf[0]=='0')
				{
					
					printf("\nNew MCAST ChatClient:\t (%s:%d)[%s]\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port),userBuf);
					
					if (send(psd, MIP, strlen(MIP), 0) <0 )
						perror("sending MIP message");
					sleep(1);
					if (send(psd, MPort, strlen(MPort), 0) <0 )
						perror("sending MPort message");
					sleep(1);
					if (send(psd, listNumber , strlen(listNumber), 0) <0 )
						perror("sending list message");
					sleep(1);
					if (send(psd, exitNumber, strlen(exitNumber), 0) <0 )
						perror("sending exit message");
				}
				if(buf[0]=='1')
				{
					
					printf("\nNew UDP ChatClient:\t (%s:%d)[%s]\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port),userBuf);
					
					if (send(psd, MPort, strlen(MPort), 0) <0 )
						perror("sending MPort message");
					sleep(1);
					if (send(psd, listNumber , strlen(listNumber), 0) <0 )
						perror("sending list message");
					sleep(1);
					if (send(psd, exitNumber, strlen(exitNumber), 0) <0 )
						perror("sending exit message");
				}	
			  
			}
			
		//user login
			
			
			
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
	
	int p1, p2, p3, p4;
	int l;
	int e;
    srand(no);
    p1 = rand() % 15 + 224;
    p2 = rand() % 255;
    p3 = rand() % 255;
    p4 = rand() % 255;
	printf(">> MCAST IP:\t %d.%d.%d.%d\n",p1,p2,p3,p4);
	sprintf(MIP, "%d.%d.%d.%d\n",p1,p2,p3,p4);
	
    randomPort = rand() % 1000 + 10000;
    printf(">> MCAST Port:\t %d\n", randomPort);
    sprintf(MPort,"%d", randomPort);
	
	l=rand() % 10000000+10000000;
    printf(">> LIST Code:\t %d\n", l);
    sprintf(listNumber,"%d", l);
	
	e=rand() % 10000000+10000000;
    printf(">> END Code:\t %d\n", e);
    sprintf(exitNumber,"%d", e);
		
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

void PIPEhandler(int sig){
    signal(SIGPIPE, PIPEhandler);
    close(i);
}

void QUIThandler(int sig){
	printf("\nChatServer Terminated\n");
	exit(0);
}
void getIPaddress ()
{
	char ThisHost[128];
	struct hostent *hp, *gethostbyname ();
	struct sockaddr_in server;
	gethostname (ThisHost, 128);
	hp = gethostbyname (ThisHost);
	bcopy (hp->h_addr, &(server.sin_addr), hp->h_length);
	UnicastIPaddress = inet_ntoa (server.sin_addr);
	printf("\n>> MyIP :\t %s\n", UnicastIPaddress);
}
