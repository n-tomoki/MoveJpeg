

#include "pch.h"
#include "MoveJpeg.h"
#include "gv.h"
#include "File.h"


//===========================================================================
// �J�n���I������
//===========================================================================

/// <summary>�R���X�g���N�^</summary>
CGv::CGv()
{
	m_bInit  = FALSE;
	m_dwInst = 0;

	m_hszService = 0;
	m_hszTopic   = 0;
	m_hConv      = 0;
}


/// <summary>�f�X�g���N�^</summary>
CGv::~CGv()
{
	End();
}


//===========================================================================
// ���������I������
//===========================================================================

/// <summary>
/// ����������
/// </summary>
/// <returns>FALSE:����/TRUE:���s</returns>
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
/// �I������
/// </summary>
void CGv::End()
{
	CheckDIBRequest();
	DDE_End();
	GvClose();
}

//---------------------------------------------------------------------------
// �v���t�@�C��
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
/// �uGV.EXE�v�t�@�C��������H
/// </summary>
/// <returns>FALSE:����/TRUE:�Ȃ�</returns>
BOOL CGv::GvScan()
{
	if (File::IsExistFile(m_strGvPath)) { return FALSE; }

	AfxGetMainWnd()->MessageBoxA("�gGV.EXE�h������܂���", "AutoGv", MB_ICONSTOP|MB_OK);

	return TRUE;
}


//===========================================================================
// �c�c�d
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
// �f�u�֌W
//===========================================================================

/// <summary>
/// �R�[���o�b�N�֐�
/// </summary>
HDDEDATA CGv::DdemlCallback(UINT uType, UINT uFmt, HCONV hconv, HSZ hsz1, HSZ hsz2, HDDEDATA hdata, DWORD dwData1, DWORD dwData2)
{
	return (HDDEDATA)NULL;
}


/// <summary>
/// �R�}���h���M
/// </summary>
/// <param name="pszBuf">������</param>
/// <returns>0:����/1:���s</returns>
int CGv::SendCmd(const char *pszBuf)
{
	int nErr = 0;
	HDDEDATA hRet;

	if (!m_bInit)   { Init();   }
	if (!m_bInit)   { return 1; }
	if (!GvCheck()) { return 1; }
	if (!m_hConv)   { return 1; }

	hRet = DdeClientTransaction(
		(LPBYTE)pszBuf,      // �N���C�A���g�f�[�^
		(DWORD)strlen(pszBuf) + 1,  // �f�[�^�T�C�Y
		m_hConv,             // �ʐM�n���h��
		0,                   // �f�[�^����
		0,                   // �N���b�v�{�[�h�t�H�[�}�b�g
		XTYP_EXECUTE,        // �g�����U�N�V�����^�C�v
		1000,                // �ő�҂����ԁ@TIMEOUT_ASYNC�Ŕ񓯊��ƂȂ�
		NULL);               // �g�����U�N�V�����̌���

	if (!hRet && DdeGetLastError(m_dwInst) != DMLERR_NO_ERROR) { nErr++; }

	if (hRet) {
		DdeFreeDataHandle(hRet);
	}

	return nErr;
}


/// <summary>
/// ������̎�M
/// </summary>
/// <param name="pszBuf">�R�}���h���M</param>
/// <param name="strAns">��M������</param>
/// <returns>0:����/1:���s</returns>
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
		NULL,         // �N���C�A���g�f�[�^
		0,            // �f�[�^�T�C�Y
		m_hConv,      // �ʐM�n���h��
		hszRequest,   // �f�[�^����
		CF_TEXT,      // �N���b�v�{�[�h�t�H�[�}�b�g
		XTYP_REQUEST, // �g�����U�N�V�����^�C�v
		1000,         // �ő�҂����ԁ@TIMEOUT_ASYNC�Ŕ񓯊��ƂȂ�
		NULL);        // �g�����U�N�V�����̌���

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
/// �uGV.EXE�v�̏�Ԃ�Ԃ�
/// </summary>
/// <returns>FALSE:OK(�����ĂȂ�)/TRUE:WAIT</returns>
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
/// �uGV.EXE�v���N�����Ă���H
/// </summary>
/// <returns>FALSE:�N�����Ă��Ȃ�/TRUE:�N�����Ă���</returns>
BOOL CGv::GvCheck()
{
	if (NULL == FindWindowA("GV", NULL)) { return FALSE; }

	return TRUE;
}


