

#pragma once

#include "Memory.h"

//////////////////////////////////////////////////////////////
// キャッシュ付を使用する場合は以下の定義を“stdafx.h”or“pch.h”に明記すること
// #ifndef _CASH_FILE_
// #define _CASH_FILE_
// #endif
//
// エラーメッセージを強制的に英語にする(自動変更)
// “MsgEnglish(TRUE)”
//////////////////////////////////////////////////////////////

#ifndef _NO_DATA
#define _NO_DATA 0x100
#endif

#ifndef C_LF
#define C_LF  0x0a
#endif

#ifndef C_CR
#define C_CR  0x0d
#endif

#ifndef C_EOF
#define C_EOF 0x1a
#endif


#define FILE_ACCESS_LOCK 0x100

#define FILE_ERR_NONE                CFileException::none               //  0:エラーなし
#define FILE_ERR_GENERIC             CFileException::genericException   //  1:未定義エラー
#define FILE_ERR_FILE_NOT_FOUND      CFileException::fileNotFound       //  2:ファイル未定義
#define FILE_ERR_BAD_PATH            CFileException::badPath            //  3:不正パス
#define FILE_ERR_TOO_MANY_OPEN_FILES CFileException::tooManyOpenFiles   //  4:オープン ファイル数超過
#define FILE_ERR_ACCESS_DENIED       CFileException::accessDenied       //  5:ファイル アクセス禁止
#define FILE_ERR_INVALID_FILE        CFileException::invalidFile        //  6:不正ファイル ハンドル使用
#define FILE_ERR_REMOVE_CURRENT_DIR  CFileException::removeCurrentDir   //  7:作業中のカレント ディレクトリの削除不可
#define FILE_ERR_DIRCTORY_FULL       CFileException::directoryFull      //  8:ディレクトリ フル
#define FILE_ERR_BAD_SEEK            CFileException::badSeek            //  9:ファイル ポインタ設定エラー
#define FILE_ERR_HARD_IO             CFileException::hardIO             // 10:ハードウェア エラー
#define FILE_ERR_SHRING_VIOLALATION  CFileException::sharingViolation   // 11:SHARE.EXE 未ロードまたは共有領域ロック済み
#define FILE_ERR_LOCK_VIOLATION      CFileException::lockViolation      // 12:ロック済み領域のロック要求
#define FILE_ERR_DISK_FULL           CFileException::diskFull           // 13:ディスク フル
#define FILE_ERR_END_OF_FILE         CFileException::endOfFile          // 14:ファイルの終わりに到達 

#define FORMAT_BUFFER_SIZE 2048
#define MAX_ALLOCATION_MEMORY (1024*1024*100)	// 100MB
#define ALLOCATION_BLOCK_SIZE 10240
#define ALLOCATION_BLOCK(n) (((n / ALLOCATION_BLOCK_SIZE) + 1) * ALLOCATION_BLOCK_SIZE)


#ifndef FILE_GETS_BUFFER_SIZE
#define FILE_GETS_BUFFER_SIZE 512
#endif


class File {
protected:
	static BOOL m_bFirst;
	static BOOL m_bEnglish;

	BOOL m_bOpen;
	UINT m_uErrCode;
	CFile m_File;
	char *m_pFmtBuf;
	const char *m_pMsg;

	// キャラクターコード
public:
	enum CharCode {_PENDING = 0, _SHIFT_JIS, _UTF8};

	CharCode GetCharCode();
	CharCode SetCharCode(const CharCode enMode);
protected:
	CharCode m_enCharCode;


public:
	enum OpenMode { _READ      = 1                   , _WRITE     , _APPEND     , _RDWR,
	                _READ_LOCK = FILE_ACCESS_LOCK + 1, _WRITE_LOCK, _APPEND_LOCK, _RDWR_LOCK };

	File();
	virtual ~File();

	BOOL Open(const char *pszFileName, const OpenMode enMode, CharCode = _PENDING);
	BOOL Close();

