#include <xsp2lib.h>

#include "../otoko.h"
#include "../effect.h"
#include "../player.h"
#include "../priority.h"

#define	PALET_A	0x0100		/* 文字 BOMBER のパレット */
#define	PALET_B	0x0300		/* 文字 BACK-B のパレット */
#define	PALET_C	0x0600		/* 文字 LASER のパレット */

static short EffectMoveBombstr (EFFECT *);


void EffectInitBombstr (EFFECT * p)
{
	p->pt = obj_bombstr + player->bomb_type - 1;
	p->seq = 0;
	p->func_effect_move = EffectMoveBombstr;
	p->seq2 = player->bomb_type;	/* bomb_type を保存しておく */
	switch (player->bomb_type) {
	case BOMB_A:
		p->info = PALET_A | PRIORITY_BOMB_STR;
		break;
	case BOMB_B:
		p->info = PALET_B | PRIORITY_BOMB_STR;
		break;
	case BOMB_C:
		p->info = PALET_C | PRIORITY_BOMB_STR;
		break;
	}

    /* プレイヤーのＸ座標によって文字を出す位置を変える */
	if (player->x < 192) {
		p->seq3 = 0;
		p->lx = -8 << 16;
		p->vx = 8 << 16;
	} else {
		p->seq3 = !0;
		p->lx = -40 << 16;
		p->vx = -8 << 16;
	}
}



static short EffectMoveBombstr (EFFECT * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = player->x + ((p->lx += p->vx) >> 16);
	p->y = player->y;

	if (p->seq3 == 0) {
	    /* 右に出している時 */
		if (p->vx > 1)
			p->vx -= 2 << 16;
		else
			p->vx = 0;
	} else {
	    /* 左に出している時 */
		if (p->vx < -1)
			p->vx += 2 << 16;
		else
			p->vx = 0;
	}

	if (player->bomb_type != p->seq2)
		return (-1);
	xobj_set_st (p);

	return (0);
}
