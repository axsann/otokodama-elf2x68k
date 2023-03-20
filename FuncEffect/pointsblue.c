#include <XSP2lib.H>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMovePointsBlue (EFFECT *);


void EffectInitPointsBlue (EFFECT * p)
{
	p->pt = obj_points + p->type2;
	p->info = PALET_POINTS_BLUE | PRIORITY_POINTS;
	p->seq = 0;
	p->func_effect_move = EffectMovePointsBlue;
	p->ly = p->y << 16;
	p->vy = -1 << 14;

	{
		/* ��U�e���|�����ɃR�s�[������������ */
		signed int t = points[p->type2];
		score += t;
		extend_score -= t;
	}
}



static short EffectMovePointsBlue (EFFECT * p)
{
    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
	p->y = (p->ly += p->vy) >> 16;

	p->seq++;
	if (p->seq > 60)
		return (-1);
	else
		xobj_set_st (p);

	return (0);
}
