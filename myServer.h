#ifndef _MYSERVER_H_
#define _MYSERVER_H_

#define CLIENTCOUNT 256
#define BODYBUF 1024 

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <fcntl.h>
#include <arpa/inet.h>
#include "Mysql.h"

typedef struct msg_t
{
	char head[4];
	char body[BODYBUF];
}msg;

class myServer
{
public:
		myServer(int port);
		~myServer();
		void run();
private:
		int listen_st, epfd, socket_client[CLIENTCOUNT];
		struct epoll_event ev, events[CLIENTCOUNT];
		int socket_create(int port);
		int setnonblocking(int st);	
		int socket_accept();
		void loginMsg(int st, char *body);
		int Send(int st, const void *body, size_t len, int flag);
		int socket_recv(int st);
		void sigupmsg(int st, char *msg);
		void addUserMsg(int st, char *body);
		void selUserMsg(int st, char *body);
		void delUserMsg(int st, char *body);
		void updUserMsg(int st, char *body);
		void getUserName(int st);

};

#endif
