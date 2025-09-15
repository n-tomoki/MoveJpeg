
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


// タブサイズ
#define DEF_TAB_SIZE 8


class CLIB {
#if _MSC_VER >= 1600
	std::random_device rnd;  // 非決定的な乱数生成器
	std::mt19937       mt;   // 32ビット版メルセンヌ・ツイスタ
#else
	static BOOL m_bFirst;
#endif

public:
	// 開始処理
	CLIB();
	
	// 乱数を返す
	         long Random (const unsigned long nNum);
	unsigned long RandomA(const unsigned long nNum);
#if _MSC_VER >= 1600
private:
	unsigned long RandomX();
public:
#endif

	// 文字列を数に変換
	unsigned long StringHex2Long(const char *pszBuf);       // 16進数文字列を数に変換

	// 文字列関係
	void ZenSpc2HanSpc(char *pBuf);                         // 全角スペースを半角スペースに変換
	void AllSpaceCut(char *pBuf);                           // 文字列内の全ての空白を削除する
	void TopSpaceCut(char *pBuf);                           // 文字列の頭の空白を削除する
	void BackSpaceCut(char *pBuf);                          // 文字列の後ろの空白を削除する
	void Tab2Space(char *pstr, const int nMax, const int nTabSize = DEF_TAB_SIZE);  // タブをスペースに変換する
	WORD ToJis(BYTE c1, BYTE c2);                           // シフトＪＩＳコードをＪＩＳコードに変換する
	BOOL IsKanji(const BYTE c);                             // 漢字コード？かの判断
	int  BackSearchChar(const char *pstr, const char c);	// 文字列の後ろから指定文字を探す

	// WCHAR版
	void TopSpaceCut(WCHAR *pBuf);                          // 文字列の頭の空白を削除する
	void BackSpaceCut(WCHAR *pBuf);                         // 文字列の後ろの空白を削除する
	int  BackSearchChar(const WCHAR *pstr, const WCHAR c);  // 文字列の後ろから指定文字を探す

	// TCHAR版
	void TrimSpaceCut(TCHAR *pBuf);                         // 文字列の前と後ろの空白を削除する
	void UpperString(TCHAR *pstr);							// 文字列の小文字を大文字変換
	void UpperString(TCHAR *pstr1, const int nSize, const TCHAR *pstr2);	// 文字列の小文字を大文字変換
	void LowerString(TCHAR *pstr);							// 文字列の大文字を小文字変換
	void LowerString(TCHAR *pstr1, const int nSize, const TCHAR *pstr2);	// 文字列の大文字を小文字変換

	// 半角文字を全角文字へ変換＆全角英小文字を全角英大文字へ変換
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



