#include <xsp2lib.h>

#include "otoko.h"
#include "player.h"
#include "shot.h"
#include "priority.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define SHOT_MAX	24	/* ショット最大数 */
static SHOT shot[SHOT_MAX];	/* ワーク */

#define DISP_OFFSET_X	(-8)	/* (x,y)+(-8,-8) にスプライトを表示 */
#define DISP_OFFSET_Y	(-8)

#define SHOT_SPEED	31


/* 起動時に１度だけ呼ばれる */
void ShotInit0 (void)
{
	int i;

    /* リストをつなげる */
	shot_top = NULL;
	shot_null_top = shot;
	for (i = 0; i < SHOT_MAX; i++)
		shot[i].next = &shot[i + 1];

	shot[SHOT_MAX - 1].next = NULL;
}



/* ショット出現時に呼ばれる */
void ShotInit (unsigned char angle)
{
	SHOT *p;

	if (shot_null_top == NULL)	/* ワークの空きはあるか？ */
		return;

	p = shot_null_top;
	shot_null_top = p->next;
	p->next = shot_top;
	shot_top = p;

	p->lx = (player->x) << 16;	/* ショットの座標は自機+(0,-16) */
	p->ly = (player->y + 4) << 16;	/* 自機の少し上に出す */
	p->pt = sp_shot + pt_rot[angle];
	p->info = 0x0100 | PRIORITY_SHOT | info_rot[angle];
#if	0
	0b xxxx_xxxx
	  0 ~ 63:0
	  64 ~ 127:4
	  128 ~ 191:8
	  192 ~ 255:c
#endif
	  p->damage = 0;
	p->frame = -1;
	p->vx = vxtable[SHOT_SPEED][angle];
	p->vy = vytable[SHOT_SPEED][angle];
}



/* 垂直同期ごとに呼ばれる */
void ShotMove (void)
{
	SHOT *p, *q;

	p = shot_top;		/* 現在注目しているワーク */
	q = NULL;		/* １つ前のワーク（ワーク削除時に必要） */
	while (p != NULL) {
	    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
		p->x = ((p->lx += p->vx) >> 16) + DISP_OFFSET_X;
		p->y = ((p->ly += p->vy) >> 16) + DISP_OFFSET_Y;

		p->frame++;
	    /* ショットが敵に当たった又は画面外に出たか？ */
	    /* （画面右から出た判定と左から出た判定を１回の比較で行っている事に注目） */
		if ((p->damage)
		    || ((unsigned short) (p->x) > 256 + 16 - DISP_OFFSET_X)
		    || ((unsigned short) (p->y) > 256 + 16 - DISP_OFFSET_Y)) {

			if (q == NULL) {	/* リストの一番最初を削除 */
				shot_top = p->next;
				p->next = shot_null_top;
				shot_null_top = p;
				q = NULL;
				p = shot_top;
			} else {
				q->next = p->next;
				p->next = shot_null_top;
				shot_null_top = p;
				p = q->next;
			}
		} else {
			xsp_set_st (p);
			q = p;
			p = p->next;
		}
	}
}



/* ショット消去時に呼ばれる（面クリア等） */
void ShotTini (void)
{
	int i;

    /* リストをつなげる */
	shot_top = NULL;
	shot_null_top = shot;
	for (i = 0; i < SHOT_MAX; i++)
		shot[i].next = &shot[i + 1];

	shot[SHOT_MAX - 1].next = NULL;
}
