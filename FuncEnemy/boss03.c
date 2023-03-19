#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"

#define PALET_BOSS03	0x0400

static short EnemyMoveBoss03 (ENEMY *);
static void EnemyTiniBoss03 (ENEMY *);


void EnemyInitBoss03 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 32768;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = 1000;
	p->pt = obj_oplaser;
	p->core_pt = sp_core01;
	p->work = 0;
	p->func_enemy_move = EnemyMoveBoss03;
	p->func_enemy_tini = EnemyTiniBoss03;
}



static short EnemyMoveBoss03 (ENEMY * p)
{
	/* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->work) {
	case 0:
		if (SHORT_LY > 64) {
			p->work++;
			p->work2 = 0;
			p->work3 = 0;
			p->s_angle = 64 + 32;	/* 左から出る弾の角度 */
			p->s_angle2 = 64 - 32;	/* 右から出る弾の角度 */
		}
		break;
	case 1:
		p->work2++;
		/* 画面外に出たら消去 */
		if (SHORT_LY > 256 + 32)
			return (-1);	/* 消去 */

		if (p->work2 < 48) {
			p->work3++;
			if (p->work3 >= 4) {
				p->work3 = 0;
				EshotInit (ESHOT_NRG01, SHORT_LX, SHORT_LY + 12, 4, p->s_angle, 0);
				EshotInit (ESHOT_NRG01, SHORT_LX, SHORT_LY + 12, 4, p->s_angle2, 0);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				p->s_angle -= 8;
				p->s_angle2 += 8;
			}
		}
		if (p->work2 >= 96) {
			p->work2 = 0;
			p->work3 = 0;
			p->s_angle = 64 + 32;	/* 左から出る弾の角度 */
			p->s_angle2 = 64 - 32;	/* 右から出る弾の角度 */
		}
		break;
	}

	/* 自機ショットに当たった時の処理 */
	if (p->damage) {
		if (p->damage_core) {
			SoundSetSE (SE_CORE_DAMAGE);
			p->info = PALET_RED | PRIORITY_BOSS;
			p->damage_core = 0;
		} else {
			SoundSetSE (SE_DAMAGE);
			p->info = PALET_DAMAGE | PRIORITY_BOSS;
		}
		if ((p->hp -= p->damage) <= 0) {
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			return (-1);	/* 消去 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 300)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_BOSS03 | PRIORITY_BOSS;
		}
	}

	xobj_set_st (p);

	/* コアの表示 */
	p->core_x = p->x - 8;	/* コアの中心は (-8,-8) ドットずれる */
	p->core_y = p->y - 8;
	if (player->y < p->y + 64)	/* 自機との座標の差が小さければ赤 */
		p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;
	else
		p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniBoss03 (ENEMY * p)
{
}