	unsigned int Read();
	int  Read(char *pBuf  , const int nLen);
	BOOL Gets(CString &str, int *pNewLineSize = NULL);
	BOOL Gets(CMemory &mem, int *pNewLineSize = NULL);
	BOOL Gets(char *pBuf  , const int nMax, int *pNewLineSize = NULL);

	void Write(const char *pszBuf, const int nLen);
	void Write(const char *pszBuf);
	void Puts(const char *pszBuf);
	void Putc(const char c);
	void Printf(const char *fmt, ...);

	HANDLE GetHandle() { return m_File.m_hFile; }

private:
	DWORD64 SeekEndAction();
public:
	DWORD64 SeekEnd();
	DWORD64 SeekSet(const DWORD64 lPos);
	DWORD64 SeekOff(const DWORD64 lPos);
	DWORD64 SeekTop();
	DWORD64 GetPosition();

	time_t  GetStatusTimet();
	DWORD64 GetStatusSize();
	BYTE    GetStatusAttr();

	UINT        GetErrorCode();
	UINT        GetErrorString(char *pstr, const int nLen);
	const char *GetErrorString();

	static time_t  GetStatusTimet(const char *pszFileName);
	static DWORD64 GetStatusSize(const char *pszFileName);
	static BYTE    GetStatusAttr(const char *pszFileName);

	static BOOL IsExistFile(const char *pszFileName);
	static BOOL IsExistFolder(const char *pszFilePath);
	static BOOL Rename(const char *pszOldName, const char *pszNewName);
	static BOOL CreateEmptyFile(const char *pszFileName);
	static BOOL CreateFolder(const char *pszPath);

	static BOOL Unlink(const char *pszFileName, const BOOL bCheck = FALSE);
	static BOOL UnlinkFile(const char *pszFileName, const BOOL bCheck = FALSE);
	static BOOL UnlinkFolder(const char *pszPath, const BOOL bParentUnlink = TRUE);
protected:
	static BOOL UnlinkFolderMain(const char *pszPath);

public:
	// エラー表示の切り替え
	static BOOL MsgEnglish(const BOOL bEnglish);
	static BOOL MsgEnglish();

	// [UTF-8]<-->[SHIFT-JIS]変換関係
private:
	void WriteUtf8(const char *pszBuf);
protected:
	CharCode AnalyzeCharCode(CMemory &memBuf, CharCode enCharCode = _PENDING);
public:
	static void ShiftJisToUTF8(const char *pszShiftjis, CMemory &memUtf8);
	static void UTF8ToShiftJis(CMemory &memUtf8, CString &str);
};


#ifdef _CASH_FILE_
class CCashFile : protected File {
	char *m_pFileName;
	long  m_lMemSize;

	long MemInit(const long lSize);
	void MemFree();

protected:
	BOOL m_bUse;
	int  m_nMode;
	long m_lPos;
	long m_lSize;
	char *m_pBuf;
	CharCode m_enCharCode;

public:
	CCashFile();
	virtual ~CCashFile();

	BOOL Open(const char *pszFileName, const int nMode, const long lSize = 0, CharCode = _PENDING);
	BOOL Close(const BOOL bDestruction = FALSE);
	BOOL IsOpen() { return m_bUse; }

	unsigned int Read();
	long Read(char *pBuf, const long lSize, const long lPos = -1);
	BOOL Gets(char *pBuf, const long lMax, int *pNewLineSize = NULL);
	BOOL Gets(CString &str, int *pNewLineSize = NULL);
	BOOL Gets(CMemory &mem, int *pNewLineSize = NULL);

	BOOL Write(const char c);
	long Write(const char *pszBuf, const long lSize = -1, const long lPos = -1);
	long Puts(const char *pszBuf);
	void MemSetFill(unsigned char c);

	long        GetSize();
	const char *GetFileName();

	long SeekSet(const long lPos);
	long SeekOff(const long lPos);
	long SeekTop();
	long SeekEnd();
	long SeekPos();
};
#endif


