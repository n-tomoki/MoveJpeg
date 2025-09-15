

//#include "stdafx.h"
#include "pch.h"
#include "file.h"

//////////////////////////////////////////////////////////////
// キャッシュ付を使用する場合は以下の定義を“stdafx.h”に明記すること
// #ifndef _CASH_FILE_
// #define _CASH_FILE_
// #endif
//
// エラーメッセージを強制的に英語にする(自動変更)
// “MsgEnglish(TRUE)”
//////////////////////////////////////////////////////////////

//============================================================
// エラーメッセージの切り替え
//============================================================
BOOL File::m_bFirst;
BOOL File::m_bEnglish;

BOOL File::MsgEnglish(const BOOL bEnglish)
{
	m_bFirst   = TRUE;
	m_bEnglish = bEnglish;

	return m_bEnglish;
}


BOOL File::MsgEnglish()
{
	return m_bEnglish;
}


//============================================================
// 開始処理＆終了処理
//============================================================

/// <summary>コンストラクタ</summary>
File::File()
{
	m_bOpen      = FALSE;
	m_uErrCode   = FILE_ERR_NONE;
	m_enCharCode = _PENDING;
	
	m_pMsg    = NULL;
	m_pFmtBuf = NULL;

	if (!m_bFirst) {
		m_bFirst = TRUE;

		char sz[25];

		if (GetLocaleInfoA(LOCALE_SYSTEM_DEFAULT, LOCALE_IDEFAULTCODEPAGE, sz, 20)) {
			if (932 != atoi(sz)) {
				m_bEnglish = TRUE;
			}
		}
	}
}


/// <summary>デストラクタ</summary>
File::~File()
{
	if (m_bOpen) {
		Close();
	}

	if (m_pFmtBuf != NULL) { delete []m_pFmtBuf; }
}


//============================================================
// ファイルのオープン
//============================================================

/// <summary>
/// ファイルオープン
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <param name="enOpenMode">開くモード</param>
/// <param name="enCharCode">キャラクターコード</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::Open(const char *pszFileName, const OpenMode enOpenMode, CharCode enCharCode)
{
	BOOL bCode = FALSE;
	UINT uOpenFlag = 0;
	int  nRetry = 5;
	int  nOpenMode = (enOpenMode & ~FILE_ACCESS_LOCK);
	CFileException Excep;

	if (enOpenMode & FILE_ACCESS_LOCK) { uOpenFlag = CFile::shareExclusive; }
	else                               { uOpenFlag = CFile::shareDenyNone;  }

	if (m_bOpen) { Close(); }

	while (1) {
		if (nOpenMode == _READ) {
			bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeRead, &Excep);
		} else if (nOpenMode == _WRITE) {
			bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeCreate | CFile::modeWrite, &Excep);
		} else if (nOpenMode == _APPEND) {
			bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeReadWrite, &Excep);
			if (!bCode) {
				bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeCreate | CFile::modeWrite, &Excep);
			}
			if (bCode) {
				SeekEndAction();	// ファイルの最後に移動
			}
		} else if (nOpenMode == _RDWR) {
			bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeReadWrite, &Excep); 
			if (!bCode) {
				bCode = m_File.Open(pszFileName, uOpenFlag | CFile::modeCreate | CFile::modeWrite, &Excep);
			}
		}

		if (bCode)                                   { break; }
		if (Excep.m_cause != Excep.sharingViolation) { break; }
		if (--nRetry == 0)                           { break; }
		Sleep(100);
	}

	if (bCode) {
		m_bOpen      = TRUE;
		m_uErrCode   = FILE_ERR_NONE;
		m_enCharCode = enCharCode;
	} else {
		m_uErrCode   = Excep.m_cause;
	}
	return m_bOpen;
}


//============================================================
// ファイルのクローズ
//============================================================

/// <summary>
/// ファイルのクローズ
/// </summary>
/// <returns>TRUE:失敗/FALSE:成功</returns>
BOOL File::Close()
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return FALSE; }

	BOOL bRet = FALSE;

	try {
		m_File.Close();
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
		bRet = TRUE;
	}
	m_bOpen      = FALSE;
	m_enCharCode = _PENDING;

	return bRet;
}


//============================================================
// ファイルからの読み込み
//============================================================

/// <summary>
/// 1バイト読み込む
/// </summary>
/// <returns>成功:0x00～0xff/失敗:_NO_DATA</returns>
unsigned int File::Read()
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return _NO_DATA; }

	char c;

	try {
		if (m_File.Read(&c, 1)) {
			return (unsigned int)c;
		}
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return _NO_DATA;
}


/// <summary>
/// nバイト読み込む 
/// </summary>
/// <param name="pBuf">読み込む場所</param>
/// <param name="nLen">読み込むバイト数</param>
/// <returns>読み込んだバイト数</returns>
int File::Read(char *pBuf, const int nLen)
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return 0; }

	int n = 0;

	try {
		n = m_File.Read(pBuf, nLen);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return n;
}


