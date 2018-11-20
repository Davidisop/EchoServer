#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <tchar.h>

void main()
{
	//ȯ�� �ڵ�
	SQLHENV      g_hEnv;
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &g_hEnv) != SQL_SUCCESS)
	{
		return;
	}


	if (SQLSetEnvAttr(g_hEnv, SQL_ATTR_ODBC_VERSION,
		(SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
	{
		return;
	}



	//���� �ڵ�
	//ȯ���ڵ��� ���� ����� ȯ���ڵ��� ���ؼ� �����ڵ��� ����
	//ȯ���ڵ��� �θ� NULL�̰� �����ڵ� �θ�� ȯ���ڵ��̴�.

	SQLHDBC      g_hDbc;
	SQLAllocHandle(SQL_HANDLE_DBC, g_hEnv, &g_hDbc);

	// ���� --> *.mdb,"��ĭ�پ�� �Ѵ�."*.accdb

	//SQLTCHAR szInCon[256] = _T("DSN=YANG");



	SQLTCHAR szOutcon[256] = { 0, };
	SQLSMALLINT cbCon;
	SQLRETURN ret = SQLConnect(g_hDbc, (SQLTCHAR*)L"YANG", SQL_NTS, (SQLTCHAR*)L"sa", SQL_NTS, (SQLTCHAR*)L"kgca!@34", SQL_NTS);

	if (ret != SQL_SUCCESS && (ret != SQL_SUCCESS_WITH_INFO))
	{
		return;
	}

	//��� �ڵ�
	SQLHSTMT   g_hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, g_hDbc, &g_hStmt);


	// ���

	//// insert
	//TCHAR szSQL_2[] = _T("insert into TABLE_NEW (USERID, USERPASSWORD) values('EASTERM', 'RESURREN')");//��ɹ����� ���������
	//SQLExecDirect(g_hStmt, (SQLTCHAR*)szSQL_2, SQL_NTS); // ������ ���̽��� �ϴ� ���. g_hStmt�� ����ڵ�.


	//// edit

	//TCHAR szSQL_3[256] = _T("Update TABLE_NEW set USERID = 'AMPLIFIED' where USERID = 'AAA'");
	//SQLExecDirect(g_hStmt, (SQLTCHAR*)szSQL_3, SQL_NTS);



	// ���ν��� ���

	//?marker ��ȯ�� = ���ڰ� ?�� ��ȣ�� �ִ�.
	//SQL_PARAM_OUTPUT ����Ÿ��, SQL_PARAM_INPUT �Ű�����
	//1, 2 �� ? ��Ŀ�� ������� �־���Ѵ�.

	DWORD sReturn;
	SQLBindParameter(g_hStmt, 1, SQL_PARAM_OUTPUT, SQL_C_SHORT, SQL_INTEGER, 0, 0, &sReturn, 0, NULL);

	SQLTCHAR UserID[20] = { 'ISOP' };
	SQLBindParameter(g_hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_C_CHAR, 0, 0, &UserID, 0, NULL);

	if (SQLExecDirect(g_hStmt, (SQLTCHAR*)_T("{?=CALL YANG_FUNCTION(?)}"), SQL_NTS) != SQL_SUCCESS)
	{
		return;
	}



	//select
	SQLCHAR Name[10] = { 0, };
	SQLCHAR Password[10] = { 0, };
	SQLLEN lName, lPass;
	//SQL_C_CHAR ª�� �ؽ�Ʈ SQL_C_ULONG ����.
	//1�� �ʵ� 2�� �ʵ�.
	SQLBindCol(g_hStmt, 1, SQL_C_CHAR, Name, sizeof(Name), &lName);
	SQLBindCol(g_hStmt, 2, SQL_C_CHAR, Password, sizeof(Password), &lPass);

	//tblCigar���̺�κ��� name(�ʵ��)�� �����ض�.
	if (SQLExecDirect(g_hStmt, (SQLTCHAR*)_T("select USERID, USERPASSWORD from TABLE_NEW"), SQL_NTS) != SQL_SUCCESS)
	{
		return;
	}


	while (SQLFetch(g_hStmt) != SQL_NO_DATA)
	{
		printf("\n%s : [%s]", Name, Password);
	}


	SQLCloseCursor(g_hStmt);
	SQLDisconnect(g_hDbc);

	SQLFreeHandle(SQL_HANDLE_STMT, g_hStmt);
	SQLFreeHandle(SQL_HANDLE_DBC, g_hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, g_hEnv);

	int a = 1;
	return;
}