/* ２面ボス頭部 (02h = 02head) */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"


#define SPEED_LASER		30

static short EnemyMoveLBoss02H (ENEMY *);
static void EnemyTiniLBoss02H (ENEMY *);


void EnemyInitLBoss02H (ENEMY * p)
{
	p->vx = 0;
	p->vy = 0;
	p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = 0;
	p->pt = obj_lboss02 + 1;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveLBoss02H;
	p->func_enemy_tini = EnemyTiniLBoss02H;

	if (p->arg < 2) {
		p->work4 = PALET_LBOSS02 | PRIORITY_BOSS;
	} else {
		p->work4 = PALET_LBOSS02_1 | PRIORITY_BOSS;
		p->arg -= 2;
	}
}



static short EnemyMoveLBoss02H (ENEMY * p)
{
	/* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	if (SHORT_VY < 2)
		p->vy += 4096;
	switch (p->seq++) {
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
			EffectInit (EFFECT_POINTS_RED, POINTS_2000, p->x, p->y);
		if (disp_level == DISP_LEVEL_HIGH)
			EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
		return (-1);	/* 消去 */
		break;
	}

	p->info = p->work4;
	xobj_set_st (p);


	return (0);
}


static void EnemyTiniLBoss02H (ENEMY * p)
{
}
