#include <iostream>
#include <string>
#include <string.h>
#include <time.h>

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
//#include <my_global.h>
using namespace std;

class TestString
{
public:
    TestString(){}
    virtual ~TestString(){}

	void setvalue(int value)
	{
		char buf[32];
		sprintf(buf, "%d", value);
		aa = buf;
	}
	string AsString()
	{
		return aa;
	}
private:
	string aa;
};


int main(int argc, char *argv[])
{
    char *str1 = "aaaaaaaaa";
	const char *str2 = str1;
	cout << str2 <<endl;
	string str3 = str2;
	cout << str3 <<endl;

	time_t ti = time(NULL);
	struct tm *ptm = localtime(&ti);
	char buf[20];
	strftime(buf, sizeof(buf), "%F", ptm);
	cout << buf << endl;

	MYSQL *conn;  
    MYSQL_RES *res;  
    MYSQL_ROW row;  
    char *server = "127.0.0.1";  
    char *user = "root";  
    char *password = "mysql";  
    char *database = "test";  
    conn = mysql_init(NULL);  
    
    if (!mysql_real_connect(conn, server, \  
                            user, password, database, 0, NULL, 0)) {  
        fprintf(stderr, "%s\n", mysql_error(conn));  
        exit(1);  
    }  
    
    if (mysql_query(conn, "show tables")) {  
        fprintf(stderr, "%s\n", mysql_error(conn));  
        exit(1);  
    }  
    res = mysql_use_result(conn);  
    
    printf("MySQL Tables in mysql database:\n");  
    while ((row = mysql_fetch_row(res)) != NULL)  
        printf("%s \n", row[0]);      //获取数据库文件名  
    
    mysql_free_result(res);  
    mysql_close(conn);  

	TestString testStr;
	testStr.setvalue(123456);
	cout << testStr.AsString() <<endl;

	
    return 0;
}

