
#include "pch.h"
#include "MoveJpeg.h"
//#include "flag.h"
#include "lib.h"
#include "SearchFile.h"



/////////////////////////////////////////////////////////////////////////////
//===========================================================================
// �J�n���I������
//===========================================================================

/// <summary>�R���X�g���N�^</summary>
CSearchFile::CSearchFile()
{
}


/// <summary>�f�X�g���N�^</summary>
CSearchFile::~CSearchFile()
{
	End();
}


/// <summary>�I������(���������)</summary>
void CSearchFile::End()
{
	int i;
	int nSize = (int)m_arrFileName.GetSize();

	if (nSize) {
		for (i = 0; i < nSize; i++) {
			CBase *p = (CBase *)m_arrFileName.GetAt(0);
			delete p;
			m_arrFileName.RemoveAt(0);
		}
	}

	nSize = (int)m_arrFileExt.GetSize();
	if (nSize) {
		for (i = 0; i < nSize; i++) {
			char *p = (char *)m_arrFileExt.GetAt(0);
			delete []p;
			m_arrFileExt.RemoveAt(0);
		}
	}
}


//---------------------------------------------------------------------------
// ������
//---------------------------------------------------------------------------

/// <summary>������</summary>
void CSearchFile::Init()
{
	End();
}


//===========================================================================
// �t�@�C���̌���
//===========================================================================

/// <summary>
/// �t�@�C���̌���
/// </summary>
/// <param name="pszSearchPath">�����t�H���_</param>
/// <returns>����F0/�G���[:1</returns>
int CSearchFile::Main(const char *pszSearchPath)
{
	BOOL bSubFolder = FALSE;

	if (!GetExtSize()) {
		App.m_pMainWnd->MessageBoxA("��������g���q���o�^����Ă��܂���", "�ُ펖��", MB_ICONERROR|MB_OK);
		return 1;
	}

	if (IDYES == App.m_pMainWnd->MessageBoxA("�T�u�z���_�[���������܂����H", "�m�F", MB_ICONQUESTION|MB_YESNO)) {
		bSubFolder = TRUE;
	}

	return SearchMain(pszSearchPath, bSubFolder);
}


/// <summary>
/// �t�@�C���̌���(���C��)
/// </summary>
/// <param name="pszSearchPath">�����t�H���_</param>
/// <param name="bSubFolder">�T�u�E�t�H���_����������H</param>
/// <returns>0</returns>
int CSearchFile::SearchMain(const char *pszSearchPath, const BOOL bSubFolder)
{
	BOOL bRun = TRUE;
	CFileFind Find;
	CString str;
	CString str2;
	CString strSearchFullPath;
	FILETIME ft{};

	MakeSeachFullPath(strSearchFullPath, pszSearchPath);

	if (Find.FindFile(strSearchFullPath, 0)) {
		while (bRun) {
			bRun = Find.FindNextFile();
			if (Find.IsDots()) {
				continue;
			} else if (Find.IsDirectory()) {
				if (bSubFolder) {
					SearchMain(Find.GetFilePath(), bSubFolder);
				}
				continue;
			}
			str = Find.GetFilePath();
			str.MakeUpper();
			if (CheckFileExt(str)) {
				Find.GetLastWriteTime(&ft);

				CBase *p = new CBase;

				p->m_strFileTitle  = Find.GetFileTitle();
				p->m_strFullPath   = Find.GetFilePath();
				p->m_strFileName   = Find.GetFileName();
				p->m_dw64DateTime  = ft.dwHighDateTime;
				p->m_dw64DateTime <<= 32;
				p->m_dw64DateTime += ft.dwLowDateTime;
				
				m_arrFileName.Add((void *)p);
			}
		}
	}

	return 0;
}


/// <summary>
/// �����t�H���_�E�p�X�̍쐬
/// </summary>
/// <param name="strFullPath">�����t�H���_�E�p�X</param>
/// <param name="pszPath">�����t�H���_�E�p�X</param>
void CSearchFile::MakeSeachFullPath(CString &strFullPath, const char *pszPath)
{
	strFullPath = pszPath;

	if (strFullPath.Right(1) != '\\') {
		strFullPath += '\\';
	}
	strFullPath += "*.*";
}



//===========================================================================
// Get�V���[�Y
//===========================================================================

/// <summary>
/// �L�^�f�[�^����Ԃ�
/// </summary>
/// <returns>�f�[�^��</returns>
int CSearchFile::GetSize()
{
	return (int)m_arrFileName.GetSize();
}


