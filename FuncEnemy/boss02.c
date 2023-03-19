/* 直進してばらまき弾（弾消しあり） */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"
#include "../entry.h"

#define PALET_MAIN	0x0300

#define ENTRY_CONTROL_ADD	10	/* entry_control に足す値 */

static short EnemyMoveBoss02 (ENEMY *);
static void EnemyTiniBoss02 (ENEMY *);


void EnemyInitBoss02 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 4 * 65536;
	p->hit_px = 32;
	p->hit_py = 8;
	p->hit_sx = 32;
	p->hit_sy = 8;
	p->hit_cx = 8;
	p->damage = 0;
	p->flash = 0;
	p->hp = 200;
	p->pt = obj_coverf;
	p->seq = 0;
	p->angle = p->x + p->y;	/* 乱数 */
	p->func_enemy_move = EnemyMoveBoss02;
	p->func_enemy_tini = EnemyTiniBoss02;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveBoss02 (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
		p->vy -= 4096;
		if (p->vy < 32768) {
			p->vy = 32768;
			p->seq++;
			p->m_work = 0;
		}
		break;
	case 1:		/* カバー開ける */
		p->m_work++;
		p->pt++;
		if (p->m_work > 10) {
			SoundSetSE (SE_LOCK);	/* 「がちゃん」という効果音を鳴らす */
			p->seq++;
			p->s_work = 0;
		}
		break;
	case 2:		/* ばらまき弾 */
		if (p->s_work++ > 1) {
			p->s_work = 0;
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 5, rndtable[p->angle++] & 0xfe, 4);
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 5, rndtable[p->angle++] & 0xfe, 4);
			SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
		}
	    /* ある程度進んだらコアを赤に */
		if ((p->core_info == (PALET_CORE_BLUE | PRIORITY_BOSS_CORE)) && (SHORT_LY > 128))
			p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
		if (SHORT_LY > 192) {
			p->seq++;
			p->m_work = 0;
		}
		break;
	case 3:		/* カバー閉じる */
		p->m_work++;
		p->pt--;
		if (p->m_work > 10) {
			SoundSetSE (SE_LOCK);	/* 「がちゃん」という効果音を鳴らす */
			p->seq++;
		}
		break;
	case 4:
		if (SHORT_VY < 4)
			p->vy += 4096;
	    /* 画面外に出たら消去 */
		if (SHORT_LY > 256 + 32)
			return (-1);	/* 消去 */
		break;
	case 5:
	    /* 画面外に出たら消去 */
		if (SHORT_LY > 256 + 32)
			return (-1);	/* 消去 */
		break;
	}

    /* 自機ショットに当たった時の処理 */
	if (p->damage) {
		if (p->damage_core) {
			SoundSetSE (SE_CORE_DAMAGE);	/* 効果音 */
			p->info = PALET_RED | PRIORITY_BOSS;
			p->damage_core = 0;
		} else {
			SoundSetSE (SE_DAMAGE);	/* 効果音 */
			p->info = PALET_DAMAGE | PRIORITY_BOSS;
		}
		if ((p->hp -= p->damage) <= 0) {
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* 爆発音 */
			if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE))
				EffectInit (EFFECT_POINTS_RED, POINTS_1500, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_1000, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* 弾消し */
			return (-1);	/* 消去 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 100)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_MAIN | PRIORITY_BOSS;
		}
	}

	xobj_set_st (p);

    /* コアの表示 */
	p->core_x = p->x - 8;	/* コアの中心は (-8,-8) ドットずれる */
	p->core_y = p->y - 8;
	xsp_set_st (&(p->core_x));

	return (0);
}


static void EnemyTiniBoss02 (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
