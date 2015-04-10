# environment variables setup
#CC		=	gcc -ansi
CC		=	gcc 
RM		= rm -rf 

ifeq ($(shell uname),SunOS)
        LIBS = -lsocket -lnsl
endif

# libraries and includes
#LIBS       = -lsocket -lnsl 

# Source package description

PROGS = UMChatServer UMChatClient

all: $(PROGS)

UMChatServer:  UMChatServer.o
	$(CC)  -o UMChatServer UMChatServer.c $(LIBS)
UMChatClient :   UMChatClient.o
	$(CC)  -o   UMChatClient  UMChatClient.c $(LIBS)

clean:;
	$(RM) $(PROGS) *.ln *.BAK *.bak *.o core errs ,* *~ *.a .emacs_* tags \
	TAGS make.log MakeOut  "#"*

