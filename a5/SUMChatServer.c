#include "def.h"
#define M 0
#define U 1
#define S 2


void init_mhdr();
void display_addr();
void getMyIP();
void NtoU();

struct in_addr  ip_addr;
struct cmsghdr *cmptr;
char            str[32];
struct msghdr   mhdr;
struct iovec    iov;
char            data[8192];
char            cdata[sizeof(struct sockaddr_in) + sizeof(struct cmsghdr)];
struct cmsghdr *cmsg;
struct sockaddr_in from;
socklen_t       fromlen = sizeof(struct sockaddr_in);

char MyIP[100];
char MyPort[10];
char MyAddr[100];
char DestAddr[100];
char SrcAddr[100];
char RecievedMsg[100];

#define MAXHOSTNAME 80
#define MAXLINE 1024
#define MAXU 10
#define MAXM 10
#define MAXS 10

void reusePort(int sock);   
char RandomCODE1[10];
char RandomCODE2[10];
char *LISTmsg=RandomCODE1;
char *ENDmsg=RandomCODE2;
char MIP[100]="224.0.0.1";
char MPort[10];



void selectHandle();
void INThandler( int sig );
void SPIPEhandler(int sig);    
void UPIPEhandler(int sig);
void MPIPEhandler(int sig);
void QUIThandler(int sig);
void UDPhandle();
void TCPhandle();
void SCTP_handle();			
void  SCTPQ_handle();		
void getMessage(int inSock, int outSock);

char TypeMsg[2];
int SCTP_src=0;				
int rc;
int i;

struct sockaddr_in LocalHostR;
struct sockaddr_in GroupAddress;


struct user_name {
    char    logname[10];
}; 

char LISTflag[MAXLINE] = "YES";
int Sconnfd[MAXS];			
int Uconnfd[MAXU];
int Mconnfd[MAXM];
char LISTout[MAXLINE];
int U_no=0, M_no=0, S_no=0;	
struct sockaddr_in UList[MAXU], MList[MAXM], SList[MAXS];	
struct user_name Uname[MAXU], Mname[MAXM], Sname[MAXS];		

void RandomInit();

void TCPInit();
void SCTPInit();		
void UDPInit();
void SCTPQInit();		
void UTest();
void MTest();
void STest();		
void MtoU();
void MtoS();		
void UtoM();
void UtoS();		
void StoM();		
void StoU();		

int Ugetfd ();
int Mgetfd ();
int Sgetfd ();			

  int   TCPsd, SCTPsd, pTCPsd, pSCTPsd;		
  int   UDPsd, SCTPQsd;					
  struct   sockaddr_in server;
  struct  hostent *hp, *gethostbyname();
  struct  servent *sp;
  struct sockaddr_in from;
  int length;
  char ThisHost[80];
  int PortNumber;
  int UDPPort;
  int childpid;
  int Ui, Mi, Si;		
  int UDPpid;

char *MulticastIPAddress;
u_char TimeToLive;
u_char loop;

void init_mhdr();
void display_addr();
void displayDaddr();
void init_mhdr();

char *NewS="WELCOME, New SCTP: ";
char *NewU="WELCOME, New UDP: ";
char *NewM="WELCOME, New MCAST: ";
char *EchoTest="!E!T!";


   char ForwardMsg[MAXLINE];
   char record[100];

char username[MAXLINE];
fd_set full_fd, read_fd;    

main( argc, argv )
int argc;
char *argv[];
{

  if (argc != 2)
    {
      printf ("usage: UMChatServer <port#>\n");
      exit (-1);
    }
  
  	for (i = 0; i < MAXU; i++) Uconnfd[i] = -1;
	for (i = 0; i < MAXM; i++) Mconnfd[i] = -1;	  
	for (i = 0; i < MAXS; i++) Sconnfd[i] = -1;			
  
  
  PortNumber = htons(atoi(argv[1])); 

  getMyIP();
  printf("\n>> MyIP: %s\n",MyIP);
  RandomInit();
  SCTPInit();
  TCPInit();
  UDPInit();
  SCTPQInit();		
 
  sprintf(MyPort,"%d",ntohs(server.sin_port));
  sprintf(MyAddr,"%s:%s",MyIP,MPort);
    
	signal( SIGINT, INThandler );
	signal( SIGQUIT, QUIThandler );
	selectHandle();
  
}

