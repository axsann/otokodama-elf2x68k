/* 剣みたいに長い敵 */
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

#define HP_0	400

#define ENTRY_CONTROL_ADD	10	/* entry_control に足す値 */

enum {
	APPEAR_LEFT = 0, APPEAR_RIGHT
};

#define wabs(x) ((x) >= 0 ? (short) (x) : (short) -(x))

static short EnemyMoveSword2 (ENEMY *);
static void EnemyTiniSword2 (ENEMY *);


void EnemyInitSword2 (ENEMY * p)
{
	if (SHORT_LX < 144) {
		p->cwork = APPEAR_LEFT;
		p->vx = 65536;
	} else {
		p->cwork = APPEAR_RIGHT;
		p->vx = -65536;
	}
	p->vy = -16384;
	p->hit_px = 28;
	p->hit_py = 16;
	p->hit_sx = 48;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_sword;
	p->seq = 0;
	p->m_work = 0;
	p->s_work = 0;
	p->core_pt = sp_core01;
	p->func_enemy_move = EnemyMoveSword2;
	p->func_enemy_tini = EnemyTiniSword2;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveSword2 (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {

	case 0:
		if (p->m_work++ > 150) {
			if (p->cwork == APPEAR_LEFT) {
				if ((p->vx -= 2048) <= 0)
					p->seq++;
			} else {
				if ((p->vx += 2048) >= 0)
					p->seq++;
			}
		}
	    /* 攻撃処理 */
#define INTERVAL0_1	14
#define SPEED_SHOT0_1	16
		if (p->s_work++ > INTERVAL0_1) {
			p->s_work = 0;
			EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT0_1, 64, 0);
			SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
		}
		break;

	case 1:
		if (p->vy < 16384) {
			p->vy += 2048;
		} else {
			p->seq++;
		}
		break;

	    /* カバー開く */
	case 2:
		p->pt++;
		if (p->pt >= obj_sword + 24) {	/* カバー開ききったら */
			p->seq++;
			p->s_work = p->s_work2 = p->s_work3 = 0;
		}
		break;

	case 3:
	    /* 攻撃処理 */

	    /* なぎ倒し弾 */
		switch (p->s_work2++) {
#define INTERVAL1_1	20
#define SPEED_SHOT1_1	10
		case INTERVAL1_1 * 1:
			p->s_angle = psearch (p->x - 16, p->y) - 2;	/* 自機の方向をサーチ */
			p->s_angle2 = psearch (p->x + 16, p->y) + 2;	/* 自機の方向をサーチ */
		case INTERVAL1_1 * 2:
		case INTERVAL1_1 * 3:
		case INTERVAL1_1 * 4:
			EshotInit (ESHOT_NRG02, p->x - 16, p->y + 1, SPEED_SHOT1_1, p->s_angle, 0);
			EshotInit (ESHOT_NRG02, p->x + 16, p->y + 1, SPEED_SHOT1_1, p->s_angle2, 0);
			SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
			break;
		case INTERVAL1_1 * 7:
			p->s_work2 = 0;
			break;
		}

	    /* ３方向弾 */
#define INTERVAL1_2	25
#define SPEED_SHOT1_2	8
		switch (p->s_work3++) {
		case INTERVAL1_2 * 1:
			{
				char a = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT1_2, a, 0);
				EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT1_2, a + 24, 0);
				EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT1_2, a - 24, 0);
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
			}
			break;
		case INTERVAL1_2 * 6:
			p->s_work3 = 0;
			break;
		}

		if (SHORT_LY > 192)
			p->seq++;
		break;

	    /* 撤退 */
	case 4:
		if (SHORT_VY < 1)
			p->vy += 4096;
	    /* 画面外に出たら消去 */
		if (SHORT_LY > 256 + 64 + 16)
			return (-1);	/* 消去 */
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
			if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS))
				EffectInit (EFFECT_POINTS_RED, POINTS_2000, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_1000, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
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

    /* 自機のすぐ前にいると赤 */
	if (wabs (player->x - p->x) <= 8)
		p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
	else
		p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniSword2 (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
