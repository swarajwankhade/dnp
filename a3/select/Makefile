CC              =       gcc 
SOLARISHOME     = /usr/dt
RM              = rm -rf 

# libraries and includes
CFLAGS          = $(NORMCFLAGS)
#Solaris
#LIBS       = -lsocket -lnsl 
LIBS       = -lnsl -lpthread

PROGS = UMChatServer UMChatClient  

all: $(PROGS)

ChatServer.o: UMChatServer.c
	$(CC) -c  UMChatServer.c  $(LIBS)
ChatClient.o: UMChatClient.c
	$(CC) -c UMChatClient.c $(LIBS) 

clean:;
	rm -f $(PROGS) *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags \
	TAGS make.log MakeOut  "#"*
