

//#include "stdafx.h"
#include "pch.h"
#include <mbctype.h>
#include "lib.h"


CLIB LIB;
#if _MSC_VER < 1600
BOOL CLIB::m_bFirst;
#endif

//=====================================================
// �J�n���I������
//=====================================================
CLIB::CLIB()
{
#if _MSC_VER < 1600
	if (!m_bFirst) {
		m_bFirst = TRUE;
		srand((unsigned)time(NULL));
	}
#else
	mt.seed(rnd());
#endif
}


//-----------------------------------------------------
// ������Ԃ�
//-----------------------------------------------------
long CLIB::Random(const unsigned long nNum)
{
	return (long)(RandomA(nNum) & 0x7fffffff);
}

unsigned long CLIB::RandomA(const unsigned long nNum)
{
	unsigned long    lVal   = 0;
	unsigned __int64 n64Val = 0;

#if _MSC_VER >= 1600
	n64Val = (unsigned __int64)mt();
#else
	n64Val = (unsigned __int64)rand() * (unsigned __int64)rand();
#endif

	lVal = (unsigned long)(n64Val % (unsigned __int64)nNum);

	return lVal;
}

#if _MSC_VER >= 1600
unsigned long CLIB::RandomX()
{
	return mt();
}
#endif



//=====================================================
// ������𐔂ɕϊ�
//=====================================================
//-----------------------------------------------------
// �P�U�i������𐔂ɕϊ�
//-----------------------------------------------------
unsigned long CLIB::StringHex2Long(const char *pszBuf)
{
	int n;
	unsigned long ulAns = 0;
	char c;
	const char *cp = pszBuf;

	while (*cp == ' ' ) { cp++; }
	while (*cp != '\0') {
		c = *cp++;
		
		if ('a' <= c && c <= 'z') { c -= 0x20; }
		if ((c >= '0' && c <= '9') || ( c >= 'A' && c <= 'F')) {
			ulAns *= 16;
			n = c - '0';
			if (n > 9) {
				n -= 7;
			}
			ulAns += n;
		} else {
			break;
		}
	}
	return ulAns;
}



//=====================================================
// ������֌W
//=====================================================

/// <summary>
/// ��������̑S�Ă̋󔒂��폜����
/// </summary>
/// <param name="pBuf">�폜���镶����</param>
void CLIB::AllSpaceCut(char *pBuf)
{
	char *cp = pBuf;
	char *e;
	char *s;

	while (*cp != '\0') {
		if (*cp == '\"') {
			cp++;
			while (1) {
				if (*cp == '\"') {
					cp++;
					break;
				} if (*cp == '\0') {
					break;
				}
				cp++;
			}
		} else if (*cp == '\t' || *cp == ' ') {
			s = cp;
			e = cp + 1;
			do {
				*s++ = *e++;
			} while(*s != '\0');
		} else {
			cp++;
		}
	}
	*cp = '\0';
}


/// <summary>
/// ������̓��̋󔒂��폜����
/// </summary>
/// <param name="pBuf">�폜������������</param>
void CLIB::TopSpaceCut(char *pBuf)
{
	char *rp = pBuf;
	char *wp = pBuf;

	while (*rp != '\0') {
		if (*rp == ' ') {
			rp++;
		} else if (*rp == '\t') {
			rp++;
		} else if ((BYTE)(*rp) == 0x81 && (BYTE)(*(rp + 1)) == 0x40) {
			rp += 2;
		} else {
			break;
		}
	}

	if (wp != rp) {
		do {
			*wp++ = *rp++;
		} while (*(wp - 1) != '\0');
	}
}


/// <summary>
/// ������̓��̋󔒂��폜����
/// </summary>
/// <param name="pBuf">�폜������������</param>
void CLIB::TopSpaceCut(WCHAR *pBuf)
{
	WCHAR *rp = pBuf;
	WCHAR *wp = pBuf;

	while (*rp != '\0') {
		if (*rp == ' ') {
			rp++;
		} else if (*rp == '�@') {
			rp++;
		} else if (*rp == '\t') {
			rp++;
		} else {
			break;
		}
	}

	if (wp != rp) {
		do {
			*wp++ = *rp++;
		} while (*(wp - 1) != '\0');
	}
}


