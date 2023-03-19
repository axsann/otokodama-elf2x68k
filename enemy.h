/* enemy.h */

#ifndef SHORT_LX
/* signed int の座標値から固定小数部（上位16bit）を取り出すマクロ */
#define SHORT_LX (*((short *) (&p->lx)))
#define SHORT_LY (*((short *) (&p->ly)))
#define SHORT_VX (*((short *) (&p->vx)))
#define SHORT_VY (*((short *) (&p->vy)))
#endif

#define ZAKO_SCORE_DISP	16	/* ザコキャラの得点を表示する回数 */

/* 敵キャラ点滅（ダメージを受けた、体力が少ない）時のタイマー用定数 */
#define TIMER_FLASH_RED		10	/* タイマーがこれ以上なら赤フラッシュ */
#define TIMER_FLASH_DAMAGE	14	/* ダメージを受けたらタイマーをこの値に */
#define TIMER_FLASH_NORMAL	15	/* これを越えたらタイマーを０に */


typedef struct _enemy {
	signed short x, y;	/* 座標 */
	short pt;		/* スプライトパターンＮｏ． */
	short info;		/* 反転コード・色・優先度を表わすデータ */
	short type;		/* 種類 */
	short arg;		/* 予備の引き数 */
	signed int lx, ly;	/* 32bit X,Y 座標 ( l = longword ) */
	signed int vx, vy;	/* 速度 */
	signed short hit_px, hit_py;	/* 対プレイヤー当たり判定 */
	signed short hit_sx, hit_sy;	/* 対ショット当たり判定（全体） */
	signed short hit_cx;	/* 対ショット当たり判定（コア） */
	signed short hp;	/* 耐久力（<=0 で破壊） */
	unsigned short timer;	/* 時間切れ用タイマー */
	short damage;		/* 今回受けたダメージ（自機ショットに当たっていなければ = 0） */
	char damage_core;	/* 今回コアにダメージを受けたら != 0 */
	char flash;		/* 残り hp が少なくてフラッシュするカウンター */
	unsigned char angle;	/* 角度 (0~255) */
	short (*func_enemy_move) (struct _enemy *);	/* 移動関数へのポインタ */
	void (*func_enemy_tini) (struct _enemy *);	/* 終了関数へのポインタ */
	struct _enemy *next;	/* 次の構造体へのポインタ */


	short work;		/* ワーク */
	short work2;
	short work3;
	short work4;
	char cwork;		/* ワーク(char) */
	char cwork2;
	char cwork3;
	char cwork4;
	char cwork5;
	char seq;		/* 現在の状態 */
	short m_work;		/* 移動用ワーク */
	short m_work2;
	short m_work3;
	short m_work4;
	short s_work;		/* ショット用ワーク */
	short s_work2;
	short s_work3;
	short s_work4;
	unsigned char s_angle;	/* ショットの角度 (0~255) */
	unsigned char s_angle2;
	unsigned char s_angle3;
	unsigned char s_angle4;


	/* 移動用ワーク（SubMoveTo() 用） */
	signed int move_ax, move_ay;	/* 加速度 */
	unsigned short move_seq;/* 加速中、等速度運動中、減速中 */
	signed short move_count;/* 加速度を何回足すか */
	signed short move_count2;	/* 一定速度で進む回数 */
	signed short move_count3;	/* 実際に加速した回数 */
	signed short move_counter;	/* 何回足すか数える */


	/* コア関係のワーク */
	signed short core_x, core_y;	/* コア座標 */
	short core_pt;		/* スプライトパターンＮｏ． */
	short core_info;	/* 反転コード・色・優先度を表わすデータ */


	/* パーツ関係のワーク */
	struct _enemy *parts1;	/* パーツへのポインタ */
	struct _enemy *parts2;
	struct _enemy *parts3;
	struct _enemy *parts4;
	short parts1_work;	/* ワーク */
	short parts1_work2;
	short parts2_work;
	short parts2_work2;
	short parts3_work;
	short parts3_work2;

	/* キャラクター間通信用ワーク */
	struct _enemy *parent;	/* 親のワーク */
	unsigned char child_kill;	/* 親が子を殺す時（親が設定） */
	unsigned char child_death;	/* 子が死んだことを親に知らせる（子が設定） */
} ENEMY;


