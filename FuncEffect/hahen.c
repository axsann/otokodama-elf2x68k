#include <xsp2lib.h>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"


static short EffectMoveHahen (EFFECT *);


void EffectInitHahen (EFFECT * p)
{
	p->pt = obj_hahen;
	p->func_effect_move = EffectMoveHahen;
	p->info = 0x0200 | PRIORITY_HAHEN;
}


static short EffectMoveHahen (EFFECT * p)
{
	if (p->pt++ >= obj_hahen + 87 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
