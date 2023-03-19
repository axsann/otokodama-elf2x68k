/* ザコ４ */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"

#define PALET_MAIN	0x0600
#define wabs(x) ((x) >= 0 ? (short) (x) : (short) -(x))
#define abs(x)  ((x) >= 0 ? (int) (x) : (int) -(x))


static short points_disp;	/* 得点を表示するか */

static short EnemyMoveZako04 (ENEMY *);
static void EnemyTiniZako04 (ENEMY *);


/* ゲーム開始時に１回だけ呼ばれる */
void EnemyInitStaticZako04 (void)
{
	points_disp = 0;
}


void EnemyInitZako04 (ENEMY * p)
{
	p->vx = 16384;
	p->vy = 4 * 65536;
	p->hit_px = 8;
	p->hit_py = 12;
	p->hit_sx = 12;
	p->hit_sy = 12;
	p->hit_cx = 4;
	p->damage = p->damage_core = 0;
	p->hp = 1;
	p->seq = 0;
	p->m_work2 = 0;
	p->work2 = 0;
	p->work4 = 0;		/* １回接近したか */
	p->angle = 64;
	p->pt = obj_zako04 + ((p->angle) >> 3);
	p->info = PALET_MAIN | PRIORITY_ZAKO;
	p->core_pt = sp_core01 + 1;
	p->core_info = PALET_CORE_BLUE | PRIORITY_ZAKO;
	p->func_enemy_move = EnemyMoveZako04;
	p->func_enemy_tini = EnemyTiniZako04;
}



static short EnemyMoveZako04 (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {
	case 0:		/* まず突っ込んでくる */
	    /* ある程度進んだか自機に近付いたら減速 */
		if ((p->m_work2++ > 45) || (p->y > player->y - 32))
			p->seq++;
		break;
	case 1:		/* 減速 */
		p->vy -= 16384;
		if (SHORT_VY < 1) {
			p->seq++;
			p->work2 = 0;
			p->s_work2 = 0;
			p->core_info = PALET_CORE_RED | PRIORITY_ZAKO;	/* コア赤くなる */
		}
		break;

	    /* 直線運動しつつこちらを向く */
	case 2:
	    /* 移動処理 */
		p->vy -= 512;
		if (SHORT_VY < 0)
			p->seq++;

		if (p->work2++ > 4) {
			p->work2 = 0;
		    /* 自機の方を向く */
			if ((unsigned char) (p->angle - psearch (p->x, p->y)) > (unsigned char) 128)
				p->angle += 4;
			else
				p->angle -= 4;

			p->pt = obj_zako04 + ((p->angle) >> 3);
		}
	    /* 攻撃処理 */
#define INTERVAL2	30
#define SPEED_SHOT	15
		if (p->s_work2++ > INTERVAL2) {
			p->s_work2 = 0;
			EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT, p->angle, 0);
			SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
		}
		break;

	    /* 撤退 */
	case 3:
		if (SHORT_VY > -3)
			p->vy -= 4096;

		if ((p->angle > 192 - 4) && (p->angle < 192 + 4)) {
			p->angle = 192;
		} else {
		    /* 上を向く */
			if ((unsigned char) (p->angle - 192) > (unsigned char) 128)
				p->angle += 4;
			else
				p->angle -= 4;
		}
		p->pt = obj_zako04 + ((p->angle) >> 3);
	    /* 画面外に出たら消去 */
		if (p->y < -32)
			return (-1);	/* 消去 */
		break;
	default:
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
				EffectInit (EFFECT_POINTS_RED, POINTS_300, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_200, p->x, p->y);
		} else {
			if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
				EffectInit (EFFECT_POINTS, POINTS_300, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS, POINTS_200, p->x, p->y);
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



static void EnemyTiniZako04 (ENEMY * p)
{
}
