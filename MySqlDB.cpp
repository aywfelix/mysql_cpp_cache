#include "MySqlDB.h"

string int2string(int n)
{
	stringstream ss;
	ss << n;
	return ss.str();
}
int string2int(string str)
{
	stringstream ss(str);
	int n = 0;
    ss >> n;
	return n;
}

//CVariant class
CVariant::CVariant():m_strvar(""), m_pData(NULL), m_DataLen(0), m_type(varNull)
{}

CVariant::~CVariant()
{
	if(m_pData != NULL)
	{
		free(m_pData);
		m_pData = NULL;
	}
}

CVariant::CVariant(const CVariant& var)
{
	m_strvar = var.m_strvar;
    memcpy(m_pData, var.m_pData, m_DataLen);
	m_DataLen = var.m_DataLen;
	m_type = var.m_type;
}

CVariant::CVariant(string& var, CVariant::VAR_TYPE vartype)
{
	m_strvar = var;
	m_type = vartype;
	m_pData = NULL;
}
CVariant::CVariant(int var)
{
	char buf[33];
	memset(buf, '\0', sizeof(buf));
	snprintf(buf, 32, "%d", var);
	m_strvar = buf;
	m_type = varInt;
	m_pData = NULL;
}

CVariant::CVariant(long long var)
{
	char buf[65];
	memset(buf, '\0', sizeof(buf));
	snprintf(buf, 64, "%lld", var);
	m_strvar = buf;
	m_type = varLLong;
	m_pData = NULL;
}

CVariant::CVariant(double var)
{
	char buf[24];
	memset(buf, '\0', sizeof(buf));
	snprintf(buf, 23, "%.8f", var);
	m_strvar = buf;
	m_type = varDouble;
	m_pData = NULL;
}

CVariant::CVariant(string& var)
{
	m_strvar = var;
	m_type = varString;
}

CVariant::CVariant(const char* var)
{
	m_strvar = var;
	m_type = varString;
	m_pData = NULL;
}

CVariant::CVariant(bool var)
{
	char buf[2];
	if(var)
	    snprintf(buf, 1, "%d", 1);
	else
		snprintf(buf, 1, "%d", 0);
	m_type = varBool;
	m_pData = NULL;
}

CVariant::CVariant(time_t var)
{
	char buf[20];
	memset(buf, '\0', sizeof(buf));
	struct tm *ptm;
	ptm = localtime(&var);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ptm);
	m_strvar = buf;
	m_type = varDateTime;
	m_pData = NULL;
}

CVariant::CVariant(void* var, unsigned int len)
{
	m_pData = (void*)malloc(len);
	if(m_pData)
	{
		memcpy(m_pData, var, len);
		m_DataLen = len;
		m_type = varBinary;		
	}
	else
	{
		m_pData = NULL;
		m_DataLen = 0;
	}
}

CVariant::CVariant(CRecordset& var)
{
	m_strvar = var.ToString();
	m_type = varRecordSet;
}

CVariant& CVariant::operator=(const CVariant& var)
{
	if(this == &var)
		return *this;
	else
	{
		m_strvar = var.m_strvar;
		m_pData = var.m_pData;
		m_DataLen = var.m_DataLen;
		m_type = var.m_type;
		return *this;
	}
}

CVariant::operator string()
{
	if(m_type = varBinary)
	{
		char buf[32];
		sprintf(buf, "[Binary Data Len=%d]", m_DataLen);
		return buf;
	}
	else		
		return m_strvar; 
}

CVariant::operator const char*()
{
	if(m_type = varBinary)
	{
	    char buf[32];
		sprintf(buf, "[Binary Data Len=%d]", m_DataLen);
		string str = buf;
		return str.c_str();
	}
	else		
		return m_strvar.c_str();
}