/// <summary>
/// ������̌��̋󔒂��폜����
/// </summary>
/// <param name="pBuf">�폜������������</param>
void CLIB::BackSpaceCut(char *pBuf)
{
	char *cp;

	cp = pBuf + strlen(pBuf) - 1;
	while(cp >= pBuf) {
		if (*cp == '\t' || *cp == ' ' || *cp == '\n' || *cp == '\r') {
		} else if ((BYTE)(*cp) == 0x40 && (BYTE)(*(cp - 1)) == 0x81) {
			cp--;
		} else {
			break;
		}
		*cp-- = '\0';
	}
}


/// <summary>
/// ������̌��̋󔒂��폜����
/// </summary>
/// <param name="pBuf">�폜������������</param>
void CLIB::BackSpaceCut(WCHAR *pBuf)
{
	WCHAR *cp;

	cp = pBuf + wcslen(pBuf) - 1;
	while(cp >= pBuf) {
		if (*cp == '\t' || *cp == ' ' || *cp == '\n' || *cp == '\r' || *cp == '�@') {
		} else {
			break;
		}
		*cp-- = '\0';
	}
}


/// <summary>
/// ������̑O�ƌ��̋󔒂��폜����
/// </summary>
/// <param name="pBuf">�폜������������</param>
void CLIB::TrimSpaceCut(TCHAR *pBuf)
{
	TopSpaceCut(pBuf);
	BackSpaceCut(pBuf);
}


/// <summary>
/// �^�u���X�y�[�X�ɕϊ����� 
/// </summary>
/// <param name="dit"></param>
/// <param name="nBufMax"></param>
/// <param name="TabSize"></param>
void CLIB::Tab2Space(char *pstr, const int nBufMax, const int TabSize)
{
	int i;
	int tab = 0;
	int nLen = nBufMax;
	WORD k;
	char *src = new char[nBufMax + 10];
	char *inp = src;
	char *out = pstr;

	strcpy_s(src, nBufMax, pstr);
	while (*inp != '\0') {
		if (*inp == '\t') {
			for (i = tab; i < TabSize; i++) {
				*out = ' ';
				out++;
				nLen--;
			}
			tab = 0;
			inp++;
		} else if (IsKanji(*inp)){
			k = ToJis(*inp , *(inp + 1));
			if (k >= 0x2b24 && k <= 0x2b6f){
				*out = *inp;
				out++;
				inp++;
				nLen--;
				*out = *inp;
				out++;
				inp++;
				tab++;
				nLen--;
			} else {
				*out = *inp;
				out++;
				inp++;
				tab++;
				nLen--;
			}
		} else {
			*out = *inp;
			out++;
			inp++;
			tab++;
			nLen--;
		}
		if (tab >= TabSize) {
			tab = 0;
		}
		if (nLen < TabSize) {
			out = pstr;
			break;
		}
	}
	*out = '\0';
	
	delete []src;
}


//-----------------------------------------------------
// �V�t�g�i�h�r�R�[�h���i�h�r�R�[�h�ɕϊ�����
//-----------------------------------------------------
WORD CLIB::ToJis(BYTE c1, BYTE c2)
{
	if ((c1 -= 0x70) >= 0x30){
		c1 -= 0x40;
	}
	c1 *= 2;
	if ((c2 -= 0x1f) >= 0x80 - 0x1f){
		--c2;
	}
	if (c2 >= 0x7f){
		return (c1 << 8) + ((c2-0x5e) & 255);
	}
	return ((c1 - 1) << 8) + (c2 & 255);
}


//-----------------------------------------------------
// �Q�o�C�g�����̂P�o�C�g�ڂ��̔��f
//-----------------------------------------------------
BOOL CLIB::IsKanji(const BYTE c)
{
#ifdef _UNICODE
	return FALSE;
#else
	return _ismbblead(c);
#endif
}


//-----------------------------------------------------
// �S�p�X�y�[�X�𔼊p�X�y�[�X�ɕϊ�
//-----------------------------------------------------
void CLIB::ZenSpc2HanSpc(char *pBuf)
{
	char *cp = pBuf;

	while (*cp != '\0') {
		if ((BYTE)(*cp) == 0x81 && (BYTE)(*(cp + 1)) == 0x40) {
			*cp = ' ';
			cp++;
			*cp = ' ';
		}
		cp++;
	}
}


