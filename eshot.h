/* eshot.h */

#define ESHOT_ERASE	14	/* 弾消し用定数 */

typedef struct _eshot {
	signed short x, y;	/* 座標 */
	short pt;		/* スプライトパターンＮｏ． */
	short info;		/* 反転コード・色・優先度を表わすデータ */
	short type;		/* 種類 */
	signed int lx, ly;	/* 32bit X,Y 座標 ( l = longword ) */
	signed int vx, vy;	/* 速度 */
	signed short hit_p;	/* 対プレイヤー当たり判定（X,Y 共通） */
	unsigned char angle;	/* 角度 (0~255) */
	unsigned char speed;	/* 速度 (0~7) */
	struct _eshot *next;	/* 次の構造体へのポインタ */
} ESHOT;


#ifdef GLOBAL_DEFINE		/* グローバル変数の定義と宣言を１つにまとめるテク */
#define Extern			/* Extern をヌル文字列に置換 */
#else
#define Extern extern		/* Extern を extern に置換 */
#endif


/* グローバル変数 */
Extern ESHOT *eshot_top,	/* 使用中のワークのリスト */
 *eshot_null_top;		/* 空のワークのリスト */
Extern char eshot_erase;	/* >0 なら弾を消す */


enum {
	ESHOT_NRG01 = 0,	/* エネルギー弾（極小） */
	ESHOT_NRG02,		/* 　　 〃 　　（小） */
	ESHOT_NRG03,		/* 　　 〃 　　（中） */
	ESHOT_NRG04,		/* 　　 〃 　　（大） */
	ESHOT_NRG05,		/* 　　 〃 　　（特大） */
	ESHOT_NRG06,		/* 　　 〃 　　（特大） */
	ESHOT_NRG22,		/* 　　 〃 　　（小横に２個） */
	ESHOT_NRG32,		/* 　　 〃 　　（中横に２個） */
	ESHOT_NRG23,		/* 　　 〃 　　（小３角形に３個） */
	ESHOT_NRG24,		/* 　　 〃 　　（小正方形に４個） */
	ESHOT_NRG14N,		/* 　　 〃 　　（小斜めに４個） */
	ESHOT_NRG24N,		/* 　　 〃 　　（中斜めに４個） */
	ESHOT_NRG34N,		/* 　　 〃 　　（中斜めに４個） */
	ESHOT_NRG34,		/* 　　 〃 　　（中正方形に４個） */
	ESHOT_NRG15,		/* 　　 〃 　　（小５個） */
	ESHOT_NRG35,		/* 　　 〃 　　（小と大正方形に５個） */
	ESHOT_MIS01,		/* ミサイル弾（方向のあるやつ） */
	ESHOT_MIS02,
	ESHOT_LASER01,		/* レーザー */
	ESHOT_LASEREX,		/* 極太レーザー */
	ESHOT_SEARCH		/* 誘導弾 */
};


/* 関数プロトタイプ宣言 */
void EshotInit0 (void);
void EshotInit (short, signed short, signed short, unsigned char, unsigned char, signed short);
void EshotMove (void);
void EshotTini (void);
