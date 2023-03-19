#include <xsp2lib.h>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMoveExplPlayer (EFFECT *);


void EffectInitExplPlayer (EFFECT * p)
{
	p->pt = obj_explall;
	p->info = 0x0100 | PRIORITY_PLAYER_EXPL;
	p->func_effect_move = EffectMoveExplPlayer;
}



static short EffectMoveExplPlayer (EFFECT * p)
{
	if (p->pt++ >= obj_explall + 38 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
