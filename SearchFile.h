
#ifndef __SEARCH_FILE_H__
#define __SEARCH_FILE_H__


class CSearchFile {
	/////////////////////////////////////////////////////////////////////////////
	class CBase {
	public:
		int     m_nSelecct;
		CString m_strFullPath;
		CString m_strFileTitle;
		CString m_strFileName;
		unsigned long long m_dw64DateTime;

		CBase() {
			m_nSelecct     = -1;
			m_dw64DateTime = 0;
		}
	};
	/////////////////////////////////////////////////////////////////////////////


public:
	CSearchFile();
	virtual ~CSearchFile();

private:
	CPtrArray m_arrFileName;
	CPtrArray m_arrFileExt;

	void End();

	int  SearchMain(const char *pszPath, const BOOL bSubFolder);
	void MakeSeachFullPath(CString &strFullPath, const char *pszPath);
	int  GetExtSize();
	BOOL GetScanExt(const int nNum, char *pExt, const int nBufSize);
	BOOL CheckFileExt(const char *pszFileName);

public:
	void Init();
	int  Main(const char *pszPath);

	int     GetSize();
	BOOL    GetFilePath(const int nNum, CString &strFullPath);
	BOOL    GetFileTitle(const int nNum, CString &strFileTitle);
	BOOL    GetFileName(const int nNum, CString &strFileName);
	CString GetFileName(const int nNum);
	BOOL    GetFileExt(const int nNum, CString &strExt);
	int     GetSelectNum(const int nNum);
	int     GetSelectCount();

	void SetScanExt(const char *pszFileExt);
	int  SetSelectNum(const int nNum, const int nSelect);


	void InitRandom();
	void InitSortName();
	void InitSortLastWrite(const BOOL bRevase = FALSE);
};

#endif

