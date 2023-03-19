/* psearch.h */

#ifdef GLOBAL_DEFINE		/* グローバル変数の定義と宣言を１つにまとめるテク */
#define Extern			/* Extern をヌル文字列に置換 */
#else
#define Extern extern		/* Extern を extern に置換 */
#endif


/* グローバル変数 */
Extern unsigned char pstable[256][256];


/* 関数プロトタイプ宣言 */
unsigned char psearch (signed short, signed short);
unsigned char msearch (signed short, signed short, signed short, signed short);
