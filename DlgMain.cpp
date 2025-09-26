
// DlgMain.cpp : 実装ファイル
//

#include "pch.h"
#include "framework.h"
#include "MoveJpeg.h"
#include "DlgMain.h"
#include "afxdialogex.h"
#include "DlgRadioNamePath.h"


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


/// <summary>
/// CopyFileExのコールバック関数
/// </summary>
/// <param name="TotalFileSize">バイト単位の総ファイルサイズ</param>
/// <param name="TotalBytesTransferred">転送された総バイト数</param>
/// <param name="StreamSize">このストリームの総バイト数</param>
/// <param name="StreamBytesTransferred">このストリームに対して転送された総バイト数</param>
/// <param name="dwStreamNumber">現在のストリーム</param>
/// <param name="dwCallbackReason">CopyProgressRoutine 関数が呼び出された理由</param>
/// <param name="hSourceFile">コピー元ファイルのハンドル</param>
/// <param name="hDestinationFile">コピー先ファイルのハンドル</param>
/// <param name="lpData">CopyFileEx 関数から渡される</param>
/// <returns>リターン コード</returns>
DWORD CALLBACK CopyProgressRoutine(
	__int64 TotalFileSize,          // バイト単位の総ファイルサイズ
	__int64 TotalBytesTransferred,  // 転送された総バイト数
	__int64 StreamSize,             // このストリームの総バイト数
	__int64 StreamBytesTransferred, // このストリームに対して転送された総バイト数
	DWORD   dwStreamNumber,         // 現在のストリーム
	DWORD   dwCallbackReason,       // CopyProgressRoutine 関数が呼び出された理由
	HANDLE  hSourceFile,            // コピー元ファイルのハンドル
	HANDLE  hDestinationFile,       // コピー先ファイルのハンドル
	LPVOID  lpData);                // CopyFileEx 関数から渡される




CDlgMain::CDlgMain(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MOVEJPEG_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIconA(IDR_MAINFRAME);

	m_bEnding     = FALSE;
	m_bInitDialog = FALSE;
	
	m_bFolderButtonWrite = FALSE;

	m_pGv   = NULL;
	m_pScan = NULL;
	m_pFont = NULL;

	m_nDispNumber = 0;
	m_nDispMaxNum = 0;

	// 画像反転
	m_pIWICFactory = NULL;
}


CDlgMain::~CDlgMain()
{
	if (m_pIWICFactory != NULL) { m_pIWICFactory->Release(); }

	delete m_pGv;
	delete m_pScan;
	delete m_pFont;
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
	ON_BN_CLICKED(IDC_BUTTON_BACK, &CDlgMain::OnBnClickedButtonBack)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CDlgMain::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_RADIO_FOLDER0, &CDlgMain::OnBnClickedRadioFolder0)
	ON_BN_CLICKED(IDC_RADIO_FOLDER1, &CDlgMain::OnBnClickedRadioFolder1)
	ON_BN_CLICKED(IDC_RADIO_FOLDER2, &CDlgMain::OnBnClickedRadioFolder2)
	ON_BN_CLICKED(IDC_RADIO_FOLDER3, &CDlgMain::OnBnClickedRadioFolder3)
	ON_BN_CLICKED(IDC_RADIO_FOLDER4, &CDlgMain::OnBnClickedRadioFolder4)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_FLIP, &CDlgMain::OnBnClickedButtonImageFlip)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CANCEL, &CDlgMain::OnBnClickedButtonSelectCancel)
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

		pSysMenu->RemoveMenu(SC_RESTORE , MF_BYCOMMAND);  // ウィンドウを通常の位置とサイズに戻します。
		pSysMenu->RemoveMenu(SC_MAXIMIZE, MF_BYCOMMAND);  // ウィンドウを最大化します。
		pSysMenu->RemoveMenu(SC_MINIMIZE, MF_BYCOMMAND);  // ウィンドウを最小化します。
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

#if !_DEBUG
	SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
