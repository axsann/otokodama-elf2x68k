#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMoveLastPlayer (EFFECT *);


void EffectInitLastPlayer (EFFECT * p)
{
	p->lx = p->x * 65536;
	p->ly = p->y * 65536;
	p->vx = 0;
	p->vy = -2 * 65536;

	p->pt = obj_player + PLAYER_PT_CENTER;
	p->info = PALET_PLAYER | PRIORITY_PLAYER;
	p->seq = p->seq2 = 0;
	p->func_effect_move = EffectMoveLastPlayer;
}



static short EffectMoveLastPlayer (EFFECT * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:		/* 上に上がる */
		if (p->seq2++ >= 32) {
			p->seq++;
			p->seq2 = 0;
			if (p->x < 144)
				p->pt--;
			else
				p->pt++;
		}
		break;
	case 1:
		if (p->vy > 65536)
			p->vy -= 4096;

		if (p->x < 144)
			p->vx -= 4096;
		else
			p->vx += 4096;

		if (p->seq2++ > 8) {
			p->seq++;
			p->seq2 = 0;
			if (p->x < 144)
				p->pt--;
			else
				p->pt++;
		}
		break;
	case 2:
		if (p->vy > 65536)
			p->vy -= 4096;

		if (p->x < 144) {
			if (p->vx > -2 * 65536)
				p->vx -= 4096;
			if (p->x < 0)
				return (-1);
		} else {
			if (p->vx < 2 * 65536)
				p->vx += 4096;
			if (p->x > 256 + 16)
				return (-1);
		}
		break;
	}
	xobj_set_st (p);

	return (0);
}
