
#include "pch.h"
#include "MoveJpeg.h"
//#include "flag.h"
#include "lib.h"
#include "SearchFile.h"



/////////////////////////////////////////////////////////////////////////////
CSearchFile::CBase::CBase()
{
	m_bChg    = FALSE;
	m_bDelete = FALSE;
	
	m_dw64DateTime = 0;
}

/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 開始＆終了処理
//===========================================================================
CSearchFile::CSearchFile()
{
}


CSearchFile::~CSearchFile()
{
	End();
}


void CSearchFile::End()
{
	int i;
	int nSize = GetSize();

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


//===========================================================================
// 初期化
//===========================================================================
void CSearchFile::Init()
{
	End();
}


//===========================================================================
// ファイルの検索
//===========================================================================
int CSearchFile::Main(const char *pszSearchPath)
{
	BOOL bSubFolder = FALSE;

	if (!GetExtSize()) { return 1; }

	if (IDYES == App.m_pMainWnd->MessageBoxA("サブホルダーも検索しますか？", "", MB_ICONQUESTION|MB_YESNO)) {
		bSubFolder = TRUE;
	}

	return SearchMain(pszSearchPath, bSubFolder);
}


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
				p->m_dw64DateTime  = ft.dwHighDateTime;
				p->m_dw64DateTime <<= 32;
				p->m_dw64DateTime += ft.dwLowDateTime;
				
				m_arrFileName.Add((void *)p);
			}
		}
	}

	return 0;
}


void CSearchFile::MakeSeachFullPath(CString &strFullPath, const char *pszPath)
{
	strFullPath = pszPath;

	if (strFullPath.Right(1) != '\\') {
		strFullPath += '\\';
	}
	strFullPath += "*.*";
}



//===========================================================================
//===========================================================================
int CSearchFile::GetSize()
{
	return (int)m_arrFileName.GetSize();
}


BOOL CSearchFile::GetFilePath(const int nNum, CString &strFileName)
{
	BOOL bRet = 0;
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) {
		strFileName.Empty();
		bRet = 1;
	} else {
		CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

		strFileName = p->m_strFullPath;
	}

	return bRet;
}

BOOL CSearchFile::GetFileTitle(const int nNum, CString &str)
{
	BOOL bRet = 0;
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) {
		str.Empty();
		bRet = 1;
	} else {
		CBase *p = (CBase *)m_arrFileName.GetAt(nNum);
		
		if (p->m_strNewTitle.IsEmpty()) {
			str = p->m_strFileTitle;
		} else {
			str = p->m_strNewTitle;
		}
	}

	return bRet;
}

BOOL CSearchFile::GetFileExt(const int nNum, CString &strExt)
{
	int n;
	const char *cp;
	CString str;

	GetFilePath(nNum, str);

	n = str.ReverseFind('.');
	if (n < 0) { return TRUE; }
	
	cp = (const char *)str;
	strExt = (cp + n);

	return FALSE;
}


BOOL CSearchFile::GetChengTitel(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return FALSE; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	return p->m_bChg;
}


void CSearchFile::SetNewTitle(const int nNum, const char *pszTitle)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return; }

	if (strlen(pszTitle) == 0) { return; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	p->m_bChg        = TRUE;
	p->m_strNewTitle = pszTitle;
}


BOOL CSearchFile::ChgFileDeleteFlag(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return FALSE; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	if (p->m_bDelete) { p->m_bDelete = FALSE; }
	else              { p->m_bDelete = TRUE;  }

	return p->m_bDelete;
}


BOOL CSearchFile::IsDelete(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return FALSE; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	return p->m_bDelete;
}



//===========================================================================
// 検索拡張子の関係
//===========================================================================
//---------------------------------------------------------------------------
// 拡張子のセット
//---------------------------------------------------------------------------
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


int CSearchFile::GetExtSize()
{
	return (int)m_arrFileExt.GetSize();
}

int CSearchFile::GetScanExt(const int nNum, char *pExt, const int nBufSize)
{
	int nSize = GetExtSize();

	if (nNum < 0 || nNum >= nSize) { return (1); }

	strcpy_s(pExt, nBufSize, (char *)m_arrFileExt.GetAt(nNum));

	return 0;
}


int CSearchFile::CheckFileExt(const char *pszFileName)
{
	int i;
	int nRet = 0;
	int nLoop = GetExtSize();
	char szExt[MAX_PATH + 5];
	const char *cp;

	for (i = 0; i < nLoop; i++) {
		GetScanExt(i, szExt, MAX_PATH);
		if (NULL != (cp = strstr(pszFileName, szExt))) {
			if (*(cp + strlen(szExt)) == '\0') {
				nRet = 1;
				break;
			}
		}
	}
	return nRet;
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
