CC              =       gcc 
SOLARISHOME     = /usr/dt
RM              = rm -rf 

# libraries and includes
CFLAGS          = $(NORMCFLAGS)
#Solaris
#LIBS       = -lsocket -lnsl 
LIBS       = -lnsl 

PROGS = UMChatServer UMChatClient

all: $(PROGS)

UMChatClient.o: UMChatClient.c
	$(CC) -c UMChatClient.c $(LIBS) 
UMChatClient :  UMChatClient.o
	$(CC) -o UMChatClient UMChatClient.o $(LIBS)	
UMChatServer.o: UMChatServer.c
	$(CC) -c UMChatServer.c $(LIBS) 
UMChatServer :  UMChatServer.o 
	$(CC) -o UMChatServer UMChatServer.o $(LIBS)
clean:;
	rm -f $(PROGS) *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags \
	TAGS make.log MakeOut  "#"*
