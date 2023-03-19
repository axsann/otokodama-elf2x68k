/* 表示なしの得点加算 */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMovePoints (EFFECT *);


void EffectInitPoints (EFFECT * p)
{
	p->func_effect_move = EffectMovePoints;

	{
		/* 一旦テンポラリにコピーした方が速い */
		signed int t = points[p->type2];
		score += t;
		extend_score -= t;
	}
}



static short EffectMovePoints (EFFECT * p)
{
	return (-1);
}
