#include <xsp2lib.h>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMoveExplZako (EFFECT *);


void EffectInitExplZako (EFFECT * p)
{
	p->pt = obj_explall + 71;
	p->info = 0x0100 | PRIORITY_ZAKO_EXPL;
	p->func_effect_move = EffectMoveExplZako;
}



static short EffectMoveExplZako (EFFECT * p)
{
	if (p->pt++ >= obj_explall + 71 + 29 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
