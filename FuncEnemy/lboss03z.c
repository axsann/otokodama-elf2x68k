/* ３面ボスザコ (03z = 03 zako) */
/* 一度に４基出現する小さいボス */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../entry.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"


#define SPEED_LASER		30

#define HP_0	300		/* 耐久力 */

#define SPEED_1		28	/* 横移動の加速度 */
#define SPEED_1N	20	/* 横移動の加速度を足す回数 */
#define SPEED_2		12	/* 撤退時の加速度 */
#define SPEED_2N	24	/* 撤退時の加速度を足す回数 */
#define SPEED_3		16	/* 爆発時の加速度 */
#define SPEED_3N	32	/* 爆発時の加速度を足す回数 */

#define SPEED_SHOT1	12	/* １段階目扇型ショット速度 */
#define SPEED_SHOT2	10	/* ２段階目伸びるショットの一番遅い弾の速度 */

#define ENTRY_CONTROL_ADD	10	/* entry_control に足す値 */

#define SEQ_DEATH	3

enum {
	MOVETO_L = 0,
	MOVETO_R,
};

static short EnemyMoveLBoss03Z (ENEMY *);
static void EnemyTiniLBoss03Z (ENEMY *);


void EnemyInitLBoss03Z (ENEMY * p)
{
	p->vx = 0;
	p->vy = 4 << 16;

	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->seq = 0;
	p->pt = obj_lboss03 + 8;
	p->angle = 0;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	p->m_work = 0;
	p->s_work = 0;
	p->func_enemy_move = EnemyMoveLBoss03Z;
	p->func_enemy_tini = EnemyTiniLBoss03Z;

	if (SHORT_LX < 128 + 16)
		p->cwork = 0;
	else
		p->cwork = !0;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveLBoss03Z (ENEMY * p)
{
	signed short offset1[32][2] =	/* 銀玉の座標オフセット */
	{
		{-16, 16},
		{-18, 12},
		{-20, 8},
		{-22, 4},
		{-22, 0},
		{-22, -4},
		{-20, -8},
		{-18, -12},
		{-16, -16},
		{-12, -18},
		{-8, -20},
		{-4, -22},
		{0, -22},
		{4, -22},
		{8, -20},
		{12, -18},
		{16, -16},
		{18, -12},
		{20, -8},
		{22, -4},
		{22, 0},
		{22, 4},
		{20, 8},
		{18, 12},
		{16, 16},
		{12, 18},
		{8, 20},
		{4, 22},
		{0, 22},
		{-4, 22},
		{-8, 20},
		{-12, 18},
	};

    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->pt++;
	p->angle++;


	switch (p->seq) {
	    /* 登場直後、減速する */
	case 0:
		p->vy -= 4096;
		if (p->vy < 0) {
			p->seq++;
			p->vy = 0;
			if (p->cwork == 0) {
				SubEnemyMoveToInit (p, SHORT_LX + 128, SHORT_LY - 16, SPEED_1, SPEED_1N);
				p->m_work = MOVETO_L;	/* 次の移動方向 */
			} else {
				SubEnemyMoveToInit (p, SHORT_LX - 128, SHORT_LY - 16, SPEED_1, SPEED_1N);
				p->m_work = MOVETO_R;
			}
			p->m_work2 = p->m_work3 = p->s_work = 0;
		}
		break;

	    /* 上空で移動＆攻撃 */
	case 1:
		if (SubEnemyMoveTo (p) < 0) {
			short m_work_table[2] =
			{MOVETO_R, MOVETO_L};
			signed short dx_table[8] =
			{48, -32, 48, -48, 32, -48, 0, 0};
			signed short dy_table[8] =
			{-16, 8, 16, -24, 32, -80, 0, 0};

			if (p->m_work3++ > 24) {
				p->seq++;	/* 撤退 */
				SubEnemyMoveToInit (p, SHORT_LX, 256 + 128, SPEED_2, SPEED_2N);
			} else {
				p->m_work2++;
				switch (p->m_work2) {
				case 6:	/* 自機に突っ込んでくる */
					p->work = SHORT_LX;	/* 現在の座標を保存 */
					p->work2 = SHORT_LY + 64;
					SubEnemyMoveToInit (p, player->x, player->y, SPEED_1, SPEED_1N);
					break;
				case 7:	/* 自機に突っ込んだ後戻る */
					SubEnemyMoveToInit (p, p->work, p->work2, SPEED_1, SPEED_1N);
					break;
				case 8:
					p->m_work2 = 0;
				    /* ここに break がない事に注意 */
				default:
					if (p->cwork == 0) {
						SubEnemyMoveToInit (p, SHORT_LX + dx_table[p->m_work2], SHORT_LY + dy_table[p->m_work2], SPEED_1, SPEED_1N);
					} else {
						SubEnemyMoveToInit (p, SHORT_LX - dx_table[p->m_work2], SHORT_LY + dy_table[p->m_work2], SPEED_1, SPEED_1N);
					}
					p->m_work = m_work_table[p->m_work];	/* 次の移動方向 */
					break;
				}
			}
		}
	    /* 次に攻撃処理 */
		if (p->m_work2 < 6) {
			p->s_work++;
			switch (p->s_work) {
#define	INTERVAL	3
			case INTERVAL * 1:
			case INTERVAL * 2:
			case INTERVAL * 3:
			case INTERVAL * 4:
			case INTERVAL * 5:
				EnemyInit (ENEMY_LBOSS03G, SHORT_LX, SHORT_LY, 0, p);	/* 銀玉を発射 */
				break;
			case INTERVAL * 30:
				p->s_work = 0;
				break;
			default:
				break;
			}
			if (p->s_work < INTERVAL * 6) {
				p->pt++;
				p->angle++;
			}
		}
		break;

	case 2:
	default:
		if (SubEnemyMoveTo (p) < 0)
			SubEnemyMoveToInit (p, SHORT_LX, 256 + 128, SPEED_2, SPEED_2N);
	    /* 画面外に出たら消去 */
		if (SHORT_LY > 256 + 64)
			return (-1);	/* 消去 */
		break;

	case SEQ_DEATH:	/* 死んだ時の処理 */
		if (SHORT_VY < 4)
			p->vy += 16384;
		if (p->m_work++ > 16) {
			p->m_work = 0;
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL);
		}
	    /* 画面外に出たら消去 */
		if (SHORT_LY > 256 + 64)
			return (-1);	/* 消去 */
		break;
	}


    /* 自機ショットに当たった時の処理 */
	if ((p->seq != SEQ_DEATH) && (p->damage)) {
		p->info = PALET_RED | PRIORITY_BOSS_PARTS_L;
		p->damage_core = 0;
		if ((p->hp -= p->damage) <= 0) {
		    /* 死んだ */
			p->seq = SEQ_DEATH;
			p->m_work = 0;
			p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
			if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS))
				EffectInit (EFFECT_POINTS_RED, POINTS_5000, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_3000, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if (((p->flash++ > TIMER_FLASH_RED) && (p->hp < 100)) && (p->seq != SEQ_DEATH)) {
			p->info = PALET_RED | PRIORITY_BOSS_PARTS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			if (p->arg == 0)
				p->info = PALET_LBOSS03 | PRIORITY_BOSS_PARTS_L;
			else
				p->info = PALET_LBOSS03_1 | PRIORITY_BOSS_PARTS_L;
		}
	}
	if (p->pt > obj_lboss03 + 8 + 7)
		p->pt -= 8;
	xobj_set_st (p);


    /* コアの表示 */
	if (p->seq != SEQ_DEATH) {	/* 死んだらコア状態は変化させない */
		if (player->y < p->y + 96)	/* 自機との座標の差が小さければ赤 */
			p->core_info = PALET_CORE_RED | PRIORITY_BOSS;
		else
			p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS;
	}
	p->core_x = p->x - 8;	/* コアの中心は (-8,-8) ドットずれる */
	p->core_y = p->y;
	xsp_set_st (&(p->core_x));

    /* 周囲の銀球の表示 */
	{
		unsigned char a, a2;
		a = (p->angle &= 31);

		if (p->hp > HP_0 / 5 * 4) {
			xsp_set (p->x + offset1[a][0] - 8, p->y + offset1[a][1], sp_lboss03g, PALET_LBOSS03 | PRIORITY_BOSS_PARTS_L);
		}
		if (p->hp > HP_0 / 5 * 3) {
			a2 = a + 8;
			a2 &= 31;
			xsp_set (p->x + offset1[a2][0] - 8, p->y + offset1[a2][1], sp_lboss03g, PALET_LBOSS03 | PRIORITY_BOSS_PARTS_L);
		}
		if (p->hp > HP_0 / 5 * 2) {
			a2 = a + 16;
			a2 &= 31;
			xsp_set (p->x + offset1[a2][0] - 8, p->y + offset1[a2][1], sp_lboss03g, PALET_LBOSS03 | PRIORITY_BOSS_PARTS_L);
		}
		if (p->hp > HP_0 / 5 * 1) {
			a2 = a + 24;
			a2 &= 31;
			xsp_set (p->x + offset1[a2][0] - 8, p->y + offset1[a2][1], sp_lboss03g, PALET_LBOSS03 | PRIORITY_BOSS_PARTS_L);
		}
	}

	return (0);
}



static void EnemyTiniLBoss03Z (ENEMY * p)
{
	entry_control -= ENTRY_CONTROL_ADD;
}