#endif

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

	if      (pWnd == GetDlgItem(IDC_BUTTON_QUIT))        { PostMessageA(WM_COMMAND, IDC_BUTTON_QUIT); }
	else if (pWnd == GetDlgItem(IDC_BUTTON_BACK))        { PostMessageA(WM_COMMAND, IDC_BUTTON_BACK); }
	else if (pWnd == GetDlgItem(IDC_BUTTON_NEXT))        { PostMessageA(WM_COMMAND, IDC_BUTTON_NEXT); }
	else if (pWnd == GetDlgItem(IDC_BUTTON_SCAN_FILE))   { PostMessageA(WM_COMMAND, IDC_BUTTON_SCAN_FILE); }
	else if (pWnd == GetDlgItem(IDC_BUTTON_SELECT_PATH)) { PostMessageA(WM_COMMAND, IDC_BUTTON_SELECT_PATH); }
	else if (pWnd == GetDlgItem(IDC_EDIT_SCANPATH))      { PostMessageA(WM_COMMAND, IDC_BUTTON_SCAN_FILE); }
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

	FolderButtonInit();
}


/// <summary>「GV.EXE」の初期設定</summary>
void CDlgMain::InitGv()
{
	m_pGv->AddFileList("OFF");
	m_pGv->AutoAdjustMode("ON", "ON", "ON", "ON");
	m_pGv->DispMsg("OFF");
	m_pGv->Quantize("AUTO");
}


/// <summary>
/// フォントを作成する
/// </summary>
/// <param name="nFontH"></param>
/// <returns></returns>
CFont *CDlgMain::MakeFont(const int nFontH)
{
	CFont *pFont = new CFont();
	pFont->CreateFontA(
		nFontH,							// int nHeight
		0,							// int nWidth
		0,							// int nEscapement
		0,							// int nOrientation
		FW_NORMAL,					// int nWeight
		0,							// BYTE bItalic
		0,							// BYTE bUnderline
		0,							// BYTE cStrikeOut
		DEFAULT_CHARSET,			// BYTE nCharSet
		OUT_DEFAULT_PRECIS,			// BYTE nOutPrecision
		CLIP_DEFAULT_PRECIS,		// BYTE nClipPrecision
		NONANTIALIASED_QUALITY,		// BYTE nQuality or 'DEFAULT_QUALITY'
		FIXED_PITCH | FF_MODERN,	// BYTE nPitchAndFamily
		"ＭＳ ゴシック");			// LPCTSTR lpszFacename

	return pFont;
}


/// <summary>「終わり」のボタンが押された</summary>
void CDlgMain::OnBnClickedButtonQuit()
{
	End();
}


/// <summary>
/// 終了処理
/// </summary>
/// <param name="nEndCode">終了コード[0]</param>
void CDlgMain::End(const int nEndCode)
{
	if (m_bEnding) { return; }
	m_bEnding = TRUE;

	if (!nEndCode) {
		if (m_pScan->GetSelectCount()) {
			int n = MessageBoxA("画像ファイルを指定フォルダに移動しますか？",
				"確認",
				MB_ICONQUESTION | MB_YESNOCANCEL);
			if (n == IDYES) {
				n = ExecCopy();

				CString str;
				str.Format("%d個のファイルを処理した", n);

				MessageBoxA(str, "確認", MB_ICONINFORMATION|MB_OK);
			} else if (n == IDCANCEL) {
				m_bEnding = FALSE;
				return;
			}
		}
	}

	CString str;

	GetDlgItem(IDC_EDIT_SCANPATH)->GetWindowTextA(str);

	App.WriteParamFileString("PARAM", "SCAN_PATH" , str);

	m_pGv->End();

	FolderButtonSave();
	FolderButtonRelease();

	if (m_pIWICFactory != NULL) {
		m_pIWICFactory->Release();
		m_pIWICFactory = NULL;
	}

	EndDialog(IDOK);
}


//===========================================================================
// フォルダボタン(ラジオボタン)関係
//===========================================================================

