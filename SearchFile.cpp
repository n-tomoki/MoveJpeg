
#include "pch.h"
#include "MoveJpeg.h"
//#include "flag.h"
#include "lib.h"
#include "SearchFile.h"



/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 開始＆終了処理
//===========================================================================

/// <summary>コンストラクタ</summary>
CSearchFile::CSearchFile()
{
}


/// <summary>デストラクタ</summary>
CSearchFile::~CSearchFile()
{
	End();
}


/// <summary>終了処理(メモリ解放)</summary>
void CSearchFile::End()
{
	int i;
	int nSize = (int)m_arrFileName.GetSize();

	if (nSize) {
		for (i = 0; i < nSize; i++) {
			CBase *p = (CBase *)m_arrFileName.GetAt(0);
			delete p;
			m_arrFileName.RemoveAt(0);
		}
	}

	nSize = (int)m_arrFileExt.GetSize();
	if (nSize) {
		for (i = 0; i < nSize; i++) {
			char *p = (char *)m_arrFileExt.GetAt(0);
			delete []p;
			m_arrFileExt.RemoveAt(0);
		}
	}
}


//---------------------------------------------------------------------------
// 初期化
//---------------------------------------------------------------------------

/// <summary>初期化</summary>
void CSearchFile::Init()
{
	End();
}


//===========================================================================
// ファイルの検索
//===========================================================================

/// <summary>
/// ファイルの検索
/// </summary>
/// <param name="pszSearchPath">検索フォルダ</param>
/// <returns>正常：0/エラー:1</returns>
int CSearchFile::Main(const char *pszSearchPath)
{
	BOOL bSubFolder = FALSE;

	if (!GetExtSize()) {
		App.m_pMainWnd->MessageBoxA("検索する拡張子が登録されていません", "異常事態", MB_ICONERROR|MB_OK);
		return 1;
	}

	if (IDYES == App.m_pMainWnd->MessageBoxA("サブホルダーも検索しますか？", "確認", MB_ICONQUESTION|MB_YESNO)) {
		bSubFolder = TRUE;
	}

	return SearchMain(pszSearchPath, bSubFolder);
}


/// <summary>
/// ファイルの検索(メイン)
/// </summary>
/// <param name="pszSearchPath">検索フォルダ</param>
/// <param name="bSubFolder">サブ・フォルダも検索する？</param>
/// <returns>0</returns>
int CSearchFile::SearchMain(const char *pszSearchPath, const BOOL bSubFolder)
{
	BOOL bRun = TRUE;
	CFileFind Find;
	CString str;
	CString str2;
	CString strSearchFullPath;
	FILETIME ft{};

	MakeSeachFullPath(strSearchFullPath, pszSearchPath);

	if (Find.FindFile(strSearchFullPath, 0)) {
		while (bRun) {
			bRun = Find.FindNextFile();
			if (Find.IsDots()) {
				continue;
			} else if (Find.IsDirectory()) {
				if (bSubFolder) {
					SearchMain(Find.GetFilePath(), bSubFolder);
				}
				continue;
			}
			str = Find.GetFilePath();
			str.MakeUpper();
			if (CheckFileExt(str)) {
				Find.GetLastWriteTime(&ft);

				CBase *p = new CBase;

				p->m_strFileTitle  = Find.GetFileTitle();
				p->m_strFullPath   = Find.GetFilePath();
				p->m_strFileName   = Find.GetFileName();
				p->m_dw64DateTime  = ft.dwHighDateTime;
				p->m_dw64DateTime <<= 32;
				p->m_dw64DateTime += ft.dwLowDateTime;
				
				m_arrFileName.Add((void *)p);
			}
		}
	}

	return 0;
}


/// <summary>
/// 検索フォルダ・パスの作成
/// </summary>
/// <param name="strFullPath">検索フォルダ・パス</param>
/// <param name="pszPath">検索フォルダ・パス</param>
void CSearchFile::MakeSeachFullPath(CString &strFullPath, const char *pszPath)
{
	strFullPath = pszPath;

	if (strFullPath.Right(1) != '\\') {
		strFullPath += '\\';
	}
	strFullPath += "*.*";
}



