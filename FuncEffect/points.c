/* �\���Ȃ��̓��_���Z */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMovePoints (EFFECT *);


void EffectInitPoints (EFFECT * p)
{
	p->func_effect_move = EffectMovePoints;

	{
		/* ��U�e���|�����ɃR�s�[������������ */
		signed int t = points[p->type2];
		score += t;
		extend_score -= t;
	}
}



static short EffectMovePoints (EFFECT * p)
{
	return (-1);
}
