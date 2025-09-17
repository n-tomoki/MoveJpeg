
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

	// �摜�C���[�W���ł��邾��������(�c���ǂ��炩���w��T�C�Y�ȏ�̍ŏ����̑傫���Łj�\������B
	int  ViewDraftEx(const char *pszFileName, const char cFlag = ' ', const int nWidth = -1, const int nHeight = -1);
	// �w�肳�ꂽ�t�@�C����\������B
	int  FileOpen(const char *pszFileName);
	// �w�肳�ꂽ�t�@�C������]�t���O�Ŏw�肳�ꂽ�����ɉ�]���ĕ\������B
	int  FileOpenEx(const char *pszFileName, const char cFlag ='L');
	// �w�肳�ꂽ�t�@�C����ۑ�����B
	int  FileSaveEx(const char *pszFileName, const char *pszImage = "ON", const char *pszSaveFalg = "ON");
	// �摜�ǂݍ��݂𒆒f����
	int  FileClose();
	// �f�u�̈ʒu�A�y�уT�C�Y��ύX����B
	int  SetWindowPos(const int nFlag = -1, const int nPosX = 0, const int nPosY = 0, const int nSizeX = 200, const int nSizeY = 200, const int nMode = 0);
	// �N���b�v�{�[�h�ɕ\�����̃f�[�^�������o���B
	int  ClipCopy();
	// �N���b�v�{�[�h�ɕ\�����̃f�[�^��\���C���[�W�̂܂܏����o���B
	int  ClipCopyViewImage();
	// �N���b�v�{�[�h�̃f�[�^����荞�ށB
	int  ClipPaste();
	// �\������Ă���摜��ǎ��ɐݒ肷��B
	int  SetWallPaper(const char *pszMode = "TILE");
	// �f�u�̌��F�I�v�V������ݒ�^���F���s���B
	int  Quantize(const char *pszMode = "AUTO", const char *pszColor = "-", const char *pszPass = "-", const char *pszDither = "-");
	// �g�嗦���w�肷��B���݂̕\����Ԃ��ύX����B
	int  ZoomRate(const int nZoom);
	// �A�X�y�N�g����w�肷��B���݂̕\����Ԃ��ύX����B
	int  Aspect(const int nAspect, const char *pszType);
	// �\������Ă���摜����]���ĕ\������B
	int  Rotate(const char c = 'L');
	// �f�u���I������B
	int  ExitGV();
	// �f�u�̕\���E��\����؂�ւ���B
	int  ShowGV(const char *pszMode = "ON");
	// �@�\�F�f�u�̃A�C�R����Ԃ�؂�ւ���B
	int  IconGV(const char *pszMode = "ON");
	// �����A�X�y�N�g�䒲���̐ݒ������B
	int  AutoAspect(const char *pszMode = "ON");
	// �����\���T�C�Y�����̐ݒ������B
	int  AutoResize(const char *pszMode = "ON");
	// �摜�\�����ɓ����Ƀh�L�������g�t�@�C����\���^��\���̐ݒ������B
	int  AutoOpenDocument(const char *pszMode = "ON");
	// MAG�̕����ۑ��f�[�^�̔��f���@�̐ݒ������B
	int  PartMAGCheck(const char *pszMode = "AUTO");
	// �\���C���[�W�̍쐬���@�̐ݒ������B
	int  ViewImageMode(const char *pszMode = "Speed");
	// �摜�̕\���C���[�W���w��T�C�Y�ɐݒ肳��Ă�����@�ō쐬����B
	int  ResizeDIB(const int nWihth, const int nHeigth);
	// �A�j���[�V����GIF�̐ݒ�A������s���B
	int  GifAnimation(const char *pszMode = "ON");
	// ���b�Z�[�W�̕\���̗L���̐ݒ������B
	int  DispMsg(const char *pszMode = "ON");
	// �\�������t�@�C����\���t�@�C�����X�g�ɒǉ�����A���Ȃ���ݒ肷��B
	int  AddFileList(const char *pszMode = "ON");
	// �\���̎��������̗L���A���@����ݒ肷��B
	int  AutoAdjustMode(const char *pszAuto = "ON", const char *pszSize = "ON", const char *pszPos = "ON", const char *pszZoom = "ON");
	// �S��ʕ\��������B�S��ʃ��[�h����������Ƃ��͑��̐ݒ�͖����B
	int  MaxDisp(const char *pszMode = "ON", const char *pszSize = "ON", const char *pszBackColor = "White");
};