/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="pBuf">読み込む場所</param>
/// <param name="nMax">読み込む最大バイト数</param>
/// <param name="pNewLineSize">改行バイト数を返す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::Gets(char *pBuf, const int nMax, int *pNewLineSize)
{
	*pBuf = '\0';
	if (!m_bOpen) { return FALSE; }

#ifdef _DEBUG
	if (m_enCharCode == _UTF8) {
		TRACE("UTF-8モードで、[File::Gets(char,int,int*)]が呼ばれた！！");
	}
#endif

	int nLen;
	int nNewLineSize = 0;
	BOOL bRet = FALSE;
	unsigned int n;
	char *cp = pBuf;

	nLen = nMax - 1;
	while (--nLen) {
		n = Read();
		if (n == _NO_DATA) { // 最後まで読んだら終わり
			break;
		} else if (n == 0) { // 文字列の読み込みで「0」はありえない
			bRet = FALSE;
			break;
		}
		bRet = TRUE;
		if (n == C_LF || n == C_CR) {
			nNewLineSize = 1;
			unsigned int n2 = Read();	// dumy
			if (n2 == _NO_DATA) {}
			else if (n == C_LF && n2 == C_CR) { nNewLineSize = 2; }
			else if (n == C_CR && n2 == C_LF) { nNewLineSize = 2; }
			else {
				SeekOff(-1);
			}
			break;
		}
		*cp = (char)n;
		cp++;
	}
	*cp = '\0';

	if (pNewLineSize != NULL) { *pNewLineSize = nNewLineSize; }

	return bRet;
}


/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="str">CStringを渡す</param>
/// <param name="pNewLineSize">改行バイト数を返す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::Gets(CString &str, int *pNewLineSize)
{
	str.Empty();
	if (!m_bOpen) { return FALSE; }

	CMemory tmp;

	BOOL bRet = Gets(tmp, pNewLineSize);

	// UTF8ならShiftJisに変換する
	if (bRet) {
		if (m_enCharCode == _PENDING) { m_enCharCode = AnalyzeCharCode(tmp); }
		if (m_enCharCode == _UTF8)    { UTF8ToShiftJis(tmp, str); }
		else                          { str = (const char *)tmp;  }
	}

	return bRet;
}


/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="mem">CMemoryを渡す</param>
/// <param name="pNewLineSize">改行バイト数を返す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::Gets(CMemory &mem, int *pNewLineSize)
{
	mem.Clear();
	if (!m_bOpen) { return FALSE; }

	int n;
	int nCnt;
	int nNewLineSize = 0;
	BOOL bRet = FALSE;
	BOOL bRun = TRUE;
	char sz[FILE_GETS_BUFFER_SIZE+5];

	while (bRun) {
		nCnt = 0;
		while (nCnt < FILE_GETS_BUFFER_SIZE) {
			n = Read();
			if (n == _NO_DATA) { // 最後まで読んだら終わり
				bRun = FALSE;
				break;
			} else if (n == 0) { // 文字列の読み込みで「0」はありえない
				bRun = FALSE;
				bRet = FALSE;
				break;
			}
			bRet = TRUE;
			if (n == C_LF || n == C_CR) {
				nNewLineSize = 1;
				unsigned int n2 = Read();	// dumy
				if (n2 == _NO_DATA) {}
				else if (n == C_LF && n2 == C_CR) { nNewLineSize = 2; }
				else if (n == C_CR && n2 == C_LF) { nNewLineSize = 2; }
				else {
					SeekOff(-1);
				}
				bRun = FALSE;
				break;
			}
			sz[nCnt++] = n;
		}
		sz[nCnt] = '\0';
		mem += sz;
	}
	
	if (pNewLineSize != NULL) { *pNewLineSize = nNewLineSize; }

	return bRet;
}


//============================================================
// ファイルの書き込み
//============================================================

/// <summary>
/// nバイト書き込む
/// </summary>
/// <param name="pBuf">書き込む文字列</param>
/// <param name="nLen">書き込むバイト数</param>
void File::Write(const char *pszBuf, const int nLen)
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return; }

	try {
		m_File.Write(pszBuf, nLen);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}
}


/// <summary>
/// 文字列を書き込む
/// </summary>
/// <param name="pszBuf">書き込む文字列</param>
void File::Write(const char *pszBuf)
{
	if (!m_bOpen)        { return; }
	if (pszBuf  == NULL) { return; }
	if (*pszBuf == '\0') { return; }

	if (m_enCharCode == _UTF8) {
		WriteUtf8(pszBuf);
	} else {
		Write(pszBuf, (int)strlen(pszBuf));
	}
}


/// <summary>
/// 1行書き込む
/// </summary>
/// <param name="pszBuf">書き込む文字列</param>
void File::Puts(const char *pszBuf)
{
	if (!m_bOpen) { return; }

	Write(pszBuf);
	Write("\r\n", 2);
}


/// <summary>
/// 1バイト書き込む
/// </summary>
/// <param name="c">書き込む文字</param>
void File::Putc(const char c)
{
	if (!m_bOpen) { return; }

	Write(&c, 1);
}


/// <summary>
/// 指定されたフォーマットで書き込む
/// </summary>
/// <param name="fmt">フォーマット</param>
void File::Printf(const char *fmt, ...)
{
	if (!m_bOpen) { return; }

	va_list args;
	va_start(args, fmt);

	if (m_pFmtBuf == NULL) { m_pFmtBuf  = new char[FORMAT_BUFFER_SIZE + 2]; }

	_vsnprintf_s(m_pFmtBuf, FORMAT_BUFFER_SIZE, FORMAT_BUFFER_SIZE, fmt, args);
	Write(m_pFmtBuf);
}


//============================================================
// ファイルのポイント操作
//============================================================

/// <summary>
/// ファイルポインタを移動する
/// </summary>
/// <param name="lPos">移動する位置</param>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekSet(const DWORD64 lPos)
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return -1; }

	DWORD64 dw64Val = -1;

	try {
		dw64Val = m_File.Seek(lPos, CFile::begin);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return dw64Val;
}