//===========================================================================
// Getシリーズ
//===========================================================================

/// <summary>
/// 記録データ数を返す
/// </summary>
/// <returns>データ数</returns>
int CSearchFile::GetSize()
{
	return (int)m_arrFileName.GetSize();
}


/// <summary>
/// ファイルパスを返す
/// </summary>
/// <param name="nNum">インデックス番号</param>
/// <param name="str">ファイルパス</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CSearchFile::GetFilePath(const int nNum, CString &str)
{
	BOOL bRet = FALSE;
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) {
		str.Empty();
		bRet = TRUE;
	} else {
		CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

		str = p->m_strFullPath;
	}

	return bRet;
}


/// <summary>
/// ファイルのタイトルを返す
/// </summary>
/// <param name="nNum">インデックス番号</param>
/// <param name="str">タイトル</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CSearchFile::GetFileTitle(const int nNum, CString &str)
{
	BOOL bRet = FALSE;
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) {
		str.Empty();
		bRet = TRUE;
	} else {
		CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

		str = p->m_strFileTitle;
	}

	return bRet;
}


/// <summary>
/// ファイル名を返す
/// </summary>
/// <param name="nNum">インデックス番号</param>
/// <param name="str">ファイル名</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CSearchFile::GetFileName(const int nNum, CString &str)
{
	BOOL bRet = FALSE;
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) {
		str.Empty();
		bRet = TRUE;
	} else {
		CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

		str = p->m_strFileName;
	}

	return bRet;
}

// <summary>
/// ファイル名を返す
/// </summary>
/// <param name="nNum">インデックス番号</param>
/// <param name="str">ファイル名</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
CString CSearchFile::GetFileName(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return ""; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	return  p->m_strFileName;
}


/// <summary>
/// ファイルの拡張子を返す
/// </summary>
/// <param name="nNum">インデックス番号</param>
/// <param name="strExt">拡張子</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CSearchFile::GetFileExt(const int nNum, CString &str)
{
	int n;
	const char *cp;
	CString strTmp;

	GetFilePath(nNum, strTmp);

	n = strTmp.ReverseFind('.');
	if (n <= 0) { return TRUE; }
	
	cp  = (const char *)strTmp;
	str = (cp + n);

	return FALSE;
}


/// <summary>
/// 選択番号を設定する
/// </summary>
/// <param name="nNum">インデックス番号</param>
/// <param name="nSelect">番号</param>
/// <returns>選択番号、エラーは(-1)</returns>
int CSearchFile::SetSelectNum(const int nNum, const int nSelect)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return -1; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	p->m_nSelecct = nSelect;

	return nSelect;
}


/// <summary>
/// 選択番号を返す
/// </summary>
/// <param name="nNum">インデックス番号</param>
/// <returns>選択番号、エラーは(-1)</returns>
BOOL CSearchFile::GetSelectNum(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return -1; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	return p->m_nSelecct;
}


/// <summary>
/// 移動先をセットされた数を返す
/// </summary>
/// <returns>セットされた数</returns>
int CSearchFile::GetSelectCount()
{
	int nRet  = 0;
	int nSize = GetSize();

	for (int i = 0; i < nSize; i++) {
		CBase *p = (CBase *)m_arrFileName.GetAt(i);
		if (p->m_nSelecct >= 0) {
			nRet++;
		}
	}

	return nRet;
}



//===========================================================================
// 検索拡張子の関係
//===========================================================================

/// <summary>
/// 拡張子のセット 
/// </summary>
/// <param name="pszFileExt">拡張子</param>
void CSearchFile::SetScanExt(const char *pszFileExt)
{
	if (*pszFileExt != '\0') {
		int nLen = (int)strlen(pszFileExt);

		char *pExt = new char[nLen + 5];

		strcpy_s(pExt, nLen + 1 , pszFileExt);
		_strupr_s(pExt, nLen + 1);
		m_arrFileExt.Add((void *)pExt);
	}
}


/// <summary>
/// 登録した拡張子の数を返す
/// </summary>
/// <returns>登録数</returns>
int CSearchFile::GetExtSize()
{
	return (int)m_arrFileExt.GetSize();
}


