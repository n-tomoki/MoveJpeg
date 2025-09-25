// DlgRadioNamePath.cpp : 実装ファイル
//

#include "pch.h"
#include "MoveJpeg.h"
#include "afxdialogex.h"
#include "DlgRadioNamePath.h"
#include "lib.h"


// CDlgRadioNamePath ダイアログ

IMPLEMENT_DYNAMIC(CDlgRadioNamePath, CDialogEx)

CDlgRadioNamePath::CDlgRadioNamePath(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RADIO_NAME_PATH, pParent)
{
	m_hIcon = AfxGetApp()->LoadIconA(IDR_MAINFRAME);

	m_bEnding     = FALSE;
	m_bInitDialog = FALSE;

	m_pName = NULL;
	m_pPath = NULL;
}

CDlgRadioNamePath::~CDlgRadioNamePath()
{
	if (m_pName != NULL) { delete []m_pName; m_pName = NULL; }
	if (m_pPath != NULL) { delete []m_pPath; m_pPath = NULL; }
}

void CDlgRadioNamePath::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgRadioNamePath, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgRadioNamePath::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgRadioNamePath::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_QUIT, &CDlgRadioNamePath::OnBnClickedButtonQuit)
	ON_BN_CLICKED(IDC_BUTTON_WRITE_QUIT, &CDlgRadioNamePath::OnBnClickedButtonWriteQuit)
	ON_WM_ENDSESSION()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_PATH, &CDlgRadioNamePath::OnBnClickedButtonSelectPath)
END_MESSAGE_MAP()


// CDlgRadioNamePath メッセージ ハンドラー

BOOL CDlgRadioNamePath::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CMenu *pMenu = GetSystemMenu(FALSE);
	if (pMenu != nullptr)
	{
		pMenu->RemoveMenu(SC_RESTORE , MF_BYCOMMAND);  // ウィンドウを通常の位置とサイズに戻します。
		pMenu->RemoveMenu(SC_MAXIMIZE, MF_BYCOMMAND);  // ウィンドウを最大化します。
		pMenu->RemoveMenu(SC_MINIMIZE, MF_BYCOMMAND);  // ウィンドウを最小化します。
	}


	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	DragAcceptFiles();

	Init();
	InitWindowSize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void CDlgRadioNamePath::OnBnClickedOk()
{
	CWnd *pWnd = GetFocus();

	if      (pWnd == GetDlgItem(IDC_BUTTON_QUIT))       { PostMessageA(WM_COMMAND, IDC_BUTTON_QUIT); }
	else if (pWnd == GetDlgItem(IDC_BUTTON_WRITE_QUIT)) { PostMessageA(WM_COMMAND, IDC_BUTTON_WRITE_QUIT); }
	else {
		NextDlgCtrl();
	}
}

void CDlgRadioNamePath::OnBnClickedCancel()
{
	PostMessageA(WM_COMMAND, IDC_BUTTON_QUIT);
}


/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 開始＆終了処理
//===========================================================================

/// <summary>
/// 初期化
/// </summary>
void CDlgRadioNamePath::Init()
{
	((CEdit *)GetDlgItem(IDC_EDIT_NAME))->SetLimitText(SELECT_RADIO_NAME_SIZE + 1);
	((CEdit *)GetDlgItem(IDC_EDIT_PATH))->SetLimitText(SELECT_RADIO_PATH_SIZE + 1);

	if (m_pName != NULL) { GetDlgItem(IDC_EDIT_NAME)->SetWindowTextA(m_pName); }
	if (m_pPath != NULL) { GetDlgItem(IDC_EDIT_PATH)->SetWindowTextA(m_pPath); }
}


/// <summary>
/// 閉じる(変更を無効)
/// </summary>
void CDlgRadioNamePath::OnBnClickedButtonQuit()
{
	End(0);
}


/// <summary>
/// 閉じる(変更を有効)
/// </summary>
void CDlgRadioNamePath::OnBnClickedButtonWriteQuit()
{
	char *pName = new char[SELECT_RADIO_NAME_SIZE + 1];
	char *pPath = new char[SELECT_RADIO_PATH_SIZE + 1];

	int nLenName = GetDlgItem(IDC_EDIT_NAME)->GetWindowTextA(pName, SELECT_RADIO_NAME_SIZE);
	*(pName + nLenName) = '\0';

	int nLenPath = GetDlgItem(IDC_EDIT_PATH)->GetWindowTextA(pPath, SELECT_RADIO_PATH_SIZE);
	*(pPath + nLenPath) = '\0';

	LIB.TrimSpaceCut(pName);
	LIB.TrimSpaceCut(pPath);

	if (strlen(pName) == 0) { strcpy_s(pName, nLenName, "---"); }

	if (strcmp(pName, "---")) {
		if (!File::IsExistFolder(pPath)) {
			MessageBoxA("指定されたフォルダが存在しません", "確認", MB_ICONEXCLAMATION|MB_OK);
			goto WRITE_QUIT_END;
		}
	}


	SetParam(pName, pPath);

	End(1);

WRITE_QUIT_END:
	delete []pName;
	delete []pPath;
}

/// <summary>
/// 終了処理
/// </summary>
/// <param name="nEndCode">終了コード(0:変更を無効/1:変更を有効)</param>
void CDlgRadioNamePath::End(const int nEndCode)
{
	if (m_bEnding) { return; }
	m_bEnding = TRUE;

	EndDialog(nEndCode);	
}


