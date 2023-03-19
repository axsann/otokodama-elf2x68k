#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../entry.h"

#ifdef DEBUG
#include <stdlib.h>
#include <stdio.h>
#include "../txfont.h"
#endif

#define PALET_MVLASER	0x0400
#define SPEED_LASER		28

#define HP_0		600
#define TIMER_1		1500	/* これ以上経ったら撤退 */

#define SPEED_1		3	/* 加速度 */
#define SPEED_1N	28	/* 加速度を足す回数 */

#define abs(x)  ((x) >= 0 ? (int) (x) : (int) -(x))
#define SEQ_2ND		2

#define ENTRY_CONTROL_ADD	100	/* entry_control に足す値 */

enum {
	MOVETO_INIT = 0,	/* 初期状態 */
	MOVETO_L2R,		/* 左から右に移動 */
	MOVETO_R2L
};

static short EnemyMoveMvLaser (ENEMY *);
static void EnemyTiniMvLaser (ENEMY *);


void EnemyInitMvLaser (ENEMY * p)
{
	if (p->lx < 0)
		p->vx = 4 * 65536;
	else
		p->vx = -4 * 65536;
	p->vy = -(2.5 * 65536);
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = p->damage_core = 0;
	p->timer = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_mvlaser;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveMvLaser;
	p->func_enemy_tini = EnemyTiniMvLaser;

	p->s_work = 0;
	p->s_work2 = 0;

    /* まず砲台を生成 */
	p->parts1 = EnemyInit (ENEMY_MVLASERH, -32, 0, 0, p);
	p->parts2 = EnemyInit (ENEMY_MVLASERH, 32, 0, 0, p);

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveMvLaser (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->timer++;

#ifdef DEBUG
	{
		char temp_str[64];
		sprintf (temp_str, "TIMER %05hd", p->timer);
		TxfontCursor (20, 3);
		TxfontPuts (temp_str);
		sprintf (temp_str, "   HP %05hd", p->hp);
		TxfontCursor (20, 4);
		TxfontPuts (temp_str);
	}
#endif

	switch (p->seq) {
	case 0:		/* 減速 */
		if (p->vx > 0)
			p->vx -= 2400;
		else
			p->vx += 2400;
		if (abs (p->vx) < 2400)
			p->vx = 0;

		if (p->vy < 0) {
			p->vy += 2048;
		} else {
			p->seq++;
			p->vx = 0;
			p->vy = 0;
			p->m_work = MOVETO_INIT;	/* 移動方向 */
		}
		break;

	    /* 第１形態 */
	case 1:		/* 左右に移動しつつ攻撃 */
	    /* まず移動処理 */
		if (p->m_work == MOVETO_INIT) {
		    /* 初期状態なら */
			if (p->x > 128 + 16)
				p->m_work = MOVETO_R2L;	/* 移動方向 */
			else
				p->m_work = MOVETO_L2R;	/* 移動方向 */
			p->m_work2 = 0;
		}
		switch (p->m_work2) {
		case 0:
			p->vx = p->vy = 0;	/* 念のため */
		    /* 左の砲台が健在なら */
			if ((p->parts1) && (p->parts1->seq == 0))
				p->parts1->seq = 1;	/* 発射指示 */
		    /* 右の砲台が健在なら */
			if ((p->parts2) && (p->parts2->seq == 0))
				p->parts2->seq = 1;	/* 発射指示 */
		    /* 砲台が両方ともなくなったら */
			if ((p->parts1 == 0) && (p->parts2 == 0)) {
				p->seq++;
				p->s_work = p->s_work2 = 0;
				p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
			}
			p->m_work2++;
			p->m_work3 = 0;
			break;
		case 1:
			if (p->m_work3++ > 20) {
			    /* 初回なら */
				signed short dx, dy;
				signed short dx_table[3] =
				{0, 54 + 16, 256 - 54 + 16};
				short m_work_table[3] =
				{MOVETO_INIT, MOVETO_R2L, MOVETO_L2R};

				p->m_work2++;
			    /* 順に左→中央→右→中央→左へ移動 */
				dx = dx_table[p->m_work];
				p->m_work = m_work_table[p->m_work];

			    /* player->y は乱数の種として使用しています */
				if (SHORT_LY < 80)
					dy = (((unsigned short) rndtable[(player->y) & 0xff]) / 4 + 32 + 16);
				else
					dy = 64;
			    /* 移動方向を設定 */
				SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
				p->s_work = 0;
				p->s_work2 = 0;
			}
			break;
		case 2:
		    /* 設定値に従って移動 */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* 移動方向再設定 */
			break;
		}

	    /* 次に攻撃処理 */
		if (p->m_work2 == 2) {
			switch (p->s_work++) {
			case 0:
				if (p->s_work2 == 0)
					p->s_angle = 64 + 8 * 3;
				else
					p->s_angle = 64 - 8 * 3;
				p->s_work3 = 7;	/* 弾の速度 */
				break;
#define	INTERVAL1_2	7
			case 10 + INTERVAL1_2 * 0:
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
			case 10 + INTERVAL1_2 * 1:
			case 10 + INTERVAL1_2 * 2:
			case 10 + INTERVAL1_2 * 3:
			case 10 + INTERVAL1_2 * 4:
			case 10 + INTERVAL1_2 * 5:
			case 10 + INTERVAL1_2 * 6:
			case 10 + INTERVAL1_2 * 7:
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, p->s_work3, p->s_angle, 4);
				if (p->s_work2 == 0)
					p->s_angle -= 8;
				else
					p->s_angle += 8;
				p->s_work3++;
				break;
			case 60:
				p->s_work = 0;
				if (p->s_work2 == 0)
					p->s_work2 = 1;
				else
					p->s_work2 = 0;
				break;
			default:
				break;
			}
		}
		break;

	    /* 第２形態（砲台が破壊された後） */
	case SEQ_2ND:
		if (p->vx > 4096)
			p->vx -= 4096;
		else if (p->vx < -4096)
			p->vx += 4096;
		if (SHORT_VY < 1)
			p->vy += 2048;
		if (p->s_work2++ > 2) {
			p->s_work2 = 0;
			EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY, 9, (rndtable[p->s_angle++] & 127), 4);
			EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY, 9, (rndtable[p->s_angle++] & 127), 4);
			SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
		}
	    /* 画面外に出たら消去 */
		if (SHORT_LY > 256 + 48 + 16)
			return (-1);	/* 消去 */
		break;
	}
	if ((p->seq < SEQ_2ND) && (p->timer > TIMER_1))
		p->seq = SEQ_2ND;

	if ((p->parts1) && (p->parts1->child_death))
		p->parts1 = 0;	/* これ以降参照しない */
	if ((p->parts2) && (p->parts2->child_death))
		p->parts2 = 0;	/* これ以降参照しない */




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
				EffectInit (EFFECT_POINTS_RED, POINTS_8000, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_2000, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* 弾消し */
			return (-1);	/* 消去 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 200)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_MVLASER | PRIORITY_BOSS;
		}
	}
	xobj_set_st (p);

    /* コアの表示 */
	p->core_x = p->x - 8;	/* コアの中心は (-8,-8) ドットずれる */
	p->core_y = p->y - 8;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniMvLaser (ENEMY * p)
{
    /* パーツが存在していたら消すように指示 */
	if (p->parts1)
		p->parts1->child_kill = !0;
	if (p->parts2)
		p->parts2->child_kill = !0;
	entry_control -= ENTRY_CONTROL_ADD;
}
