
#ifndef __SEARCH_FILE_H__
#define __SEARCH_FILE_H__


class CSearchFile {
	/////////////////////////////////////////////////////////////////////////////
	class CBase {
	public:
		BOOL m_bChg;
		BOOL m_bDelete;
		CString m_strFullPath;
		CString m_strFileTitle;
		CString m_strNewTitle;
		unsigned long long m_dw64DateTime;

		CBase();
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
	int  GetScanExt(const int nNum, char *pExt, const int nBufSize);
	int  CheckFileExt(const char *pszFileName);

public:
	void Init();
	int  Main(const char *pszPath);

	int  GetSize();
	BOOL GetFilePath(const int nNum, CString &strFileName);
	BOOL GetFileTitle(const int nNum, CString &strFileName);
	BOOL GetFileExt(const int nNum, CString &strExt);
	BOOL GetChengTitel(const int nNum);

	void SetNewTitle(const int nNum, const char *pszTilte);
	void SetScanExt(const char *pszFileExt);


	void InitRandom();
	void InitSortName();
	void InitSortLastWrite(const BOOL bRevase = FALSE);

	// ファイル削除？？
	BOOL ChgFileDeleteFlag(const int nNum);
	BOOL IsDelete(const int nNum);
};

#endif

