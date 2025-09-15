
// DlgMain.h : ヘッダー ファイル
//

#pragma once

#include "Gv.h"
#include "File.h"
#include "SearchFile.h"


// CDlgMain ダイアログ
class CDlgMain : public CDialogEx
{
// コンストラクション
public:
	CDlgMain(CWnd* pParent = nullptr);	// 標準コンストラクター
	virtual ~CDlgMain();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOVEJPEG_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;
	BOOL  m_bEnding;


	void Init();
	void End(const int nEndCode = 0);


	// GV.EXE関係
	CGv *m_pGv;
	void InitGv();

	CSearchFile *m_pScan;


	// ダイアログのアイティムを移動する
protected:
	BOOL         m_bInitDialog;
	CSize        m_sizeDlgMin;
	CSize        m_sizeDlgOld;
	const TCHAR *m_pszWindowPos = _T("WindowPos");

	void InitWindowSize();
	void InitWindowPos();
	void SaveWindowPos();
	void MoveDlgItem(const UINT uID, const int l, const int t, const int r, const int b);



protected:
	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonQuit();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonSelectPath();
	afx_msg void OnBnClickedButtonScanFile();
};