/// <summary>
/// ファイルポインタを移動する
/// </summary>
/// <param name="lPos">移動するオフセット</param>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekOff(const DWORD64 lPos)
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return -1; }

	DWORD64 dw64Val = -1;

	try {
		dw64Val = m_File.Seek(lPos, CFile::current);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return dw64Val;
}


/// <summary>
/// ファイルポインタを先頭に移動する
/// </summary>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekTop()
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return -1; }

	DWORD64 dw64Val = -1;

	try {
		m_File.Seek(0, CFile::begin);
		dw64Val = 0;
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return dw64Val;
}


/// <summary>
/// ファイルポインタを最後に移動する
/// </summary>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekEnd()
{
	if (!m_bOpen) { return -1; }

	return SeekEndAction();
}


/// <summary>
/// ファイルポインタを最後に移動する
/// </summary>
/// <returns>ファイルポインタ/失敗:-1</returns>
DWORD64 File::SeekEndAction()
{
	DWORD64 dw64Val = -1;

	m_uErrCode = FILE_ERR_NONE;
	try {
		dw64Val = m_File.Seek(0, CFile::end);
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}

	return dw64Val;
}


/// <summary>
/// ファイルポインタを位置を返す
/// </summary>
/// <returns>ファイルポインタの位置</returns>
DWORD64 File::GetPosition()
{
	m_uErrCode = FILE_ERR_NONE;
	if (!m_bOpen) { return 0; }

	DWORD64 dw64Pos = -1;

	try {
		dw64Pos = m_File.GetPosition();
	} catch (CFileException *e) {
		m_uErrCode = e->m_cause;
		e->Delete();
	}
	return dw64Pos;
}
	

//============================================================
// ファイルの削除
//============================================================

/// <summary>
/// ファイルの削除
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <param name="bCheck">消えたか？確認する(FALSE)</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL File::Unlink(const char *pszFileName, const BOOL bCheck)
{
	BOOL bErr = FALSE;

	try {
		if (IsExistFile(pszFileName)) {
			CFile::Remove(pszFileName);

			if (bCheck == TRUE) {
				// 最大約２秒まで待つ
				int nLoop = 200;
				do {
					Sleep(10);
					if (!IsExistFile(pszFileName)) { nLoop = 0; }
				} while (nLoop--);
			}
		}
	} catch (CFileException *e) {
		bErr = TRUE;
		e->Delete();
	}

	return bErr;
}


/// <summary>
/// ファイルの削除
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <param name="bCheck">消えたか？確認する(FALSE)</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL File::UnlinkFile(const char *pszFileName, const BOOL bCheck)
{
	return Unlink(pszFileName, bCheck);
}


/// <summary>
/// 指定フォルダ以下を削除する
/// </summary>
/// <param name="pszPath">フォルダ</param>
/// <param name="bParentUnlink">指定フォルダも削除する(TRUE)</param>
/// <returns>TRUE</returns>
BOOL File::UnlinkFolder(const char *pszPath, const BOOL bParentUnlink)
{
	UnlinkFolderMain(pszPath);
	if (bParentUnlink) {
		RemoveDirectoryA(pszPath);
	}

	return TRUE;
}


/// <summary>
/// 指定フォルダ以下を削除する(実行部)
/// </summary>
/// <param name="pszPath">フォルダ</param>
/// <returns>TRUE</returns>
BOOL File::UnlinkFolderMain(const char *pszPath)
{
	int nPathLen   = (int)strlen(pszPath) + 5;
	char *pSrcFile = new char[nPathLen + 1];

	strcpy_s(pSrcFile, nPathLen, pszPath);
	strcat_s(pSrcFile, nPathLen, "\\*.*");

	CFileFind Find;
	BOOL bFind = Find.FindFile(pSrcFile);
	while (bFind) {
		bFind = Find.FindNextFileA();
		if (Find.IsDots()) continue;

		if (Find.IsDirectory()) {
			int nLen = Find.GetFilePath().GetLength() + 2;
			char *p  = new char[nLen + 1];

			strcpy_s(p, nLen, (const char *)Find.GetFilePath());
			UnlinkFolderMain(p);
			RemoveDirectoryA(p);

			delete []p;
			continue;
		}

		Unlink(Find.GetFilePath());
	}

	delete []pSrcFile;

	return TRUE;
}


//============================================================
// ステイタス関係
//============================================================

/// <summary>
/// ファイルの更新秒を返す
/// </summary>
/// <returns>更新秒</returns>
time_t File::GetStatusTimet()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return st.m_mtime.GetTime();
}


/// <summary>
/// ファイルの更新秒を返す
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>更新秒</returns>
time_t File::GetStatusTimet(const char *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return st.m_mtime.GetTime();
}


/// <summary>
/// ファイルサイズを返す
/// </summary>
/// <returns>ファイルサイズ</returns>
DWORD64 File::GetStatusSize()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return st.m_size;
}


/// <summary>
/// ファイルサイズを返す
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>ファイルサイズ</returns>
DWORD64 File::GetStatusSize(const char *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return st.m_size;
}


/// <summary>
/// ファイルのアトリビュートを返す
/// </summary>
/// <returns>ファイルのアトリビュート(エラー:-1)</returns>
BYTE  File::GetStatusAttr()
{
	if (!m_bOpen) { return -1; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return -1; }

	return (BYTE)st.m_attribute; 
}


/// <summary>
/// ファイルのアトリビュートを返す
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>ファイルのアトリビュート(エラー:-1)</returns>
BYTE  File::GetStatusAttr(const char *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return -1; }

	return (BYTE)st.m_attribute; 
}