/// <summary>
/// �t�@�C���p�X��Ԃ�
/// </summary>
/// <param name="nNum">�C���f�b�N�X�ԍ�</param>
/// <param name="str">�t�@�C���p�X</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL CSearchFile::GetFilePath(const int nNum, CString &str)
{
	BOOL bRet = FALSE;
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) {
		str.Empty();
		bRet = TRUE;
	} else {
		CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

		str = p->m_strFullPath;
	}

	return bRet;
}


/// <summary>
/// �t�@�C���̃^�C�g����Ԃ�
/// </summary>
/// <param name="nNum">�C���f�b�N�X�ԍ�</param>
/// <param name="str">�^�C�g��</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL CSearchFile::GetFileTitle(const int nNum, CString &str)
{
	BOOL bRet = FALSE;
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) {
		str.Empty();
		bRet = TRUE;
	} else {
		CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

		str = p->m_strFileTitle;
	}

	return bRet;
}


/// <summary>
/// �t�@�C������Ԃ�
/// </summary>
/// <param name="nNum">�C���f�b�N�X�ԍ�</param>
/// <param name="str">�t�@�C����</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL CSearchFile::GetFileName(const int nNum, CString &str)
{
	BOOL bRet = FALSE;
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) {
		str.Empty();
		bRet = TRUE;
	} else {
		CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

		str = p->m_strFileName;
	}

	return bRet;
}

// <summary>
/// �t�@�C������Ԃ�
/// </summary>
/// <param name="nNum">�C���f�b�N�X�ԍ�</param>
/// <param name="str">�t�@�C����</param>
/// <returns>FALSE:����/TRUE:���s</returns>
CString CSearchFile::GetFileName(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return ""; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	return  p->m_strFileName;
}


/// <summary>
/// �t�@�C���̊g���q��Ԃ�
/// </summary>
/// <param name="nNum">�C���f�b�N�X�ԍ�</param>
/// <param name="strExt">�g���q</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL CSearchFile::GetFileExt(const int nNum, CString &str)
{
	int n;
	const char *cp;
	CString strTmp;

	GetFilePath(nNum, strTmp);

	n = strTmp.ReverseFind('.');
	if (n <= 0) { return TRUE; }
	
	cp  = (const char *)strTmp;
	str = (cp + n);

	return FALSE;
}


/// <summary>
/// �I��ԍ���ݒ肷��
/// </summary>
/// <param name="nNum">�C���f�b�N�X�ԍ�</param>
/// <param name="nSelect">�ԍ�</param>
/// <returns>�I��ԍ��A�G���[��(-1)</returns>
int CSearchFile::SetSelectNum(const int nNum, const int nSelect)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return -1; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	p->m_nSelecct = nSelect;

	return nSelect;
}


/// <summary>
/// �I��ԍ���Ԃ�
/// </summary>
/// <param name="nNum">�C���f�b�N�X�ԍ�</param>
/// <returns>�I��ԍ��A�G���[��(-1)</returns>
BOOL CSearchFile::GetSelectNum(const int nNum)
{
	int nSize = GetSize();

	if (nNum < 0 || nNum >= nSize) { return -1; }

	CBase *p = (CBase *)m_arrFileName.GetAt(nNum);

	return p->m_nSelecct;
}


/// <summary>
/// �ړ�����Z�b�g���ꂽ����Ԃ�
/// </summary>
/// <returns>�Z�b�g���ꂽ��</returns>
int CSearchFile::GetSelectCount()
{
	int nRet  = 0;
	int nSize = GetSize();

	for (int i = 0; i < nSize; i++) {
		CBase *p = (CBase *)m_arrFileName.GetAt(i);
		if (p->m_nSelecct >= 0) {
			nRet++;
		}
	}

	return nRet;
}



//===========================================================================
// �����g���q�̊֌W
//===========================================================================

/// <summary>
/// �g���q�̃Z�b�g 
/// </summary>
/// <param name="pszFileExt">�g���q</param>
void CSearchFile::SetScanExt(const char *pszFileExt)
{
	if (*pszFileExt != '\0') {
		int nLen = (int)strlen(pszFileExt);

		char *pExt = new char[nLen + 5];

		strcpy_s(pExt, nLen + 1 , pszFileExt);
		_strupr_s(pExt, nLen + 1);
		m_arrFileExt.Add((void *)pExt);
	}
}


