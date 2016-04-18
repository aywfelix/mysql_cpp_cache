#include <mysql.h>  
#include <my_global.h>  
#include <stdio.h>

int main()  
{  
    MYSQL *conn;  
    MYSQL_RES *res;  
    MYSQL_ROW row;  
    char *server = "localhost";  
    char *user = "root";  
    char *password = "mysql";  
    char *database = "testdb";  
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
  
  
    return 0;  
}  