//============================================================
// ファイルの名前変更
//============================================================

/// <summary>
/// ファイル名の変更
/// </summary>
/// <param name="pszOldName">旧ファイル名</param>
/// <param name="pszNewName">新ファイル名</param>
/// <returns>FALSE:成功/TRUE:失敗</returns>
BOOL File::Rename(const char *pszOldName, const char *pszNewName)
{
	BOOL bErr = FALSE;

	try {
		if (IsExistFile(pszOldName)) {
			CFile::Rename(pszOldName, pszNewName);
		} else {
			bErr = TRUE;
		}
	} catch(CFileException *p) {
		bErr = TRUE;
		p->Delete();
	}

	return bErr;
}


//============================================================
// 空ファイルの作成
//============================================================

/// <summary>
/// 空ファイルの作成
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL File::CreateEmptyFile(const char *pszFileName)
{
	BOOL  bOpen = FALSE;
	CFile File;

	if (TRUE == (bOpen = File.Open(pszFileName, CFile::modeCreate | CFile::modeWrite))) {
		File.Close();
	}

	return bOpen;
}


/// <summary>
/// フォルダの作成
/// </summary>
/// <param name="pszPath">フォルダ名</param>
/// <returns>TRUE:成功/FALSE:失敗(既にフォルダが有る)</returns>
BOOL File::CreateFolder(const char *pszPath)
{
	return CreateDirectoryA(pszPath, NULL);
}


//=============================================================
// [UTF-8]<-->[SHIFT-JIS]変換関係
//=============================================================

/// <summary>
/// UTF-8に変換して書き込む
/// </summary>
/// <param name="pszBuf">書き込む文字列</param>
void File::WriteUtf8(const char *pszBuf)
{
	CMemory tmp;

	ShiftJisToUTF8(pszBuf, tmp);
	Write(tmp, (int)strlen(tmp));
}


/// <summary>
/// キャラクターコードの判断を頑張る
/// </summary>
/// <param name="memBuf">読み込んだ文字列</param>
File::CharCode File::AnalyzeCharCode(CMemory &memBuf, CharCode enCharCode)
{
	if (enCharCode != _PENDING) { return enCharCode; }

	const BYTE *cp = (const BYTE *)memBuf;
	int nSize      = memBuf.GetSize();

	while (nSize-- && enCharCode == _PENDING) {
		// １バイト目
		if      (0x81 <= *cp && *cp <= 0x9f) { enCharCode = _SHIFT_JIS; }
		else if (0xc0 <= *cp && *cp <= 0xdf) { enCharCode = _UTF8;      }
		else if (0xf0 <= *cp && *cp <= 0xf7) { enCharCode = _UTF8;      }
		else if (0xa0 <= *cp && *cp <= 0xc1) { enCharCode = _SHIFT_JIS; }
		else if (0xe0 <= *cp && *cp <= 0xef) {
			// ２バイト目
			cp++;
			if (0x40 <= *cp && *cp <= 0x7e) { enCharCode = _SHIFT_JIS; }
			else if (0x80 <= *cp) {
				if (*cp <= 0xbf)  { // 0x80-0xbf
					// ３バイト目
					cp++;
					if (0x80 <= *cp && *cp <= 0xbf) { enCharCode = _UTF8;      }
					else                            { enCharCode = _SHIFT_JIS; }
				} else if (*cp <= 0xef) { // 0xc0-0xef
					enCharCode = _SHIFT_JIS;
				}
			}
		}
		cp++;
	}

	return enCharCode;
}