void selectHandle()
{
	while(1)
	{
		read_fd = full_fd;
		memcpy(&read_fd, &full_fd, sizeof(full_fd));
		select(FD_SETSIZE, &read_fd, NULL, NULL, NULL);
		
		if (FD_ISSET(TCPsd, &read_fd))
			TCPhandle();
		else if (FD_ISSET(SCTPsd, &read_fd))
			SCTP_handle();
		else if (FD_ISSET(SCTPQsd, &read_fd))
			SCTPQ_handle();			
		else
			UDPhandle();
	}
}



void TCPhandle()
{

  int index;
  //for(;;)  
  {
    fromlen = sizeof(from);
    if (  (pTCPsd  = accept(TCPsd, (SA *)&from, &fromlen)) == -1) {
        if (errno == EINTR) return;
    }
    if (pTCPsd > 1) {
	sprintf(SrcAddr, "%s:%d",
                inet_ntop(AF_INET, &from.sin_addr, str, sizeof(str)), ntohs(from.sin_port));

        if( (rc=recv(pTCPsd, username, sizeof(username), 0)) < 0)
                perror("receiving username  message");

	memset(ForwardMsg, '\0', MAXLINE);
   	sprintf(record,"[%s] ", SrcAddr);
	strcat (ForwardMsg, record);

        if( (rc=recv(pTCPsd, TypeMsg, sizeof(TypeMsg), 0)) < 0)
                perror("receiving TypeMsg  message");
        if (rc > 0){
                TypeMsg[rc]='\0';
						
		if (strcmp(TypeMsg,"0")==0)  {
    			printf("\nThe New MCAST Client --> (%s:%d) [%s]\n", 
				inet_ntoa(from.sin_addr), ntohs(from.sin_port), username);
    			if (send(pTCPsd, MIP, strlen(MIP), 0) <0 ) perror("sending MIP message");
			usleep(1000);
    			if (send(pTCPsd, MPort, strlen(MPort), 0) <0 ) perror("sending MPort message");
			usleep(1000);
    			if (send(pTCPsd, RandomCODE1, strlen(RandomCODE1), 0) <0 ) perror("sending RandomCODE1 message");
			usleep(1000);
    			if (send(pTCPsd, RandomCODE2, strlen(RandomCODE2), 0) <0 ) perror("sending RandomCODE2 message");
			usleep(1000);
			index = Mgetfd();
        		Mconnfd[index] = pTCPsd;
        		fromlen = sizeof (MList[index]);
        		getpeername(pTCPsd,(SA *)&MList[index],&fromlen);
                        sprintf(Mname[index].logname,"%s", username);
			M_no++;
			strcat (ForwardMsg, NewM);
			strcat (ForwardMsg, username);
			strcat (ForwardMsg, "\n");
		        if (sendto(UDPsd, ForwardMsg, strlen(ForwardMsg), 0,
                    		(SA *)&GroupAddress, sizeof(GroupAddress)) < 0)
                        		perror("TCPhandle: sendto UDPsd\n");
			 for (Si = 0; Si < MAXS; Si++)
				if (Sconnfd[Si] != -1) 
						 if ( sendto (SCTPQsd, ForwardMsg, strlen(ForwardMsg), 0, 
								(SA *)&SList[Si], sizeof(SList[Si])) <0 ) perror ("EndMsg: sendto SCTPQsd\n");					

		}else{
    			printf("\n The New UDP Client --> (%s:%d) [%s]\n", 
				inet_ntoa(from.sin_addr), ntohs(from.sin_port), username);
    			if (send(pTCPsd, MPort, strlen(MPort), 0) <0 ) perror("sending MPort message");
			usleep(1000);
    			if (send(pTCPsd, RandomCODE1, strlen(RandomCODE1), 0) <0 ) perror("sending RandomCODE1 message");
			usleep(1000);
    			if (send(pTCPsd, RandomCODE2, strlen(RandomCODE2), 0) <0 ) perror("sending RandomCODE2 message");
			usleep(1000);
                        index = Ugetfd ();
                        Uconnfd[index] = pTCPsd;
                        fromlen = sizeof (UList[index]);
                        getpeername(pTCPsd,(SA *)&UList[index],&fromlen);
                        sprintf(Uname[index].logname,"%s", username);
			U_no++;
			strcat (ForwardMsg, NewU);
			strcat (ForwardMsg, username);
			strcat (ForwardMsg, "\n");
		        if (sendto(UDPsd, ForwardMsg, strlen(ForwardMsg), 0,
                    		(SA *)&GroupAddress, sizeof(GroupAddress)) < 0)
                        		perror("TCPhandle: sendto UDPsd\n");
			 for (Si = 0; Si < MAXS; Si++)													
				if (Sconnfd[Si] != -1) 
						 if ( sendto (SCTPQsd, ForwardMsg, strlen(ForwardMsg), 0, 
								(SA *)&SList[Si], sizeof(SList[Si])) <0 ) perror ("EndMsg: sendto SCTPQsd\n");	
			 for (Ui = 0; Ui < MAXM; Ui++)															
				if (Uconnfd[Ui] != -1) 
						 if ( sendto (SCTPQsd, ForwardMsg, strlen(ForwardMsg), 0, 
								(SA *)&UList[Ui], sizeof(UList[Ui])) <0 ) perror ("EndMsg: sendto SCTPQsd\n");								
		}
                fflush(stdout);
        } 
    }
  } 
}


