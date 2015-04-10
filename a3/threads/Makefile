CC              =       gcc 
RM              = rm -rf 

# libraries and includes
CFLAGS          = $(NORMCFLAGS)
#Solaris
#LIBS       = -lsocket -lnsl 
LIBS       = -lnsl  -lpthread

PROGS = UMChatClient UMChatServer

all: $(PROGS)

UMChatClient:  UMChatClient.o
	$(CC) $(CFLAGS) -o UMChatClient UMChatClient.c $(LIBS)
UMChatServer:  UMChatServer.o
	$(CC) $(CFLAGS) -o UMChatServer UMChatServer.c $(LIBS)
clean:;
	rm -f $(PROGS) core *.o