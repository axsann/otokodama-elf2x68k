#include <XSP2lib.H>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMoveExplMini (EFFECT *);


void EffectInitExplMini (EFFECT * p)
{
	p->pt = obj_explall + 100;
	p->info = 0x0100 | PRIORITY_ZAKO_EXPL;
	p->func_effect_move = EffectMoveExplMini;
}



static short EffectMoveExplMini (EFFECT * p)
{
	if (p->pt++ >= obj_explall + 100 + 6 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
