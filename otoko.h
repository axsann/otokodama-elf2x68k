/* otoko.h */

/* #define DEBUG *//* デバッグモード */


#define STAGES	4		/* 総面数 */
#define SPEED_MAX	32
#define EXTEND_EVERY	100000	/* エブリ設定 */
#define BG_PALETS	64	/* BGで使用しているパレット数 */
#define REPLAY_DATA_SIZE_MAX	131072	/* リプレイデータのサイズの最大値（16の倍数にすること） */


#ifdef GLOBAL_DEFINE		/* グローバル変数の定義と宣言を１つにまとめるテク */
#define Extern			/* Extern をヌル文字列に置換 */
#else
#define Extern extern		/* Extern を extern に置換 */
#endif


/* グローバル変数 */
Extern char stage;		/* 面数（１面=0） */
Extern char pause_flag;		/* =!0 なら一時停止中 */
Extern signed int score;	/* 得点 */
Extern signed int extend_score;	/* エクステンドする得点 */

Extern short error_level;	/* 起動時のエラーチェック */
Extern char *error_message;

Extern signed int vxtable[SPEED_MAX][256], vytable[SPEED_MAX][256];
Extern short pt_rot[256];
Extern short info_rot[256];
Extern unsigned char rndtable[256];

Extern short obj_player, obj_oplaser, obj_coverf, obj_covern, obj_round4, sp_eshot01, sp_eshotera,
  obj_explall, obj_bombstr, obj_blaser, obj_hahen, obj_points, obj_searchz, obj_sword, obj_zako02,
  obj_zako03, obj_zako04, obj_mvlaser, obj_mvlaserh, obj_lboss01, obj_lboss02, obj_lboss03,
  sp_lboss03g, obj_lboss04, sp_core01, sp_shot, sp_laser01, sp_cursor, obj_titspark, obj_titmoji;

Extern unsigned char bg_map[STAGES][30 + 32 * 128 * 2];
Extern unsigned char bg_sp[STAGES][32 * 256];
Extern unsigned short stage_palet_table[STAGES][33][BG_PALETS];
Extern signed short bg_contrast_control;	/* BG_xxx が入る */
Extern unsigned short *bg_contrast_ptr;
Extern unsigned short bg_contrast_num;	/* 何段階のコントラストがあるか（33に固定） */

Extern char disp_level;		/* 表示負荷レベル */
Extern char e_option;
Extern char standard_play;	/* 標準設定（残機３、１面から開始）か？ */
Extern char replay_flag;	/* リプレイ中か？ */

Extern unsigned char replay_data[REPLAY_DATA_SIZE_MAX];	/* リプレイデータ */
Extern int replay_data_size;	/* リプレイデータの大きさ */
Extern unsigned char high_score_replay_data[REPLAY_DATA_SIZE_MAX];	/* ハイスコアのリプレイデ
									   ータ */
Extern int high_score_replay_data_size;	/* ハイスコアのリプレイデータの大きさ */


#ifdef DEBUG
Extern short eshot_sum;
Extern short enemy_sum;
#endif


enum {
	ERROR_NON = 0,		/* エラーなし */
	ERROR_TUSEMD,		/* テキスト VRAM が使用中 */
	ERROR_GUSEMD,		/* グラフィック VRAM が使用中 */
	ERROR_FILE,		/* ファイルが読み込めなかった */
	ERROR_PIC_MEMORY,	/* .PIC 展開用メモリが足りなかった */
	ERROR_PIC_FILE,		/* .PIC ファイルが読み込めなかった */
	ERROR_MUSIC,		/* 音楽関係 */
};


enum {
	DISP_LEVEL_LOW = 0,	/* 表示負荷を軽目に */
	DISP_LEVEL_MID,
	DISP_LEVEL_HIGH,
};


enum {
	REPLAY_NON = 0,		/* リプレイ中ではない（通常のゲーム中） */
	REPLAY_BEFORE,		/* 直前のゲームのリプレイ中 */
	REPLAY_HIGH,		/* ハイスコアのリプレイ中 */
};