/// <summary>フォルダボタンの初期化</summary>
void CDlgMain::FolderButtonInit()
{
	BOOL bRun = TRUE;
	int  nCnt = 0;
	int  nErrCode =   0;
	int  nErrBit  = 0x1;
	char szKeyName[20];
	char szKeyPath[20];
	CString strName;
	CString strPath;

	for (int i = 0; i < SELECT_RADIO_MAXNUM; i++) {
		BOOL bUse = TRUE;

		sprintf_s(szKeyName, "FolderName%02d", i);
		sprintf_s(szKeyPath, "FolderPath%02d", i);

		strName = App.GetParamFileString("FOLDER", szKeyName);
		strPath = App.GetParamFileString("FOLDER", szKeyPath);

		if      (strName.IsEmpty()) { bUse = FALSE; strName = "---"; }
		else if (strName == "----") { bUse = FALSE; }

		if (bUse) {
			if      (strPath.IsEmpty())             { bUse = FALSE; }
			else if (!File::IsExistFolder(strPath)) { bUse = FALSE; nErrCode |= nErrBit; }
		}

		App.WriteParamFileString("FOLDER", szKeyName, strName);
		App.WriteParamFileString("FOLDER", szKeyPath, strPath);

		int nLenName = strName.GetLength() + 1;
		int nLenPath = strPath.GetLength() + 1;


		SButtonBase *p = new SButtonBase();
		p->m_pName     = new char[nLenName + 1];
		p->m_pPath     = new char[nLenPath + 1];
		p->m_bUse      = bUse;

		strcpy_s(p->m_pName, nLenName, (const char *)strName);
		strcpy_s(p->m_pPath, nLenPath, (const char *)strPath);

		m_arrButton.Add((void *)p);
		nErrBit <<= 1;
	}

	m_pFont = MakeFont(14);
	for (int i = 0; i < SELECT_RADIO_MAXNUM; i++) {
		SButtonBase *pBase = (SButtonBase *)m_arrButton.GetAt(i);

		GetDlgItem(IDC_RADIO_FOLDER0 + i)->SetFont(m_pFont);
	}

	FolderButtonSetName();

	if (nErrCode) {
		MessageBoxA("指定されたフォルダが存在しません", "確認", MB_ICONEXCLAMATION|MB_OK);
	}

	EnableButton(FALSE);
}


/// <summary>
/// フォルダ選択ラジオボタンのラベル名を設定します。
/// </summary>
void CDlgMain::FolderButtonSetName()
{
	for (int i = 0; i < SELECT_RADIO_MAXNUM; i++) {
		SButtonBase *pBase = (SButtonBase *)m_arrButton.GetAt(i);

		GetDlgItem(IDC_RADIO_FOLDER0 + i)->SetWindowTextA(pBase->m_pName);
	}
}


/// <summary>
/// フォルダ選択ラジオボタンを保存
/// </summary>
void CDlgMain::FolderButtonSave()
{
	if (!m_bFolderButtonWrite) { return; }
	m_bFolderButtonWrite = FALSE;

	char szKeyName[20];
	char szKeyPath[20];
	CString strName;
	CString strPath;

	for (int i = 0; i < SELECT_RADIO_MAXNUM; i++) {
		SButtonBase *p = (SButtonBase *)m_arrButton.GetAt(i);

		sprintf_s(szKeyName, "FolderName%02d", i);
		sprintf_s(szKeyPath, "FolderPath%02d", i);

		App.WriteParamFileString("FOLDER", szKeyName, p->m_pName);
		App.WriteParamFileString("FOLDER", szKeyPath, p->m_pPath);
	}
}


/// <summary>
/// フォルダ選択ラジオボタンのメモリ解放
/// </summary>
void CDlgMain::FolderButtonRelease()
{
	int nSize = (int)m_arrButton.GetSize();

	if (nSize > 0) {
		for (int i = 0; i < nSize; i++) {
			SButtonBase *p = (SButtonBase *)m_arrButton.GetAt(0);
			delete []p->m_pName; p->m_pName = NULL;
			delete []p->m_pPath; p->m_pPath = NULL;
			delete p;
			m_arrButton.RemoveAt(0);
		}
	}
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


/// <summary>
/// フォルダ内の検索をする
/// </summary>
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
			GvFileOpen(strFilePath);
		}
	} else {
		strDispMsg.Format("(xxxx/xxxx)");
	}

	GetDlgItem(IDC_STATIC_DISP_NUMBER)->SetWindowTextA(strDispMsg);
	EnableButton();
}


/// <summary>
/// 「GV.EXE」に指定ファイルを表示させる
/// </summary>
/// <param name="pszFileName"></param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CDlgMain::GvFileOpen(const char *pszFileName)
{
	BOOL bRet = FALSE;

	if (m_pGv->FileOpen(pszFileName)) {
		if (!m_pGv->ReStart()) {
			InitGv();
			if (m_pGv->FileOpen(pszFileName)) {
				bRet = TRUE;
			}
		}
	}

	return bRet;
}


