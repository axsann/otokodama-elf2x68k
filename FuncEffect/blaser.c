#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../effect.h"
#include "../priority.h"
#include "../damage.h"

#define PALET_BLASER	0x0300

static short EffectMoveBLaser (EFFECT *);


void EffectInitBLaser (EFFECT * p)
{
	p->pt = obj_blaser;
	p->info = PALET_BLASER | PRIORITY_BOMBER;
	p->seq = 0;
	p->seq2 = 0;
	p->func_effect_move = EffectMoveBLaser;
}



static short EffectMoveBLaser (EFFECT * p)
{
	/* プレイヤーが死んだらレーザーも消す */
	if (player->seq == PLAYER_SEQ_DEAD)
		return (-1);

	switch (p->seq++) {
	case 2:
	case 4:
	case 6:
	case 8:
	case 10:
		p->pt++;
		break;
	case PLAYER_BLASER_END - 10:
	case PLAYER_BLASER_END - 8:
	case PLAYER_BLASER_END - 6:
	case PLAYER_BLASER_END - 4:
	case PLAYER_BLASER_END - 2:
		p->pt--;
		break;
	case PLAYER_BLASER_END:
		return (-1);
		break;
	default:
		break;
	}

	p->seq2++;
	if (p->seq2 > 15)
		p->seq2 = 0;
	p->x = player->x;
	p->y = player->y - 16 + p->seq2;
	xobj_set_st (p);

	return (0);
}