/// <summary>
/// �uGV.EXE�v���N��������
/// </summary>
/// <returns>FALSE:����/TRUE:���s</returns>
BOOL CGv::GvOpen()
{
	BOOL bRet = TRUE;
	CString str;

	if (GvCheck()) { GvClose();   }
	if (GvCheck()) { return bRet; } // ���s

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
/// �O���v���O�����̎��s
/// </summary>
/// <param name="pCmd">�R�}���h</param>
/// <param name="pCmd2">����</param>
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
/// �uGV.EXE�v�����
/// </summary>
/// <returns>FALSE</returns>
BOOL CGv::GvClose()
{
	// �N�����Ă��Ȃ�
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
/// �uGV.EXE�v���ċN��������
/// </summary>
/// <returns>FALSE:����/TRUE:���s</returns>
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
// �f�u�ւ̃R�}���h
//===========================================================================
// �摜�C���[�W���ł��邾��������(�c���ǂ��炩���w��T�C�Y�ȏ�̍ŏ����̑傫���Łj�\������B
int  CGv::ViewDraftEx(const char *pszFileName, const char cFlag, const int nWidth, const int nHeight)
{
	return 1;
}


// �w�肳�ꂽ�t�@�C����\������B
int  CGv::FileOpen(const char *pszFileName)
{
	int nErr = 0;
	CString str;

	str.Format("FileOpen(\"%s\")", pszFileName);

	nErr = SendCmd(str);
	CheckDIBRequest();

	return nErr;
}


// �w�肳�ꂽ�t�@�C������]�t���O�Ŏw�肳�ꂽ�����ɉ�]���ĕ\������B
int  CGv::FileOpenEx(const char *pszFileName, const char cFlag)
{
	return 1;
}


// �w�肳�ꂽ�t�@�C����ۑ�����B
int  CGv::FileSaveEx(const char *pszFileName, const char *pszImage, const char *pszSaveFalg)
{
	return 1;
}


// �摜�ǂݍ��݂𒆒f����
int  CGv::FileClose()
{
	return 1;
}


// �f�u�̈ʒu�A�y�уT�C�Y��ύX����B
int  CGv::SetWindowPos(const int nFlag, const int nPosX, const int nPosY, const int nSizeX, const int nSizeY, const int nMode)
{
	return 1;
}


// �N���b�v�{�[�h�ɕ\�����̃f�[�^�������o���B
int  CGv::ClipCopy()
{
	return SendCmd("ClipCopy()");
}


// �N���b�v�{�[�h�ɕ\�����̃f�[�^��\���C���[�W�̂܂܏����o���B
int  CGv::ClipCopyViewImage()
{
	return SendCmd("ClipCopyViewImage()");
}


// �N���b�v�{�[�h�̃f�[�^����荞�ށB
int  CGv::ClipPaste()
{
	return 1;
}


// �\������Ă���摜��ǎ��ɐݒ肷��B
//   CENTER:�����ɕ\��
//   TILE:�S�̂ɕ\��
int  CGv::SetWallPaper(const char *pszMode)
{
	CString str;

	str  = "SetWallPaper(";
	str += pszMode;
	str += ")";
	return SendCmd(str);
}


// �f�u�̌��F�I�v�V������ݒ�^���F���s���B
//  ���[�h  ON�F���F������
//	       OFF�F���F�����Ȃ�
//	      AUTO�F���F���ʂ��f�B�X�v���C�h���C�o�̔\�͂ɂ��킹��
//	      EXEC�F���݂̐ݒ�Ŏ��ۂɌ��F����
//	�F��     8�F���F��̐F����8�F�ɂ���
//	        16�F���F��̐F����16�F�ɂ���
//	       256�F���F��̐F����256�F�ɂ���
//�@�@�@   �@-�F�ύX���Ȃ�
//	Path     1�F���F���@��1Pass�ɂ���
//           2�F���F���@��2Pass(Speed)�ɂ���
//	         3�F���F���@��2Pass(Quality)�ɂ���
//�@�@�@�@�@�@-�F�ύX���Ȃ�
//	Dither  ON�F���F���Ƀf�B�U���g��
//	       OFF�F���F���Ƀf�B�U���g��Ȃ�
//�@�@�@    �@-�F�ύX���Ȃ�
int  CGv::Quantize(const char *pszMode, const char *pszColor, const char *pszPass, const char *pszDither)
{
	CString str;

	str.Format("Quantize(%s,%s,%s,%s)", pszMode, pszColor, pszPass, pszDither);
	return SendCmd(str);
}


// �g�嗦���w�肷��B���݂̕\����Ԃ��ύX����B
int  CGv::ZoomRate(const int nZoom)
{
	return 1;
}


// �A�X�y�N�g����w�肷��B���݂̕\����Ԃ��ύX����B
int  CGv::Aspect(const int nAspect, const char *pszType)
{
	return 1;
}


// �\������Ă���摜����]���ĕ\������B
int  CGv::Rotate(const char c)
{
	return 1;
}


// �f�u���I������B
int  CGv::ExitGV()
{
	return 1;
}


// �f�u�̕\���E��\����؂�ւ���B
int  CGv::ShowGV(const char *pszMode)
{
	return 1;
}


// �@�\�F�f�u�̃A�C�R����Ԃ�؂�ւ���B
int  CGv::IconGV(const char *pszMode)
{
	return 1;
}


// �����A�X�y�N�g�䒲���̐ݒ������B
int  CGv::AutoAspect(const char *pszMode)
{
	return 1;
}


// �����\���T�C�Y�����̐ݒ������B
int  CGv::AutoResize(const char *pszMode)
{
	return 1;
}


// �摜�\�����ɓ����Ƀh�L�������g�t�@�C����\���^��\���̐ݒ������B
int  CGv::AutoOpenDocument(const char *pszMode)
{
	return 1;
}


// MAG�̕����ۑ��f�[�^�̔��f���@�̐ݒ������B
int  CGv::PartMAGCheck(const char *pszMode)
{
	return 1;
}


// �\���C���[�W�̍쐬���@�̐ݒ������B
int  CGv::ViewImageMode(const char *pszMode)
{
	return 1;
}


// �摜�̕\���C���[�W���w��T�C�Y�ɐݒ肳��Ă�����@�ō쐬����B
int  CGv::ResizeDIB(const int nWihth, const int nHeigth)
{
	return 1;
}


// �A�j���[�V����GIF�̐ݒ�A������s���B
int  CGv::GifAnimation(const char *pszMode)
{
	return 1;
}


// ���b�Z�[�W�̕\���̗L���̐ݒ������B
//   ON�F���b�Z�[�W��G���[���_�C�A���O�{�b�N�X�ɂ���ĕ\������
//�@OFF�F���b�Z�[�W��G���[��\�����Ȃ�
int  CGv::DispMsg(const char *pszMode)
{
	CString str;

	str.Format("DispMsg(%s)", pszMode);
	return SendCmd(str);
}


// �\�������t�@�C����\���t�@�C�����X�g�ɒǉ�����A���Ȃ���ݒ肷��B
//   ON�F�\�������t�@�C����\���t�@�C�����X�g�ɒǉ�����
//  OFF�F�\�������t�@�C����\���t�@�C�����X�g�ɒǉ����Ȃ�
int  CGv::AddFileList(const char *pszMode)
{
	CString str;

	str.Format("AddFileList(%s)", pszMode);
	return SendCmd(str);
}


// �\���̎��������̗L���A���@����ݒ肷��B
//   ��������    ON�F�t�@�C����ǂݍ��񂾂Ƃ��ɕ\���̎�������������
//        �@�@  OFF�F�t�@�C����ǂݍ��񂾂Ƃ��ɕ\���̎������������Ȃ�
//   �T�C�Y����  ON�F�E�B���h�E�T�C�Y���摜�T�C�Y�ɍ��킹��
//          �@�@OFF�F�E�B���h�E�T�C�Y��ύX���Ȃ�
//   �ʒu����    ON�F�E�B���h�E�ʒu���E�B���h�E����ʂ���͂ݏo���Ȃ��ʒu�Ɉړ�����
//        �@  �@OFF�F�E�B���h�E�ʒu���ړ����Ȃ�
//   �g�嗦����  ON�F�g�嗦���摜�S�̂��E�B���h�E�Ɏ��܂�悤�ɒ�������
//        �@  �@OFF�F�g�嗦�𒲐����Ȃ�
int  CGv::AutoAdjustMode(const char *pszAuto, const char *pszSize, const char *pszPos, const char *pszZoom)
{
	CString str;

	str.Format("AutoAdjustMode(%s,%s,%s,%s)", pszAuto, pszSize, pszPos, pszZoom);
	return SendCmd(str);
}


// �S��ʕ\��������B�S��ʃ��[�h����������Ƃ��͑��̐ݒ�͖����B
int  CGv::MaxDisp(const char *pszMode, const char *pszSize, const char *pszBackColor)
{
	return 1;
}

