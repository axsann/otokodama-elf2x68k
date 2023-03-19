/* shot.h */

typedef struct _shot {
	signed short x, y;	/* 座標 */
	short pt;		/* スプライトパターンＮｏ． */
	short info;		/* 反転コード・色・優先度を表わすデータ */
	short type;		/* 種類 */
	signed int lx, ly;	/* 32bit X,Y 座標 ( l = longword ) */
	signed int vx, vy;	/* 速度 */
	short damage;		/* 敵に当たったら = !0 */
	char frame;		/* 出現してから何フレーム経ったか */
	struct _shot *next;	/* 次の構造体へのポインタ */
} SHOT;


#ifdef GLOBAL_DEFINE		/* グローバル変数の定義と宣言を１つにまとめるテク */
#define Extern			/* Extern をヌル文字列に置換 */
#else
#define Extern extern		/* Extern を extern に置換 */
#endif

Extern SHOT *shot_top,		/* 使用中のワークのリスト */
 *shot_null_top;		/* 空のワークのリスト */


/* 関数プロトタイプ宣言 */
void ShotInit0 (void);
void ShotInit (unsigned char);
void ShotMove (void);
void ShotTini (void);
