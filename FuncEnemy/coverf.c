/* 前のカバーを開いてジグザグ扇弾（を出す敵） */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"

#define PALET_COVERF	0x0600

#define SPEED_1		2	/* 加速度 */
#define SPEED_1N	24	/* 加速度を足す回数 */

#define SPEED_2		3	/* 撤退時の加速度 */
#define SPEED_2N	28	/* 加速度を足す回数 */


enum {
	MOVETO_INIT = 0,	/* 初期状態 */
	MOVETO_L,		/* 左に移動 */
	MOVETO_R
};

static short EnemyMoveCoverF (ENEMY *);
static void EnemyTiniCoverF (ENEMY *);


void EnemyInitCoverF (ENEMY * p)
{
	p->vx = 0;
	p->vy = 2 * 65536;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = 300;
	p->pt = obj_coverf;
	p->core_pt = sp_core01;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveCoverF;
	p->func_enemy_tini = EnemyTiniCoverF;

	if (SHORT_LX < (128 + 16))
		p->cwork = 0;	/* 左から出現した */
	else
		p->cwork = !0;
}



static short EnemyMoveCoverF (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:		/* 出現後減速 */
		if (SHORT_LY > 32) {
			p->vy -= 2048;
		}
		if (SHORT_VY < 0) {
			p->vy = 0;
			p->seq++;
			p->m_work = MOVETO_INIT;
			p->m_work2 = 0;
			p->m_work3 = 0;	/* 左右に移動した回数 */
			p->s_work = 0;
		}
		break;
	case 1:
	    /* まず移動処理 */
		if (p->m_work == MOVETO_INIT) {
		    /* 初期状態なら */
			if (p->cwork == 0)
				p->m_work = MOVETO_R;	/* 移動方向 */
			else
				p->m_work = MOVETO_L;	/* 移動方向 */
			p->m_work2 = 0;
		}
		if (p->m_work2 == 0) {
		    /* 初回なら */
			signed short dx, dy;
			signed short dx_table[3] =
			{0, -64, 64};
			signed short dy_table[6] =
			{24, -16, 4, 24, -8, 8};
			short m_work_table[3] =
			{MOVETO_INIT, MOVETO_R, MOVETO_L};

			p->m_work2 = !0;
		    /* 順に左→右→左へ移動 */
			dx = SHORT_LX + dx_table[p->m_work];
			dy = SHORT_LY + dy_table[p->m_work];
			p->m_work = m_work_table[p->m_work];

			if (p->m_work3++ < 4) {
				SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
			} else {
				SubEnemyMoveToInit (p, p->x, 256 + 64, SPEED_2, SPEED_2N);
				p->seq++;	/* 撤退 */
			}
		} else {
		    /* 設定値に従って移動 */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* 移動方向再設定 */
		}

	    /* 次に攻撃処理 */
#define TIMER_SHOT_START	30
#define TIMER_SHOT_END	(TIMER_SHOT_START + 15 * 6)

		p->s_work++;

		if ((p->s_work >= TIMER_SHOT_START - 11) && (p->s_work < TIMER_SHOT_START))
			p->pt++;
		if ((p->s_work >= TIMER_SHOT_END) && (p->s_work < TIMER_SHOT_END + 11))
			p->pt--;

		switch (p->s_work) {
		case TIMER_SHOT_START - 1:
			p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
			break;
		case TIMER_SHOT_START + 15 * 0:
		case TIMER_SHOT_START + 15 * 2:
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle - 24, 0);
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle - 8, 0);
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle + 8, 0);
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle + 24, 0);
			SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
			break;
		case TIMER_SHOT_START + 15 * 1:
		case TIMER_SHOT_START + 15 * 3:
		case TIMER_SHOT_START + 15 * 5:
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle - 32, 0);
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle - 16, 0);
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle, 0);
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle + 16, 0);
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, 7, p->s_angle + 32, 0);
			SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
			break;
		case TIMER_SHOT_END + 55:
			p->s_work = 0;
			break;
		default:
			break;
		}
		break;

	case 2:
	    /* 設定値に従って移動 */
		if ((SubEnemyMoveTo (p) < 0) || (SHORT_LY > 256 + 32))
			return (-1);	/* 消去 */
		if (p->pt > obj_coverf)
		p->pt--;

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
			p->info = PALET_COVERF | PRIORITY_BOSS;
		}
	}

	xobj_set_st (p);

    /* コアの表示 */
	if (p->y + 64 > player->y)
		p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
	else
		p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;

	p->core_x = p->x - 8;	/* コアの中心は (-8,-8) ドットずれる */
	p->core_y = p->y - 8;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniCoverF (ENEMY * p)
{
}
