/* １面ボスのカバー */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"

static short EnemyMoveLBoss01C (ENEMY *);
static void EnemyTiniLBoss01C (ENEMY *);


void EnemyInitLBoss01C (ENEMY * p)
{
	p->vx = 0;
	p->vy = 0;
	p->hit_px = 0;
	p->hit_py = 0;
	p->hit_sx = 0;
	p->hit_sy = 0;
	p->hit_cx = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = 1000;
	p->pt = obj_lboss01;
	p->work = 0;
	p->func_enemy_move = EnemyMoveLBoss01C;
	p->func_enemy_tini = EnemyTiniLBoss01C;

	if (p->arg < 1) {
		p->info = PALET_LBOSS01 | PRIORITY_BOSS_PARTS;
	} else {
		p->arg -= 2;
		p->info = PALET_LBOSS01_1 | PRIORITY_BOSS_PARTS;
	}
}



static short EnemyMoveLBoss01C (ENEMY * p)
{
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
			EffectInit (EFFECT_POINTS_RED, POINTS_3000, p->x, p->y);
		if (disp_level == DISP_LEVEL_HIGH)
			EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
		return (-1);	/* 消去 */
		break;
	}
	xobj_set_st (p);


	return (0);
}


static void EnemyTiniLBoss01C (ENEMY * p)
{
}