CVariant::operator long long()
{
	long long lldata;
	switch(m_type)
	{
	case varInt:
	case varLLong:
	case varDouble:
	case varBool:
	case varString:
		if(sscanf(m_strvar.c_str(), "%lld", &lldata))
			return lldata;
		else
			return 0L;
	case varDateTime:
		struct tm ttm;
		if(sscanf(m_strvar.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &ttm.tm_year, &ttm.tm_mon, &ttm.tm_mday, &ttm.tm_hour, &ttm.tm_min, &ttm.tm_sec) == 6)
		{
			ttm.tm_mon -= 1;
			ttm.tm_year -= 1900;
			return mktime(&ttm);
		}
	case varBinary:
	case varRecordSet:
	case varNull:
		return 0L;
	default:
		return 0L;
	}
}

CVariant::operator int()
{
	int in;
	switch(m_type)
	{
	case varDouble:
		int in2;
		if(sscanf(m_strvar.c_str(), "%d.%d", &in,&in2) == 2)
			if(in2 >= 0.5)
			    return in+1;
		    else
				return in;
	case varInt:
	case varLLong:
	case varBool:
	case varString:
		errno = 0;
		in = strtol(m_strvar.c_str(), NULL, 10);
		if(errno == ERANGE && (in == LONG_MAX || in == LONG_MIN))
		{
			fprintf(stderr, "convert failed\n");
			return 0L;
		}
		else 
			return in;
	case varBinary:
	case varNull:
	case varRecordSet:
	case varDateTime:
		return 0;
	default:
		return 0;
	}
}

CVariant::operator double()
{
	double dn;
	switch(m_type)
	{
	case varInt:
	case varLLong:
	case varDouble:
	case varString:
	case varBool:
		errno = 0;
		dn = strtod(m_strvar.c_str(), NULL);
		if(errno == ERANGE && (dn == LONG_MAX || dn == LONG_MIN))
		{
			fprintf(stderr, "convert to double failed\n");
			return 0;
		}else
			return dn;
	case varBinary:
	case varDateTime:
	case varRecordSet:
	case varNull:
	default:
		return 0;
	}
}

CVariant::operator bool()
{
	int in;
	if(m_type == varBool)
	{
		if(sscanf(m_strvar.c_str(), "%d", &in) == 1)
		{
			if(in == 1)
				return true;
			return false;
		}			
	}
	return false;
}

CVariant::operator time_t()
{
	if(m_type == varDateTime)
	{
		struct tm ttm;
		if(sscanf(m_strvar.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &ttm.tm_year, &ttm.tm_mon, &ttm.tm_mday, &ttm.tm_hour, &ttm.tm_min, &ttm.tm_sec) == 6)
		{
			ttm.tm_mon -= 1;
			ttm.tm_year -= 1900;
			return mktime(&ttm);
		}
	}
	else
		return time(NULL);
}

CVariant::operator CRecordset()
{
	if(m_type == varRecordSet)
	{
		if(m_strvar.size() == 0)
		{
			return CRecordset();
		}
		return CRecordset(m_strvar);		
	}
	else
		return CRecordset();
}

CVariant::operator void*()
{
	if(m_type == varBinary)
	{
		return m_pData;
	}
	return NULL;
}

string CVariant::AsString()
{
	if(m_type == varRecordSet || m_type == varBinary)
    {
        return "";
    }
	return m_strvar;
}
string CVariant::AsDate()
{
	if(m_type == varDateTime)
	{
	    string date = m_strvar.substr(0, m_strvar.find(' '));
		return date;
	}
	else
	{
		char buf[20];
		struct tm *ptm;
		time_t ti = time(NULL);
		ptm = localtime(&ti);
		strftime(buf, sizeof(buf), "%F", ptm);
		return string(buf);
	}
}
string CVariant::AsYear()
{
	if(m_type == varDateTime)
	{
	    string date = m_strvar.substr(0, m_strvar.find('-'));
		return date;
	}
	else
	{
		char buf[20];
		struct tm *ptm;
		time_t ti = time(NULL);
		ptm = localtime(&ti);
		strftime(buf, sizeof(buf), "%Y", ptm);
		return string(buf);
	}
}
string CVariant::AsTime()
{
	struct tm ttm;
	if(sscanf(m_strvar.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &ttm.tm_year, &ttm.tm_mon, &ttm.tm_mday, &ttm.tm_hour, &ttm.tm_min, &ttm.tm_sec) == 6)
	{
		ttm.tm_mon -= 1;
		ttm.tm_year -= 1900;
		char buf[32];
		sprintf(buf, "%lld", mktime(&ttm));
		return buf;
	}
}