/// <summary>
/// 拡張子を返す
/// </summary>
/// <param name="nNum">インデックス番号</param>
/// <param name="pExt">保存するバッファポインタ</param>
/// <param name="nBufSize">確保しているバッファサイズ</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CSearchFile::GetScanExt(const int nNum, char *pExt, const int nBufSize)
{
	int nSize = GetExtSize();

	if (nNum < 0 || nNum >= nSize) { return TRUE; }

	strcpy_s(pExt, nBufSize, (char *)m_arrFileExt.GetAt(nNum));

	return FALSE;
}



/// <summary>
/// 指定されたファイル名が、スキャン対象の拡張子のいずれかで終わっているかを判定します。
/// </summary>
/// <param name="pszFileName">判定するファイル名の文字列。</param>
/// <returns>FALSE:見つけられない/TRUE:見つけた</returns>
BOOL CSearchFile::CheckFileExt(const char *pszFileName)
{
	int i;
	BOOL bRet = FALSE;
	int nLoop = GetExtSize();
	char szExt[MAX_PATH + 5];
	const char *cp;

	for (i = 0; i < nLoop; i++) {
		GetScanExt(i, szExt, MAX_PATH);
		if (NULL != (cp = strstr(pszFileName, szExt))) {
			if (*(cp + strlen(szExt)) == '\0') {
				bRet = TRUE;
				break;
			}
		}
	}
	return bRet;
}


/// <summary>
/// ランダム表示用
/// </summary>
void CSearchFile::InitRandom()
{
	int i;
	int n;
	int nNum;
	void *p1;
	void *p2;

	nNum = GetSize();

	for (i = 0; i < nNum; i++) {
		n = LIB.Random(nNum);
		if (n != i) {
			p1 = m_arrFileName.GetAt(n);
			p2 = m_arrFileName.GetAt(i);
			m_arrFileName.SetAt(n, p2);
			m_arrFileName.SetAt(i, p1);
		}
	}
}



/// <summary>
/// 名前でソートする
/// </summary>
void CSearchFile::InitSortName()
{
	BOOL bSwap;
	CBase *pi, *pj;
	int nSize = GetSize();

	for (int i = 0; i < nSize - 1; i++) {
		pi = (CBase *)m_arrFileName.GetAt(i);
		for (int j = i + 1; j < nSize; j++) {
			pj = (CBase *)m_arrFileName.GetAt(j);

			bSwap = FALSE;
			const char *p1 = pi->m_strFileTitle;
			const char *p2 = pj->m_strFileTitle;

			while (*p1 != '\0' && *p2 != '\0') {
				BYTE c1 = (BYTE)*p1;
				BYTE c2 = (BYTE)*p2;

				if ('a' <= c1 && c1 <= 'z') { c1 = c1 - 0x20; }
				if ('a' <= c2 && c2 <= 'z') { c2 = c2 - 0x20; }

				if (c1 > c2) {
					bSwap = TRUE;
					break;
				} else if (c1 < c2) {
					break;
				}
				p1++;
				p2++;
			}

			if (bSwap) {
				m_arrFileName.SetAt(i, (void *)pj);
				m_arrFileName.SetAt(j, (void *)pi);
				pi = pj;
			}
		}
	}
}


/// <summary>
/// 更新日時でソートする
/// </summary>
/// <param name="bRevase">FALSE:旧→新/TRUE:新→旧</param>
void CSearchFile::InitSortLastWrite(const BOOL bRevase)
{
	BOOL bSwap;
	CBase *pi, *pj;
	int nSize = GetSize();

	for (int i = 0; i < nSize - 1; i++) {
		pi = (CBase *)m_arrFileName.GetAt(i);
		for (int j = i + 1; j < nSize; j++) {
			pj = (CBase *)m_arrFileName.GetAt(j);

			bSwap = bRevase;
			if (pi->m_dw64DateTime > pj->m_dw64DateTime) {
				bSwap = !bRevase;
			}

			if (bSwap) {
				m_arrFileName.SetAt(i, (void *)pj);
				m_arrFileName.SetAt(j, (void *)pi);
				pi = pj;
			}
		}
	}

}
