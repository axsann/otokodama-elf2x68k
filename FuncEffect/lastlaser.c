#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../effect.h"
#include "../priority.h"
#include "../damage.h"

#define PALET_MAIN	0x0300

static short EffectMoveLastLaser (EFFECT *);


void EffectInitLastLaser (EFFECT * p)
{
	p->y = 256+16;
	p->pt = obj_blaser;
	p->info = PALET_MAIN | PRIORITY_BOMBER;
	p->seq = 0;
	p->seq2 = 0;
	p->func_effect_move = EffectMoveLastLaser;
}



static short EffectMoveLastLaser (EFFECT * p)
{
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
	if (p->seq2 > 15) {
		p->seq2 = 0;
		p->y -= 16;
	}
	p->y++;
	xobj_set_st (p);

	return (0);
}