CVariant::VAR_TYPE CVariant::getType()
{
	return m_type;
}
unsigned int CVariant::getLength()
{
	return m_DataLen;
}
bool CVariant::isNull()
{
	return (m_pData == NULL || (m_strvar.empty() && m_DataLen == 0));
}

bool CVariant::Clear()
{
	if(m_pData != NULL)
	{
		free(m_pData);
		m_pData = NULL;
	}
	m_strvar.clear();
	m_DataLen = 0;
	m_type = varNull;
}


//CSqlString sql语句处理类

CSqlString::CSqlString()
{
	m_sql = "";
}
CSqlString::~CSqlString()
{}
CSqlString::CSqlString(const CSqlString& csql)
{
	m_sql = csql.m_sql;
}

CSqlString::CSqlString(const char* csql)
{
	m_sql = csql;
}
CSqlString::CSqlString(const string& csql)
{
	m_sql = csql;
}

CSqlString& CSqlString::operator=(const CSqlString& csql)
{
	if(this == &csql)
		return *this;
	else
	{
		m_sql = csql.m_sql;
		return *this;
	}	
}

CSqlString& CSqlString::operator=(const char* csql)
{
	m_sql = csql;
	return *this;
}
CSqlString& CSqlString::operator=(const string& csql)
{
	m_sql = csql;
	return *this;
}
CSqlString& CSqlString::operator=(string& csql)
{
	m_sql = csql;
	return *this;
}
CSqlString& CSqlString::operator+=(const char* csql)
{
	string tmp = csql;
	m_sql = m_sql+tmp;
	return *this;
}
CSqlString& CSqlString::operator+=(string& csql)
{
	m_sql = m_sql + csql;
	return *this;
}
CSqlString& CSqlString::operator+=(CSqlString& csql)
{
	m_sql = m_sql + csql.m_sql;
	return *this;
}
CSqlString operator+(const CSqlString& csql1,const CSqlString& csql2)
{
	string strsql = csql1.m_sql + csql2.m_sql;
	return CSqlString(strsql);
}
CSqlString operator+(const CSqlString& csql1, const char* csql2)
{
	string strsql = csql1.m_sql + csql2;
	return CSqlString(strsql);
}
CSqlString operator+(const char* csql1, const CSqlString& csql2)
{
	string strsql = csql2.m_sql + csql1;
	return CSqlString(strsql);
}

void CSqlString::SetParam(const char *str, int var)
{
	char buf[32];
	sprintf(buf, "%d", var);
	SetParam(str, buf);
}
void CSqlString::SetParam(const char *str, long long var)
{
	char buf[64];
	sprintf(buf, "%lld", var);
	SetParam(str, buf);
}
void CSqlString::SetParam(const char *str, double var)
{
	char buf[64];
	sprintf(buf, "%.8f", var);
	SetParam(str, buf);
}
void CSqlString::SetParam(const char *str, const string& var)
{
	SetParam(str, var.c_str());
}
void CSqlString::SetParam(const char *str, const char *var)
{
	unsigned int pos = 0;
	int len = strlen(str);
	int newlen = strlen(var);
	pos = m_sql.find(str);
	while(pos != string::npos)
	{
		m_sql.replace(pos, len, var);
		pos = m_sql.find(str, pos + newlen);
	}
}

