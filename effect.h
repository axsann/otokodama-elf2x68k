/* effect.h */

typedef struct _effect {
	signed short x, y;	/* 座標 */
	short pt;		/* スプライトパターンＮｏ． */
	short info;		/* 反転コード・色・優先度を表わすデータ */
	short type;		/* 種類 */
	short type2;		/* 種類その２ */
	signed int lx, ly;	/* 32bit X,Y 座標 ( l = longword ) */
	signed int vx, vy;	/* 速度 */
	short seq;		/* 現在の状態 */
	short seq2;
	short seq3;
	short (*func_effect_move) (struct _effect *);	/* 移動関数へのポインタ */
	struct _effect *next;	/* 次の構造体へのポインタ */
} EFFECT;


#ifdef GLOBAL_DEFINE		/* グローバル変数の定義と宣言を１つにまとめるテク */
#define Extern			/* Extern をヌル文字列に置換 */
#else
#define Extern extern		/* Extern を extern に置換 */
#endif


/* グローバル変数 */
Extern int *points;


enum {
	EFFECT_EXPL = 0,
	EFFECT_EXPLZAKO,
	EFFECT_EXPLMINI,
	EFFECT_EXPLPLAYER,
	EFFECT_BOMBSTRA,
	EFFECT_BOMBSTRB,
	EFFECT_BOMBSTRC,
	EFFECT_HAHEN,
	EFFECT_HAHENMINI,
	EFFECT_POINTS_RED,
	EFFECT_POINTS_BLUE,
	EFFECT_POINTS,
	EFFECT_BOMBER,
	EFFECT_BACKB,
	EFFECT_BLASER,
	EFFECT_TITSPARK,
	EFFECT_TITMOJI,
	EFFECT_LASTLASER,
	EFFECT_LASTPLAYER,
};


enum {
	POINTS_100 = 0,
	POINTS_200,
	POINTS_300,
	POINTS_400,
	POINTS_500,
	POINTS_600,
	POINTS_700,
	POINTS_800,
	POINTS_900,
	POINTS_1000,
	POINTS_1200,
	POINTS_1500,
	POINTS_1800,
	POINTS_2000,
	POINTS_2500,
	POINTS_3000,
	POINTS_4000,
	POINTS_5000,
	POINTS_6000,
	POINTS_7000,
	POINTS_8000,
	POINTS_9000,
	POINTS_10000,
	POINTS_12000,
	POINTS_15000,
	POINTS_18000,
	POINTS_20000,
	POINTS_25000,
	POINTS_28000,
	POINTS_30000
};



/* 関数プロトタイプ宣言 */
void EffectInit0 (void);
void EffectInit (short, short, short, short);
void EffectMove (void);
void EffectTini (void);


void EffectInitExpl (EFFECT *);
void EffectInitExplZako (EFFECT *);
void EffectInitExplMini (EFFECT *);
void EffectInitExplPlayer (EFFECT *);
void EffectInitBombstr (EFFECT *);
void EffectInitHahen (EFFECT *);
void EffectInitHahenMini (EFFECT *);
void EffectInitPointsRed (EFFECT *);
void EffectInitPointsBlue (EFFECT *);
void EffectInitPoints (EFFECT *);
void EffectInitBomber (EFFECT *);
void EffectInitBackB (EFFECT *);
void EffectInitBLaser (EFFECT *);
void EffectInitTitSpark (EFFECT *);
void EffectInitTitMoji (EFFECT *);
void EffectInitLastLaser (EFFECT *);
void EffectInitLastPlayer (EFFECT *);