void SCTP_handle()										
{

  int index;
  //for(;;)  
  {

    fromlen = sizeof(from);
    if (  (pSCTPsd  = accept(SCTPsd, (SA *)&from, &fromlen)) == -1) {
        if (errno == EINTR) return;
    }
    if (pSCTPsd > 1) {
	sprintf(SrcAddr, "%s:%d",
                inet_ntop(AF_INET, &from.sin_addr, str, sizeof(str)), ntohs(from.sin_port));

        if( (rc=recv(pSCTPsd, username, sizeof(username), 0)) < 0)
                perror("receiving username  message");

	memset(ForwardMsg, '\0', MAXLINE);
   	sprintf(record,"[%s] ", SrcAddr);
	strcat (ForwardMsg, record);


        if (rc > 0){
    			printf("\nTHE New SCTP Client --> (%s:%d) [%s]\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port)+1, username);
    			if (send(pSCTPsd, MPort, strlen(MPort), 0) <0 ) perror("sending MPort message");
			usleep(1000);
    			if (send(pSCTPsd, RandomCODE1, strlen(RandomCODE1), 0) <0 ) perror("sending RandomCODE1 message");
			usleep(1000);
    			if (send(pSCTPsd, RandomCODE2, strlen(RandomCODE2), 0) <0 ) perror("sending RandomCODE2 message");
			usleep(1000);
			index = Sgetfd();
        		Sconnfd[index] = pSCTPsd;
        		fromlen = sizeof (SList[index]);
        		getpeername(pSCTPsd,(SA *)&SList[index],&fromlen);
				int tmp = ntohs(SList[index].sin_port);
				SList[index].sin_port = htons(SList[index].sin_port)+1;
                        sprintf(Sname[index].logname,"%s", username);
			S_no++;
			strcat (ForwardMsg, NewS);
			strcat (ForwardMsg, username);
			strcat (ForwardMsg, "\n");
        } 
    }
  } 
}



void RandomInit(){
        int no = time(NULL);
	int randomPort;
	int randomLIST;
	int randomEND;
	int p1, p2, p3, p4;
        srand(no);
        p1 = rand() % 15 + 224;
        p2 = rand() % 255;
        p3 = rand() % 255;
        p4 = rand() % 255;
	printf("THE MCAST IP --> %d.%d.%d.%d\n",p1,p2,p3,p4);
	sprintf(MIP, "%d.%d.%d.%d\n",p1,p2,p3,p4);
        randomPort = rand() % 1000 + 10000;
        printf("THE MCAST Port --> %d\n", randomPort);
        sprintf(MPort,"%d", randomPort);
        randomLIST = rand() % 1000000+1000000;
        printf("THE LIST Code --> %d\n", randomLIST);
        sprintf(RandomCODE1,"%d", randomLIST);
        randomEND = rand() % 1000000+1000000;
        printf("THE END Code --> %d\n", randomEND);
        sprintf(RandomCODE2,"%d", randomEND);
}


void INThandler( int sig ) {
        signal(SIGINT, INThandler);
        memset (LISTout, '\0', MAXLINE);
        UTest();
        MTest();
		STest();
	if ( strcmp(LISTflag,"YES") == 0 ) {
	 	printf("\n\nClients LIST :\n%s\n", LISTout);
	} 
	selectHandle();    
}

void  UTest(){
    signal(SIGPIPE, UPIPEhandler);
	sprintf(record, "\n  UDP Clients List: \n");
	strcat(LISTout, record);
	strcat(LISTout, "  *************************\n");
        for (Ui = 0; Ui < MAXU; Ui++) {
             if (Uconnfd[Ui] != -1) {
                  if ((send (Uconnfd[Ui], "p", 1, 0)) < 0) {
                                close (Uconnfd[Ui]);
                                Uconnfd[Ui] = -1;
				U_no--;
                  } 
                  else {
			sprintf(record, "  (%s:%d) [%s]\n", inet_ntoa (UList[Ui].sin_addr), 
				ntohs(UList[Ui].sin_port), Uname[Ui].logname);
			strcat(LISTout, record);
                  } 
             }
        }
	sprintf(record, "  ******(Total: %d)******\n", U_no);
        strcat(LISTout, record);
}

void  MTest(){
    signal(SIGPIPE, MPIPEhandler);
	sprintf(record, "\n  MCAST Clients List:\n");
	strcat(LISTout, record);
	strcat(LISTout, "  ***********************\n");
        for (Mi = 0; Mi < MAXM; Mi++) {
             if (Mconnfd[Mi] != -1) {
                  if ((send (Mconnfd[Mi], "p", 1, 0)) < 0) {
                                close (Mconnfd[Mi]);
                                Mconnfd[Mi] = -1;
				M_no--;
                  } 
                  else {
			sprintf(record, "  (%s:%d)\n", inet_ntoa (MList[Mi].sin_addr), ntohs(MList[Mi].sin_port));
			sprintf(record, "  (%s:%d) [%s]\n", inet_ntoa (MList[Mi].sin_addr), 
				ntohs(MList[Mi].sin_port), Mname[Mi].logname);
			strcat(LISTout, record);
                  } 
             }
        }
	sprintf(record, "  ******(Total: %d)*******\n", M_no);
        strcat(LISTout, record);
}

		
void  STest(){
    signal(SIGPIPE, SPIPEhandler);
	sprintf(record, "\n  SCTP Clients List:\n");
	strcat(LISTout, record);
	strcat(LISTout, "  *********************\n");
	
        for (Si = 0; Si < MAXS; Si++) {
             if (Sconnfd[Si] != -1) {
                  if ((send (Sconnfd[Si], "p", 1, 0)) < 0) {
                                close (Sconnfd[Si]);
                                Sconnfd[Si] = -1;
				S_no--;
                  } 
                  else {
			sprintf(record, "  (%s:%d) [%s]\n", inet_ntoa (SList[Si].sin_addr), 
				SList[Si].sin_port, Sname[Si].logname);
			strcat(LISTout, record);
                  } 
             }
        }
	sprintf(record, "  ******(Total: %d)*******\n", S_no);
        strcat(LISTout, record);
}

void UPIPEhandler(int sig){

        signal(SIGPIPE, UPIPEhandler);
        close(Uconnfd[Ui]);
	Uconnfd[Ui] = -1;
}
void MPIPEhandler(int sig){

        signal(SIGPIPE, MPIPEhandler);
        close(Mconnfd[Mi]);
	Mconnfd[Mi] = -1;
}

void SPIPEhandler(int sig){

        signal(SIGPIPE, SPIPEhandler);
        close(Sconnfd[Si]);
	Sconnfd[Si] = -1;
}

void QUIThandler(int sig)
{
	printf("ENDING...Informing Clients...\n");
	if (sendto(UDPsd, ENDmsg, strlen(ENDmsg), 0, (SA *)&GroupAddress, sizeof(GroupAddress)) < 0) 
        perror("EndMsg UtoM: sendto UDPsd\n");
	
	
     for (Ui = 0; Ui < MAXU; Ui++)
        if (Uconnfd[Ui] != -1)
            if ( sendto (UDPsd, ENDmsg, strlen(ENDmsg), 0, (SA *)&UList[Ui], sizeof(UList[Ui])) <0 ) 
				perror ("EndMsg: sendto UDPsd\n");
						
     for (Si = 0; Si < MAXS; Si++)					
        if (Sconnfd[Si] != -1) 
                 if ( sendto (SCTPQsd, ENDmsg, strlen(ENDmsg), 0, 
                        (SA *)&SList[Si], sizeof(SList[Si])) <0 ) perror ("EndMsg: sendto SCTPQsd\n");						
            

	printf("\nChatServer Termination Complete\n");
	
	system("./cleanup.sh");
	exit(0);
}


void TCPInit()
{

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port =  PortNumber;

  TCPsd = socket (AF_INET,SOCK_STREAM,IPPROTO_TCP); 
  reusePort(TCPsd);
  if ( bind( TCPsd, (SA *) &server, sizeof(server) ) < 0 ) {
    close(TCPsd);
    perror("binding name to stream socket");
    exit(-1);
  }
  FD_SET(TCPsd, &full_fd);   

  length = sizeof(server);
  if ( getsockname (TCPsd, (SA *)&server,&length) ) {
    perror("getting socket name");
    exit(0);
  }
  listen(TCPsd,4);
}

		
void SCTPInit()
{

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port =  PortNumber;

  SCTPsd = socket (AF_INET,SOCK_STREAM,IPPROTO_SCTP); 
  reusePort(SCTPsd);
  if ( bind( SCTPsd, (SA *) &server, sizeof(server) ) < 0 ) {
    close(SCTPsd);
    perror("binding name to stream socket");
    exit(-1);
  }
  FD_SET(SCTPsd, &full_fd);   

  length = sizeof(server);
  if ( getsockname (SCTPsd, (SA *)&server,&length) ) {
    perror("getting socket name");
    exit(0);
  }
  listen(SCTPsd,4);
 }


void UDPInit ()
{

        MulticastIPAddress = MIP;
        UDPPort = htons(atoi( MPort ) );

        LocalHostR.sin_family = AF_INET;
        LocalHostR.sin_port = UDPPort;
        LocalHostR.sin_addr.s_addr = htonl(INADDR_ANY);


        if ((UDPsd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                printf("can't create UDP socket R: \n");
                exit(-1);
        }
        reusePort(UDPsd);

        if (bind(UDPsd, (SA *) & LocalHostR, sizeof(LocalHostR)) < 0) {
                printf("error in bind R\n");
                exit(-1);
        }
		
		FD_SET(UDPsd, &full_fd);   
		
        TimeToLive = 2;
        setTTLvalue(UDPsd, &TimeToLive);


		displayDaddr(UDPsd);

        joinGroup(UDPsd, MIP);

        GroupAddress.sin_family = AF_INET;
        GroupAddress.sin_port = UDPPort;
        GroupAddress.sin_addr.s_addr = inet_addr(MulticastIPAddress);
}


		
void SCTPQInit()
{

	UDPPort = htons(atoi( MPort ));
	LocalHostR.sin_family = AF_INET;
	LocalHostR.sin_port = UDPPort;
	LocalHostR.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((SCTPQsd = socket(AF_INET, SOCK_SEQPACKET,IPPROTO_SCTP)) < 0) {
		printf("Can't create SEQPACKET socket: \n");
		exit(-1);
	}	

	reusePort(SCTPQsd);

	if (bind(SCTPQsd, (SA *) & LocalHostR, sizeof(LocalHostR)) < 0) {

		printf("Error in binding SCTPQsd\n");
		exit(-1);
	}   
	listen(SCTPQsd,1);
	FD_SET (SCTPQsd, &full_fd);		

	displayDaddr(SCTPQsd);

}



int Ugetfd ()
{
  int i;

  for (i = 0; i < MAXU; i++) {
        if (Uconnfd[i] == -1) return (i);
  }
  if (i == MAXU) {
      printf ("MaxU out fds\n");
      exit (-1);
  }
}

int Mgetfd ()
{
  int i;

  for (i = 0; i < MAXM; i++) {
        if (Mconnfd[i] == -1) return (i);
  }
  if (i == MAXM) {
      printf ("MaxM out fds\n");
      exit (-1);
  }
}

		
int Sgetfd ()
{
  int i;

  for (i = 0; i < MAXS; i++) {
        if (Sconnfd[i] == -1) return (i);
  }
  if (i == MAXS) {
      printf ("MaxS out fds\n");
      exit (-1);
  }
}

void  UDPhandle()
{

int ret;
        //while (1) 
		{

                init_mhdr();

                if ((ret = recvmsg(UDPsd, &mhdr, MSG_WAITALL)) < 0) {
			if (errno == EINTR)
                                return;
                        else
                                perror("recvmsg(UDPsd)");

                }
		sprintf(RecievedMsg, data);
                display_addr(UDPsd);
       }

}

void  SCTPQ_handle()
{

int ret;
SCTP_src = 1;
        //while (1) 
		{

                int len = sizeof(from);
				memset(RecievedMsg, '\0', MAX_LEN);				
				
				if (ret = recvfrom(SCTPQsd, RecievedMsg, MAX_LEN, 0, (SA *)&from, (socklen_t *)&len) < 0)
				{
					printf("ERROR RECEIVING SCTPQ Msg\n");
					exit(-1);
				}
				sprintf(SrcAddr, "%s:%d", inet_ntop(AF_INET, &from.sin_addr, str, sizeof(str)), ntohs(from.sin_port));

				if (strcmp(RecievedMsg,LISTmsg)==0)  
				{
					printf("  Got (LISTmsg)");
					
					sprintf(LISTflag,"NO");
					usleep(1000);

					
					memset (LISTout, '\0', MAXLINE);
					UTest();
					MTest();	
					STest();
					 

					usleep(1000);
					if (sendto(SCTPQsd, LISTout, strlen(LISTout), 0, (SA *)&from, sizeof(from)) < 0) 
						perror("sendto LISTout to UDPsd\n");
					sprintf(LISTflag,"YES");
				}
				else
				{
					StoM();
					StoU();
				}
                
       }

}

void
init_mhdr()
{
        bzero(cdata, sizeof(cdata));
        bzero(data, sizeof(data));
        bzero(&mhdr, sizeof(mhdr));
        mhdr.msg_name = &from;
        mhdr.msg_namelen = fromlen;
        mhdr.msg_iov = &iov;
        mhdr.msg_iovlen = 1;
        mhdr.msg_control = cdata;
        mhdr.msg_controllen = sizeof(cdata);
        iov.iov_base = data;
        iov.iov_len = sizeof(data);
        cmsg = (struct cmsghdr *) cdata;
}


		
void
display_addr(int UDPsd)
{

	struct in_pktinfo *pktinfo;

	for (cmptr = CMSG_FIRSTHDR(&mhdr); cmptr != NULL; cmptr = CMSG_NXTHDR(&mhdr, cmptr)) 
	{
		if (cmptr->cmsg_level == IPPROTO_IP && cmptr->cmsg_type == IP_PKTINFO) 
		{
		
				pktinfo = (struct in_pktinfo*)CMSG_DATA(cmptr);
				memcpy(&ip_addr, &pktinfo->ipi_addr, sizeof(struct in_addr));
				sprintf(DestAddr, "%s\n",
				inet_ntop(AF_INET, &ip_addr, str, sizeof(str)));
				sprintf(SrcAddr, "%s:%d",
				inet_ntop(AF_INET, &from.sin_addr, str, sizeof(str)), ntohs(from.sin_port));

			if (strcmp(DestAddr, MIP) == 0 ) 
			{
				printf("\n Got MCAST Msg --> %s", RecievedMsg );
				printf("   From --> %s\n", SrcAddr);
				memset (LISTout, '\0', MAXLINE);
					UTest();
					MTest();	
					STest();	
				MtoU();
				if (strcmp(SrcAddr, MyAddr) == 0 && (SCTP_src == 0))
				{	
				}
				else
				{
					MtoS();
					SCTP_src = 0;
				}
			}
			else 
			{
				if (strcmp(RecievedMsg, EchoTest) == 0 ) 
				{
					
					usleep(1000);
					if (sendto(UDPsd, EchoTest, strlen(EchoTest), 0,(SA *)&from, sizeof(from)) < 0) 
						perror("sendto EchoTest to UDPsd\n");
					return;           
				}
				printf("\n Got UDP Msg --> %s", RecievedMsg);
				printf("   From --> %s\n", SrcAddr);
				if (strcmp(RecievedMsg,LISTmsg)==0)  
				{
					printf("  Got (LISTmsg)");
					printf("   From: %s\n", SrcAddr);
					sprintf(LISTflag,"NO");
					usleep(1000);

					memset (LISTout, '\0', MAXLINE);
					UTest();
					MTest();	
					STest();
					 

					usleep(1000);
					if (sendto(UDPsd, LISTout, strlen(LISTout), 0, (SA *)&from, sizeof(from)) < 0) 
						perror("sendto LISTout to UDPsd\n");
					sprintf(LISTflag,"YES");
				}
				else 
				{
					UtoM();
					UtoS();
				}
		}

		}
	}
}

void getMyIP(){

char ThisHost[100];

   gethostname(ThisHost, MAXLINE);

   if  ( (hp = gethostbyname(ThisHost)) == NULL ) {
      fprintf(stderr, "Can't find host name\n");
      exit(-1);
   }
   bcopy ( hp->h_addr, &(server.sin_addr), hp->h_length);
   sprintf(MyIP, "%s", inet_ntoa(server.sin_addr));
}
void UtoM()
{

   char ForwardMsg[MAXLINE];
   char record[100];
   if (strcmp(RecievedMsg,ENDmsg)==0)  {
		printf("  Got (ENDmsg)");
	        printf("   From: %s\n", SrcAddr);
		sprintf(RecievedMsg, "TERMINATED, Bye.\n");
  			sprintf(LISTflag,"NO");
			usleep(1000);
			
			memset (LISTout, '\0', MAXLINE);
			UTest();
			MTest();	
			STest();
						
			usleep(1000);
  			sprintf(LISTflag,"YES");
   }

   memset(ForwardMsg, '\0', MAXLINE);
   sprintf(record,"[%s] ", SrcAddr);
   strcat (ForwardMsg, record);
   strcat (ForwardMsg, RecievedMsg);

	printf("Sending MCAST Message.....\n");
	if (sendto(UDPsd, ForwardMsg, strlen(ForwardMsg), 0,
                    (SA *)&GroupAddress, sizeof(GroupAddress)) < 0) 
                        printf("UtoM: sendto UDPsd\n");
}

void MtoU()
{

   memset(ForwardMsg, '\0', MAXLINE);
   sprintf(record,"[%s] ", SrcAddr);
   strcat (ForwardMsg, record);
   strcat (ForwardMsg, RecievedMsg);  

     for (Ui = 0; Ui < MAXU; Ui++)
        if (Uconnfd[Ui] != -1){ 
           printf("   Sending-->");
           if (strcmp(SrcAddr,MyAddr)==0) {
           	if ( sendto (UDPsd, RecievedMsg, strlen(RecievedMsg), 0, 
              		(SA *)&UList[Ui], sizeof(UList[Ui])) <0 ) perror ("MtoU: sendto UDPsd\n");
	  	printf ("(%s:%d):%s", inet_ntoa (UList[Ui].sin_addr), ntohs(UList[Ui].sin_port), RecievedMsg);
           } else {
                 if ( sendto (UDPsd, ForwardMsg, strlen(ForwardMsg), 0, 
                        (SA *)&UList[Ui], sizeof(UList[Ui])) <0 ) perror ("MtoU: sendto UDPsd\n");
	  	printf ("(%s,%d):%s", inet_ntoa (UList[Ui].sin_addr), ntohs(UList[Ui].sin_port), ForwardMsg);
           }
        }
}

void MtoS()
{

   memset(ForwardMsg, '\0', MAXLINE);
   sprintf(record,"[%s] ", SrcAddr);
   strcat (ForwardMsg, record);
   strcat (ForwardMsg, RecievedMsg);  
	printf("SCTP clients being SENT....\n");
    for (Si = 0; Si < MAXS; Si++)
        if (Sconnfd[Si] != -1){ 
           printf("   Sending-->");
           if (strcmp(SrcAddr,MyAddr)==0) {
           	if ( sendto (SCTPQsd, RecievedMsg, strlen(RecievedMsg), 0, 
              		(SA *)&SList[Si], sizeof(SList[Si])) <0 ) perror ("MtoS: sendto SCTPQsd\n");
	  	printf ("(%s:%d):%s", inet_ntoa (SList[Si].sin_addr), SList[Si].sin_port, RecievedMsg);
           } else {
                 if ( sendto (SCTPQsd, ForwardMsg, strlen(ForwardMsg), 0, 
                        (SA *)&SList[Si], sizeof(SList[Si])) <0 ) perror ("MtoS: sendto SCTPQsd\n");
	  	printf ("(%s,%d):%s", inet_ntoa (SList[Si].sin_addr), SList[Si].sin_port, ForwardMsg);
           }
        }
}


void StoU()
{

   char ForwardMsg[MAXLINE];
   char record[100];
   if (strcmp(RecievedMsg,ENDmsg)==0)  {
		printf("  Got (ENDmsg)");
	        printf("   From: %s\n", SrcAddr);
		sprintf(RecievedMsg, "TERMINATED, Bye\n");
  			sprintf(LISTflag,"NO");
			usleep(1000);
			
			memset (LISTout, '\0', MAXLINE);
			UTest();
			MTest();	
			STest();
						
			usleep(1000);
  			sprintf(LISTflag,"YES");
   }

   memset(ForwardMsg, '\0', MAXLINE);
   sprintf(record,"[%s] ", SrcAddr);
   strcat (ForwardMsg, record);
   strcat (ForwardMsg, RecievedMsg);

}

		
void UtoS()
{

   char ForwardMsg[MAXLINE];
   char record[100];
   if (strcmp(RecievedMsg,ENDmsg)==0)  {
		printf("  Got (ENDmsg)");
	        printf("   From: %s\n", SrcAddr);
		sprintf(RecievedMsg, "TERMINATED, Bye.\n");
  			sprintf(LISTflag,"NO");
			usleep(1000);
			
			memset (LISTout, '\0', MAXLINE);
			UTest();
			MTest();	
			STest();
						
			usleep(1000);
  			sprintf(LISTflag,"YES");
   }

   memset(ForwardMsg, '\0', MAXLINE);
   sprintf(record,"[%s] ", SrcAddr);
   strcat (ForwardMsg, record);
   strcat (ForwardMsg, RecievedMsg);
	printf("Sending to SCTP clients....\n");
    for (Si = 0; Si < MAXS; Si++)
        if (Sconnfd[Si] != -1){ 
           printf("   Sending-->");
           if (strcmp(SrcAddr,MyAddr)==0) {
           	if ( sendto (SCTPQsd, RecievedMsg, strlen(RecievedMsg), 0, 
              		(SA *)&SList[Si], sizeof(SList[Si])) <0 ) perror ("UtoS: sendto SCTPQsd\n");
	  	printf ("(%s:%d):%s", inet_ntoa (SList[Si].sin_addr), SList[Si].sin_port, RecievedMsg);
           } else {
                 if ( sendto (SCTPQsd, ForwardMsg, strlen(ForwardMsg), 0, 
                        (SA *)&SList[Si], sizeof(SList[Si])) <0 ) perror ("UtoS: sendto SCTPQsd\n");
	  	printf ("(%s,%d):%s", inet_ntoa (SList[Si].sin_addr), SList[Si].sin_port, ForwardMsg);
           }
        }
}

void StoM()
{

   char ForwardMsg[MAXLINE];
   char record[100];
   if (strcmp(RecievedMsg,ENDmsg)==0)  {
		printf("  Got (ENDmsg)");
	        printf("   From: %s\n", inet_ntoa(from.sin_addr));
		sprintf(RecievedMsg, "TERMINATED, Bye\n");
  			sprintf(LISTflag,"NO");
			usleep(1000);
			
			memset (LISTout, '\0', MAXLINE);
			UTest();
			MTest();	
			STest();
						
			usleep(1000);
  			sprintf(LISTflag,"YES");
   }

   memset(ForwardMsg, '\0', MAXLINE);
   sprintf(record,"[%s] ", SrcAddr);
   sprintf(record,"[%s:%d] ", inet_ntoa(from.sin_addr), from.sin_port);
   strcat (ForwardMsg, record);
   strcat (ForwardMsg, RecievedMsg);

	printf("Sending MCAST Message.....\n");
	if (sendto(UDPsd, ForwardMsg, strlen(ForwardMsg), 0,
                    (SA *)&GroupAddress, sizeof(GroupAddress)) < 0) 
                        printf("StoM: sendto UDPsd\n");
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


void getMessage(int inSock, int outSock)
{
        int             bytes = 0;
        char            recvBuf[MAXLINE];
        char            sender[100];
        struct sockaddr_in from;
        int fromlen;


        for (;;) {
                memset(recvBuf, '\0', MAXLINE);
                fromlen = sizeof(from);
                bytes = recvfrom(inSock, recvBuf, MAXLINE, 0, (SA *) &from, &fromlen)  ;
                if (bytes < 0) {
                        printf("error in reading from socket\n");
                        exit(-1);
                } 

                else {          
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
setLoopback(int s, u_char loop)
{
        if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char *) &loop,
                       sizeof(u_char)) == -1) {
                printf("error in disabling loopback\n");
        }
}

void 
displayDaddr(int s)
{
        int             one = 1;

        if (setsockopt(s, IPPROTO_IP, IP_PKTINFO, (char *) &one, sizeof(one)) < 0)
                perror("IP_RECVDSTADDR setsockopt error");


}
