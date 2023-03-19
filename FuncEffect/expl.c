#include <xsp2lib.h>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

/* 外部からは見えないけど移動ルーチンは２つある */
static short EffectMoveExpl (EFFECT *);
static short EffectMoveExplLite (EFFECT *);


void EffectInitExpl (EFFECT * p)
{
	if (disp_level == DISP_LEVEL_LOW) {
		p->pt = obj_explall + 38;	/* 表示負荷の軽い方 */
		p->func_effect_move = EffectMoveExplLite;
	} else {
		p->pt = obj_explall;
		p->func_effect_move = EffectMoveExpl;
	}
	p->info = 0x0100 | PRIORITY_BOSS_EXPL;
}



/* 表示負荷の軽い爆発パターン */
static short EffectMoveExplLite (EFFECT * p)
{
	if (p->pt++ >= obj_explall + 38 + 33 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}


/* 通常の爆発パターン */
static short EffectMoveExpl (EFFECT * p)
{
	if (p->pt++ >= obj_explall + 38 - 1)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
