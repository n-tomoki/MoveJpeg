
// DlgMain.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "MoveJpeg.h"
#include "DlgMain.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDlgMain ダイアログ



CDlgMain::CDlgMain(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOVEJPEG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIconA(IDR_MAINFRAME);

	m_bEnding     = FALSE;
	m_bInitDialog = FALSE;

	m_pGv   = NULL;
	m_pScan = NULL;

	m_nDispNumber = 0;
	m_nDispMaxNum = 0;
}


CDlgMain::~CDlgMain()
{
	delete m_pGv;
	delete m_pScan;
}


void CDlgMain::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDlgMain, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK,&CDlgMain::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL,&CDlgMain::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_QUIT,&CDlgMain::OnBnClickedButtonQuit)
	ON_WM_ENDSESSION()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_PATH,&CDlgMain::OnBnClickedButtonSelectPath)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_FILE,&CDlgMain::OnBnClickedButtonScanFile)
END_MESSAGE_MAP()


// CDlgMain メッセージ ハンドラー

BOOL CDlgMain::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadStringA(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenuA(MF_SEPARATOR);
			pSysMenu->AppendMenuA(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	DragAcceptFiles();

	Init();
	InitWindowSize();
	InitWindowPos();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CDlgMain::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CDlgMain::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessageA(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CDlgMain::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDlgMain::OnBnClickedOk()
{
	CWnd *pWnd = GetFocus();

	if (pWnd == GetDlgItem(IDC_BUTTON_QUIT)) { PostMessageA(WM_COMMAND, IDC_BUTTON_QUIT); }
	else {
		NextDlgCtrl();
	}
}

void CDlgMain::OnBnClickedCancel()
{
	PostMessageA(WM_COMMAND, IDC_BUTTON_QUIT);
}


/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// 開始＆終了処理
//===========================================================================
void CDlgMain::Init()
{
	CString str = App.GetParamFileString("PARAM","SCAN_PATH","");

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_EDIT_SCANPATH);

	pEdit->SetLimitText(1024);
	pEdit->SetWindowTextA(str);


	m_pGv = new CGv;
	if (!m_pGv->Init()) { InitGv(); }

	m_pScan = new CSearchFile;
}


/// <summary>「GV.EXE」の初期設定</summary>
void CDlgMain::InitGv()
{
	m_pGv->AddFileList("OFF");
	m_pGv->AutoAdjustMode("ON", "ON", "ON", "ON");
	m_pGv->DispMsg("OFF");
	m_pGv->Quantize("AUTO");
}


void CDlgMain::OnBnClickedButtonQuit()
{
	End();
}

void CDlgMain::End(const int nEndCode)
{
	if (m_bEnding) { return; }
	m_bEnding = TRUE;

	CString str;

	GetDlgItem(IDC_EDIT_SCANPATH)->GetWindowTextA(str);

	App.WriteParamFileString("PARAM","SCAN_PATH", str);

	m_pGv->End();

	EndDialog(nEndCode ? IDCANCEL : IDOK);
}


//===========================================================================
//===========================================================================

/// <summary>
/// 検索フォルダの選択
/// </summary>
void CDlgMain::OnBnClickedButtonSelectPath()
{
	CString str;

	GetDlgItem(IDC_EDIT_SCANPATH)->GetWindowTextA(str);

	if (!App.SelectFolder(str, this->m_hWnd)) {
		GetDlgItem(IDC_EDIT_SCANPATH)->SetWindowTextA(str);
	}
}



void CDlgMain::OnBnClickedButtonScanFile()
{
	CString str;

	GetDlgItem(IDC_EDIT_SCANPATH)->GetWindowTextA(str);

	if (File::IsExistFolder(str) == FALSE) {
		MessageBox("フォルダが存在しません", "確認", MB_ICONINFORMATION | MB_OK);
		return;
	}

	m_pScan->Init();
	m_pScan->SetScanExt(".jpeg");
	m_pScan->SetScanExt(".jpg");
	m_pScan->SetScanExt(".png");
	m_pScan->Main(str);

	m_nDispNumber = 0;
	m_nDispMaxNum = m_pScan->GetSize();
	UpdateDispNumber();
}



/// <summary>
/// 現在の表示番号を更新する
/// </summary>
void CDlgMain::UpdateDispNumber()
{
	CString strDispMsg;
	CString strFilePath;

	if (m_pScan->GetSize() > 0) {
		strDispMsg.Format("(%04d/%04d)", m_nDispNumber, m_nDispMaxNum);

		if (!m_pScan->GetFilePath(m_nDispNumber, strFilePath)) {
			m_pGv->FileOpen(strFilePath);
		}
	} else {
		strDispMsg.Format("(xxxx/xxxx)");
	}

	GetDlgItem(IDC_STATIC_DISP_NUMBER)->SetWindowTextA(strDispMsg);
}


/// <summary>
/// “WM_DROPFILES”のメッセージ処理をする 
/// </summary>
/// <param name="hDropInfo">ドロップ情報</param>
void CDlgMain::OnDropFiles(HDROP hDropInfo)
{
	int nSize = DragQueryFileA(hDropInfo, -1, NULL, 0);
	char szFull[MAX_PATH + 5];


	// １つの場合
	int nLen = DragQueryFileA(hDropInfo, 0, szFull, MAX_PATH);
	if (!nLen) { return; }

	if (File::IsExistFile(szFull)) {
		MessageBox("フォルダを指定して下さい", "確認", MB_ICONINFORMATION | MB_OK);
		return;
	}

	GetDlgItem(IDC_EDIT_SCANPATH)->SetWindowTextA(szFull);

	// ドロップされたファイルの情報を解放する
	DragFinish(hDropInfo);
}

//===========================================================================
// 強制終了処理
//===========================================================================

/// <summary>
/// Windowsが終了(ユーザーがログアウト)するかも？
/// </summary>
/// <param name="bEnding">TRUE:終了(ログアウト)する/FALSE:しない</param>
void CDlgMain::OnEndSession(BOOL bEnding)
{
	CDialogEx::OnEndSession(bEnding);

	if (bEnding) { End(); }
}


//================================================================================
// ダイアログのアイティムを移動する
// ウインドウ位置を保存＆復元
//================================================================================

/// <summary>最初のウインドウ位置を保存</summary>
void CDlgMain::InitWindowSize()
{
	CRect rc;
	GetWindowRect(rc) ; m_sizeDlgMin = rc.Size();
	ScreenToClient(rc); m_sizeDlgOld = rc.Size();

	m_bInitDialog = TRUE;
}


/// <summary>最小のウインドウサイズ</summary>
void CDlgMain::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	CDialogEx::OnGetMinMaxInfo(lpMMI);

	if (m_bInitDialog) {
		lpMMI->ptMinTrackSize.x = m_sizeDlgMin.cx;
		lpMMI->ptMinTrackSize.y = m_sizeDlgMin.cy;
		// lpMMI->ptMaxTrackSize.x = m_sizeDlgMin.cx;
		// lpMMI->ptMaxTrackSize.y = m_sizeDlgMin.cy;
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
void CDlgMain::MoveDlgItem(const UINT uID, const int l, const int t, const int r, const int b)
{
	CRect rc;

	GetDlgItem(uID)->GetWindowRect(rc);
	ScreenToClient(rc);
	rc.InflateRect(-l, -t, r, b);
	GetDlgItem(uID)->MoveWindow(rc);
}


/// <summary>ダイアログのサイズが変更された</summary>
void CDlgMain::OnSize(UINT nType, int cx, int cy)
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


			MoveDlgItem(IDC_EDIT_SCANPATH     ,  0,  0, dx,  0); 
			MoveDlgItem(IDC_BUTTON_SELECT_PATH, dx,  0, dx,  0); 
			MoveDlgItem(IDC_BUTTON_SCAN_FILE  , dx,  0, dx,  0); 
			MoveDlgItem(IDC_BUTTON_QUIT       , dx,  0, dx, dy);
//			MoveDlgItem(IDC_BUTTON_GO         ,  0,  0, dx, dy);

			SaveWindowPos();

			Invalidate(FALSE);
		}
		break;
	}
}