CSqlString::operator const char*()
{
	return m_sql.c_str();
}

//CConnection class

CConnection::CConnection()
{
	if(mysql_init(&mysql) == NULL)
	{
		fprintf(stderr, "init mysql failed:%s\n", mysql_error(&mysql));
	}
}

CConnection::~CConnection()
{
	mysql_close(&mysql);
}

bool CConnection::ConnectDB(const string& host, const string& user, const string& password, const string& database)
{
	if(mysql_real_connect(&mysql, host.c_str(), user.c_str(), password.c_str(), database.c_str(), 0, NULL, 0) == NULL)
	{
		fprintf(stderr, "connect to mysql failed:%s\n", mysql_error(&mysql));
		return false;
	}
	//mysql_set_character_set(&mysql, "gbk");
	mysql_real_query(&mysql, "SET NAMES GBK", 15);
	SetAutoCommit(0); //关闭自动提交
	return true;
}
void CConnection::SetCharacter(const char* character)
{
	mysql_set_character_set(&mysql, character);
}
bool CConnection::SetAutoCommit(int mode)
{
	if(mysql_autocommit(&mysql, 0) !=0) //关闭自动提交
	{
		fprintf(stderr, "begin transaction failed:%s\n", mysql_error(&mysql));
		return false;
	}
	return true;
}

bool CConnection::BeginTrans()
{
	bool res = SetAutoCommit(0);
	return res;
}

bool CConnection::EndTrans()
{
	if(mysql_commit(&mysql) != 0)
	{
		fprintf(stderr, "commmit transaction failed:%s\n", mysql_error(&mysql));
		return false;
	}
	return true;
}

bool CConnection::RollBack()
{
	if(mysql_rollback(&mysql) != 0)
	{
		fprintf(stderr, "rollback transaction failed:%s\n", mysql_error(&mysql));
		return false;
	}
	return true;
}

bool CConnection::ConnectStatus()
{
	if(mysql_ping(&mysql) != 0)
		return false;
	return true;
}

bool CConnection::ExecuteSQL(const string& strsql, CRecordset* recset)
{
	MYSQL_RES *result;
	MYSQL_FIELD *fields;
	MYSQL_ROW row;
	int nfields = 0;
	int res = 0;
	if(recset == NULL)
	{
		BeginTrans();
		if((res = mysql_real_query(&mysql, strsql.c_str(), strsql.length())) !=0)
		{
			fprintf(stderr, "excute sql failed:%s\n", mysql_error(&mysql));
			RollBack();
			return false;
		}
		EndTrans();
		return true;
	}
	if((res = mysql_real_query(&mysql, strsql.c_str(), strsql.length())) !=0)
	{
		fprintf(stderr, "excute sql failed:%s\n", mysql_error(&mysql));
		return false;
	}
	if((result = mysql_store_result(&mysql)) == NULL)
	{
		fprintf(stderr, "store sql result failed:%s\n", mysql_error(&mysql));
		return false;
	}
	fields = mysql_fetch_fields(result);
    nfields = mysql_num_fields(result);
	for(int i = 0;i<nfields; ++i)
	{
        //这里可能会因为数据类型有问题,没有去仔细测试
		switch(fields[i].type)
		{
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
			recset->FieldAdd(fields[i].name, CVariant::varInt);
			break;
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_FLOAT:
			recset->FieldAdd(fields[i].name, CVariant::varDouble);
			break;
		case MYSQL_TYPE_LONGLONG:
			recset->FieldAdd(fields[i].name, CVariant::varLLong);
			break;
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_TIME2:
		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_DATETIME2:
			recset->FieldAdd(fields[i].name, CVariant::varDateTime);
			break;
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:
			recset->FieldAdd(fields[i].name, CVariant::varString);
			break;
		case MYSQL_TYPE_BIT:
			recset->FieldAdd(fields[i].name, CVariant::varBool);
			break;
		case MYSQL_TYPE_NULL:
			recset->FieldAdd(fields[i].name, CVariant::varNull);
			break;
		default:
			fprintf(stderr, "err type\n");
			return false;
			break;
		}	
	}

	while((row = mysql_fetch_row(result)))
	{
		recset->AddNew();
		for(int i = 0; i< nfields; ++i)
		{
			recset->FieldSetValue(i, row[i]);
		}
		recset->Update();
	}
	if(result != NULL)
		mysql_free_result(result);
	return true;
}

