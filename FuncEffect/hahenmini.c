#include <xsp2lib.h>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"


static short EffectMoveHahenMini (EFFECT *);


void EffectInitHahenMini (EFFECT * p)
{
	p->pt = obj_hahen + 87;
	p->func_effect_move = EffectMoveHahenMini;
	p->info = 0x0200 | PRIORITY_HAHEN;
}


static short EffectMoveHahenMini (EFFECT * p)
{
	if (p->pt++ >= obj_hahen + 87 + 50 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