//-----------------------------------------------------
// ������̏�������啶���ϊ�
//-----------------------------------------------------
void CLIB::UpperString(TCHAR *pstr)
{
	TCHAR *cp = pstr;

	while (*cp) {
		if (IsKanji((BYTE)*cp)) {
			if (*(cp + 1) == '\0') {
				break;
			}
			cp++;
		} else if (*cp >= 'a' && *cp <= 'z') {
			*cp -= 0x20;
		}
		cp++;
	}
}

void CLIB::UpperString(TCHAR *pstr1, const int nSize, const TCHAR *pstr2)
{
#ifdef _UNICODE
	wcscpy_s(pstr1, nSize, pstr2);
#else
	strcpy_s(pstr1, nSize, pstr2);
#endif
	UpperString(pstr1);
}



//-----------------------------------------------------
// ������̑啶�����������ϊ�
//-----------------------------------------------------
void CLIB::LowerString(TCHAR *pstr)
{
	TCHAR *cp = pstr;

	while (*cp) {
		if (IsKanji((BYTE)*cp)) {
			if (*(cp + 1) == '\0') {
				break;
			}
			cp++;
		} else if (*cp >= 'A' && *cp <= 'Z') {
			*cp += 0x20;
		}
		cp++;
	}
}

void CLIB::LowerString(TCHAR *pstr1, const int nSize, const TCHAR *pstr2)
{
#ifdef _UNICODE
	wcscpy_s(pstr1, nSize, pstr2);
#else
	strcpy_s(pstr1, nSize, pstr2);
#endif
	LowerString(pstr1);
}



//-----------------------------------------------------
// ������̌�납��w�蕶����T��
//-----------------------------------------------------

/// <summary>
/// ������̌�납��w�蕶����T��
/// </summary>
/// <param name="pstr">������</param>
/// <param name="c">�T������</param>
/// <returns>�������ʒu</returns>
int CLIB::BackSearchChar(const char *pstr, const char c)
{
	int nVal = -1;
	const char *cp;

	cp = pstr + strlen(pstr);

	while (cp >= pstr) {
		if (*cp == c) { 
			break;
		}
		cp--;
	}

	if (cp >= pstr) {
		nVal = (int)(cp - pstr);
	}

	return nVal;
}


/// <summary>
/// ������̌�납��w�蕶����T��
/// </summary>
/// <param name="pstr">������</param>
/// <param name="c">�T������</param>
/// <returns>�������ʒu</returns>
int CLIB::BackSearchChar(const WCHAR *pstr, const WCHAR c)
{
	int nVal = -1;
	const WCHAR *cp;

	cp = pstr + wcslen(pstr);

	while (cp >= pstr) {
		if (*cp == c) { 
			break;
		}
		cp--;
	}

	if (cp >= pstr) {
		nVal = (int)(cp - pstr);
	}

	return nVal;
}


//-----------------------------------------------------
// ���p������S�p�����֕ϊ�
// �S�p�p��������S�p�p�啶���֕ϊ�
//-----------------------------------------------------
BOOL CLIB::HanKaku2Zenkaku(const char *psz, char *pBuf, const int nBufSize)
{
	BOOL bVal = FALSE;
	const char *rp = psz;
	char *wp  = pBuf;
	int nSize = nBufSize - 1;
	char szKanji[2];
	BYTE c;
	BYTE d;

	while (*rp != '\0' && nSize != 0) {
		c = *rp++;

		if (_ismbblead(c)) {
			d = *rp++;
			if (c == 0x82) {
				if (0x81 <= d && d <= 0x9a) {
					d -= 0x21;
				}
			}
			*wp++ = c;
			*wp++ = d;
		} else if (0xa1 <= c && c <= 0xdf) {
			if (ChengKatakana(c, (BYTE)*rp, szKanji)) { rp++; }
			*wp++ = szKanji[0];
			*wp++ = szKanji[1];
		} else if (0x20 <= c && c <= 0x5f) {
			ChengEimoji(c, (BYTE)*rp, szKanji);
			*wp++ = szKanji[0];
			*wp++ = szKanji[1];
		} else {
			*wp++ = c;
		}
	}
	*wp = '\0';

	if (!nSize) { bVal = TRUE; }
	return bVal;
}

