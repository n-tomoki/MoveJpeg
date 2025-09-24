#pragma once
#include "afxdialogex.h"
#include "File.h"


// CDlgRadioNamePath ダイアログ

class CDlgRadioNamePath : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgRadioNamePath)

public:
	CDlgRadioNamePath(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CDlgRadioNamePath();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RADIO_NAME_PATH };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	HICON m_hIcon;
	BOOL  m_bEnding;
	char *m_pName;
	char *m_pPath;

	void Init();
	void End(const int nEndCode = 0);

public:
	void SetParam(const char *pszName, const char *pszPath);
	void GetParam(CString &strName, CString &strPath);

	// ダイアログのアイティムを移動する
protected:
	BOOL m_bInitDialog;
	CSize m_sizeDlgMin;
	CSize m_sizeDlgOld;

	void InitWindowSize();
	void MoveDlgItem(const UINT uID, const int l, const int t, const int r, const int b);


protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnBnClickedButtonWriteQuit();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonSelectPath();
};