/// <summary>
/// 各ボタンの状態を設定する
/// </summary>
void CDlgMain::EnableButton(BOOL bEnable)
{
	if (!bEnable) {
		GetDlgItem(IDC_BUTTON_BACK)         ->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_NEXT)         ->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_IMAGE_FLIP)   ->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SELECT_CANCEL)->EnableWindow(FALSE);
	} else {
		GetDlgItem(IDC_BUTTON_IMAGE_FLIP)->EnableWindow(TRUE);

		if (m_nDispNumber == 0) {
			GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(FALSE);
		} else {
			GetDlgItem(IDC_BUTTON_BACK)->EnableWindow(TRUE);
		}

		if (m_nDispNumber < m_nDispMaxNum - 1) {
			GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(TRUE);
		} else {
			GetDlgItem(IDC_BUTTON_NEXT)->EnableWindow(FALSE);
		}
		
		EnableButtonRadio();
	}
}


/// <summary>
/// ラジオボタンの状態を設定する
/// </summary>
void CDlgMain::EnableButtonRadio()
{
	int nRadio = m_pScan->GetSelectNum(m_nDispNumber);

	if (nRadio >= 0) {
		GetDlgItem(IDC_BUTTON_SELECT_CANCEL)->EnableWindow(TRUE);
	} else {
		GetDlgItem(IDC_BUTTON_SELECT_CANCEL)->EnableWindow(FALSE);
	}

	for (int i = 0; i < SELECT_RADIO_MAXNUM; i++) {
		BOOL bEnable = TRUE;
		SButtonBase *p = (SButtonBase *)m_arrButton.GetAt(i);

		if (nRadio == i) {
			CheckDlgButton(IDC_RADIO_FOLDER0 + i, BST_CHECKED);
		} else {
			CheckDlgButton(IDC_RADIO_FOLDER0 + i, BST_UNCHECKED);
		}
	}
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
// 画像の表示関係
//===========================================================================

/// <summary>
/// 次の画像を表示
/// </summary>
void CDlgMain::OnBnClickedButtonNext()
{
	if (m_nDispNumber < m_nDispMaxNum - 1) {
		m_nDispNumber++;
	}
	UpdateDispNumber();
}


/// <summary>
/// 前の画像を表示
/// </summary>
void CDlgMain::OnBnClickedButtonBack()
{
	if (m_nDispNumber) {
		m_nDispNumber--;
	}
	UpdateDispNumber();
}


//===========================================================================
// ラジオボタンの処理
//===========================================================================

/// <summary>「フォルダ0」が押された</summary>
void CDlgMain::OnBnClickedRadioFolder0()
{
	SetRadioSelect(0);
}

/// <summary>フォルダ１」が押された</summary>
void CDlgMain::OnBnClickedRadioFolder1()
{
	SetRadioSelect(1);
}


/// <summary>「フォルダ２」が押された</summary>
void CDlgMain::OnBnClickedRadioFolder2()
{
	SetRadioSelect(2);
}


/// <summary>「フォルダ３」が押された</summary>
void CDlgMain::OnBnClickedRadioFolder3()
{
	SetRadioSelect(3);
}


/// <summary>「フォルダ４」が押された</summary>
void CDlgMain::OnBnClickedRadioFolder4()
{
	SetRadioSelect(4);
}


/// <summary>
/// ラジオボタンで選択されたフォルダ番号をセットする
/// </summary>
/// <param name="nNum">番号</param>
void CDlgMain::SetRadioSelect(const int nNum)
{
	if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		// ラジオボタンの名称とパスを編集する
		SetRadioNamePath(nNum);
	} else {
		m_pScan->SetSelectNum(m_nDispNumber, nNum);
		EnableButtonRadio();
	}
}


