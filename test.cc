#include "Mysql.h"
#include <iostream>
#include <string.h>
using namespace std;

int main()
{
/*		char userInfo[8][512];
		memset(userInfo, 0, sizeof(userInfo));
		strcpy(userInfo[0], "qiaominghe");
		strcpy(userInfo[1], "qiaominghe");
		strcpy(userInfo[2], "qiaominghe");
		strcpy(userInfo[3], "qiaominghe");
		strcpy(userInfo[4], "qiaominghe");
		strcpy(userInfo[5], "qiaominghe");
		strcpy(userInfo[6], "qiaominghe");
		strcpy(userInfo[7], "qiaominghe");

	printf("insert into usrs(name,tel,mail,work,creater,create_time,privilege,passwd) values('%s','%s','%s','%s','%s','%s','%s','%s')",userInfo[0],userInfo[1],userInfo[2],userInfo[3],userInfo[4],userInfo[5],userInfo[6],userInfo[7]);
		Mysql my;
		//char userInfo[1024];
		int flaga = my.select_user("root");
		cout << flaga << endl;
		int flag = my.add_user(userInfo);
		cout << flag << endl;
		*/

		Mysql my;
		char *name= NULL;
	    char p[1024] = {0};
		//char *p = "00000000000";
		//char *pa = "11111111111";
		//int flag = my.add_role(name, p);
		//int flaga = my.modify_role(name, pa);
		int flaga = my.select_roles_name(&name);
		//cout << flag << endl;
		cout << flaga << endl;
		cout << name << endl;
		//delete p;
		return 1;
}