const char szKATAKANA1[] = "�@�B�u�v�A�E���@�B�D�F�H�������b";
const char szKATAKANA2[] = "�[�A�C�E�G�I�J�L�N�P�R�T�V�X�Z�\";
const char szKATAKANA3[] = "�^�`�c�e�g�i�j�k�l�m�n�q�t�w�z�}";
const char szKATAKANA4[] = "�~���������������������������@�@";

BOOL CLIB::ChengKatakana(const BYTE c1, const BYTE c2, char *pVal)
{
	pVal[0] = ' ';
	pVal[1] = ' ';

	BOOL bVal = FALSE;
	int nNum;

	if (0xa1 <= c1 && c1 <= 0xdf) {
		nNum = c1 - 0xa0;
		if (nNum < 0x10) {
			nNum *= 2;
			memcpy_s(pVal, 2, szKATAKANA1 + nNum, 2);
		} else if (nNum < 0x20) {
			nNum &= 0x0f;
			nNum *= 2;
			memcpy_s(pVal, 2, szKATAKANA2 + nNum, 2);
		} else if (nNum < 0x30) {
			nNum &= 0x0f;
			nNum *= 2;
			memcpy_s(pVal, 2, szKATAKANA3 + nNum, 2);
		} else if (nNum < 0x40) {
			nNum &= 0x0f;
			nNum *= 2;
			memcpy_s(pVal, 2, szKATAKANA4 + nNum, 2);
		}
		if (c2 == 0xde) { // ���_
			pVal[1]++;
			bVal = TRUE;
		} else if (c2 == 0xdf) { // �����_
			pVal[1]++;
			pVal[1]++;
			bVal = TRUE;
		}
	}

	return bVal;
}

const char szEimoji1[] = "�@�I�h���������f�i�j���{�C�|�D�^";
const char szEimoji2[] = "�O�P�Q�R�S�T�U�V�W�X�F�G�������H";
const char szEimoji3[] = "���`�a�b�c�d�e�f�g�h�i�j�k�l�m�n";
const char szEimoji4[] = "�o�p�q�r�s�t�u�v�w�x�y�m���n�O�Q";

BOOL CLIB::ChengEimoji(const BYTE c1, const BYTE c2, char *pVal)
{
	pVal[0] = ' ';
	pVal[1] = ' ';

	int nNum;

	if (0x20 <= c1 && c1 <= 0x5f) {
		nNum = c1 - 0x20;
		if (nNum < 0x10) {
			nNum *= 2;
			memcpy_s(pVal, 2, szEimoji1 + nNum, 2);
		} else if (nNum < 0x20) {
			nNum &= 0x0f;
			nNum *= 2;
			memcpy_s(pVal, 2, szEimoji2 + nNum, 2);
		} else if (nNum < 0x30) {
			nNum &= 0x0f;
			nNum *= 2;
			memcpy_s(pVal, 2, szEimoji3 + nNum, 2);
		} else if (nNum < 0x40) {
			nNum &= 0x0f;
			nNum *= 2;
			memcpy_s(pVal, 2, szEimoji4 + nNum, 2);
		}
	}

	return FALSE;
}


