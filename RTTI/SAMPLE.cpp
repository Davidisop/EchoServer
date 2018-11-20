#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <tchar.h>

void main()
{
	//환경 핸들
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



	//연결 핸들
	//환경핸들을 먼저 만들고 환경핸들을 통해서 연결핸들을 생성
	//환경핸들은 부모가 NULL이고 연결핸들 부모는 환경핸들이다.

	SQLHDBC      g_hDbc;
	SQLAllocHandle(SQL_HANDLE_DBC, g_hEnv, &g_hDbc);

	// 주의 --> *.mdb,"한칸뛰어야 한다."*.accdb

	//SQLTCHAR szInCon[256] = _T("DSN=YANG");



	SQLTCHAR szOutcon[256] = { 0, };
	SQLSMALLINT cbCon;
	SQLRETURN ret = SQLConnect(g_hDbc, (SQLTCHAR*)L"YANG", SQL_NTS, (SQLTCHAR*)L"sa", SQL_NTS, (SQLTCHAR*)L"kgca!@34", SQL_NTS);

	if (ret != SQL_SUCCESS && (ret != SQL_SUCCESS_WITH_INFO))
	{
		return;
	}

	//명령 핸들
	SQLHSTMT   g_hStmt;
	SQLAllocHandle(SQL_HANDLE_STMT, g_hDbc, &g_hStmt);


	// 출력

	//// insert
	//TCHAR szSQL_2[] = _T("insert into TABLE_NEW (USERID, USERPASSWORD) values('EASTERM', 'RESURREN')");//명령문장이 만들어지면
	//SQLExecDirect(g_hStmt, (SQLTCHAR*)szSQL_2, SQL_NTS); // 데이터 베이스에 하는 명령. g_hStmt는 명령핸들.


	//// edit

	//TCHAR szSQL_3[256] = _T("Update TABLE_NEW set USERID = 'AMPLIFIED' where USERID = 'AAA'");
	//SQLExecDirect(g_hStmt, (SQLTCHAR*)szSQL_3, SQL_NTS);



	// 프로시져 사용

	//?marker 반환값 = 인자값 ?이 번호가 있다.
	//SQL_PARAM_OUTPUT 리턴타입, SQL_PARAM_INPUT 매개변수
	//1, 2 는 ? 마커의 순서대로 넣어야한다.

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
	//SQL_C_CHAR 짧은 텍스트 SQL_C_ULONG 숫자.
	//1번 필드 2번 필드.
	SQLBindCol(g_hStmt, 1, SQL_C_CHAR, Name, sizeof(Name), &lName);
	SQLBindCol(g_hStmt, 2, SQL_C_CHAR, Password, sizeof(Password), &lPass);

	//tblCigar테이블로부터 name(필드명)을 리턴해라.
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