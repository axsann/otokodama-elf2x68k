/* ４面ボスのカバー (04V = 04 coVer) */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../enemy.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"

static short EnemyMoveLBoss04V (ENEMY *);
static void EnemyTiniLBoss04V (ENEMY *);


void EnemyInitLBoss04V (ENEMY * p)
{
	p->vx = 0;
	p->vy = 0;
	p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = 0;
	p->pt = obj_lboss04 + 8;
	p->work = 0;
	p->func_enemy_move = EnemyMoveLBoss04V;
	p->func_enemy_tini = EnemyTiniLBoss04V;

	p->info = PALET_LBOSS04 | PRIORITY_BOSS_PARTS;
}



static short EnemyMoveLBoss04V (ENEMY * p)
{
	if ((p->child_kill) || (p->child_death))
		return (-1);	/* 消去 */

    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	if (SHORT_VY < 2)
		p->vy += 4096;
	switch (p->work++) {
	case 0:
	case 8 + 15 * 1:
	case 8 + 15 * 2:
		EffectInit (EFFECT_EXPL, 0, p->x, p->y);
		SoundSetSE (SE_EXPL);	/* 爆発音を鳴らす */
		break;
	case 8 + 15 * 4:
		EffectInit (EFFECT_EXPL, 0, p->x, p->y);
		SoundSetSE (SE_EXPL_M);	/* 爆発音を鳴らす */
		if (p->arg == 0)
			EffectInit (EFFECT_POINTS_RED, POINTS_30000, p->x, p->y);
		if (disp_level == DISP_LEVEL_HIGH)
			EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
		p->child_death = !0;	/* 消去準備 */
		break;
	}
	xobj_set_st (p);


	return (0);
}



static void EnemyTiniLBoss04V (ENEMY * p)
{
}
