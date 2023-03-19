/* player.h */

typedef struct {
	signed short x, y;	/* 座標 */
	short pt;		/* スプライトパターンＮｏ． */
	short info;		/* 反転コード・色・優先度を表わすデータ */
	signed short bank;	/* 自機の傾き */
	signed int lx, ly;	/* 32bit X,Y 座標 ( l = longword ) */
	char seq;		/* 自機の状態 */
	char seq2;		/* 自機の状態（死亡時の再登場までのカウンター） */
	short muteki;		/* 無敵時のカウンター（>0 で無敵） */
	char flash;		/* 無敵時のパレットフラッシュ用カウンター */
	char shot_timer;	/* 弾の広がり具合を計測するためのタイマー */
	char shot_timer_current;/* Ｂボタンを押した瞬間の shot_timer */
	char shot_cont;		/* 弾を連続的に出すためのカウンター */
	char high_speed;	/* 高速移動中なら !0 */
	short a_button;		/* 前回Ａボタンが押されたか */
	short b_button;		/* 前回Ｂボタンが押されたか */
	char b_button_next;	/* =!0:ショットを出し終える前にＢボタンが押された */
	char bomb_type;		/* ボム種類（=0 でボム文字列なし） */
	char bomb_flash_state;	/* =0 で通常、=1 で白フラッシュ、=2で通常パレット */
	char bomb_flash_timer;	/* フラッシュ用タイマー */
	short blaser;		/* ボンバーレーザーの有効時間 */
	char bomb_hitcheck;	/* ボンバー当たり判定用 */
	char bomb_guage_disp;	/* ボンバーゲージ表示／非表示 */
	short bomb_hitcheck_time;	/* ボンバー当たり判定の残り時間 */
	signed int bomb_energy;	/* ボンバーエネルギー */
	short rest;		/* 残機数 */
	char game_over;	/* ゲームオーバーになった要因 */
	unsigned int replay_counter;	/* リプレイデータを指すカウンター */
} PLAYER;



#ifdef GLOBAL_DEFINE		/* グローバル変数の定義と宣言を１つにまとめるテク */
#define Extern			/* Extern をヌル文字列に置換 */
#else
#define Extern extern		/* Extern を extern に置換 */
#endif


/* グローバル変数 */
Extern PLAYER player[1];


enum {
	PLAYER_SEQ_ALIVE = 0,	/* プレイヤーは生きている */
	PLAYER_SEQ_APPEAR,	/* 登場中（画面下から出てくる・操作不能） */
	PLAYER_SEQ_MUTEKI,	/* 無敵（登場直後） */
	PLAYER_SEQ_BARRIER,	/* バリアで無敵 */
	PLAYER_SEQ_DEAD,	/* 死んでいる */
	PLAYER_SEQ_LAST		/* オールクリア */
};

enum {
	PLAYER_PT_RIGHT2 = 0,	/* 自機が一番右に傾いている時のパターン */
	PLAYER_PT_RIGHT1,	/* 　〃　少し右に　　　　　〃　　　　　 */
	PLAYER_PT_CENTER,	/* 　〃　正面を向いている時のパターン */
	PLAYER_PT_LEFT1,	/* 自機が少し右に傾いている時のパターン */
	PLAYER_PT_LEFT2		/* 　〃　一番右に　　　　　〃　　　　　 */
};


enum {
	BOMB_NON = 0,
	BOMB_A,			/* BOMBER */
	BOMB_B,			/* BACK-B */
	BOMB_C			/* LASER */
};

enum {
	BG_FLASH_NON = 0,	/* 背景はフラッシュ中でない */
	BG_FLASH_WHITE,		/* 背景は白フラッシュ中 */
	BG_FLASH_NORMAL		/* 背景はフラッシュ中だが通常パレット */
};


enum {
	GAME_PLAY = 0,		/* ゲーム中 */
	GAME_OVER,		/* 死んでゲームオーバー */
	GAME_COMPLETE,		/* オールクリア */
	GAME_ABORT,		/* [SELECT]+[START] で中断 */
};


/* 関数プロトタイプ宣言 */
void PlayerInit0 (void);
void PlayerInit (void);
void PlayerMove (void);
void PlayerTini (void);
void DispRest (void);
