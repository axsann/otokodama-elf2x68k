/* ４面でわらわらと登場するザコ */
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

#define PALET_MAIN	0x0600

#define SPEED_1		4	/* 速度 */
#define	SPEED_2		8	/* ２段階目の速度 */

#define ENTRY_CONTROL_ADD	1	/* entry_control に足す値 */


static short points_disp;	/* 得点を表示するか */

static short EnemyMoveZako032 (ENEMY *);
static void EnemyTiniZako032 (ENEMY *);


/* ゲーム開始時に１回だけ呼ばれる */
void EnemyInitStaticZako032 (void)
{
	points_disp = 0;
}



void EnemyInitZako032 (ENEMY * p)
{
	if (p->lx < 0) {
	    /* 負のＸ座標が指定された場合はプレイヤーの反対側の画面左右から出現 */
		if (player->x > 144) {
			p->lx = (-32) * 65536;
			p->angle = msearch (SHORT_LX, SHORT_LY, (256 + 16 + 32), SHORT_LY + 32);
		} else {
			p->lx = (256 + 16 + 32) * 65536;
			p->angle = msearch (SHORT_LX, SHORT_LY, -32, SHORT_LY + 32);
		}
	} else {
		if (player->x > 144)
			p->lx = 144 * 65536 - p->lx;
		else
			p->lx = 144 * 65536 + p->lx;
		p->angle = psearch (SHORT_LX, SHORT_LY);
	}
	p->vx = vxtable[SPEED_1][p->angle];
	p->vy = vytable[SPEED_1][p->angle];
	p->pt = obj_zako02;
	p->hit_px = 12;
	p->hit_py = 12;
	p->hit_sx = 12;
	p->hit_sy = 12;
	p->hit_cx = 8;
	p->damage = p->damage_core = 0;
	p->hp = 1;
	p->seq = 0;
	p->m_work = 0;
	p->s_work = p->s_work2 = p->s_work3 = 0;
	p->pt = obj_zako03 + 2;
	p->info = PALET_MAIN | PRIORITY_ZAKO;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_ZAKO;

	p->func_enemy_move = EnemyMoveZako032;
	p->func_enemy_tini = EnemyTiniZako032;

	entry_control += ENTRY_CONTROL_ADD;
}



static short EnemyMoveZako032 (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:
		if (p->m_work++ > 30) {
			p->seq++;
		}
		break;
	case 1:
		break;
	}

    /* 攻撃開始 */
	switch (p->s_work) {

	case 0:		/* 伸びる弾 */
#define INTERVAL1_0		24
#define SPEED_SHOT1_0	8
		if (p->s_work2++ > INTERVAL1_0) {
			p->s_work2 = 0;
			switch (p->s_work3++) {
			case 0:
			case 1:
				{
					char a = psearch (SHORT_LX, SHORT_LY);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_0, a, 0);
					EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT1_0 + 4, a, 0);
					SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				}
				break;
			case 2:
			default:
				p->core_info = PALET_CORE_RED | PRIORITY_ZAKO;	/* 赤くなる */
				p->s_work++;
				p->s_work3 = 0;
				p->vx = vxtable[SPEED_2][p->angle];
				p->vy = vytable[SPEED_2][p->angle];
				break;
			}
		}
		break;

	case 1:
	    /* 画面外に出たら消去 */
	    /* （画面左右から出た判定と上下から出た判定を１回ずつの比較で行っている） */
		if (((unsigned short) p->x > 256 + 32) || ((unsigned short) p->y > 256 + 32))
			return (-1);	/* 消去 */
		break;
	}



    /* 自機ショットに当たった時の処理 */
    /* ザコは一撃で死ぬのでコア云々は考慮しなくて良い */
	if (p->damage) {
		EffectInit (EFFECT_EXPLZAKO, 0, p->x, p->y);
		SoundSetSE (SE_EXPL);
		if (points_disp < ZAKO_SCORE_DISP) {
			points_disp++;
			if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
				EffectInit (EFFECT_POINTS_RED, POINTS_600, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_100, p->x, p->y);
		} else {
			if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
				EffectInit (EFFECT_POINTS, POINTS_600, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS, POINTS_100, p->x, p->y);
		}
		return (-1);	/* 消去 */
	}
	xobj_set_st (p);

    /* コアの表示 */
	p->core_x = p->x - 8;
	p->core_y = p->y - 8;
	xsp_set_st (&(p->core_x));

	return (0);
}



static void EnemyTiniZako032 (ENEMY * p)
{
#define SPEED_SHOT1_1	10
	char a = psearch (SHORT_LX, SHORT_LY);

	EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_1, a - 8, 0);
	EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1_1, a + 8, 0);

	entry_control -= ENTRY_CONTROL_ADD;
}
