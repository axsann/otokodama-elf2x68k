/* txfont.h */

#ifdef GLOBAL_DEFINE		/* グローバル変数の定義と宣言を１つにまとめるテク */
#define Extern			/* Extern をヌル文字列に置換 */
#else
#define Extern extern		/* Extern を extern に置換 */
#endif


/* グローバル変数 */
Extern unsigned char font_data[2048];	/* 読み込んだフォントデータ */


/* 関数プロトタイプ宣言 */
void TxfontPage (char *);
void TxfontCursor (short, short);
int TxfontInit0 (void);
void TxfontPutchar (char);
void TxfontPuts (char *);
void TxfontTini0 (void);