//=====================================================
// Base64
//=====================================================
unsigned char *base64 = (unsigned char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/// <summary>
/// Base64�̃f�R�[�h
/// </summary>
/// <param name="pstr">Base64������</param>
/// <param name="pBuf">�f�R�[�h���ꂽ�o�C�i���[</param>
/// <returns>�f�R�[�h���ꂽ�T�C�Y</returns>
int CLIB::Base64toCode(const char *pstr, char *pBuf)
{
	int n, i;
	int nLoop;
	int nVal = 0;
	const char *rp = pstr;
	Base64Code b64 = {};

	n = (int)strlen(rp);
	char *pTmp = new char[n + 1];
	char *wp   = pTmp;

	while (*rp) {
		if (*rp == '\r' || *rp == '\n') {
			rp++;
		} else {
			*wp++ = *rp++;
		}
	}
	*wp = '\0';

	rp = pstr;
	wp = pBuf;

	b64.uData = 0;
	if (strlen(rp) % 4) { return 0; }

	while (*rp) {
		b64.uData = 0;
		nLoop = 3;
		for (i = 0; i < 4; i++) {
			n = Base64CodeValue(*rp++);
			if (n < 0) { 
				nLoop--;
				n = 0;
			}
			b64.uData <<= 6;
			b64.uData |= n;
		}
		for (i = 1; i <= nLoop; i++) {
			*wp++ = b64.sz[3 - i];
			nVal++;
		}
	}

	delete []pTmp;

	return nVal;
}

int CLIB::Base64CodeValue(const unsigned char c)
{
	if (c == '+') { return 62;           }
	if (c == '/') { return 63;           }
	if (c <= '9') { return c - '0' + 52; }
	if (c <= 'Z') { return c - 'A';      }
	if (c <= 'z') { return c - 'a' + 26; }

	return -1;
}


/// <summary>
/// Base64�̃G���R�[�h
/// </summary>
/// <param name="pstr">�G���R�[�h����o�C�i���[</param>
/// <param name="nDataSize">�G���R�[�h����T�C�Y</param>
/// <param name="pBuf">Base64�̕�����</param>
/// <returns>�G���R�[�h�����T�C�Y</returns>
int CLIB::CodeToBase64(const char *pstr, const int nDataSize, char *pBuf)
{
	int nCnt = 0;
	int n;
	int nRetSize = 0;
	char *wp = pBuf;
	Base64Code b64 = {};

	while (nCnt < nDataSize) {
		b64.uData = 0;
		n = 0;
		for (int i = 0; i < 3; i++, nCnt++) {
			b64.uData <<= 8;
			if (nCnt < nDataSize) {
				b64.uData |= *(pstr + nCnt);
			} else {
				b64.uData |= '\0';
				n++;
			}
		}

		CodeBase64Value(&b64, n);
		for (int i = 0; i < 4; i++) {
			*wp++ = b64.sz[i];
		}
		nRetSize += 4;
	}
	*wp = '\0';

	return nRetSize;
}


int CLIB::CodeBase64Value(Base64Code *pb64, const int n)
{
	long lData = pb64->uData;
	long lNum  = 0;

	for (int i = 0; i < 4; i++) {
		if (n == 0) {
			lNum = lData & 0x3F;
			pb64->sz[3 - i] = *(base64 + lNum);
		} else if (n == 1) {
			if (i == 0) {
				pb64->sz[3 - i] = '=';
			} else {
				lNum = lData & 0x3F;
				pb64->sz[3 - i] = *(base64 + lNum);
			}
		} else if (n == 2) {
			if (i == 0 || i == 1) {
				pb64->sz[3 - i] = '=';
			} else {
				lNum = lData & 0x3F;
				pb64->sz[3 - i] = *(base64 + lNum);
			}
		}
		lData >>= 6;
	}

	return 0;
}




//=====================================================
// �b�q�b�v�Z
//=====================================================
//-----------------------------------------------------
// CRC16
//-----------------------------------------------------
int  CLIB::CreateCRC16(unsigned char *pBuf, const int nLen)
{
	unsigned long lCrc = 0;

	*(pBuf + nLen)     = '\0';	// CRC16(H) Clear
	*(pBuf + nLen + 1) = '\0';	// CRC16(L) Clear

	lCrc = CheckCRC16(pBuf, nLen + 2);

	*(pBuf + nLen)     = (unsigned char)((lCrc >> 8) & 0xff);	// CRC16(H)
	*(pBuf + nLen + 1) = (unsigned char)( lCrc       & 0xff);	// CRC16(L)

	return nLen + 2;
}


unsigned long CLIB::CheckCRC16(const unsigned char *pBuf, const int nLen)
{
	int i;
	int j;
	unsigned long lCrc = 0;
	const unsigned char *cp = pBuf;

	for (i = 0; i < nLen; i++) {
		lCrc = lCrc | *cp++;
		for (j = 0; j < 8; j++) {
			lCrc <<= 1;
			if (lCrc & 0x1000000) {
				lCrc ^= EX_CRCITU;
			}
		}
	}
	lCrc >>= 8;

	return 0x0000ffff & lCrc;
}


