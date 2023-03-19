/* 回転４枚羽 */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"
#include "../entry.h"

#define PALET_MAIN	0x0800

#define HP_0	180
#define SHOT_SPEED		7

static short EnemyMoveRound4 (ENEMY *);
static void EnemyTiniRound4 (ENEMY *);

enum {
	APPEAR_LEFT = 0, APPEAR_RIGHT
};

#define ENTRY_CONTROL_ADD	10	/* entry_control に足す値 */


void EnemyInitRound4 (ENEMY * p)
{
	if (SHORT_LX < 144) {
		p->cwork = APPEAR_LEFT;
		p->vx = 5 * 65536;
	} else {
		p->cwork = APPEAR_RIGHT;
		p->vx = -5 * 65536;
	}
	p->vy = -(3 << 16);
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_round4 + 25;
	p->seq = 0;
	p->work2 = 0;
	p->m_work3 = 0;		/* 回転角（8+8bit 固定小数点） */
	p->m_work4 = 2 << 8;	/* 回転角の速度 */

	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
	p->func_enemy_move = EnemyMoveRound4;
	p->func_enemy_tini = EnemyTiniRound4;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveRound4 (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
	    /* 移動処理 */
		if (p->cwork == APPEAR_LEFT)
			if (p->vx > 0)
				p->vx -= 4096;
			else
				p->vx = 0;
		else if (p->vx < 0)
			p->vx += 4096;
		else
			p->vx = 0;
		if (SHORT_VY < 0)
			p->vy += 4096;

	    /* 回転処理 */
		p->m_work3 += p->m_work4;
		if (p->m_work4 > 64)
			p->m_work4 -= 4;
		p->pt = obj_round4 + 25 + *(char *) (&p->m_work3);
		if (p->m_work3 > (8 << 8)) {
			p->m_work3 -= (8 << 8);
			p->pt = obj_round4 + 25;
			if (p->m_work4 < 192) {
			    /* 回転数が落ちてほとんど停止したら次の段階へ移行 */
				p->seq++;
				p->vx = 0;
				p->pt = obj_round4;
			}
		}
		break;

	case 1:		/* 羽を開く */
		p->pt++;
		if (p->pt == obj_round4 + 13)
			SoundSetSE (SE_LOCK);	/* 「がちゃん」という効果音を鳴らす */

		if (p->pt > obj_round4 + 23) {
			p->seq++;
			p->s_work = p->s_work2 = p->s_work3 = 0;
			p->s_angle = 64;
			p->m_work3 = 0;	/* 回転角（8+8bit固定小数点） */
			p->m_work4 = 16;	/* 回転角加速度 */
			if (p->x > player->x)	/* この時点で自機をサーチ */
				p->cwork2 = !0;
			else
				p->cwork2 = 0;
		}
		break;

	case 2:		/* 開いた状態で回転 */
	    /* 移動処理 */
		if (p->vy < 65536)
			p->vy += 4096;

	    /* 回転処理 */
		p->m_work3 += p->m_work4;
		if (p->m_work4 < 256)
			p->m_work4 += 4;
		else
			p->m_work4 = 256;
		if (p->m_work3 > (8 << 8))
			p->m_work3 -= (8 << 8);
		p->pt = obj_round4 + 25 + 9 + *(char *) (&p->m_work3);

	    /* 攻撃処理 */
		switch (p->s_work) {
		case 0:
			if (p->s_work2++ > 20) {
				p->s_work++;
				p->s_work2 = 0;
			}
			break;

		case 1:
#define INTERVAL2_1	7
#define SPEED_SHOT2	8
			if (p->s_work2++ > INTERVAL2_1) {
				char a = p->s_angle;
				p->s_work2 = 0;
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, a+64*0, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, a+64*1, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, a+64*2, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, a+64*3, 2);
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				p->s_angle+=12;
				if (p->s_work3++ == 8)
					p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
				if (p->s_work3 > 10)
					p->seq++;
			}
			break;
		}
		break;

	case 3:		/* 上昇して逃げる */
	    /* 移動処理 */
		if (p->vy > -65536)
			p->vy -= 4096;

	    /* 回転処理 */
		p->m_work3 += p->m_work4;
		if (p->m_work4 < 256)
			p->m_work4 += 4;
		else
			p->m_work4 = 256;
		if (p->m_work3 > (8 << 8))
			p->m_work3 -= (8 << 8);
		p->pt = obj_round4 + 25 + 9 + *(char *) (&p->m_work3);
		break;
	}

    /* 画面外に出たら消去 */
	if (SHORT_LY < -48)
		return (-1);	/* 消去 */


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
				EffectInit (EFFECT_POINTS_RED, POINTS_1200, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_700, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);

			return (-1);	/* 消去 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 80)) {
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
static void EnemyTiniRound4 (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
