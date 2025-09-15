

//#include "stdafx.h"
#include "pch.h"
#include "file.h"

//////////////////////////////////////////////////////////////
// �L���b�V���t���g�p����ꍇ�͈ȉ��̒�`���gstdafx.h�h�ɖ��L���邱��
// #ifndef _CASH_FILE_
// #define _CASH_FILE_
// #endif
//
// �G���[���b�Z�[�W�������I�ɉp��ɂ���(�����ύX)
// �gMsgEnglish(TRUE)�h
//////////////////////////////////////////////////////////////

//============================================================
// �G���[���b�Z�[�W�̐؂�ւ�
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
// �J�n�������I������
//============================================================

/// <summary>�R���X�g���N�^</summary>
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


/// <summary>�f�X�g���N�^</summary>
File::~File()
{
	if (m_bOpen) {
		Close();
	}

	if (m_pFmtBuf != NULL) { delete []m_pFmtBuf; }
}


//============================================================
// �t�@�C���̃I�[�v��
//============================================================

/// <summary>
/// �t�@�C���I�[�v��
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <param name="enOpenMode">�J�����[�h</param>
/// <param name="enCharCode">�L�����N�^�[�R�[�h</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
				SeekEndAction();	// �t�@�C���̍Ō�Ɉړ�
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
// �t�@�C���̃N���[�Y
//============================================================

/// <summary>
/// �t�@�C���̃N���[�Y
/// </summary>
/// <returns>TRUE:���s/FALSE:����</returns>
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
// �t�@�C������̓ǂݍ���
//============================================================

/// <summary>
/// 1�o�C�g�ǂݍ���
/// </summary>
/// <returns>����:0x00�`0xff/���s:_NO_DATA</returns>
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
/// n�o�C�g�ǂݍ��� 
/// </summary>
/// <param name="pBuf">�ǂݍ��ޏꏊ</param>
/// <param name="nLen">�ǂݍ��ރo�C�g��</param>
/// <returns>�ǂݍ��񂾃o�C�g��</returns>
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
/// 1�s�ǂݍ���
/// </summary>
/// <param name="pBuf">�ǂݍ��ޏꏊ</param>
/// <param name="nMax">�ǂݍ��ލő�o�C�g��</param>
/// <param name="pNewLineSize">���s�o�C�g����Ԃ�</param>
/// <returns>TRUE:����/FALSE:���s</returns>
BOOL File::Gets(char *pBuf, const int nMax, int *pNewLineSize)
{
	*pBuf = '\0';
	if (!m_bOpen) { return FALSE; }

#ifdef _DEBUG
	if (m_enCharCode == _UTF8) {
		TRACE("UTF-8���[�h�ŁA[File::Gets(char,int,int*)]���Ă΂ꂽ�I�I");
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
		if (n == _NO_DATA) { // �Ō�܂œǂ񂾂�I���
			break;
		} else if (n == 0) { // ������̓ǂݍ��݂Łu0�v�͂��肦�Ȃ�
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
/// 1�s�ǂݍ���
/// </summary>
/// <param name="str">CString��n��</param>
/// <param name="pNewLineSize">���s�o�C�g����Ԃ�</param>
/// <returns>TRUE:����/FALSE:���s</returns>
BOOL File::Gets(CString &str, int *pNewLineSize)
{
	str.Empty();
	if (!m_bOpen) { return FALSE; }

	CMemory tmp;

	BOOL bRet = Gets(tmp, pNewLineSize);

	// UTF8�Ȃ�ShiftJis�ɕϊ�����
	if (bRet) {
		if (m_enCharCode == _PENDING) { m_enCharCode = AnalyzeCharCode(tmp); }
		if (m_enCharCode == _UTF8)    { UTF8ToShiftJis(tmp, str); }
		else                          { str = (const char *)tmp;  }
	}

	return bRet;
}


/// <summary>
/// 1�s�ǂݍ���
/// </summary>
/// <param name="mem">CMemory��n��</param>
/// <param name="pNewLineSize">���s�o�C�g����Ԃ�</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
			if (n == _NO_DATA) { // �Ō�܂œǂ񂾂�I���
				bRun = FALSE;
				break;
			} else if (n == 0) { // ������̓ǂݍ��݂Łu0�v�͂��肦�Ȃ�
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
// �t�@�C���̏�������
//============================================================

/// <summary>
/// n�o�C�g��������
/// </summary>
/// <param name="pBuf">�������ޕ�����</param>
/// <param name="nLen">�������ރo�C�g��</param>
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
/// ���������������
/// </summary>
/// <param name="pszBuf">�������ޕ�����</param>
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
/// 1�s��������
/// </summary>
/// <param name="pszBuf">�������ޕ�����</param>
void File::Puts(const char *pszBuf)
{
	if (!m_bOpen) { return; }

	Write(pszBuf);
	Write("\r\n", 2);
}


/// <summary>
/// 1�o�C�g��������
/// </summary>
/// <param name="c">�������ޕ���</param>
void File::Putc(const char c)
{
	if (!m_bOpen) { return; }

	Write(&c, 1);
}


/// <summary>
/// �w�肳�ꂽ�t�H�[�}�b�g�ŏ�������
/// </summary>
/// <param name="fmt">�t�H�[�}�b�g</param>
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
// �t�@�C���̃|�C���g����
//============================================================

/// <summary>
/// �t�@�C���|�C���^���ړ�����
/// </summary>
/// <param name="lPos">�ړ�����ʒu</param>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
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
/// �t�@�C���|�C���^���ړ�����
/// </summary>
/// <param name="lPos">�ړ�����I�t�Z�b�g</param>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
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
/// �t�@�C���|�C���^��擪�Ɉړ�����
/// </summary>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
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
/// �t�@�C���|�C���^���Ō�Ɉړ�����
/// </summary>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
DWORD64 File::SeekEnd()
{
	if (!m_bOpen) { return -1; }

	return SeekEndAction();
}


/// <summary>
/// �t�@�C���|�C���^���Ō�Ɉړ�����
/// </summary>
/// <returns>�t�@�C���|�C���^/���s:-1</returns>
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
/// �t�@�C���|�C���^���ʒu��Ԃ�
/// </summary>
/// <returns>�t�@�C���|�C���^�̈ʒu</returns>
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
// �t�@�C���̍폜
//============================================================

/// <summary>
/// �t�@�C���̍폜
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <param name="bCheck">���������H�m�F����(FALSE)</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL File::Unlink(const char *pszFileName, const BOOL bCheck)
{
	BOOL bErr = FALSE;

	try {
		if (IsExistFile(pszFileName)) {
			CFile::Remove(pszFileName);

			if (bCheck == TRUE) {
				// �ő��Q�b�܂ő҂�
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
/// �t�@�C���̍폜
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <param name="bCheck">���������H�m�F����(FALSE)</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL File::UnlinkFile(const char *pszFileName, const BOOL bCheck)
{
	return Unlink(pszFileName, bCheck);
}


/// <summary>
/// �w��t�H���_�ȉ����폜����
/// </summary>
/// <param name="pszPath">�t�H���_</param>
/// <param name="bParentUnlink">�w��t�H���_���폜����(TRUE)</param>
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
/// �w��t�H���_�ȉ����폜����(���s��)
/// </summary>
/// <param name="pszPath">�t�H���_</param>
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
// �X�e�C�^�X�֌W
//============================================================

/// <summary>
/// �t�@�C���̍X�V�b��Ԃ�
/// </summary>
/// <returns>�X�V�b</returns>
time_t File::GetStatusTimet()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return st.m_mtime.GetTime();
}


/// <summary>
/// �t�@�C���̍X�V�b��Ԃ�
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>�X�V�b</returns>
time_t File::GetStatusTimet(const char *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return st.m_mtime.GetTime();
}


/// <summary>
/// �t�@�C���T�C�Y��Ԃ�
/// </summary>
/// <returns>�t�@�C���T�C�Y</returns>
DWORD64 File::GetStatusSize()
{
	if (!m_bOpen) { return 0; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return 0; }

	return st.m_size;
}


/// <summary>
/// �t�@�C���T�C�Y��Ԃ�
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>�t�@�C���T�C�Y</returns>
DWORD64 File::GetStatusSize(const char *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return 0; }

	return st.m_size;
}


/// <summary>
/// �t�@�C���̃A�g���r���[�g��Ԃ�
/// </summary>
/// <returns>�t�@�C���̃A�g���r���[�g(�G���[:-1)</returns>
BYTE  File::GetStatusAttr()
{
	if (!m_bOpen) { return -1; }

	CFileStatus st;

	if (FALSE == m_File.GetStatus(st)) { return -1; }

	return (BYTE)st.m_attribute; 
}


/// <summary>
/// �t�@�C���̃A�g���r���[�g��Ԃ�
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>�t�@�C���̃A�g���r���[�g(�G���[:-1)</returns>
BYTE  File::GetStatusAttr(const char *pszFileName)
{
	CFileStatus st;

	if (FALSE == CFile::GetStatus(pszFileName, st)) { return -1; }

	return (BYTE)st.m_attribute; 
}


//============================================================
// �t�@�C���̖��O�ύX
//============================================================

/// <summary>
/// �t�@�C�����̕ύX
/// </summary>
/// <param name="pszOldName">���t�@�C����</param>
/// <param name="pszNewName">�V�t�@�C����</param>
/// <returns>FALSE:����/TRUE:���s</returns>
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
// ��t�@�C���̍쐬
//============================================================

/// <summary>
/// ��t�@�C���̍쐬
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
/// �t�H���_�̍쐬
/// </summary>
/// <param name="pszPath">�t�H���_��</param>
/// <returns>TRUE:����/FALSE:���s(���Ƀt�H���_���L��)</returns>
BOOL File::CreateFolder(const char *pszPath)
{
	return CreateDirectoryA(pszPath, NULL);
}


//=============================================================
// [UTF-8]<-->[SHIFT-JIS]�ϊ��֌W
//=============================================================

/// <summary>
/// UTF-8�ɕϊ����ď�������
/// </summary>
/// <param name="pszBuf">�������ޕ�����</param>
void File::WriteUtf8(const char *pszBuf)
{
	CMemory tmp;

	ShiftJisToUTF8(pszBuf, tmp);
	Write(tmp, (int)strlen(tmp));
}


/// <summary>
/// �L�����N�^�[�R�[�h�̔��f���撣��
/// </summary>
/// <param name="memBuf">�ǂݍ��񂾕�����</param>
File::CharCode File::AnalyzeCharCode(CMemory &memBuf, CharCode enCharCode)
{
	if (enCharCode != _PENDING) { return enCharCode; }

	const BYTE *cp = (const BYTE *)memBuf;
	int nSize      = memBuf.GetSize();

	while (nSize-- && enCharCode == _PENDING) {
		// �P�o�C�g��
		if      (0x81 <= *cp && *cp <= 0x9f) { enCharCode = _SHIFT_JIS; }
		else if (0xc0 <= *cp && *cp <= 0xdf) { enCharCode = _UTF8;      }
		else if (0xf0 <= *cp && *cp <= 0xf7) { enCharCode = _UTF8;      }
		else if (0xa0 <= *cp && *cp <= 0xc1) { enCharCode = _SHIFT_JIS; }
		else if (0xe0 <= *cp && *cp <= 0xef) {
			// �Q�o�C�g��
			cp++;
			if (0x40 <= *cp && *cp <= 0x7e) { enCharCode = _SHIFT_JIS; }
			else if (0x80 <= *cp) {
				if (*cp <= 0xbf)  { // 0x80-0xbf
					// �R�o�C�g��
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
/// ShiftJis��UTF-8�ɕϊ�����
/// </summary>
/// <param name="pszShiftjis">�ϊ����̕�����</param>
/// <param name="memUtf8">�ϊ���̕�����</param>
void File::ShiftJisToUTF8(const char *pszShiftjis, CMemory &memUtf8)
{
	// �ϊ����镶���񂪂Ȃ��ꍇ�́A�ϊ����������Ȃ�
	if (*pszShiftjis == '\0') {
		memUtf8.Clear();
		return;
	}

	// Uniocde�ɕϊ���̃T�C�Y���v�Z����
	int nLenShiftjis = (int)strlen(pszShiftjis); 
	int nLenUnicode  = MultiByteToWideChar(CP_ACP, 0, pszShiftjis, nLenShiftjis + 1, NULL, 0);

	// Uniocde�ɕϊ�����
	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
	MultiByteToWideChar(CP_ACP, 0, pszShiftjis, nLenShiftjis + 1, pBufUnicode, nLenUnicode);
	
	// UTF-8�ɕϊ���̃T�C�Y���v�Z����
	int nLenUtf8 = WideCharToMultiByte(CP_UTF8, 0, pBufUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// UTF-8�ɕϊ�����
	char *pBufUTF8 = new char[nLenUtf8 + 1];
	WideCharToMultiByte(CP_UTF8, 0, pBufUnicode, nLenUnicode, pBufUTF8, nLenUtf8, NULL, NULL);

	memUtf8 = pBufUTF8;

	delete []pBufUTF8;
	delete []pBufUnicode;
}


/// <summary>
/// UTF-8��ShiftJis�ɕϊ�����
/// </summary>
/// <param name="memUtf8">�ϊ����̕�����</param>
/// <param name="str">�ϊ���̕�����</param>
void File::UTF8ToShiftJis(CMemory &memUtf8, CString &str)
{
	int nLenUtf8 = memUtf8.GetSize();

	// �ϊ����镶���񂪂Ȃ��ꍇ�́A�ϊ����������Ȃ�
	if (nLenUtf8 == 0) {
		str.Empty();
		return;
	}

	// Unicode�ɕϊ���̃T�C�Y���v�Z����
	int nLenUnicode = MultiByteToWideChar(CP_UTF8, 0, memUtf8, nLenUtf8 + 1, NULL, 0);

	// UniCode�ɕϊ�����
	wchar_t *pBufUnicode = new wchar_t[nLenUnicode + 1];
	MultiByteToWideChar(CP_UTF8, 0, memUtf8, nLenUtf8 + 1, pBufUnicode, nLenUnicode);

	// ShiftJis�ɕϊ���̃T�C�Y���v�Z����
	int nLenShiftJis = WideCharToMultiByte(CP_ACP, 0, pBufUnicode, nLenUnicode, NULL, 0, NULL, NULL);

	// ShiftJis�ɕϊ�����
	char *pBufShiftJis = new char[nLenShiftJis + 1];
	WideCharToMultiByte(CP_ACP, 0, pBufUnicode, nLenUnicode, pBufShiftJis, nLenShiftJis, NULL, NULL);

	str = pBufShiftJis;

	delete []pBufShiftJis;
	delete []pBufUnicode;
}



//============================================================
// �g�p����L�����N�^�[�R�[�h�֌W
//============================================================

/// <summary>
/// ���݂̃L�����N�^�[�R�[�h��Ԃ�
/// </summary>
/// <returns>_PENDING/_SHIFT_JIS/_UTF8</returns>
File::CharCode File::GetCharCode()
{
	return m_enCharCode;
}


/// <summary>
/// �L�����N�^�[�R�[�h���Z�b�g����(��x�Z�b�g����ƕύX�͏o���܂���)
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
// ���̑�
//============================================================

/// <summary>
/// �t�@�C�������邩�H
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <returns>TRUE:����/FALSE:�Ȃ�</returns>
BOOL File::IsExistFile(const char *pszFileName)
{
	DWORD dwbAttr = GetFileAttributesA(pszFileName);

	return (dwbAttr != INVALID_FILE_ATTRIBUTES && !(dwbAttr & FILE_ATTRIBUTE_DIRECTORY));
}


/// <summary>
/// �t�H���_�����邩�H
/// </summary>
/// <param name="pszFilePath">�t�H���_��</param>
/// <returns>TRUE:����/FALSE:�Ȃ�</returns>
BOOL File::IsExistFolder(const char *pszFilePath)
{
	DWORD dwbAttr = GetFileAttributesA(pszFilePath);

	return (dwbAttr != INVALID_FILE_ATTRIBUTES && dwbAttr & FILE_ATTRIBUTE_DIRECTORY);
}


/// <summary>
/// �Ō�̃G���[�R�[�h��Ԃ�
/// </summary>
/// <returns>�G���[�R�[�h</returns>
UINT File::GetErrorCode()
{
	return m_uErrCode;
}


/// <summary>
/// �Ō�̃G���[���e��Ԃ�
/// </summary>
/// <param name="pstr">�󂯎��ʒu</param>
/// <param name="nMaxLen">�󂯎��ő啶����</param>
/// <returns>�G���[�R�[�h</returns>
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
/// �Ō�̃G���[���e
/// </summary>
/// <returns>�G���[���e</returns>
const char *File::GetErrorString()
{
	m_pMsg = NULL;

	switch (GetErrorCode()) {
	case FILE_ERR_NONE:
		if (m_bEnglish) { m_pMsg = "No error occurred."; }
		else            { m_pMsg = "�G���[�͂���܂���"; }
		break;

	case FILE_ERR_GENERIC:
	default:
		if (m_bEnglish) { m_pMsg = "An unspecified error occurred."; }
		else            { m_pMsg = "�s���ȃG���[���������܂���"; }
		break;
		
	case FILE_ERR_FILE_NOT_FOUND:
		if (m_bEnglish) { m_pMsg = "The file could not be located."; }
		else            { m_pMsg = "�t�@�C��������܂���"; }
		break;
		
	case FILE_ERR_BAD_PATH:
		if (m_bEnglish) { m_pMsg = "All or part of the path is invalid."; }
		else            { m_pMsg = "�p�X���s���ł�"; }
		break;
		
	case FILE_ERR_TOO_MANY_OPEN_FILES:
		if (m_bEnglish) { m_pMsg = "The permitted number of open files was exceeded."; }
		else            { m_pMsg = "����ȏ�t�@�C�����I�[�v���ł��܂���"; }
		break;
		
	case FILE_ERR_ACCESS_DENIED:
		if (m_bEnglish) { m_pMsg = "The file could not be accessed."; }
		else            { m_pMsg = "�t�@�C���̃A�N�Z�X���֎~����Ă��܂�"; }
		break;
		
	case FILE_ERR_INVALID_FILE:
		if (m_bEnglish) { m_pMsg = "There was an attempt to use an invalid file handle."; }
		else            { m_pMsg = "�t�@�C���n���h�����s���ł�"; }
		break;
		
	case FILE_ERR_REMOVE_CURRENT_DIR:
		if (m_bEnglish) { m_pMsg = "The current working directory cannot be removed."; }
		else            { m_pMsg = "�J�����g�E�t�H���_�[�̍폜�͂ł��܂���"; }
		break;

	case FILE_ERR_DIRCTORY_FULL:
		if (m_bEnglish) { m_pMsg = "There are no more directory entries."; }
		else            { m_pMsg = "�t�H���_�[����t�ł�"; }
		break;
		
	case FILE_ERR_BAD_SEEK:
		if (m_bEnglish) { m_pMsg = "There was an error trying to set the file pointer."; }
		else            { m_pMsg = "�t�@�C���E�V�[�N�Ɏ��s���܂���"; }
		break;

	case FILE_ERR_HARD_IO:
		if (m_bEnglish) { m_pMsg = "There was a hardware error."; }
		else            { m_pMsg = "�n�[�h�E�F�A�E�G���[����"; }
		break;

	case FILE_ERR_SHRING_VIOLALATION:
		if (m_bEnglish) { m_pMsg = "SHARE.EXE was not loaded, or a shared region was locked."; }
		else            { m_pMsg = "���̃A�v���P�[�V�������t�@�C�������b�N���Ă��܂�"; }
		break;
		
	case FILE_ERR_LOCK_VIOLATION:
		if (m_bEnglish) { m_pMsg = "There was an attempt to lock a region that was already locked."; }
		else            { m_pMsg = "���b�N�ςݗ̈�̃��b�N�v��"; }
		break;

	case FILE_ERR_DISK_FULL:
		if (m_bEnglish) { m_pMsg = "The disk is full."; }
		else            { m_pMsg = "�f�B�X�N����t�ł�"; }
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
// ��C�A�N�Z�X
//********************************************************************************

/// <summary>�R���X�g���N�^</summary>
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

/// <summary>�f�X�g���N�^</summary>
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
/// �t�@�C���I�[�v��
/// </summary>
/// <param name="pszFileName">�t�@�C����</param>
/// <param name="nMode">�J�����[�h</param>
/// <param name="lSize">�m�ۂ���e��(�������ݎ�:Default=0)</param>
/// <param name="enCharCode">�L�����N�^�[�R�[�h</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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

			// 100MB�𒴂���t�@�C���͈���Ȃ�
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
/// �t�@�C���̃N���[�Y
/// </summary>
/// <param name="bDestruction">�j������(Deault:FALSE)</param>
/// <returns>FALSE:����/FALSE:���s</returns>
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
				bRet = TRUE;	// �G���[
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
// �ǂݍ���
//================================================================================

/// <summary>
/// 1�o�C�g�ǂݍ���
/// </summary>
/// <returns>����:0x00�`0xff/���s:_NO_DATA</returns>
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
/// n�o�C�g�ǂݍ���
/// </summary>
/// <param name="pBuf">�ǂݍ��ޏꏊ</param>
/// <param name="lSize">�ǂݍ��ރo�C�g��</param>
/// <param name="lPos">�ǂݍ��݈ʒu(Default:-1)</param>
/// <returns>�ǂݍ��񂾃o�C�g��</returns>
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
/// 1�s�ǂݍ���
/// </summary>
/// <param name="pBuf">�ǂݍ��ރ|�C���^</param>
/// <param name="lMax">�ǂݍ��ލő�o�C�g��</param>
/// <param name="pNewLineSize">���s�o�C�g����Ԃ�</param>
/// <returns>TRUE:����/FALSE:���s</returns>
BOOL CCashFile::Gets(char *pBuf, const long lMax, int *pNewLineSize)
{
#ifdef _DEBUG
	if (m_enCharCode == _UTF8) {
		TRACE("UTF-8���[�h�ŁA[CCashFile::Gets(char,int,int*)]���Ă΂ꂽ�I�I");
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
		if (n == _NO_DATA) { // �Ō�܂œǂ񂾂�I���
			break;
		} else if (n == 0) { // ������̓ǂݍ��݂Łu0�v�͂��肦�Ȃ�
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
/// 1�s�ǂݍ���
/// </summary>
/// <param name="str">�ǂݍ��ޏꏊ</param>
/// <param name="pNewLineSize">���s�o�C�g����Ԃ�</param>
/// <returns>TRUE:����/FALSE:���s</returns>
BOOL CCashFile::Gets(CString &str, int *pNewLineSize)
{
	if (pNewLineSize != NULL) { *pNewLineSize = 0; }

	str.Empty();
	if (!m_bUse)                                             { return FALSE; }
	if (!(m_nMode == File::_READ || m_nMode == File::_RDWR)) { return FALSE; }

	CMemory tmp;

	BOOL bRet = Gets(tmp, pNewLineSize);

	// UTF8�Ȃ�ShiftJis�ɕϊ�����
	if (bRet) {
		if (m_enCharCode == _PENDING) { m_enCharCode = AnalyzeCharCode(tmp); }
		if (m_enCharCode == _UTF8)    { UTF8ToShiftJis(tmp, str); }
		else                          { str = (const char *)tmp;  }
	}

	return bRet;
}


/// <summary>
/// 1�s�ǂݍ���
/// </summary>
/// <param name="str">�ǂݍ��ޏꏊ</param>
/// <param name="pNewLineSize">���s�o�C�g����Ԃ�</param>
/// <returns>TRUE:����/FALSE:���s</returns>
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
			if (n == _NO_DATA) { // �Ō�܂œǂ񂾂�I���
				bRun = FALSE;
				break;
			} else if (n == 0) { // ������̓ǂݍ��݂Łu0�v�͂��肦�Ȃ�
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
/// �t�@�C���T�C�Y��Ԃ�
/// </summary>
/// <returns>�t�@�C���T�C�Y</returns>
long CCashFile::GetSize()
{
	if (!m_bUse) { return 0; }

	return  m_lSize;
}


/// <summary>
/// �t�@�C������Ԃ�
/// </summary>
/// <returns>�t�@�C����</returns>
const char *CCashFile::GetFileName()
{
	return m_pFileName;
}


//================================================================================
// ��������
//================================================================================

/// <summary>
/// 1�o�C�g��������
/// </summary>
/// <param name="c">�������ޕ���</param>
/// <returns>FALSE:���s/TRUE:����</returns>
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
/// n�o�C�g��������
/// </summary>
/// <param name="pszBuf">�������ޕ�����</param>
/// <param name="lSize">�������ރo�C�g��(Default:-1)</param>
/// <param name="lPos">�������ވʒu(Default:-1)</param>
/// <returns>�������񂾃o�C�g��</returns>
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
/// 1�s��������
/// </summary>
/// <param name="pBuf">�������ޕ�����</param>
/// <returns>�������񂾃o�C�g��</returns>
long CCashFile::Puts(const char *pszBuf)
{
	if (!m_bUse) { return 0; }

	long lRet = 0;

	lRet  = Write(pszBuf);
	lRet += Write("\r\n");

	return lRet;

}


/// <summary>
/// �m�ۂ����o�b�t�@�G���A���w��R�[�h�Ŗ��߂�
/// </summary>
/// <param name="c">�R�[�h</param>
void CCashFile::MemSetFill(unsigned char c)
{
	memset((void *)m_pBuf, c, m_lSize);
}


//================================================================================
// Seek
//================================================================================

/// <summary>
/// �t�@�C���|�C���^���ړ�����
/// </summary>
/// <param name="lPos">�ړ�����ʒu</param>
long CCashFile::SeekSet(const long lPos)
{
	if (!m_bUse) { return 0; }

	if (lPos >= 0 && lPos < m_lSize) {
		m_lPos = lPos;
	}

	return m_lPos;
}

/// <summary>
/// �t�@�C���|�C���^���ړ�����
/// </summary>
/// <param name="lPos">�ړ�����I�t�Z�b�g</param>
long CCashFile::SeekOff(const long lPos)
{
	if (!m_bUse) { return 0; }

	return SeekSet(m_lPos + lPos);
}


/// <summary>�t�@�C���|�C���^��擪�Ɉړ�����</summary>
long CCashFile::SeekTop()
{
	if (!m_bUse) { return 0; }

	m_lPos = 0;

	return m_lPos;
}


/// <summary>�t�@�C���|�C���^���Ō�Ɉړ�����</summary>
long CCashFile::SeekEnd()
{
	if (!m_bUse) { return 0; }

	m_lPos = m_lSize;

	return m_lPos;
}


/// <summary>
/// �t�@�C���|�C���^���ʒu��Ԃ�
/// </summary>
/// <returns>�t�@�C���|�C���^�̈ʒu</returns>
long CCashFile::SeekPos()
{
	if (!m_bUse) { return 0; }

	return m_lPos;
}


//================================================================================
// �������[
//================================================================================

/// <summary>
/// �������[�̊m��
/// </summary>
/// <param name="lSize">�m�ۂ���T�C�Y</param>
/// <returns>�m�ۂ����T�C�Y</returns>
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


/// <summary>�������[�̉��</summary>
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