//===========================================================================
// 強制終了処理
//===========================================================================

/// <summary>
/// Windowsが終了(ユーザーがログアウト)するかも？
/// </summary>
/// <param name="bEnding">TRUE:終了(ログアウト)する/FALSE:しない</param>
void CDlgRadioNamePath::OnEndSession(BOOL bEnding)
{
	CDialogEx::OnEndSession(bEnding);

	if (bEnding) { End(); }
}


//===========================================================================
// いろいろ
//===========================================================================

/// <summary>
/// パラメータをセットする
/// </summary>
/// <param name="pszName">名前</param>
/// <param name="pszPath">パス</param>
void CDlgRadioNamePath::SetParam(const char *pszName, const char *pszPath)
{
	int nLenName = (int)strlen(pszName) + 1;
	int nLenPath = (int)strlen(pszPath) + 1;

	if (m_pName != NULL) { delete []m_pName; }
	if (m_pPath != NULL) { delete []m_pPath; }

	m_pName = new char[nLenName + 1];
	m_pPath = new char[nLenPath + 1];

	strcpy_s(m_pName, nLenName, pszName);
	strcpy_s(m_pPath, nLenPath, pszPath);
}


/// <summary>
/// パラメータを返す
/// </summary>
/// <param name="strName">名前</param>
/// <param name="strPath">パス</param>
void CDlgRadioNamePath::GetParam(CString &strName, CString &strPath)
{
	strName = m_pName;
	strPath = m_pPath;
}



/// <summary>
/// “WM_DROPFILES”のメッセージ処理をする 
/// </summary>
/// <param name="hDropInfo">ドロップ情報</param>
void CDlgRadioNamePath::OnDropFiles(HDROP hDropInfo)
{
	int nSize = DragQueryFileA(hDropInfo, -1, NULL, 0);
	char szFull[MAX_PATH + 5];

	int nLen = DragQueryFileA(hDropInfo, 0, szFull, MAX_PATH);
	if (!nLen) { return; }

	if (!File::IsExistFolder(szFull)) {
	} else {
		GetDlgItem(IDC_EDIT_PATH)->SetWindowTextA(szFull);
	}

	// ドロップされたファイルの情報を解放する
	DragFinish(hDropInfo);
}


/// <summary>
/// 移動先フォルダの選択
/// </summary>
void CDlgRadioNamePath::OnBnClickedButtonSelectPath()
{
	CString str;

	GetDlgItem(IDC_EDIT_PATH)->GetWindowTextA(str);

	if (!App.SelectFolder(str, this->m_hWnd)) {
		GetDlgItem(IDC_EDIT_PATH)->SetWindowTextA(str);
	}
}


//================================================================================
// ダイアログのアイティムを移動する
//================================================================================

/// <summary>最初のウインドウ位置を保存</summary>
void CDlgRadioNamePath::InitWindowSize()
{
	CRect rc;
	GetWindowRect(rc) ; m_sizeDlgMin = rc.Size();
	ScreenToClient(rc); m_sizeDlgOld = rc.Size();

	m_bInitDialog = TRUE;
}


/// <summary>最小のウインドウサイズ</summary>
void CDlgRadioNamePath::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialogEx::OnGetMinMaxInfo(lpMMI);

	if (m_bInitDialog) {
		lpMMI->ptMinTrackSize.x = m_sizeDlgMin.cx;
		lpMMI->ptMinTrackSize.y = m_sizeDlgMin.cy;
		// lpMMI->ptMaxTrackSize.x = m_sizeDlgMin.cx;
		lpMMI->ptMaxTrackSize.y = m_sizeDlgMin.cy;
	}
}


/// <summary>
/// アイテムの移動
/// </summary>
/// <param name="uID">アイテムコード</param>
/// <param name="l">左上の移動距離</param>
/// <param name="t">左下の移動距離</param>
/// <param name="r">右上の移動距離</param>
/// <param name="b">右下の移動距離</param>
void CDlgRadioNamePath::MoveDlgItem(const UINT uID, const int l, const int t, const int r, const int b)
{
	CRect rc;

	GetDlgItem(uID)->GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-l, -t, r, b);
	GetDlgItem(uID)->MoveWindow(rc);
}


/// <summary>ダイアログのサイズが変更された</summary>
void CDlgRadioNamePath::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	switch (nType) {
	case SIZE_MINIMIZED:
	//	ShowWindow(SW_MINIMIZE);  <--- アイコン化
	//	ShowWindow(SW_HIDE);      <--- アイコン化
		break;
		
	case SIZE_RESTORED:
		if (m_bInitDialog && !IsIconic()) {
			CRect rcNew;
			GetWindowRect(rcNew);
			ScreenToClient(rcNew);
			int dx = rcNew.Width() - m_sizeDlgOld.cx;
			int dy = rcNew.Height() - m_sizeDlgOld.cy;
			m_sizeDlgOld = rcNew.Size();

			MoveDlgItem(IDC_EDIT_NAME         ,  0,  0, dx,  0); 
			MoveDlgItem(IDC_EDIT_PATH         ,  0,  0, dx,  0); 
			MoveDlgItem(IDC_BUTTON_QUIT       , dx, dy, dx, dy);
			MoveDlgItem(IDC_BUTTON_WRITE_QUIT , dx, dy, dx, dy);
			MoveDlgItem(IDC_BUTTON_SELECT_PATH, dx, dy, dx, dy);

			Invalidate(FALSE);
		}
		break;
	}
}


