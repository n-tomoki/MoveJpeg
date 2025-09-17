
#pragma once

#include <DDEML.H>

class CGv {
protected:
	HSZ m_hszService;
	HSZ m_hszTopic;
	HCONV m_hConv;

	BOOL  m_bInit;
	DWORD m_dwInst;
	CString m_strGvPath;

	void GetProfile();
	void PutProfile();

	BOOL GvScan();
	BOOL GvCheck();
	BOOL GvOpen();
	BOOL GvClose();

	int  SendCmd(const char *PszBuf);
	int  SendRequestText(const char *pszBuf, CString &strAns);
	BOOL CheckDIBRequest();

	int DDE_Start();
	int DDE_End();
	static HDDEDATA DdemlCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, DWORD dwData1, DWORD dwData2);

	void ExecuteCmdLine(const char *pCmd, const char *pCmd2);


public:
	CGv();
	virtual ~CGv();

	BOOL Init();
	void End();
	BOOL ReStart();

	// 画像イメージをできるだけ高速に(縦横どちらかが指定サイズ以上の最小限の大きさで）表示する。
	int  ViewDraftEx(const char *pszFileName, const char cFlag = ' ', const int nWidth = -1, const int nHeight = -1);
	// 指定されたファイルを表示する。
	int  FileOpen(const char *pszFileName);
	// 指定されたファイルを回転フラグで指定された方向に回転して表示する。
	int  FileOpenEx(const char *pszFileName, const char cFlag ='L');
	// 指定されたファイルを保存する。
	int  FileSaveEx(const char *pszFileName, const char *pszImage = "ON", const char *pszSaveFalg = "ON");
	// 画像読み込みを中断する
	int  FileClose();
	// ＧＶの位置、及びサイズを変更する。
	int  SetWindowPos(const int nFlag = -1, const int nPosX = 0, const int nPosY = 0, const int nSizeX = 200, const int nSizeY = 200, const int nMode = 0);
	// クリップボードに表示中のデータを書き出す。
	int  ClipCopy();
	// クリップボードに表示中のデータを表示イメージのまま書き出す。
	int  ClipCopyViewImage();
	// クリップボードのデータを取り込む。
	int  ClipPaste();
	// 表示されている画像を壁紙に設定する。
	int  SetWallPaper(const char *pszMode = "TILE");
	// ＧＶの減色オプションを設定／減色を行う。
	int  Quantize(const char *pszMode = "AUTO", const char *pszColor = "-", const char *pszPass = "-", const char *pszDither = "-");
	// 拡大率を指定する。現在の表示状態も変更する。
	int  ZoomRate(const int nZoom);
	// アスペクト比を指定する。現在の表示状態も変更する。
	int  Aspect(const int nAspect, const char *pszType);
	// 表示されている画像を回転して表示する。
	int  Rotate(const char c = 'L');
	// ＧＶを終了する。
	int  ExitGV();
	// ＧＶの表示・非表示を切り替える。
	int  ShowGV(const char *pszMode = "ON");
	// 機能：ＧＶのアイコン状態を切り替える。
	int  IconGV(const char *pszMode = "ON");
	// 自動アスペクト比調整の設定をする。
	int  AutoAspect(const char *pszMode = "ON");
	// 自動表示サイズ調整の設定をする。
	int  AutoResize(const char *pszMode = "ON");
	// 画像表示時に同時にドキュメントファイルを表示／非表示の設定をする。
	int  AutoOpenDocument(const char *pszMode = "ON");
	// MAGの部分保存データの判断方法の設定をする。
	int  PartMAGCheck(const char *pszMode = "AUTO");
	// 表示イメージの作成方法の設定をする。
	int  ViewImageMode(const char *pszMode = "Speed");
	// 画像の表示イメージを指定サイズに設定されている方法で作成する。
	int  ResizeDIB(const int nWihth, const int nHeigth);
	// アニメーションGIFの設定、操作を行う。
	int  GifAnimation(const char *pszMode = "ON");
	// メッセージの表示の有無の設定をする。
	int  DispMsg(const char *pszMode = "ON");
	// 表示したファイルを表示ファイルリストに追加する、しないを設定する。
	int  AddFileList(const char *pszMode = "ON");
	// 表示の自動調整の有無、方法をを設定する。
	int  AutoAdjustMode(const char *pszAuto = "ON", const char *pszSize = "ON", const char *pszPos = "ON", const char *pszZoom = "ON");
	// 全画面表示をする。全画面モードを解除するときは他の設定は無効。
	int  MaxDisp(const char *pszMode = "ON", const char *pszSize = "ON", const char *pszBackColor = "White");
};

