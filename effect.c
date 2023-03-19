#include "effect.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define EFFECT_MAX	32	/* エフェクト最大数 */
static EFFECT effect[EFFECT_MAX];	/* ワーク */
static EFFECT *effect_top,	/* 使用中のワークのリスト */
 *effect_null_top;		/* 空のワークのリスト */

typedef void (*func_init) (struct _effect *);
func_init FuncEffectInit[]=
{
	EffectInitExpl,
	EffectInitExplZako,
	EffectInitExplMini,
	EffectInitExplPlayer,
	EffectInitBombstr,
	EffectInitBombstr,
	EffectInitBombstr,
	EffectInitHahen,
	EffectInitHahenMini,
	EffectInitPointsRed,
	EffectInitPointsBlue,
	EffectInitPoints,
	EffectInitBomber,
	EffectInitBackB,
	EffectInitBLaser,
	EffectInitTitSpark,
	EffectInitTitMoji,
	EffectInitLastLaser,
	EffectInitLastPlayer,
};


static int points_table[]=
{
	100, 200, 300, 400, 500, 600, 700, 800, 900, 1000,
	1200, 1500, 1800, 2000, 2500, 3000, 4000, 5000, 6000, 7000,
	8000, 9000, 10000, 12000, 15000, 18000, 20000, 25000, 28000, 30000
};


/* 起動時に１度だけ呼ばれる */
void EffectInit0 (void)
{
	int i;

    /* リストをつなげる */
	effect_top = NULL;
	effect_null_top = effect;
	for (i = 0; i < EFFECT_MAX; i++)
		effect[i].next = &effect[i + 1];

	effect[EFFECT_MAX - 1].next = NULL;

	points = &points_table[0];
}



/* エフェクト出現時に呼ばれる */
void EffectInit (short type, short type2, short x, short y)
{
	EFFECT *p;

	if (effect_null_top == NULL)
		return;		/* 空きのワークがない（キャラクターオーバー） */

	p = effect_null_top;
	effect_null_top = p->next;
	p->next = effect_top;
	effect_top = p;

	p->type = type;
	p->type2 = type2;
	p->x = x;
	p->y = y;
	FuncEffectInit[type] (p);	/* 初期化ルーチンへ飛ぶ（関数へのポインタってヤツ） */
}



/* 垂直同期ごとに呼ばれる */
void EffectMove (void)
{
	EFFECT *p, *q;

	p = effect_top;		/* 現在注目しているワーク */
	q = NULL;		/* １つ前のワーク（ワーク削除時に必要） */
	while (p != NULL) {
	    /* 移動ルーチンへ飛ぶ（関数へのポインタってヤツ） */
		if ((p->func_effect_move) (p)) {	/* 返り値が非０なら消去 */
			if (q == NULL) {	/* リストの一番最初を削除 */
				effect_top = p->next;
				p->next = effect_null_top;
				effect_null_top = p;
				q = NULL;
				p = effect_top;
			} else {
				q->next = p->next;
				p->next = effect_null_top;
				effect_null_top = p;
				p = q->next;
			}
		} else {
			q = p;
			p = p->next;
		}
	}
}



/* エフェクト消去時に呼ばれる（面クリア等） */
void EffectTini (void)
{
	int i;

    /* リストをつなげる */
	effect_top = NULL;
	effect_null_top = effect;
	for (i = 0; i < EFFECT_MAX; i++)
		effect[i].next = &effect[i + 1];

	effect[EFFECT_MAX - 1].next = NULL;
}