/// <summary>
/// ラジオボタンの名称とパスを編集する
/// </summary>
/// <param name="nNum">番号</param>
void CDlgMain::SetRadioNamePath(const int nNum)
{
	int nSize = (int)m_arrButton.GetSize();

	if (nNum >= 0 || nNum < nSize) {
		SButtonBase *p = (SButtonBase *)m_arrButton.GetAt(nNum);
		CDlgRadioNamePath dlg;

		dlg.SetParam(p->m_pName, p->m_pPath);

		if (dlg.DoModal()) {
			CString strName;
			CString strPath;

			dlg.GetParam(strName, strPath);

			int nLenName = strName.GetLength() + 1;
			int nLenPath = strPath.GetLength() + 1;

			delete []p->m_pName;
			delete []p->m_pPath;

			p->m_pName = new char[nLenName + 1];
			p->m_pPath = new char[nLenPath + 1];

			strcpy_s(p->m_pName, nLenName, (const char *)strName);
			strcpy_s(p->m_pPath, nLenPath, (const char *)strPath);

			if (strName == "----") { p->m_bUse = FALSE; }
			else                   { p->m_bUse = TRUE;  }

			GetDlgItem(IDC_RADIO_FOLDER0 + nNum)->SetWindowTextA(p->m_pName);

			m_bFolderButtonWrite = TRUE;
		}

		if (m_pScan->GetSelectNum(m_nDispNumber) >= 0) {
			CheckDlgButton(IDC_RADIO_FOLDER0 + nNum, BST_CHECKED);
		} else {
			CheckDlgButton(IDC_RADIO_FOLDER0 + nNum, BST_UNCHECKED);
		}
	}
}


/// <summary>
/// 選択を取り消す
/// </summary>
void CDlgMain::OnBnClickedButtonSelectCancel()
{
	m_pScan->SetSelectNum(m_nDispNumber, -1);
	EnableButtonRadio();
}


//===========================================================================
// 画像の転送をする
//===========================================================================

/// <summary>
/// 画像の転送をする
/// </summary>
/// <returns>コピーしたファイル数</returns>
int CDlgMain::ExecCopy()
{
	int nRet      = 0;
	int nSizeScan = m_pScan->GetSize();
	int nSizeBase = (int)m_arrButton.GetSize();
	CString strSrcPath;
	CString strDstPath;

	for (int i = 0; i < nSizeScan; i++) {
		int nNum = m_pScan->GetSelectNum(i);
		
		if (nNum >= 0 && nNum < nSizeBase) {
			SButtonBase *pBase = (SButtonBase *)m_arrButton.GetAt(nNum);

			if (pBase->m_bUse) {
				m_pScan->GetFilePath(i, strSrcPath);
				CreateDestFileName(m_pScan->GetFileName(i), pBase->m_pPath, strDstPath);
				if (CopyFileExA(strSrcPath, strDstPath, (LPPROGRESS_ROUTINE)CopyProgressRoutine, (void *)this, NULL, 0)) {
					// 成功
#if !_DEBUG
					File::UnlinkFile(strSrcPath);
#endif
					nRet++;
				} else {
					// 失敗
				}

			}
		}
	}
	return nRet;
}



/// <summary>CopyFileExのコールバック関数</summary>
DWORD CALLBACK CopyProgressRoutine(
	__int64 TotalFileSize,          // バイト単位の総ファイルサイズ
	__int64 TotalBytesTransferred,  // 転送された総バイト数
	__int64 StreamSize,             // このストリームの総バイト数
	__int64 StreamBytesTransferred, // このストリームに対して転送された総バイト数
	DWORD   dwStreamNumber,         // 現在のストリーム
	DWORD   dwCallbackReason,       // CopyProgressRoutine 関数が呼び出された理由
	HANDLE  hSourceFile,            // コピー元ファイルのハンドル
	HANDLE  hDestinationFile,       // コピー先ファイルのハンドル
	LPVOID  lpData)                 // CopyFileEx 関数から渡される
{
	return PROGRESS_CONTINUE;
}



