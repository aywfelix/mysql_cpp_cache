/*
 * Filename: MySqlDB.h
 *                
 * Version:       
 * Author:        LaiJia <laijia2008@126.com>
 * Created at:    Tue Apr 12 14:26:12 2016
 *                
 * Description:   将数据库数据缓存到内存，模拟关系数据库方式存储,可以随意查找某一行记录，某一列数据
 *
 */

#include <iostream>
#include <stdio.h>
#include <sstream>
#include <vector>
#include <map>
#include <time.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include <mysql.h>
#include <mysql_com.h>
using namespace std;


class CVariant;
class CRecordset;
class CSqlString;
class CConnection;

//统一类型的一个类
class CVariant
{
public:
	enum VAR_TYPE{varNull = 0, varInt=1, varLLong=2, varDouble=3, varString=4, varBool=5, varBinary=6, varDateTime=7, varRecordSet=8};
    CVariant();
	CVariant(const CVariant& var);
	CVariant(string& var, CVariant::VAR_TYPE vartype);
	CVariant(int var);
	CVariant(long long var);
	CVariant(double var);
	CVariant(string& var);
	CVariant(const char* var);
	CVariant(bool var);
	CVariant(time_t var);
	CVariant(void* var, unsigned int len);
	CVariant(CRecordset& var); //代表一条记录
    virtual ~CVariant();

	CVariant& operator=(const CVariant& var);
	operator string();
	operator const char*();
	operator long long();
	operator int();
	operator double();
	operator bool();
	operator time_t();
	operator CRecordset();
    operator void*();

	string AsDate(); //1999-08-09
	string AsYear(); //1999
	string AsTime(); //1999-08-09 10:10:08
	string AsString();

	VAR_TYPE getType();
	unsigned int getLength();
	bool isNull();
	bool Clear();
private:
	string m_strvar;
	void *m_pData;
	unsigned int m_DataLen;
	VAR_TYPE m_type;	
};

//sql语句处理类
class CSqlString
{
public:
    CSqlString();
	CSqlString(const CSqlString& csql);
	CSqlString(CSqlString& csql);
	CSqlString(const char* csql);
	CSqlString(const string& csql);

	CSqlString& operator=(const CSqlString& csql);
	CSqlString& operator=(const char* csql);
	CSqlString& operator=(const string& csql);
	CSqlString& operator=(string& csql);
	
	CSqlString& operator+=(const char* csql);
	CSqlString& operator+=(string& csql);
	CSqlString& operator+=(CSqlString& csql);
	
friend CSqlString operator+(const CSqlString& csql1,const CSqlString& csql2);
friend CSqlString operator+(const CSqlString& csql1, const char* csql2);
friend CSqlString operator+(const char* csql1, const CSqlString& csql2);

	void SetParam(const char *str, int var);
    void SetParam(const char *str, long long var);
    void SetParam(const char *str, double var);
    void SetParam(const char *str, const char *var);
    void SetParam(const char *str, const string &var);
	
	operator const char*();
    virtual ~CSqlString();
private:
	string m_sql;
};

//数据库处理类

class CConnection
{
public:
    CConnection();
    virtual ~CConnection();
	bool ConnectDB(const string& host, const string& user, const string& password, const string& database);
	bool ExecuteSQL(const string& strsql, CRecordset* recset = NULL);
	bool ExecuteSQL(const char* strsql, CRecordset* recset = NULL);
	bool ExecuteSQL(CSqlString& strsql, CRecordset* recset = NULL);
	void SetCharacter(const char* character);
	bool SetAutoCommit(int mode);
	bool BeginTrans();
	bool EndTrans();
	bool RollBack();
	bool ConnectStatus();
	
private:
	CConnection(const CConnection&);
	CConnection& operator=(const CConnection&);
	MYSQL mysql;
};

//数据库缓存处理类
class CRecordset
{
public:
    CRecordset();
	CRecordset(const CRecordset& rec);
	CRecordset(const string& strval);
	CRecordset& operator=(const CRecordset& rec);
    virtual ~CRecordset();
	CVariant GetFieldValue(int column);
	CVariant GetFieldValue(string& column);
	vector<CVariant> GetRecordValue(int row);
	
	void FieldSetValue(int column, string& strvar);
	void FieldSetValue(int column, char* cvar);
	void FieldSetValue(int column, int ivar);
	void FieldSetValue(int column, double dvar);
	void FieldSetValue(int column, long long llvar);
	void FieldSetValue(int column, time_t tvar);
	
	string GetFieldName(int column);
	int GetFieldIndex(string& column);
	
	CVariant::VAR_TYPE FieldGetType(string& column);
	CVariant::VAR_TYPE FieldGetType(int column);
	void FieldSetType(int index, CVariant::VAR_TYPE vartype);
	void FieldSetType(string& column, CVariant::VAR_TYPE vartype);
	
	void FieldAdd(const string& column, CVariant::VAR_TYPE vartype);
	void AddNew();
	void Update();
	int GetRows();
	int GetColumns();
	void MoveNext();
	void MoveFirst();
	void Moveto(int row);
	void Clear();
	bool IsEOF();
    bool IsEmpty();
		
	string ToString();
private:
	vector<string> vecRecords;
	int rows, columns, allrows, allcolumns;
	string fieldvalue[256];
	CVariant::VAR_TYPE fieldtype[256];
	string fieldname[256];
};
