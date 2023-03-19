/* タイトル画面のスパーク */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMoveTitSpark (EFFECT *);


void EffectInitTitSpark (EFFECT * p)
{
	p->pt = obj_titspark;
	if (p->type2 == 0)
		p->info = 0x8100 | PRIORITY_ZAKO_EXPL;
	else
		p->info = 0x0100 | PRIORITY_ZAKO_EXPL;
	p->seq = 0;
	p->func_effect_move = EffectMoveTitSpark;
}



static short EffectMoveTitSpark (EFFECT * p)
{
	if (p->seq++ >= 12) {
		return (-1);
	} else {
		xobj_set_st (p);
		p->pt++;
	}
	return (0);
}