/// <summary>
/// ウインドウが移動された
/// </summary>
/// <param name="x">移動量</param>
/// <param name="y">移動量</param>
void CDlgMain::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
	SaveWindowPos();
}


/// <summary>ウインドウ位置の保存</summary>
void CDlgMain::SaveWindowPos()
{
	if (m_bInitDialog && !IsIconic()) {
		CRect rc;
		GetWindowRect(rc);
		App.WriteParamFileInt(m_pszWindowPos, _T("x") , rc.left);
		App.WriteParamFileInt(m_pszWindowPos, _T("y") , rc.top);
		App.WriteParamFileInt(m_pszWindowPos, _T("cx"), rc.Width());
		App.WriteParamFileInt(m_pszWindowPos, _T("cy"), rc.Height());
	}
}


/// <summary>ウインドウ位置の移動</summary>
void CDlgMain::InitWindowPos()
{
	int nVirtualPosX = GetSystemMetrics(SM_XVIRTUALSCREEN);
	int nVirtualPosY = GetSystemMetrics(SM_YVIRTUALSCREEN);
	int nViewX       = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int nViewY       = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	int x  = App.GetParamFileInt(m_pszWindowPos, _T("x") );
	int y  = App.GetParamFileInt(m_pszWindowPos, _T("y") );
	int cx = App.GetParamFileInt(m_pszWindowPos, _T("cx"));
	int cy = App.GetParamFileInt(m_pszWindowPos, _T("cy"));

	CRect rtVirtual = CRect(nVirtualPosX, nVirtualPosY, nVirtualPosX + nViewX, nVirtualPosY + nViewY);

	// ディスプレイ内にウインドウの左上が無い場合は、ウインドウの移動はしない。
	if (rtVirtual.PtInRect(CPoint(x,y)) == 0 || GetAsyncKeyState(VK_SHIFT) & 0x8000) { return; }

	// 表示するウインドウサイズが、初期値のウィンドウサイズより小さい場合は、ウインドウの移動はしない
	if (m_sizeDlgMin.cx > cx || m_sizeDlgMin.cy > cy) { return; }

	MoveWindow(x, y, cx, cy);
}

