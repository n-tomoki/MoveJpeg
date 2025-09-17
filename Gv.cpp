

#include "pch.h"
#include "MoveJpeg.h"
#include "gv.h"
#include "File.h"


//===========================================================================
// 開始＆終了処理
//===========================================================================

/// <summary>コンストラクタ</summary>
CGv::CGv()
{
	m_bInit  = FALSE;
	m_dwInst = 0;

	m_hszService = 0;
	m_hszTopic   = 0;
	m_hConv      = 0;
}


/// <summary>デストラクタ</summary>
CGv::~CGv()
{
	End();
}


//===========================================================================
// 初期化＆終了処理
//===========================================================================

/// <summary>
/// 初期化処理
/// </summary>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CGv::Init()
{
	if (m_bInit) { return FALSE; }

	GetProfile();
	if (GvScan())    { return TRUE; }
	if (GvClose())   { return TRUE; }
	if (GvOpen())    { return TRUE; }
	if (DDE_Start()) { return TRUE; }

	m_bInit = TRUE;

	return FALSE;
}


/// <summary>
/// 終了処理
/// </summary>
void CGv::End()
{
	CheckDIBRequest();
	DDE_End();
	GvClose();
}

//---------------------------------------------------------------------------
// プロファイル
//---------------------------------------------------------------------------
void CGv::GetProfile()
{
	m_strGvPath = App.GetParamFileString("GV", "PATH", "GV.EXE");
	App.WriteParamFileString("GV", "PATH", m_strGvPath);
}

void CGv::PutProfile()
{
	App.WriteParamFileString("GV", "PATH", m_strGvPath);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// <summary>
/// 「GV.EXE」ファイルがある？
/// </summary>
/// <returns>FALSE:ある/TRUE:ない</returns>
BOOL CGv::GvScan()
{
	if (File::IsExistFile(m_strGvPath)) { return FALSE; }

	AfxGetMainWnd()->MessageBoxA("“GV.EXE”がありません", "AutoGv", MB_ICONSTOP|MB_OK);

	return TRUE;
}


//===========================================================================
// ＤＤＥ
//===========================================================================
int CGv::DDE_Start()
{
	if (DdeInitializeA(&m_dwInst, (PFNCALLBACK)DdemlCallback, APPCMD_CLIENTONLY, 0) != DMLERR_NO_ERROR) { return 1; }

	m_hszService = DdeCreateStringHandleA(m_dwInst, "GV"     , CP_WINANSI);
	m_hszTopic   = DdeCreateStringHandleA(m_dwInst, "Display", CP_WINANSI);
	m_hConv      = DdeConnect(m_dwInst, m_hszService, m_hszTopic, NULL);

	if (DdeGetLastError(m_dwInst) != DMLERR_NO_ERROR) { return 1; }

	return 0;
}

int CGv::DDE_End()
{
	if (m_hConv) {
		DdeFreeStringHandle(m_dwInst, m_hszService);
		DdeFreeStringHandle(m_dwInst, m_hszTopic);
		DdeDisconnect(m_hConv);
		m_hConv  = 0;
		m_dwInst = 0;
	}

	return 0;
}

//===========================================================================
// ＧＶ関係
//===========================================================================

/// <summary>
/// コールバック関数
/// </summary>
HDDEDATA CGv::DdemlCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, DWORD dwData1, DWORD dwData2)
{
	return (HDDEDATA)NULL;
}


/// <summary>
/// コマンド送信
/// </summary>
/// <param name="pszBuf">文字列</param>
/// <returns>0:成功/1:失敗</returns>
int CGv::SendCmd(const char *pszBuf)
{
	int nErr = 0;
	HDDEDATA hRet;

	if (!m_bInit)   { Init();   }
	if (!m_bInit)   { return 1; }
	if (!GvCheck()) { return 1; }
	if (!m_hConv)   { return 1; }

	hRet = DdeClientTransaction(
		(LPBYTE)pszBuf,      // クライアントデータ
		(DWORD)strlen(pszBuf) + 1,  // データサイズ
		m_hConv,             // 通信ハンドル
		0,                   // データ項目
		0,                   // クリップボードフォーマット
		XTYP_EXECUTE,        // トランザクションタイプ
		1000,                // 最大待ち時間　TIMEOUT_ASYNCで非同期となる
		NULL);               // トランザクションの結果

	if (!hRet && DdeGetLastError(m_dwInst) != DMLERR_NO_ERROR) { nErr++; }

	if (hRet) {
		DdeFreeDataHandle(hRet);
	}

	return nErr;
}


