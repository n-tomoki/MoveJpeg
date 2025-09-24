
// MoveJpeg.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


#define SELECT_RADIO_MAXNUM       5
#define SELECT_RADIO_NAME_SIZE  100
#define SELECT_RADIO_NAME_SIZE 1000


// CApp:
// このクラスの実装については、MoveJpeg.cpp を参照してください
//

class CApp : public CWinApp
{
public:
	CApp();
	virtual ~CApp();

// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装
private:
	CString m_strParamFileName;

	void InitPath();

public:
	CString m_strWorkPath;

	// パラメータファイル
	CString GetParamFileString  (const char *pszSection, const char *pszEntry, const char *pszDefault = "", const char *pszFileName = "");
	int     GetParamFileInt     (const char *pszSection, const char *pszEntry, const int nDefault = 0     , const char *pszFileName = "");
	BOOL    GetParamFileBOOL    (const char *pszSection, const char *pszEntry, const BOOL bDefault = FALSE, const char *pszFileName = "");
	DWORD   GetParamFileHex     (const char *pszSection, const char *pszEntry, const DWORD dwDefault = 0  , const char *pszFileName = "");
	BOOL    WriteParamFileString(const char *pszSection, const char *pszEntry, const char *pszValue       , const char *pszFileName = "");
	BOOL    WriteParamFileInt   (const char *pszSection, const char *pszEntry, const int nValue           , const char *pszFileName = "");
	BOOL    WriteParamFileBOOL  (const char *pszSection, const char *pszEntry, const BOOL bValue          , const char *pszFileName = "");
	BOOL    WriteParamFileHex   (const char *pszSection, const char *pszEntry, const DWORD nValue         , const char *pszFileName = "");
	// レジストリー
	CString RegGetParamFileString  (const char *pszSection, const char *pszEntry, const char *pszDefault = "");
	int     RegGetParamFileInt     (const char *pszSection, const char *pszEntry, const int nDefault = 0     );
	BOOL    RegGetParamFileBOOL    (const char *pszSection, const char *pszEntry, const BOOL bDefault = FALSE);
	DWORD   RegGetParamFileHex     (const char *pszSection, const char *pszEntry, const DWORD dwDefault = 0  );
	BOOL    RegWriteParamFileString(const char *pszSection, const char *pszEntry, const char *pszValue       );
	BOOL    RegWriteParamFileInt   (const char *pszSection, const char *pszEntry, const int nValue           );
	BOOL    RegWriteParamFileBOOL  (const char *pszSection, const char *pszEntry, const BOOL bValue          );
	BOOL    RegWriteParamFileHex   (const char *pszSection, const char *pszEntry, const DWORD nValue         );
	// パスの最後が'\'で終わってたら'TRUE'を返す
	BOOL    AnalyzePathFolder(const char *pszPath);
	// メッセージ処理
	BOOL    DoBackground();
	/// フォルダを選択する
	int     SelectFolder(CString &strPath, HWND hWnd);


public:
	DECLARE_MESSAGE_MAP()
};

extern CApp App;
