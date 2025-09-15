
#pragma once

#if _MSC_VER >= 1600
#include <random>
#endif

//#define CRC16  0x1007
//#define EX_CRC16  0x100700


#define CRC16  0xA001
#define CRCITU 0x1021
#define EX_CRC16  0xA00100
#define EX_CRCITU 0x102100


// �^�u�T�C�Y
#define DEF_TAB_SIZE 8


class CLIB {
#if _MSC_VER >= 1600
	std::random_device rnd;  // �񌈒�I�ȗ���������
	std::mt19937       mt;   // 32�r�b�g�Ń����Z���k�E�c�C�X�^
#else
	static BOOL m_bFirst;
#endif

public:
	// �J�n����
	CLIB();
	
	// ������Ԃ�
	         long Random (const unsigned long nNum);
	unsigned long RandomA(const unsigned long nNum);
#if _MSC_VER >= 1600
private:
	unsigned long RandomX();
public:
#endif

	// ������𐔂ɕϊ�
	unsigned long StringHex2Long(const char *pszBuf);       // 16�i��������𐔂ɕϊ�

	// ������֌W
	void ZenSpc2HanSpc(char *pBuf);                         // �S�p�X�y�[�X�𔼊p�X�y�[�X�ɕϊ�
	void AllSpaceCut(char *pBuf);                           // ��������̑S�Ă̋󔒂��폜����
	void TopSpaceCut(char *pBuf);                           // ������̓��̋󔒂��폜����
	void BackSpaceCut(char *pBuf);                          // ������̌��̋󔒂��폜����
	void Tab2Space(char *pstr, const int nMax, const int nTabSize = DEF_TAB_SIZE);  // �^�u���X�y�[�X�ɕϊ�����
	WORD ToJis(BYTE c1, BYTE c2);                           // �V�t�g�i�h�r�R�[�h���i�h�r�R�[�h�ɕϊ�����
	BOOL IsKanji(const BYTE c);                             // �����R�[�h�H���̔��f
	int  BackSearchChar(const char *pstr, const char c);	// ������̌�납��w�蕶����T��

	// WCHAR��
	void TopSpaceCut(WCHAR *pBuf);                          // ������̓��̋󔒂��폜����
	void BackSpaceCut(WCHAR *pBuf);                         // ������̌��̋󔒂��폜����
	int  BackSearchChar(const WCHAR *pstr, const WCHAR c);  // ������̌�납��w�蕶����T��

	// TCHAR��
	void TrimSpaceCut(TCHAR *pBuf);                         // ������̑O�ƌ��̋󔒂��폜����
	void UpperString(TCHAR *pstr);							// ������̏�������啶���ϊ�
	void UpperString(TCHAR *pstr1, const int nSize, const TCHAR *pstr2);	// ������̏�������啶���ϊ�
	void LowerString(TCHAR *pstr);							// ������̑啶�����������ϊ�
	void LowerString(TCHAR *pstr1, const int nSize, const TCHAR *pstr2);	// ������̑啶�����������ϊ�

	// ���p������S�p�����֕ϊ����S�p�p��������S�p�p�啶���֕ϊ�
public:
	BOOL HanKaku2Zenkaku(const char *psz, char *pBuf, const int nBufSize);
private:
	BOOL ChengKatakana(const BYTE c1, const BYTE c2, char *pVal);
	BOOL ChengEimoji  (const BYTE c1, const BYTE c2, char *pVal);

	// CRC16
public:
	int           CreateCRC16(unsigned char *pBuf, const int nLen);
	unsigned long CheckCRC16(const unsigned char *pBuf, const int nLen);

	// Base64
private:
	union Base64Code {
		unsigned long  uData;
		unsigned char  sz[4];
	};
	int Base64CodeValue(const unsigned char c);
	int CodeBase64Value(Base64Code *pb64, const int n);
public:
	int Base64toCode(const char *pstr, char *pBuf);
	int CodeToBase64(const char *pstr, const int nSize, char *pBuf);
};

extern CLIB LIB;



