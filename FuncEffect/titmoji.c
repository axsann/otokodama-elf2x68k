/* ƒ^ƒCƒgƒ‹‰æ–Ê‚Ì "OTOKODAMA" ‚Ì•¶Žš */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../effect.h"
#include "../priority.h"

static short EffectMoveTitMoji (EFFECT *);


void EffectInitTitMoji (EFFECT * p)
{
	p->seq = 0;
	p->func_effect_move = EffectMoveTitMoji;
}



static short EffectMoveTitMoji (EFFECT * p)
{
	if (p->seq++ >= 6) {
		return (-1);
	} else {
	    /* OTOKO */
		xobj_set (p->seq * 16 - 8 * 16 + 128 + 16 + 16, 128 + 16, obj_titmoji, 0x130);
	    /* DAMA */
		xobj_set (128-p->seq * 16 + 128 + 16 + 16, 128 + 16, obj_titmoji + 1, 0x130);
	}
	return (0);
}
