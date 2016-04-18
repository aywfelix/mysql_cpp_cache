#include "MySqlDB.h"

string dbServer = "127.0.0.1";
string dbUser = "root";
string dbPwd = "mysql";
string dbDatabase = "test";

int main(int argc, char *argv[])
{
    CConnection conn;
    CRecordset rec;
	CSqlString strSql;
	if(!conn.ConnectDB(dbServer, dbUser, dbPwd, dbDatabase))
	{
		fprintf(stderr, "connect to database err\n");
		return -1;
	}

	//test1
    strSql = "select * from user";
	if(conn.ExecuteSQL(strSql, &rec))
	{
		printf("excute ok\n");
	}
	int rows = rec.GetRows();
	int columns = rec.GetColumns();
	cout << rows << " "<<columns <<endl;
	while(!rec.IsEOF())
	{
		for(int i=0;i<columns; ++i)
		{
			cout << rec.GetFieldName(i) << ":" <<rec.GetFieldValue(i).AsString() << " ";
		}
		cout << endl;
		rec.MoveNext();
	}
	//test2
	// strSql = "update user set age=20 where id = @id";
	// strSql.SetParam("@id", 1);
	// cout <<"sql:" <<(const char*)strSql << endl;
	// if(conn.ExecuteSQL(strSql))
	// {
	// 	printf("excute ok\n");
	// }

	//test3  rows start from 0
	// rec.Moveto(1);
	// while(!rec.IsEOF())
	// {
	// 	for(int i=0;i<columns; ++i)
	// 	{
	// 		cout << rec.GetFieldName(i) << ":" <<rec.GetFieldValue(i).AsString() << " ";
	// 	}
	// 	cout << endl;
	// 	rec.MoveNext();
	// }

	//test4
	// strSql = "insert into user(name, age, sex, password) values('ÄÝÄÝ', 18, 1, 'nini')";
	// cout <<"sql:" <<(const char*)strSql << endl;
	// if(conn.ExecuteSQL(strSql))
	// {
	// 	printf("excute ok\n");
	// }

	//test5
    return 0;
	
}


//g++ -g -o  test testMysql.cpp MySqlDB.h MySqlDB.cpp -I/usr/local/mysql/include -L/usr/local/mysql/lib/libmysqlclient -lmysqlclient
