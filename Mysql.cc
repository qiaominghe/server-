#include "Mysql.h"

int Mysql::mysql_real_query1(char *sql)
{
	if (mysql_query(conn,sql)){
		fprintf(stderr,"%s\n\n",mysql_error(conn));
		fprintf(stderr,"%s\n",sql);

		return  (-1);
	}

	return (0);
}

int  Mysql::select_user_store(char *sql,char *user_info)
{	
	
	int reta = mysql_real_query1(sql);
	if ( 0 != reta){
		return (-1);
	}
     //process result  
    result = mysql_store_result(conn);  
    if (result == NULL){  
        if (mysql_error(conn))  
            fprintf(stderr, "%s\n", mysql_error(conn));  
        else  
            fprintf(stderr, "%s\n", "unknown error\n");  
        return -1;  
    }  

	row = mysql_fetch_row(result);
	if (!row)
	{
			cout << "the user you selected not exist!" << endl;
			return -2;
	}

	if (user_info == NULL)
	{
			return 0;
	}
	sprintf(user_info, "%s:%s:%s:%s:%s:%s:%s:%s", row[0], row[1],row[2],row[3],row[4],row[5],row[6],row[7]);

    return (0);
}	

Mysql::Mysql()
{
    //create connection to MySQL  
    conn = mysql_init(NULL);  
    if (mysql_real_connect(conn, "localhost", "root", "qiaominghe", "zhong_soft", 0, NULL, 0) == NULL){  
        fprintf(stderr, "Sorry, no database connection ...\n");
    mysql_query(conn, "SET NAMES 'utf8'");  
	printf("daole\n");
	}
}

Mysql::~Mysql()
{
    //release memory, server connection  
    mysql_free_result(result);  
    mysql_close(conn);  
}



int Mysql::add_user(char user_info[8][512])
{
	char sql[MAX_SQL_LEN];
	bzero(sql,MAX_SQL_LEN);
	sprintf(sql,"insert into usrs(name,tel,mail,work,creater,create_time,privilege,passwd) values('%s','%s','%s','%s','%s','%s','%s','%s')",user_info[0],user_info[1],user_info[2],user_info[3],user_info[4],user_info[5],user_info[6],user_info[7]);

	printf("ni hao%s\n", sql);	
	return mysql_real_query1(sql); 
}

int Mysql::del_user(char *name)
{
	char sql[MAX_SQL_LEN];
	bzero(sql,MAX_SQL_LEN);
	sprintf(sql,"delete from usrs where name = '%s'",name);

	return mysql_real_query1(sql);
}

int Mysql::select_users_name(char **p)
{
	char sql[MAX_SQL_LEN];
	
	bzero(sql,MAX_SQL_LEN);
	sprintf(sql,"select name from usrs");
	int ret = mysql_real_query1(sql);
	if (ret != 0)
			return -1;

	result = mysql_store_result(conn);
	unsigned int num = mysql_field_count(conn);
	unsigned int count = mysql_num_rows(result);
	cout << "users name count:" << count << endl;
	char *buffer = new char[count * 8];
	bzero(buffer, count * 8);
	while (row = mysql_fetch_row(result))
	{
			for (unsigned int i = 0; i < num; i++)
			{
					strcat(buffer, row[i]);
					strcat(buffer, ":");
			}
	}
	*p = buffer;

	return 0;
}
int Mysql::select_user(char* acnt,char *user_info)
{
 	if (NULL == acnt){	//如果输入acnt为NULL,则返回为-1
		return (-1);
	}

	char sql[MAX_SQL_LEN];
	
	bzero(sql,MAX_SQL_LEN);
	sprintf(sql,"select * from usrs where name = '%s'",acnt);
	cout << sql << endl;
	
	return	select_user_store(sql, user_info);
}


int Mysql::modify_user(char user_info[8][512])
{
	char sql[MAX_SQL_LEN];
	
	bzero(sql,MAX_SQL_LEN);
	sprintf(sql,"update usrs set name = '%s',tel = '%s',mail='%s',work = '%s',creater = '%s',create_time = '%s',privilege = '%s',passwd = '%s'where name = '%s'",user_info[0],user_info[1],user_info[2],user_info[3],user_info[4],user_info[5],user_info[6],user_info[7], user_info[0]);
	printf("update usrs set name = '%s',tel = '%s',mail='%s',work = '%s',creater = '%s',create_time = '%s',role = '%s',passwd = '%s'\n",user_info[0],user_info[1],user_info[2],user_info[3],user_info[4],user_info[5],user_info[6],user_info[7]);
	
	return mysql_real_query1(sql);	
}