/* 関数プロトタイプ宣言 */
void EnemyInit0 (void);
void EnemyInitStatic (void);
ENEMY *EnemyInit (short, signed short, signed short, short, struct _enemy *);
void EnemyMove (void);
void EnemyTini (void);

/* サービスルーチン */
void SubEnemyMoveToInit (ENEMY *, signed short, signed short,
			 unsigned char, signed short);
short SubEnemyMoveTo (ENEMY *);
short SubEnemyMoveToAbort (ENEMY *);


/* FuncEnemy/ 以下の敵移動ルーチン */
void EnemyInitBoss02 (ENEMY *);
void EnemyInitBoss03 (ENEMY *);
void EnemyInitCoverF (ENEMY *);
void EnemyInitCoverN (ENEMY *);
void EnemyInitCoverN2 (ENEMY *);

void EnemyInitLBoss01 (ENEMY *);
void EnemyInitLBoss01C (ENEMY *);

void EnemyInitLBoss02 (ENEMY *);
void EnemyInitLBoss02E (ENEMY *);
void EnemyInitLBoss02H (ENEMY *);
void EnemyInitLBoss02W (ENEMY *);

void EnemyInitLBoss03 (ENEMY *);
void EnemyInitStaticLBoss03G (void);
void EnemyInitLBoss03G (ENEMY *);
void EnemyInitLBoss03P (ENEMY *);
void EnemyInitLBoss03U (ENEMY *);
void EnemyInitLBoss03Z (ENEMY *);

void EnemyInitLBoss04 (ENEMY *);
void EnemyInitLBoss04C (ENEMY *);
void EnemyInitLBoss04E (ENEMY *);
void EnemyInitLBoss04S (ENEMY *);
void EnemyInitLBoss04V (ENEMY *);
void EnemyInitLBoss04W (ENEMY *);

void EnemyInitMvLaser (ENEMY *);
void EnemyInitMvLaserH (ENEMY *);
void EnemyInitOpLaser (ENEMY *);
void EnemyInitOpLinear (ENEMY *);
void EnemyInitRound4 (ENEMY *);

void EnemyInitStaticSearchZ (void);
void EnemyInitSearchZ (ENEMY *);

void EnemyInitSword (ENEMY *);
void EnemyInitSword2 (ENEMY *);

void EnemyInitStaticZako02 (void);
void EnemyInitZako02 (ENEMY *);

void EnemyInitStaticZako031 (void);
void EnemyInitZako031 (ENEMY *);

void EnemyInitStaticZako032 (void);
void EnemyInitZako032 (ENEMY *);

void EnemyInitStaticZako033 (void);
void EnemyInitZako033 (ENEMY *);

void EnemyInitStaticZako04 (void);
void EnemyInitZako04 (ENEMY *);

void EnemyInitStaticZako041 (void);
void EnemyInitZako041 (ENEMY *);


enum {
	ENEMY_BOSS02 = 0,
	ENEMY_BOSS03,
	ENEMY_COVERF,
	ENEMY_COVERN,
	ENEMY_COVERN2,
	ENEMY_LBOSS01,
	ENEMY_LBOSS01C,
	ENEMY_LBOSS02,
	ENEMY_LBOSS02E,
	ENEMY_LBOSS02H,
	ENEMY_LBOSS02W,
	ENEMY_LBOSS03,
	ENEMY_LBOSS03G,
	ENEMY_LBOSS03P,
	ENEMY_LBOSS03U,
	ENEMY_LBOSS03Z,
	ENEMY_LBOSS04,
	ENEMY_LBOSS04C,
	ENEMY_LBOSS04E,
	ENEMY_LBOSS04S,
	ENEMY_LBOSS04V,
	ENEMY_LBOSS04W,
	ENEMY_MVLASER,
	ENEMY_MVLASERH,
	ENEMY_OPLASER,
	ENEMY_OPLINEAR,
	ENEMY_ROUND4,
	ENEMY_SEARCHZ,
	ENEMY_SWORD,
	ENEMY_SWORD2,
	ENEMY_ZAKO02,
	ENEMY_ZAKO031,
	ENEMY_ZAKO032,
	ENEMY_ZAKO033,
	ENEMY_ZAKO04,
	ENEMY_ZAKO041,
};