/// <summary>
/// �o�^�����g���q�̐���Ԃ�
/// </summary>
/// <returns>�o�^��</returns>
int CSearchFile::GetExtSize()
{
	return (int)m_arrFileExt.GetSize();
}


/// <summary>
/// �g���q��Ԃ�
/// </summary>
/// <param name="nNum">�C���f�b�N�X�ԍ�</param>
/// <param name="pExt">�ۑ�����o�b�t�@�|�C���^</param>
/// <param name="nBufSize">�m�ۂ��Ă���o�b�t�@�T�C�Y</param>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL CSearchFile::GetScanExt(const int nNum, char *pExt, const int nBufSize)
{
	int nSize = GetExtSize();

	if (nNum < 0 || nNum >= nSize) { return TRUE; }

	strcpy_s(pExt, nBufSize, (char *)m_arrFileExt.GetAt(nNum));

	return FALSE;
}



/// <summary>
/// �w�肳�ꂽ�t�@�C�������A�X�L�����Ώۂ̊g���q�̂����ꂩ�ŏI����Ă��邩�𔻒肵�܂��B
/// </summary>
/// <param name="pszFileName">���肷��t�@�C�����̕�����B</param>
/// <returns>FALSE:�������Ȃ�/TRUE:������</returns>
BOOL CSearchFile::CheckFileExt(const char *pszFileName)
{
	int i;
	BOOL bRet = FALSE;
	int nLoop = GetExtSize();
	char szExt[MAX_PATH + 5];
	const char *cp;

	for (i = 0; i < nLoop; i++) {
		GetScanExt(i, szExt, MAX_PATH);
		if (NULL != (cp = strstr(pszFileName, szExt))) {
			if (*(cp + strlen(szExt)) == '\0') {
				bRet = TRUE;
				break;
			}
		}
	}
	return bRet;
}


/// <summary>
/// �����_���\���p
/// </summary>
void CSearchFile::InitRandom()
{
	int i;
	int n;
	int nNum;
	void *p1;
	void *p2;

	nNum = GetSize();

	for (i = 0; i < nNum; i++) {
		n = LIB.Random(nNum);
		if (n != i) {
			p1 = m_arrFileName.GetAt(n);
			p2 = m_arrFileName.GetAt(i);
			m_arrFileName.SetAt(n, p2);
			m_arrFileName.SetAt(i, p1);
		}
	}
}



/// <summary>
/// ���O�Ń\�[�g����
/// </summary>
void CSearchFile::InitSortName()
{
	BOOL bSwap;
	CBase *pi, *pj;
	int nSize = GetSize();

	for (int i = 0; i < nSize - 1; i++) {
		pi = (CBase *)m_arrFileName.GetAt(i);
		for (int j = i + 1; j < nSize; j++) {
			pj = (CBase *)m_arrFileName.GetAt(j);

			bSwap = FALSE;
			const char *p1 = pi->m_strFileTitle;
			const char *p2 = pj->m_strFileTitle;

			while (*p1 != '\0' && *p2 != '\0') {
				BYTE c1 = (BYTE)*p1;
				BYTE c2 = (BYTE)*p2;

				if ('a' <= c1 && c1 <= 'z') { c1 = c1 - 0x20; }
				if ('a' <= c2 && c2 <= 'z') { c2 = c2 - 0x20; }

				if (c1 > c2) {
					bSwap = TRUE;
					break;
				} else if (c1 < c2) {
					break;
				}
				p1++;
				p2++;
			}

			if (bSwap) {
				m_arrFileName.SetAt(i, (void *)pj);
				m_arrFileName.SetAt(j, (void *)pi);
				pi = pj;
			}
		}
	}
}


/// <summary>
/// �X�V�����Ń\�[�g����
/// </summary>
/// <param name="bRevase">FALSE:�����V/TRUE:�V����</param>
void CSearchFile::InitSortLastWrite(const BOOL bRevase)
{
	BOOL bSwap;
	CBase *pi, *pj;
	int nSize = GetSize();

	for (int i = 0; i < nSize - 1; i++) {
		pi = (CBase *)m_arrFileName.GetAt(i);
		for (int j = i + 1; j < nSize; j++) {
			pj = (CBase *)m_arrFileName.GetAt(j);

			bSwap = bRevase;
			if (pi->m_dw64DateTime > pj->m_dw64DateTime) {
				bSwap = !bRevase;
			}

			if (bSwap) {
				m_arrFileName.SetAt(i, (void *)pj);
				m_arrFileName.SetAt(j, (void *)pi);
				pi = pj;
			}
		}
	}

}