/// <summary>
/// コピー先のファイルパスを作成する
/// </summary>
/// <param name="pszSrcName">コピー元のファイル名</param>
/// <param name="pszDstPath">コピー先のフォルダ名</param>
/// <param name="strDstPath">コピー先のファイルパス</param>
/// <returns>FALSE</returns>
BOOL CDlgMain::CreateDestFileName(const char *pszSrcName, const char *pszDstPath, CString  &strDstPath)
{
	strDstPath = pszDstPath;
	if (!App.AnalyzePathFolder(strDstPath)) {
		strDstPath += "\\";
	}
	strDstPath += pszSrcName;

	return FALSE;
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

	if (bEnding) { End(1); }
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
			MoveDlgItem(IDC_STATIC_WAKU       , 0,   0, dx, dy);
			MoveDlgItem(IDC_RADIO_FOLDER0     , 0,   0, dx, dy);
			MoveDlgItem(IDC_RADIO_FOLDER1     , 0,   0, dx, dy);
			MoveDlgItem(IDC_RADIO_FOLDER2     , 0,   0, dx, dy);
			MoveDlgItem(IDC_RADIO_FOLDER3     , 0,   0, dx, dy);
			MoveDlgItem(IDC_RADIO_FOLDER4     , 0,   0, dx, dy);

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


//===========================================================================
// 画像を反転する
//===========================================================================

/// <summary>
/// 画像を反転する
/// </summary>
void CDlgMain::OnBnClickedButtonImageFlip()
{
	CString strPath;
	CString strExt;

	m_pScan->GetFilePath(m_nDispNumber, strPath);

	int nExt = strPath.ReverseFind('.');
	if (nExt < 1) { return; }

	if (strPath.GetAt(nExt) == '.') {
		strExt = strPath.Mid(nExt);
	}

	if (!File::IsExistFile(strPath)) { return; }

	ReversalMain(strPath, strExt);
	GvFileOpen(strPath);

	SetForegroundWindow();	// フォーカスを戻す
}


/// <summary>
/// 画像反転のメイン
/// </summary>
/// <param name="strFileName">ファイル名</param>
/// <param name="strExt">拡張子</param>
/// <returns>FALSE:失敗/TRUE:成功</returns>
BOOL CDlgMain::ReversalMain(CString strFileName, CString strExt)
{
	BOOL bRet = FALSE;

	// IWICImagingFactory オブジェクトを作成
	if (!CreateIWICImagingFactory()) { return bRet; }

	HRESULT hr;

	IWICBitmapDecoder     *pIDecoder      = NULL;
	IWICBitmapFrameDecode *pIDecoderFrame = NULL;
	IWICBitmapFlipRotator *pIFlipRotator  = NULL;
	IWICFormatConverter   *pIConverter    = NULL;

	// ファイル名をUnicodeに変換する
	int nLenShiftjis = strFileName.GetLength();
	int nLenUnicode  = MultiByteToWideChar(
		CP_ACP,
		0,
		(const char *)strFileName,
		nLenShiftjis + 1,
		NULL,
		0); 

	WCHAR *pFileName = new WCHAR[nLenUnicode + 1];
	int n = MultiByteToWideChar(
		CP_ACP,
		0,
		(const char *)strFileName,
		nLenShiftjis,
		pFileName,
		nLenUnicode);
	pFileName[n] = '\0';

	// イメージファイルファイルの読み込み
	hr = m_pIWICFactory->CreateDecoderFromFilename(
		pFileName,
		NULL,                           // Do not prefer a particular vendor
		GENERIC_READ,                   // Desired read access to the file
		WICDecodeMetadataCacheOnDemand, // Cache metadata when needed
		&pIDecoder                      // Pointer to the decoder
		);

	// 読み込んだイメージを取得する
	if (SUCCEEDED(hr)) {
		hr = pIDecoder->GetFrame(0, &pIDecoderFrame);
	}

	// イメージの反転に使用する IWICBitmapFlipRotator を作成する
	if (SUCCEEDED(hr)) {
		hr = m_pIWICFactory->CreateBitmapFlipRotator(&pIFlipRotator);
	}

	// イメージの反転する
	if (SUCCEEDED(hr)) {
		hr = pIFlipRotator->Initialize(
			pIDecoderFrame,                     // Bitmap source to flip.
			WICBitmapTransformFlipHorizontal);  // Flip the pixels along the 
	}

	// 反転したイメージを取得する
	IWICBitmapSource *pSource = NULL;
	if (SUCCEEDED(hr)) {
		hr = pIFlipRotator->QueryInterface(IID_PPV_ARGS(&pSource));
	}	

	// フォーマットコンバータ生成 (32bppBGRAビットマップに変換用)
	if (SUCCEEDED(hr)) {
		hr = m_pIWICFactory->CreateFormatConverter(&pIConverter);
	}

	// イメージを 32bppBGRAビットマップ に変換
	if (SUCCEEDED(hr)) {
		hr = pIConverter->Initialize(pSource, GUID_WICPixelFormat32bppBGRA,
			WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom);
	}

	// 変換したイメージを取得
	IWICBitmapSource *pImage = NULL;
	if (SUCCEEDED(hr)) {
		hr = pIConverter->QueryInterface(IID_PPV_ARGS(&pImage));
	}

	// イメージからビットマップに変換
	HBITMAP hBmp = NULL;
	if (SUCCEEDED(hr)) {
		hBmp = CreateHBitmapFromBitmapSource(pImage);
	}

	CString strTempFile = "TempFile" + strExt;

	// ファイルに出力
	if (hBmp != NULL) {
		CImage Image;

		Image.Attach(hBmp);
		Image.Save(strTempFile);
		Image.Detach();

		DeleteObject(hBmp);
		bRet = TRUE;
	}

	if (pImage         != NULL) { pImage        ->Release(); }
	if (pSource        != NULL) { pSource       ->Release(); }
	if (pIConverter    != NULL) { pIConverter   ->Release(); }
	if (pIFlipRotator  != NULL) { pIFlipRotator ->Release(); }
	if (pIDecoderFrame != NULL) { pIDecoderFrame->Release(); }
	if (pIDecoder      != NULL) { pIDecoder     ->Release(); }
	delete []pFileName;

	if (bRet) {
		if (!CopyFileA(strTempFile, strFileName, FALSE)) {
			bRet = FALSE;
		}
	}

	return bRet;
}


/// <summary>
/// WICイメージからビットマップハンドルを生成
/// </summary>
/// <param name="pImage">WICイメージ</param>
/// <returns>ビットマップハンドル</returns>
HBITMAP CDlgMain::CreateHBitmapFromBitmapSource(IWICBitmapSource *pImage)
{
	HRESULT hr;                       // 処理結果
	WICPixelFormatGUID format;        // WICフォーマット
	BITMAPINFO bminfo = {};           // ビットマップヘッダ
	HDC hdc;                          // デバイスコンテキストハンドル
	UINT uWidth        = 0;           // ビットマップ幅
	UINT uHeight       = 0;           // ビットマップ高さ
	HBITMAP hBmp      = NULL;         // ビットマップハンドル
	LPVOID imageBits  = NULL;         // ビットマップイメージ

	// 32bppBGRAか確認
	hr = pImage->GetPixelFormat(&format);
	if (SUCCEEDED(hr)) {
		if (format == GUID_WICPixelFormat32bppBGRA) {
			hr = S_OK;
		} else {
			hr = E_FAIL;
		}
	}

	// ビットマップサイズを取得
	if (SUCCEEDED(hr)) {
	  hr = pImage->GetSize(&uWidth, &uHeight);
	}

	// DIBを生成
	if (SUCCEEDED(hr)) {
		bminfo.bmiHeader.biSize = sizeof(BITMAPINFO);
		bminfo.bmiHeader.biBitCount = 32;
		bminfo.bmiHeader.biCompression = BI_RGB;
		bminfo.bmiHeader.biWidth = uWidth;
		bminfo.bmiHeader.biHeight = -static_cast<LONG>(uHeight);
		bminfo.bmiHeader.biPlanes = 1;
		hdc = ::GetDC(HWND_DESKTOP);
		hBmp = CreateDIBSection(hdc, &bminfo, DIB_RGB_COLORS, &imageBits, NULL, 0);

		if (hBmp) {
			hr = S_OK;
		} else {
			hr = E_FAIL;
		}

		::ReleaseDC(HWND_DESKTOP, hdc);
	}

	// イメージをコピー
	if (SUCCEEDED(hr)) {
		hr = pImage->CopyPixels(NULL,
			uWidth * 4,
			uWidth * uHeight * 4,
			reinterpret_cast<BYTE *>(imageBits));
	} else if (hBmp != NULL) {
		DeleteObject(hBmp);
		hBmp = NULL;
	}

	// ビットマップハンドルを返す
	return hBmp;
}


/// <summary>
/// Windows イメージング コンポーネントの作成
/// </summary>
BOOL CDlgMain::CreateIWICImagingFactory()
{
	BOOL bRet = FALSE;

	if (m_pIWICFactory != NULL) { return TRUE; }

	HRESULT hr;

	// IWICImagingFactory オブジェクトを作成
	hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&m_pIWICFactory)
    );

	if (SUCCEEDED(hr)) { bRet           = TRUE; }
	else               { m_pIWICFactory = NULL; }

	return bRet;
}