bool CConnection::ExecuteSQL(const char* strsql, CRecordset* recset)
{
	string sql = strsql;
	bool res = ExecuteSQL(sql, recset);
	return res;
}
bool CConnection::ExecuteSQL(CSqlString& strsql, CRecordset* recset)
{
	string sql = (const char*)strsql;
	bool res = ExecuteSQL(sql, recset);
	return res;	
}

//CRecordSet class

CRecordset::CRecordset():rows(0),columns(0),allrows(0),allcolumns(0)
{
	vecRecords.clear();
	for(int i=0;i<256; ++i)
	{
	    fieldvalue[i]= "";
		fieldtype[i] = CVariant::varNull;
		fieldname[i] = "";
	}
}

CRecordset::CRecordset(const string& strval)
{
	int pos = 0, end = 0, nfields=0;
	end = strval.find('|', pos);
	while(end != string::npos)
	{
		string tmp = strval.substr(pos, end - pos);
		fieldtype[nfields] = (CVariant::VAR_TYPE)(tmp[pos]-'0');
		++nfields;
		pos = ++end;
		end = strval.find('|', pos);
	}
	int cnt = 0;
	string records = strval.substr(end);
	cout << "records == "<< records <<endl;
	pos = end = 0;
	while(true)
	{
		end = records.find('\x07', pos);
		if(end != string::npos)
			++cnt;
		if(end == string::npos)
			break;
		if(cnt == nfields)
		{
			string record = records.substr(pos, end-pos+1);
			vecRecords.push_back(record);
			cnt = 0;
		}
		pos = ++end;
	}

	allrows = vecRecords.size();
	allcolumns = nfields;
}

CRecordset::CRecordset(const CRecordset& rec)
{
	vecRecords.assign(rec.vecRecords.begin(), rec.vecRecords.end());
	rows = rec.rows;
	columns = rec.columns;
	allrows = rec.allrows;
	allcolumns = rec.allcolumns;
	for(int i=0;i<columns; ++i)
	{
		fieldvalue[i] = rec.fieldvalue[i];
		fieldtype[i] = rec.fieldtype[i];
		fieldname[i] = rec.fieldname[i];
	}
}
CRecordset& CRecordset::operator=(const CRecordset& rec)
{
	if(this == &rec)
		return *this;
	else
	{
		vecRecords.assign(rec.vecRecords.begin(), rec.vecRecords.end());
		rows = rec.rows;
		columns = rec.columns;
		allrows = rec.allrows;
		allcolumns = rec.allcolumns;
		for(int i=0;i<columns; ++i)
		{
			fieldvalue[i] = rec.fieldvalue[i];
			fieldtype[i] = rec.fieldtype[i];
			fieldname[i] = rec.fieldname[i];
		}
		return *this;
	}
}
CRecordset::~CRecordset()
{
	vecRecords.clear();
}

CVariant CRecordset::GetFieldValue(int column)
{
    string record = vecRecords[rows];
	int pos = 0, end = 0;
	while(pos != string::npos)
	{
		end = record.find('\x07', pos);
		if(column == 0)
			break;
        pos = ++end;
		--column;
	}
	string value = record.substr(pos, end - pos);
	return CVariant(value, fieldtype[column]);
}

CVariant CRecordset::GetFieldValue(string& column)
{
	return GetFieldValue(GetFieldIndex(column));	
}