/// <summary>
/// 文字列の受信
/// </summary>
/// <param name="pszBuf">コマンド送信</param>
/// <param name="strAns">受信文字列</param>
/// <returns>0:成功/1:失敗</returns>
int CGv::SendRequestText(const char *pszBuf, CString &strAns)
{
	int nErr = 0;
	HDDEDATA hRet;
	HSZ hszRequest;

	if (!m_bInit)   { Init();   }
	if (!m_bInit)   { return 1; }
	if (!GvCheck()) { return 1; }
	if (!m_hConv)   { return 1; }

	hszRequest = DdeCreateStringHandleA(m_dwInst, pszBuf, CP_WINANSI);

	hRet = DdeClientTransaction(
		NULL,         // クライアントデータ
		0,            // データサイズ
		m_hConv,      // 通信ハンドル
		hszRequest,   // データ項目
		CF_TEXT,      // クリップボードフォーマット
		XTYP_REQUEST, // トランザクションタイプ
		1000,         // 最大待ち時間　TIMEOUT_ASYNCで非同期となる
		NULL);        // トランザクションの結果

	DdeFreeStringHandle(m_dwInst, hszRequest);
	if (!hRet && DdeGetLastError(m_dwInst) != DMLERR_NO_ERROR) { nErr++; }

	if (hRet) {
		{
			char *cp;
			DWORD dwLen;
			
			dwLen = DdeGetData(hRet, NULL, 0, 0);
			cp = strAns.GetBuffer(dwLen + 20);
			DdeGetData(hRet, (unsigned char *)cp, dwLen + 10, 0);
			strAns.ReleaseBuffer();
		}
		DdeFreeDataHandle(hRet);
	}

	return nErr;
}


/// <summary>
/// 「GV.EXE」の状態を返す
/// </summary>
/// <returns>FALSE:OK(動いてない)/TRUE:WAIT</returns>
BOOL CGv::CheckDIBRequest()
{
	if (!GvCheck()) { return FALSE; }

	BOOL bRet = TRUE;
	int nCnt  = 20;
	CString str;

	while (--nCnt) {
		int n = SendRequestText("DIBRequest", str);
		if (n == 0) {
			if (str.Find("OK")   != -1) { bRet = FALSE; break; }
			if (str.Find("WAIT") != -1) { nCnt = 20; }
		}
		Sleep(100);
	}

	return bRet;
}


/// <summary>
/// 「GV.EXE」が起動している？
/// </summary>
/// <returns>FALSE:起動していない/TRUE:起動している</returns>
BOOL CGv::GvCheck()
{
	if (NULL == FindWindowA("GV", NULL)) { return FALSE; }

	return TRUE;
}


/// <summary>
/// 「GV.EXE」を起動させる
/// </summary>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CGv::GvOpen()
{
	BOOL bRet = TRUE;
	CString str;

	if (GvCheck()) { GvClose();   }
	if (GvCheck()) { return bRet; } // 失敗

	ExecuteCmdLine(m_strGvPath, "/DDE /POS(0,0)");

	int nLoop = 50;
	do {
		Sleep(100);
		if (GvCheck()) {
			bRet = FALSE;
			break;
		}
	} while (--nLoop);

	return bRet;
}


