#include <XSP2lib.H>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMoveBackB (EFFECT *);


void EffectInitBackB (EFFECT * p)
{
	p->pt = obj_explall + 168;
	p->info = 0x0100 | PRIORITY_BOMBER;
	p->func_effect_move = EffectMoveBackB;
}



static short EffectMoveBackB (EFFECT * p)
{
	if (p->pt++ >= obj_explall + 168 + 68 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