vector<CVariant> CRecordset::GetRecordValue(int row)
{
	vector<CVariant> vec;
	string record = vecRecords[row];
	int pos = 0;
	int end = record.find('\x07', pos);
	while(end != string::npos)
	{
		string str = record.substr(pos, end-pos);
		vec.push_back(str);
		pos = ++end;
		end = record.find('\x07', pos);
	}
	return vec;
}
	
void CRecordset::FieldSetValue(int column, string& svar)
{
	fieldvalue[column] = svar;
}
void CRecordset::FieldSetValue(int column, char* cvar)
{
	fieldvalue[column] = cvar;
}

void CRecordset::FieldSetValue(int column, int ivar)
{
	char buf[32];
	sprintf(buf, "%d", ivar);
	fieldvalue[column] = buf;
}
void CRecordset::FieldSetValue(int column, double dvar)
{
	char buf[64];
	sprintf(buf, "%.8f", dvar);
	fieldvalue[column] = buf;
}
void CRecordset::FieldSetValue(int column, long long llvar)
{
	char buf[128];
	sprintf(buf, "%lld", llvar);
	fieldvalue[column] = buf;
}
void CRecordset::FieldSetValue(int column, time_t tvar)
{
	char buf[32];
	struct tm *ptm = localtime(&tvar);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ptm);
	fieldvalue[column] = buf;
}


string CRecordset::GetFieldName(int column)
{
	return (fieldname[column]).length()>0?fieldname[column]:"";
}
int CRecordset::GetFieldIndex(string& column)
{
	for(int i=0; i<columns; ++i)
	{
		if(fieldname[i] == column)
			return i;
	}
	return -1;
}
CVariant::VAR_TYPE CRecordset::FieldGetType(string& column)
{
	int index = GetFieldIndex(column);
	return fieldtype[index];
}
CVariant::VAR_TYPE CRecordset::FieldGetType(int column)
{
	return fieldtype[column];
}
void CRecordset::FieldSetType(int index, CVariant::VAR_TYPE vartype)
{
	fieldtype[index] = vartype;
}
void CRecordset::FieldSetType(string& column, CVariant::VAR_TYPE vartype)
{
	int index = GetFieldIndex(column);
	fieldtype[index] = vartype;
}
void CRecordset::FieldAdd(const string& column, CVariant::VAR_TYPE vartype)
{
	fieldname[columns] = column;
	fieldtype[columns] = vartype;
	++columns;
}
void CRecordset::AddNew()
{
	for(int i=0;i<columns; ++i)
	{
		fieldvalue[i] = "";
	}
}
void CRecordset::Update()
{
	string records = "";
	for(int i=0;i<columns; ++i)
	{
		records += fieldvalue[i];
		records += '\x07';
	}
	vecRecords.push_back(records);
}
int CRecordset::GetRows()
{
	allrows = vecRecords.size();
	return allrows;
}
int CRecordset::GetColumns()
{
	allcolumns = columns;
	return allcolumns;
}
void CRecordset::MoveNext()
{
	++rows;
}
void CRecordset::MoveFirst()
{
	rows = 0;
}
void CRecordset::Moveto(int row)
{
	rows = row;
}
void CRecordset::Clear()
{
	rows = columns = allrows = allcolumns = 0;
	vecRecords.clear();
}
bool CRecordset::IsEOF()
{
	allrows = vecRecords.size();
	if(rows >= allrows)
		return true;
	return false;
}

bool CRecordset::IsEmpty()
{
	if(vecRecords.size() == 0)
		return true;
	return false;
}

string CRecordset::ToString()
{
	string res = "";
	for(int i=0;i<256; ++i)
	{
		if(fieldtype[i] == CVariant::varNull)
			break;
		res += (char)((char)fieldtype[i]+'0');
		res += '|';
	}
	for(int i=0;i<vecRecords.size(); ++i)
	{
		res += (string)vecRecords[i];		
	}
	return res;
}