/// <summary>
/// 外部プログラムの実行
/// </summary>
/// <param name="pCmd">コマンド</param>
/// <param name="pCmd2">引数</param>
void CGv::ExecuteCmdLine(const char *pCmd, const char *pCmd2)
{
	BOOL bCode;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char sz[MAX_PATH + 10];

	strcpy_s(sz, pCmd);
	if (strlen(pCmd2)) {
		strcat_s(sz, " ");
		strcat_s(sz, pCmd2);
	}

	RtlZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb          = sizeof(STARTUPINFO);
	si.dwFlags     = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOWNORMAL;

	bCode = CreateProcess(
		NULL,
		sz,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi);
	if (bCode) {
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
}


/// <summary>
/// 「GV.EXE」を閉じる
/// </summary>
/// <returns>FALSE</returns>
BOOL CGv::GvClose()
{
	// 起動していない
	if (!GvCheck()) { return FALSE; }

	int nLoop = 3;
	BOOL bRun = TRUE;

	do {
		CWnd *pWnd = AfxGetMainWnd()->FindWindowA("GV", NULL);
		if (pWnd == NULL) { break; }
		pWnd->SendMessageA(WM_CLOSE);

		for (int i = 0; i < 10; i++) {
			if (!GvCheck()) {
				bRun = FALSE;
				break;
			}
			Sleep(100);
		}
	} while(--nLoop > 0 && bRun);

	return FALSE;
}


/// <summary>
/// 「GV.EXE」を再起動させる
/// </summary>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL CGv::ReStart()
{
	if (!m_bInit) {
		if (Init()) { return TRUE; }
	} else {
		DDE_End();
		if (GvClose())   { return TRUE; }
		if (GvOpen())    { return TRUE; }
		if (DDE_Start()) { return TRUE; }
	}

	return FALSE;
}


//===========================================================================
// ＧＶへのコマンド
//===========================================================================
// 画像イメージをできるだけ高速に(縦横どちらかが指定サイズ以上の最小限の大きさで）表示する。
int  CGv::ViewDraftEx(const char *pszFileName, const char cFlag, const int nWidth, const int nHeight)
{
	return 1;
}


// 指定されたファイルを表示する。
int  CGv::FileOpen(const char *pszFileName)
{
	int nErr = 0;
	CString str;

	str.Format("FileOpen(\"%s\")", pszFileName);

	nErr = SendCmd(str);
	CheckDIBRequest();

	return nErr;
}


// 指定されたファイルを回転フラグで指定された方向に回転して表示する。
int  CGv::FileOpenEx(const char *pszFileName, const char cFlag)
{
	return 1;
}


// 指定されたファイルを保存する。
int  CGv::FileSaveEx(const char *pszFileName, const char *pszImage, const char *pszSaveFalg)
{
	return 1;
}


// 画像読み込みを中断する
int  CGv::FileClose()
{
	return 1;
}


// ＧＶの位置、及びサイズを変更する。
int  CGv::SetWindowPos(const int nFlag, const int nPosX, const int nPosY, const int nSizeX, const int nSizeY, const int nMode)
{
	return 1;
}


// クリップボードに表示中のデータを書き出す。
int  CGv::ClipCopy()
{
	return SendCmd("ClipCopy()");
}


// クリップボードに表示中のデータを表示イメージのまま書き出す。
int  CGv::ClipCopyViewImage()
{
	return SendCmd("ClipCopyViewImage()");
}


// クリップボードのデータを取り込む。
int  CGv::ClipPaste()
{
	return 1;
}


// 表示されている画像を壁紙に設定する。
//   CENTER:中央に表示
//   TILE:全体に表示
int  CGv::SetWallPaper(const char *pszMode)
{
	CString str;

	str  = "SetWallPaper(";
	str += pszMode;
	str += ")";
	return SendCmd(str);
}


// ＧＶの減色オプションを設定／減色を行う。
//  モード  ON：減色をする
//	       OFF：減色をしない
//	      AUTO：減色結果をディスプレイドライバの能力にあわせる
//	      EXEC：現在の設定で実際に減色する
//	色数     8：減色後の色数を8色にする
//	        16：減色後の色数を16色にする
//	       256：減色後の色数を256色にする
//　　　   　-：変更しない
//	Path     1：減色方法を1Passにする
//           2：減色方法を2Pass(Speed)にする
//	         3：減色方法を2Pass(Quality)にする
//　　　　　　-：変更しない
//	Dither  ON：減色時にディザを使う
//	       OFF：減色時にディザを使わない
//　　　    　-：変更しない
int  CGv::Quantize(const char *pszMode, const char *pszColor, const char *pszPass, const char *pszDither)
{
	CString str;

	str.Format("Quantize(%s,%s,%s,%s)", pszMode, pszColor, pszPass, pszDither);
	return SendCmd(str);
}


// 拡大率を指定する。現在の表示状態も変更する。
int  CGv::ZoomRate(const int nZoom)
{
	return 1;
}


// アスペクト比を指定する。現在の表示状態も変更する。
int  CGv::Aspect(const int nAspect, const char *pszType)
{
	return 1;
}


// 表示されている画像を回転して表示する。
int  CGv::Rotate(const char c)
{
	return 1;
}


// ＧＶを終了する。
int  CGv::ExitGV()
{
	return 1;
}


// ＧＶの表示・非表示を切り替える。
int  CGv::ShowGV(const char *pszMode)
{
	return 1;
}


// 機能：ＧＶのアイコン状態を切り替える。
int  CGv::IconGV(const char *pszMode)
{
	return 1;
}


// 自動アスペクト比調整の設定をする。
int  CGv::AutoAspect(const char *pszMode)
{
	return 1;
}


// 自動表示サイズ調整の設定をする。
int  CGv::AutoResize(const char *pszMode)
{
	return 1;
}


// 画像表示時に同時にドキュメントファイルを表示／非表示の設定をする。
int  CGv::AutoOpenDocument(const char *pszMode)
{
	return 1;
}


// MAGの部分保存データの判断方法の設定をする。
int  CGv::PartMAGCheck(const char *pszMode)
{
	return 1;
}


// 表示イメージの作成方法の設定をする。
int  CGv::ViewImageMode(const char *pszMode)
{
	return 1;
}


// 画像の表示イメージを指定サイズに設定されている方法で作成する。
int  CGv::ResizeDIB(const int nWihth, const int nHeigth)
{
	return 1;
}


// アニメーションGIFの設定、操作を行う。
int  CGv::GifAnimation(const char *pszMode)
{
	return 1;
}


// メッセージの表示の有無の設定をする。
//   ON：メッセージやエラーをダイアログボックスによって表示する
//　OFF：メッセージやエラーを表示しない
int  CGv::DispMsg(const char *pszMode)
{
	CString str;

	str.Format("DispMsg(%s)", pszMode);
	return SendCmd(str);
}


// 表示したファイルを表示ファイルリストに追加する、しないを設定する。
//   ON：表示したファイルを表示ファイルリストに追加する
//  OFF：表示したファイルを表示ファイルリストに追加しない
int  CGv::AddFileList(const char *pszMode)
{
	CString str;

	str.Format("AddFileList(%s)", pszMode);
	return SendCmd(str);
}


// 表示の自動調整の有無、方法をを設定する。
//   自動調整    ON：ファイルを読み込んだときに表示の自動調整をする
//        　　  OFF：ファイルを読み込んだときに表示の自動調整をしない
//   サイズ調整  ON：ウィンドウサイズを画像サイズに合わせる
//          　　OFF：ウィンドウサイズを変更しない
//   位置調整    ON：ウィンドウ位置をウィンドウが画面からはみ出さない位置に移動する
//        　  　OFF：ウィンドウ位置を移動しない
//   拡大率調整  ON：拡大率を画像全体がウィンドウに収まるように調整する
//        　  　OFF：拡大率を調整しない
int  CGv::AutoAdjustMode(const char *pszAuto, const char *pszSize, const char *pszPos, const char *pszZoom)
{
	CString str;

	str.Format("AutoAdjustMode(%s,%s,%s,%s)", pszAuto, pszSize, pszPos, pszZoom);
	return SendCmd(str);
}


// 全画面表示をする。全画面モードを解除するときは他の設定は無効。
int  CGv::MaxDisp(const char *pszMode, const char *pszSize, const char *pszBackColor)
{
	return 1;
}

