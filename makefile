.SUFFIXES: .cc .o

CC=g++
CFLAGS=-Wall -g 
LFLAGS=-lmysqlclient -ldl -lpthread -lm -lrt -lstdc++ -lz


SRCS=main.cc\
	myServer.cc\
	pub.cc\
	Mysql.cc


OBJS=$(SRCS:.cc=.o)
EXEC=serverFrist

all: $(OBJS) 
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $(EXEC)
	@echo '-------------ok--------------'

#Mysql.o:Mysql.cc
#$(CC) $(CFLAGS) -I/usr/include/ $^ -o $@

%.o:%.cc
	$(CC) $(CFLAGS)  -I/usr/include -c $<  -o   $@ 
#.cc.o:
#	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -f $(OBJS)
	rm -f core*

