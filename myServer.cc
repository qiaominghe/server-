#include "myServer.h"

using namespace std;

myServer::myServer(int port)
{
		listen_st = socket_create(port);
		if (listen_st == 0)
				exit(-1);

		epfd = 0;
		memset(&events, 0, sizeof(struct epoll_event));
}
myServer::~myServer()
{
		if (listen_st)
				close(listen_st);
}

int myServer::setnonblocking(int st) //将socket设置为非阻塞
{
		int opts = fcntl(st, F_GETFL);
		if (opts < 0)
		{
				printf("fcntl failed %s\n", strerror(errno));
				return 0;
		}
		opts = opts | O_NONBLOCK;
		if (fcntl(st, F_SETFL, opts) < 0)
		{
				printf("fcntl failed %s\n", strerror(errno));
				return 0;
		}
		return 1;
}

int myServer::socket_create(int port)//创建参数port指定端口号的server端socket
{
		int st = socket(AF_INET, SOCK_STREAM, 0);//创建TCP Socket
		int on = 1;
		if (setsockopt(st, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
		{
				printf("setsockopt failed %s\n", strerror(errno));
				return -1;
		}
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(st, (struct sockaddr *) &addr, sizeof(addr)) == -1)
		{
				printf("bind port %d failed %s\n", port, strerror(errno));
				return -1;
		}
		if (listen(st, 300) == -1)
		{
				printf("listen failed %s\n", strerror(errno));
				return -1;
		}
		return st;//返回listen的socket描述符
}
void myServer::loginMsg(int st, char *body)
{
		Mysql my;
		msg m;
		memset(&m, 0, sizeof(m));

		char *p = body;
		char name[512], password[512], userInfo[1024];

		memset(name, 0, sizeof(name));
		memset(password, 0, sizeof(password));
		memset(userInfo, 0, sizeof(userInfo));

		for (; *p; p++)
		{
				if (*p == ':')
						break;
		}
		int name_len = p - body;
		int password_len = strlen(body) - name_len - 1;
		strncpy(name, body, name_len);
		strncpy(password, p+1, password_len);
		cout << name << " " << password << endl;

		int flag = my.select_user(name, userInfo);

		for (p = userInfo + strlen(userInfo) - 1; *p; p--)
		{
				if (*p == ':')
						break;
		}

		cout <<"password:" << p+1 << endl;	
		if (flag == 0 && (strcmp(password, p+1) == 0))
		{
				m.head[0] = 1;
				Send(st, (const void *)&m, sizeof(msg), 0);
		}
		else
		{
				m.head[0] = -1;
				Send(st, (const void *)&m, sizeof(msg), 0);
		}
		return;
}
int myServer::Send(int st, const void *str, size_t len, int flag)
{
		if (send(st, str, len, flag) != -1)
		{
				cout << "send success!" << endl;


				return 0;
		}
		else
		{
				cout << "send error " << strerror(errno);
				return -1;
		}
}
int myServer::socket_accept()
{
		struct sockaddr_in client_addr;
		socklen_t len = sizeof(client_addr);
		memset(&client_addr, 0, sizeof(client_addr));
		int client_st = accept(listen_st, (struct sockaddr *) &client_addr, &len);//接收到来自client的socket连接

		if (client_st < 0)
				printf("accept failed %s\n", strerror(errno));
		else
				printf("accept by %s\n", inet_ntoa(client_addr.sin_addr));
		return client_st;//返回来自client端的socket描述符
}
void myServer::addUserMsg(int st, char *body)
{
		Mysql my;
		msg m;
		memset(&m, 0, sizeof(m));
		m.head[0] = 2;

		char *p = body;
		char *start = body;
		char userMsg[8][512];

		memset(userMsg, 0, sizeof(userMsg));

		for (int i = 0; ; )
		{
				if (*p == ':' || *p == 0)
				{
						strncpy(userMsg[i], start, p-start);
						userMsg[i][p-start] = 0;
						if (*p == 0)
								break;
						i++;
						p++;
						start = p;
				}
				else
						++p;
		}
		for (int i = 0; i < 8 ; ++i)
				cout << userMsg[i] << endl;

		int exist = my.select_user((char *)userMsg[0]);
		if (exist == -1)
		{
				m.head[1] = -1;
				sprintf(m.body, "Add user failed! The databases error!");
				Send(st, (const void *)&m, sizeof(msg), 0);
		}
		if (exist == 0)
		{
				m.head[1] = -1;
				sprintf(m.body, "Add user failed! The user you want to add is already exist!");
				Send(st, (const void *)&m, sizeof(msg), 0);
		}
		else if(exist == -2)
		{
				cout << "userMsg[0]" << userMsg[0] << endl;
				int flag = my.add_user(userMsg);
				cout << flag << endl;
				if (flag == 0)
				{
						m.head[1] = 1;
						Send(st, (const void *)&m, sizeof(msg), 0);
				}
				else
				{
						m.head[1] = -1;
						sprintf(m.body, "Add user failed! The operation you want to do is failed!");
						Send(st, (const void *)&m, sizeof(msg), 0);
				}
		}
}
void myServer::getUserName(int st)
{
		msg m;
		m.head[0] = 2;
		m.head[1] = 4;
		char *names = NULL;
		Mysql my;
		int ret = my.select_users_name(&names);

		if (ret == 0)
		{
			m.head[2] = 1;
			strcpy(m.body, names);
			Send(st, (const void *)&m, sizeof(m), 0);
		}
		else
		{
				m.head[2] = -1;
				strcpy(m.body, "Get users names failed! The databases error!");
			Send(st, (const void *)&m, sizeof(m), 0);
		}
		delete names;
		return;
}
void myServer::selUserMsg(int st, char *body)
{
		Mysql my;
		msg m;
		memset(&m, 0, sizeof(m));
		m.head[0] = 2;
		m.head[1] = 4;
		char str[1024];
		memset(str, 0, sizeof(str));

		int flag = my.select_user(body, str);
		cout << str << endl;
		cout << flag << endl;
		if (flag == 0)
		{
			m.head[2] = 2;
			strcpy(m.body, str);
		}
		else
		{
				m.head[2] = -2;
				sprintf(m.body, "select user failed! The user you want to select is not exist!");
		}
		Send(st, (const void *)&m, sizeof(m),0);
}
void myServer::delUserMsg(int st, char *body)
{
		Mysql my;
		msg m;
		memset(&m, 0, sizeof(m));
		m.head[0] = 2;
		m.head[1] = 2;
		int flag = my.select_user(body, NULL);
		if (flag == 0)
		{
				int ret = my.del_user(body);
				if (ret == 0)
				{
					m.head[1] = 2;
				}
				else
				{
						m.head[1] = -2;
						sprintf(m.body, "delete user failed! The operation you want to do is failed!");
				}

		}
		else
		{
				m.head[1] = -2;
				sprintf(m.body, "delete user failed! The user you want to delete is not exist");
		}
		Send(st, (const void *)&m, sizeof(m), 0);
}
void myServer::updUserMsg(int st, char *body)
{
		Mysql my;
		char *p = body;
		char *start = body;
		char userMsg[8][512];

		msg m;
		m.head[0] = 2;
		bzero(&m, sizeof(m));
		memset(userMsg, 0, sizeof(userMsg));

		for (int i = 0; ; )
		{
				if (*p == ':' || *p == 0)
				{
						strncpy(userMsg[i], start, p-start);
						userMsg[i][p-start] = 0;
						if (*p == 0)
								break;
						i++;
						p++;
						start = p;
				}
				else
						p++;
		}
		for (int i = 0; i < 8 ; ++i)
				cout << userMsg[i] << endl;
		int ret = my.select_user(userMsg[0]);
		cout << ret << endl;
		if (ret != 0)
		{
				m.head[1] = -3;
				sprintf(m.body, "select user failed! The user you want to select is not exist!");
		}
		else
		{
			int flag = my.modify_user(userMsg);

			if (flag == 0)
			{
					m.head[1] = 3;
			}
			else
			{
				m.head[1] = -3;
				sprintf(m.body, "update user failed! The operation you want to do is failed!");
			}
		}
		Send(st, (const void *)&m, sizeof(m), 0);
}
int myServer::socket_recv(int st)
{
		struct msg_t msg;
		memset(&msg, 0, sizeof(msg));

		ssize_t rc = recv(st, (char *) &msg, sizeof(msg), 0);//接收来自client socket发送来的消息
		if (rc <= 0)//接收失败
		{
				if (rc < 0)
						printf("recv failed %s\n", strerror(errno));
		} else//接收成功
		{
				printf("recvice msg:%d %d %s\n", msg.head[0],msg.head[1], msg.body);
				switch (msg.head[0])
				{
						case 1: //login消息
								loginMsg(st, msg.body);
								//printf("recvice msg:%d %s\n", msg.head[0], msg.body);
								break;
						case 2: //消息
								switch (msg.head[1])
								{
										case 1://add usr
												addUserMsg(st, msg.body);
												break;
										case 2://del usr
												delUserMsg(st, msg.body);
												break;
										case 3://update usr
												updUserMsg(st, msg.body);
												break;
										case 4://select usr
												if (msg.head[2] == 1)
												{
														getUserName(st);
												}
												if (msg.head[2] == 2)
												{
														selUserMsg(st, msg.body);
												}
												break;
								}
								break;
						case 3:
								switch (msg.head[1])
								{
										case 1://add usr
												//addUserMsg(st, msg.body);
												break;
										case 2://del usr
												delUserMsg(st, msg.body);
												break;
										case 3://update usr
												updUserMsg(st, msg.body);
												break;
										case 4://select usr
												selUserMsg(st, msg.body);
								}
								break;
						case 4:
								switch (msg.head[1])
								{
										case 1://add usr
												//addUserMsg(st, msg.body);
												break;
										case 2://del usr
												delUserMsg(st, msg.body);
												break;
										case 3://select usr
												updUserMsg(st, msg.body);
												break;
								}
								break;
						default: //无法识别的消息
								printf("login fail, it's not login message,%s\n",
												(const char *) &msg);
								msg.head[0] = 2; //系统消息
								msg.head[1] = 0; //无法识别的消息
								msg.head[2] = 0; //暂时保留，填0
								msg.head[3] = 0; //暂时保留，填0
								send(st, (const char *) &msg, sizeof(msg.head), 0);//给client端socket下发系统消息
								return 0;
				}
		}
		return rc;
}

void myServer::run()
{
		setnonblocking(listen_st);
		epfd = epoll_create(CLIENTCOUNT);	
		ev.data.fd = listen_st;
		ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
		epoll_ctl(epfd, EPOLL_CTL_ADD, listen_st, &ev);

		int st = 0;
		int nfds = 0;
		while (1)
		{
				nfds = epoll_wait(epfd, events, CLIENTCOUNT, -1);
				if (nfds == -1)
				{
						cout << "epoll_wait failed " << strerror(errno) << endl;
						break;
				}

				for (int i = 0; i < nfds; i++)
				{
						if (events[i].data.fd < 0)
								continue;

						if (events[i].data.fd == listen_st)
						{
								st = socket_accept();

								if (st >= 0)
								{
										setnonblocking(st);
										ev.data.fd = st;
										ev.events = EPOLLIN | EPOLLERR | EPOLLHUP; //设置要处理的事件类型	
										epoll_ctl(epfd, EPOLL_CTL_ADD, st, &ev);//将来自client端的socket描述符加入epoll
										continue;
								}
						}
						if (events[i].events & EPOLLIN) //socket收到数据
						{
								st = events[i].data.fd;
								if (socket_recv(st) <= 0)
								{
										//										user_logout(st);
										events[i].data.fd = -1;
								}
						}
						if (events[i].events & EPOLLERR) //socket错误。
						{
								st = events[i].data.fd;
								//								user_logout(st);
								events[i].data.fd = -1;
						}

						if (events[i].events & EPOLLHUP) //socket被挂断。
						{
								st = events[i].data.fd;
								//								user_logout(st);
								events[i].data.fd = -1;
						}
				}
		}
		close(epfd);
}

