#include <xsp2lib.h>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMoveBomber (EFFECT *);


void EffectInitBomber (EFFECT * p)
{
	p->pt = obj_explall + 106;
	p->info = 0x0100 | PRIORITY_BOMBER;
	p->func_effect_move = EffectMoveBomber;
}



static short EffectMoveBomber (EFFECT * p)
{
	if (p->pt++ >= obj_explall + 106 + 62 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