/// <summary>
/// ShiftJisをUTF-8に変換する
/// </summary>
/// <param name="pszShiftjis">変換元の文字列</param>
/// <param name="memUtf8">変換後の文字列</param>
void File::ShiftJisToUTF8(const char *pszShiftjis, CMemory &memUtf8)
{
	// 変換する文字列がない場合は、変換処理をしない
	if (*pszShiftjis == '\0') {
		memUtf8.Clear();
		return;
	}

	// Uniocdeに変換後のサイズを計算する
	int nLenShiftjis = (int)strlen(pszShiftjis); 
	int nLenUnicode  = MultiByteToWideChar(CP_ACP, 0, pszShiftjis, nLenShiftjis + 1, NULL, 0);

	// Uniocdeに変換する
	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
	MultiByteToWideChar(CP_ACP, 0, pszShiftjis, nLenShiftjis + 1, pBufUnicode, nLenUnicode);
	
	// UTF-8に変換後のサイズを計算する
	int nLenUtf8 = WideCharToMultiByte(CP_UTF8, 0, pBufUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// UTF-8に変換する
	char *pBufUTF8 = new char[nLenUtf8 + 1];
	WideCharToMultiByte(CP_UTF8, 0, pBufUnicode, nLenUnicode, pBufUTF8, nLenUtf8, NULL, NULL);

	memUtf8 = pBufUTF8;

	delete []pBufUTF8;
	delete []pBufUnicode;
}


/// <summary>
/// UTF-8をShiftJisに変換する
/// </summary>
/// <param name="memUtf8">変換元の文字列</param>
/// <param name="str">変換後の文字列</param>
void File::UTF8ToShiftJis(CMemory &memUtf8, CString &str)
{
	int nLenUtf8 = memUtf8.GetSize();

	// 変換する文字列がない場合は、変換処理をしない
	if (nLenUtf8 == 0) {
		str.Empty();
		return;
	}

	// Unicodeに変換後のサイズを計算する
	int nLenUnicode = MultiByteToWideChar(CP_UTF8, 0, memUtf8, nLenUtf8 + 1, NULL, 0);

	// UniCodeに変換する
	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
	MultiByteToWideChar(CP_UTF8, 0, memUtf8, nLenUtf8 + 1, pBufUnicode, nLenUnicode);

	// ShiftJisに変換後のサイズを計算する
	int nLenShiftJis = WideCharToMultiByte(CP_ACP, 0, pBufUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// ShiftJisに変換する
	char *pBufShiftJis = new char[nLenShiftJis + 1];
	WideCharToMultiByte(CP_ACP, 0, pBufUnicode, nLenUnicode, pBufShiftJis, nLenShiftJis, NULL, NULL);

	str = pBufShiftJis;

	delete []pBufShiftJis;
	delete []pBufUnicode;
}



//============================================================
// 使用するキャラクターコード関係
//============================================================

/// <summary>
/// 現在のキャラクターコードを返す
/// </summary>
/// <returns>_PENDING/_SHIFT_JIS/_UTF8</returns>
File::CharCode File::GetCharCode()
{
	return m_enCharCode;
}


/// <summary>
/// キャラクターコードをセットする(一度セットすると変更は出来ません)
/// </summary>
/// <param name="enMode">_PENDING/_SHIFT_JIS/_UTF8</param>
/// <returns></returns>
File::CharCode File::SetCharCode(const File::CharCode enMode)
{
	if (m_enCharCode == _PENDING) {
		switch (enMode) {
		case File::_PENDING:
		case File::_SHIFT_JIS:
		case File::_UTF8:
			m_enCharCode = enMode;
			break;
		}
	}

	return m_enCharCode;
}


//============================================================
// その他
//============================================================

/// <summary>
/// ファイルがあるか？
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <returns>TRUE:ある/FALSE:ない</returns>
BOOL File::IsExistFile(const char *pszFileName)
{
	DWORD dwbAttr = GetFileAttributesA(pszFileName);

	return (dwbAttr != INVALID_FILE_ATTRIBUTES && !(dwbAttr & FILE_ATTRIBUTE_DIRECTORY));
}


/// <summary>
/// フォルダがあるか？
/// </summary>
/// <param name="pszFilePath">フォルダ名</param>
/// <returns>TRUE:ある/FALSE:ない</returns>
BOOL File::IsExistFolder(const char *pszFilePath)
{
	DWORD dwbAttr = GetFileAttributesA(pszFilePath);

	return (dwbAttr != INVALID_FILE_ATTRIBUTES && dwbAttr & FILE_ATTRIBUTE_DIRECTORY);
}


/// <summary>
/// 最後のエラーコードを返す
/// </summary>
/// <returns>エラーコード</returns>
UINT File::GetErrorCode()
{
	return m_uErrCode;
}


/// <summary>
/// 最後のエラー内容を返す
/// </summary>
/// <param name="pstr">受け取る位置</param>
/// <param name="nMaxLen">受け取る最大文字数</param>
/// <returns>エラーコード</returns>
UINT File::GetErrorString(char *pstr, const int nMaxLen)
{
	const char *pMsg = GetErrorString();
	int nLen         = (int)strlen(pMsg);

	if (nLen < nMaxLen - 1) {
		strcpy_s(pstr, nMaxLen, pMsg);
	} else {
		*pstr = '\0';
	}

	return m_uErrCode;
}


/// <summary>
/// 最後のエラー内容
/// </summary>
/// <returns>エラー内容</returns>
const char *File::GetErrorString()
{
	m_pMsg = NULL;

	switch (GetErrorCode()) {
	case FILE_ERR_NONE:
		if (m_bEnglish) { m_pMsg = "No error occurred."; }
		else            { m_pMsg = "エラーはありません"; }
		break;

	case FILE_ERR_GENERIC:
	default:
		if (m_bEnglish) { m_pMsg = "An unspecified error occurred."; }
		else            { m_pMsg = "不明なエラーが発生しました"; }
		break;
		
	case FILE_ERR_FILE_NOT_FOUND:
		if (m_bEnglish) { m_pMsg = "The file could not be located."; }
		else            { m_pMsg = "ファイルがありません"; }
		break;
		
	case FILE_ERR_BAD_PATH:
		if (m_bEnglish) { m_pMsg = "All or part of the path is invalid."; }
		else            { m_pMsg = "パスが不正です"; }
		break;
		
	case FILE_ERR_TOO_MANY_OPEN_FILES:
		if (m_bEnglish) { m_pMsg = "The permitted number of open files was exceeded."; }
		else            { m_pMsg = "これ以上ファイルがオープンできません"; }
		break;
		
	case FILE_ERR_ACCESS_DENIED:
		if (m_bEnglish) { m_pMsg = "The file could not be accessed."; }
		else            { m_pMsg = "ファイルのアクセスが禁止されています"; }
		break;
		
	case FILE_ERR_INVALID_FILE:
		if (m_bEnglish) { m_pMsg = "There was an attempt to use an invalid file handle."; }
		else            { m_pMsg = "ファイルハンドルが不正です"; }
		break;
		
	case FILE_ERR_REMOVE_CURRENT_DIR:
		if (m_bEnglish) { m_pMsg = "The current working directory cannot be removed."; }
		else            { m_pMsg = "カレント・フォルダーの削除はできません"; }
		break;

	case FILE_ERR_DIRCTORY_FULL:
		if (m_bEnglish) { m_pMsg = "There are no more directory entries."; }
		else            { m_pMsg = "フォルダーが一杯です"; }
		break;
		
	case FILE_ERR_BAD_SEEK:
		if (m_bEnglish) { m_pMsg = "There was an error trying to set the file pointer."; }
		else            { m_pMsg = "ファイル・シークに失敗しました"; }
		break;

	case FILE_ERR_HARD_IO:
		if (m_bEnglish) { m_pMsg = "There was a hardware error."; }
		else            { m_pMsg = "ハードウェア・エラー発生"; }
		break;

	case FILE_ERR_SHRING_VIOLALATION:
		if (m_bEnglish) { m_pMsg = "SHARE.EXE was not loaded, or a shared region was locked."; }
		else            { m_pMsg = "他のアプリケーションがファイルをロックしています"; }
		break;
		
	case FILE_ERR_LOCK_VIOLATION:
		if (m_bEnglish) { m_pMsg = "There was an attempt to lock a region that was already locked."; }
		else            { m_pMsg = "ロック済み領域のロック要求"; }
		break;

	case FILE_ERR_DISK_FULL:
		if (m_bEnglish) { m_pMsg = "The disk is full."; }
		else            { m_pMsg = "ディスクが一杯です"; }
		break;

	case FILE_ERR_END_OF_FILE:
		if (m_bEnglish) { m_pMsg = "The end of file was reached."; }
		else            { m_pMsg = "END OF FILE"; }
		break;
	}

	return m_pMsg;
}


#ifdef _CASH_FILE_
//********************************************************************************
// 一気アクセス
//********************************************************************************

/// <summary>コンストラクタ</summary>
CCashFile::CCashFile()
{
	m_bUse      = FALSE;
	m_nMode     = File::_READ;
	m_lPos      = 0;
	m_lSize     = 0;
	m_lMemSize  = 0;
	m_pBuf      = NULL;
	m_pFileName = NULL;

	m_enCharCode = _PENDING;
}

/// <summary>デストラクタ</summary>
CCashFile::~CCashFile()
{
	if (m_bUse) {
		Close();
	}
}


//================================================================================
// Open&Close
//================================================================================

/// <summary>
/// ファイルオープン
/// </summary>
/// <param name="pszFileName">ファイル名</param>
/// <param name="nMode">開くモード</param>
/// <param name="lSize">確保する容量(書き込み時:Default=0)</param>
/// <param name="enCharCode">キャラクターコード</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL CCashFile::Open(const char *pszFileName, const int nMode, const long lSize, CharCode enCharCode)
{
	int nOpenMode = nMode & ~FILE_ACCESS_LOCK;

	ASSERT(!(nOpenMode == File::_WRITE ||nOpenMode == File::_APPEND));

	if (m_bUse)                                      { return FALSE; }
	if (pszFileName == NULL || *pszFileName == '\0') { return FALSE; }
	if (nOpenMode   == File::_WRITE)                 { return FALSE; }
	if (nOpenMode   == File::_APPEND)                { return FALSE; }

	m_nMode      = nOpenMode;
	m_enCharCode = enCharCode;

	if (m_pFileName != NULL) { delete []m_pFileName; }

	int nLen    = (int)strlen(pszFileName) + 1;
	m_pFileName = new char [nLen + 1];
	strcpy_s(m_pFileName, nLen, pszFileName);

	switch (m_nMode) {
	case File::_READ:
	case File::_RDWR:
		if (File::Open(m_pFileName, File::_READ_LOCK, m_enCharCode)) {

			// 100MBを超えるファイルは扱わない
			DWORD64 dw64Size = File::GetStatusSize();
			if (dw64Size >= MAX_ALLOCATION_MEMORY) { File::Close(); break; }

			m_lSize = MemInit((long)dw64Size);
			if (!m_lSize)                     { File::Close(); break; }
			if (!File::Read(m_pBuf, m_lSize)) { File::Close(); MemFree(); break; }
			m_bUse = TRUE;
			if (File::_APPEND == m_nMode) {
				m_lPos = m_lSize;
			} else {
				m_lPos = 0;
			}
			File::Close();
		}
		break;
	}

	return m_bUse;
}


/// <summary>
/// ファイルのクローズ
/// </summary>
/// <param name="bDestruction">破棄する(Deault:FALSE)</param>
/// <returns>FALSE:成功/FALSE:失敗</returns>
BOOL CCashFile::Close(const BOOL bDestruction)
{
	if (!m_bUse) { return FALSE; }

	BOOL bRet = FALSE;

	if (m_nMode == File::_RDWR) {
		if (!bDestruction) {
			if (File::Open(m_pFileName, File::_WRITE_LOCK, m_enCharCode)) {
				File::Write(m_pBuf, m_lSize);
				bRet = File::Close();
			} else {
				bRet = TRUE;	// エラー
			}
		}
	}

	MemFree();
	m_bUse = FALSE;
	m_enCharCode = _PENDING;

	if (m_pFileName != NULL) { delete []m_pFileName; m_pFileName = NULL; }

	return bRet;
}


//================================================================================
// 読み込み
//================================================================================

/// <summary>
/// 1バイト読み込む
/// </summary>
/// <returns>成功:0x00～0xff/失敗:_NO_DATA</returns>
unsigned int CCashFile::Read()
{
	int nCode = _NO_DATA;

	if (!m_bUse)                                             { return nCode; }
	if (!(m_nMode == File::_READ || m_nMode == File::_RDWR)) { return nCode; }

	if (m_lPos < m_lSize) {
		nCode = (unsigned char)(*(m_pBuf + m_lPos));
		m_lPos++;
	}

	return (unsigned int)nCode;
}


/// <summary>
/// nバイト読み込む
/// </summary>
/// <param name="pBuf">読み込む場所</param>
/// <param name="lSize">読み込むバイト数</param>
/// <param name="lPos">読み込み位置(Default:-1)</param>
/// <returns>読み込んだバイト数</returns>
long CCashFile::Read(char *pBuf, const long lSize, const long lPos)
{
	if (!m_bUse)                                             { return 0; }
	if (!(m_nMode == File::_READ || m_nMode == File::_RDWR)) { return 0; }

	long rp;
	long lLen = 0;

	if (lPos == -1) { rp = m_lPos; }
	else            { rp = lPos;   }

	if (rp >= m_lSize)             { return lLen;         }
	else if (rp + lSize > m_lSize) { lLen = m_lSize - rp; }
	else                           { lLen = lSize;        }

	memcpy_s((void *)pBuf, lSize, (void *)(m_pBuf + rp), (size_t)lLen);

	if (lPos == -1) { m_lPos += lLen; }

	return lLen;
}


/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="pBuf">読み込むポインタ</param>
/// <param name="lMax">読み込む最大バイト数</param>
/// <param name="pNewLineSize">改行バイト数を返す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL CCashFile::Gets(char *pBuf, const long lMax, int *pNewLineSize)
{
#ifdef _DEBUG
	if (m_enCharCode == _UTF8) {
		TRACE("UTF-8モードで、[CCashFile::Gets(char,int,int*)]が呼ばれた！！");
	}
#endif

	if (pNewLineSize != NULL) { *pNewLineSize = 0; }

	if (!m_bUse)                                             { return FALSE; }
	if (!(m_nMode == File::_READ || m_nMode == File::_RDWR)) { return FALSE; }

	int n;
	int nNewLineSize = 0;
	BOOL bRet = FALSE;
	long lCnt = lMax - 1;
	char *cp = pBuf;

	while (--lCnt) {
		n = Read();
		if (n == _NO_DATA) { // 最後まで読んだら終わり
			break;
		} else if (n == 0) { // 文字列の読み込みで「0」はありえない
			bRet = FALSE;
			break;
		}
		bRet = TRUE;
		if (n == C_CR || n == C_LF) {
			nNewLineSize = 1;
			int n2 = Read();	// dumy
			if (n2 == _NO_DATA) {}
			else if (n == C_LF && n2 == C_CR) { nNewLineSize = 2; }
			else if (n == C_CR && n2 == C_LF) { nNewLineSize = 2; }
			else {
				SeekOff(-1);
			}
			break;
		}

		*cp++ = n;
	}
	*cp = '\0';

	if (pNewLineSize != NULL) { *pNewLineSize = nNewLineSize; }

	return bRet;
}


/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="str">読み込む場所</param>
/// <param name="pNewLineSize">改行バイト数を返す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL CCashFile::Gets(CString &str, int *pNewLineSize)
{
	if (pNewLineSize != NULL) { *pNewLineSize = 0; }

	str.Empty();
	if (!m_bUse)                                             { return FALSE; }
	if (!(m_nMode == File::_READ || m_nMode == File::_RDWR)) { return FALSE; }

	CMemory tmp;

	BOOL bRet = Gets(tmp, pNewLineSize);

	// UTF8ならShiftJisに変換する
	if (bRet) {
		if (m_enCharCode == _PENDING) { m_enCharCode = AnalyzeCharCode(tmp); }
		if (m_enCharCode == _UTF8)    { UTF8ToShiftJis(tmp, str); }
		else                          { str = (const char *)tmp;  }
	}

	return bRet;
}


/// <summary>
/// 1行読み込む
/// </summary>
/// <param name="str">読み込む場所</param>
/// <param name="pNewLineSize">改行バイト数を返す</param>
/// <returns>TRUE:成功/FALSE:失敗</returns>
BOOL CCashFile::Gets(CMemory &mem, int *pNewLineSize)
{
	if (pNewLineSize != NULL) { *pNewLineSize = 0; }

	mem.Clear();
	if (!m_bUse)                                             { return FALSE; }
	if (!(m_nMode == File::_READ || m_nMode == File::_RDWR)) { return FALSE; }

	int n;
	int nCnt;
	int nNewLineSize = 0;
	BOOL bRet = FALSE;
	BOOL bRun = TRUE;
	char sz[FILE_GETS_BUFFER_SIZE+5];

	while (bRun) {
		nCnt = 0;
		while (nCnt < FILE_GETS_BUFFER_SIZE) {
			n = Read();
			if (n == _NO_DATA) { // 最後まで読んだら終わり
				bRun = FALSE;
				break;
			} else if (n == 0) { // 文字列の読み込みで「0」はありえない
				bRun = FALSE;
				bRet = FALSE;
				break;
			}
			bRet = TRUE;
			if (n == C_LF || n == C_CR) {
				nNewLineSize = 1;
				unsigned int n2 = Read();	// dumy
				if (n2 == _NO_DATA) {}
				else if (n == C_LF && n2 == C_CR) { nNewLineSize = 2; }
				else if (n == C_CR && n2 == C_LF) { nNewLineSize = 2; }
				else {
					SeekOff(-1);
				}
				bRun = FALSE;
				break;
			}
			sz[nCnt++] = n;
		}
		sz[nCnt] = '\0';
		mem += sz;
	}

	if (pNewLineSize != NULL) { *pNewLineSize = nNewLineSize; }

	return bRet;
}


/// <summary>
/// ファイルサイズを返す
/// </summary>
/// <returns>ファイルサイズ</returns>
long CCashFile::GetSize()
{
	if (!m_bUse) { return 0; }

	return  m_lSize;
}


/// <summary>
/// ファイル名を返す
/// </summary>
/// <returns>ファイル名</returns>
const char *CCashFile::GetFileName()
{
	return m_pFileName;
}


//================================================================================
// 書き込み
//================================================================================

/// <summary>
/// 1バイト書き込む
/// </summary>
/// <param name="c">書き込む文字</param>
/// <returns>FALSE:失敗/TRUE:成功</returns>
BOOL CCashFile::Write(const char c)
{
	if (!m_bUse)                   { return FALSE; }
	if (m_nMode    != File::_RDWR) { return FALSE; }
	if (m_lPos + 1 >= m_lSize)     { return FALSE; }

	*(m_pBuf + m_lPos) = c;

	m_lPos++;

	return TRUE;
}


/// <summary>
/// nバイト書き込む
/// </summary>
/// <param name="pszBuf">書き込む文字列</param>
/// <param name="lSize">書き込むバイト数(Default:-1)</param>
/// <param name="lPos">書き込む位置(Default:-1)</param>
/// <returns>書き込んだバイト数</returns>
long CCashFile::Write(const char *pszBuf, const long lSize, const long lPos)
{
	if (!m_bUse)                { return 0; }
	if (m_nMode != File::_RDWR) { return 0; }

	long lLen = -1;
	long wp;
	CMemory memUtf8;

	if (m_enCharCode == _UTF8) {
		 ShiftJisToUTF8(pszBuf, memUtf8);
		 pszBuf = memUtf8;
		 lLen   = memUtf8.GetSize();
	}

	if      (lSize == -1) { lLen = (long)strlen(pszBuf); }
	else if (lLen  == -1) { lLen = lSize;                }

	if (lPos == -1) { wp = m_lPos; }
	else            { wp = lPos;   }

	if (wp >= m_lSize) { return 0; }

	if (wp + lLen > m_lSize) { lLen = m_lSize - wp; }

	memcpy_s((void *)(m_pBuf + wp), (m_lSize - wp), (void *)pszBuf, (size_t)lLen);

	m_lPos = wp + lLen;

	return lLen;
}


/// <summary>
/// 1行書き込む
/// </summary>
/// <param name="pBuf">書き込む文字列</param>
/// <returns>書き込んだバイト数</returns>
long CCashFile::Puts(const char *pszBuf)
{
	if (!m_bUse) { return 0; }

	long lRet = 0;

	lRet  = Write(pszBuf);
	lRet += Write("\r\n");

	return lRet;

}


/// <summary>
/// 確保したバッファエリアを指定コードで埋める
/// </summary>
/// <param name="c">コード</param>
void CCashFile::MemSetFill(unsigned char c)
{
	memset((void *)m_pBuf, c, m_lSize);
}


//================================================================================
// Seek
//================================================================================

/// <summary>
/// ファイルポインタを移動する
/// </summary>
/// <param name="lPos">移動する位置</param>
long CCashFile::SeekSet(const long lPos)
{
	if (!m_bUse) { return 0; }

	if (lPos >= 0 && lPos < m_lSize) {
		m_lPos = lPos;
	}

	return m_lPos;
}

/// <summary>
/// ファイルポインタを移動する
/// </summary>
/// <param name="lPos">移動するオフセット</param>
long CCashFile::SeekOff(const long lPos)
{
	if (!m_bUse) { return 0; }

	return SeekSet(m_lPos + lPos);
}


/// <summary>ファイルポインタを先頭に移動する</summary>
long CCashFile::SeekTop()
{
	if (!m_bUse) { return 0; }

	m_lPos = 0;

	return m_lPos;
}


/// <summary>ファイルポインタを最後に移動する</summary>
long CCashFile::SeekEnd()
{
	if (!m_bUse) { return 0; }

	m_lPos = m_lSize;

	return m_lPos;
}


/// <summary>
/// ファイルポインタを位置を返す
/// </summary>
/// <returns>ファイルポインタの位置</returns>
long CCashFile::SeekPos()
{
	if (!m_bUse) { return 0; }

	return m_lPos;
}


//================================================================================
// メモリー
//================================================================================

/// <summary>
/// メモリーの確保
/// </summary>
/// <param name="lSize">確保するサイズ</param>
/// <returns>確保したサイズ</returns>
long CCashFile::MemInit(const long lSize)
{
	long lVal = lSize;

	if (m_pBuf != NULL) { *m_pBuf = '\0'; }

	if (lVal < 1)                     { return 0; }
	if (lVal > MAX_ALLOCATION_MEMORY) { lVal = MAX_ALLOCATION_MEMORY; }

	if (m_pBuf == NULL) {
		m_lMemSize = ALLOCATION_BLOCK(lVal);
		m_pBuf     = new char[m_lMemSize + 10];
		*m_pBuf    = '\0';
	} else {
		if (lVal > m_lMemSize) {
			delete []m_pBuf;
			m_pBuf = NULL;
			MemInit(lVal);
		}

	}

	return lVal;
}


/// <summary>メモリーの解放</summary>
void CCashFile::MemFree()
{
	if (m_pBuf != NULL) {
		delete []m_pBuf;
		m_pBuf = NULL;
	}
	m_lPos     = 0;
	m_lSize    = 0;
	m_lMemSize = 0;
}



#endif

