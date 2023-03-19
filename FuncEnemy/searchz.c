/* 自機をサーチするザコ */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"

#define PALET_MAIN	0x0800
#define PALET_MAIN1	0x0600

#define wabs(x) ((x) >= 0 ? (short) (x) : (short) -(x))
#define abs(x)  ((x) >= 0 ? (int) (x) : (int) -(x))


static short points_disp;	/* 得点を表示するか */
static short points_disp1;

static short EnemyMoveSearchZ (ENEMY *);
static void EnemyTiniSearchZ (ENEMY *);


/* ゲーム開始時に１回だけ呼ばれる */
void EnemyInitStaticSearchZ (void)
{
	points_disp = 0;
	points_disp1 = 0;
}


void EnemyInitSearchZ (ENEMY * p)
{
	signed short x = SHORT_LX;
    /* このザコに関してはＸ座標は中心からの距離で */
    /* 常にプレイヤーの反対側に現れる */
	if (player->x < 144) {
		p->lx = (player->x + x) * 65536;
		p->vx = 16384;
	} else {
		p->lx = (player->x - x) * 65536;
		p->vx = -16384;
	}
	p->vy = 4 * 65536;

	p->hit_px = 8;
	p->hit_py = 12;
	p->hit_sx = 12;
	p->hit_sy = 12;
	p->hit_cx = 4;
	p->damage = p->damage_core = 0;
	p->hp = 1;
	p->seq = 0;
	p->m_work = 0;
	p->m_work4 = 0;		/* １回接近したか */
	p->angle = 64;
	p->pt = obj_searchz + ((p->angle) >> 3);

	if (p->arg == 0)
		p->info = PALET_MAIN | PRIORITY_ZAKO;
	else
		p->info = PALET_MAIN1 | PRIORITY_ZAKO;

	p->core_pt = sp_core01 + 1;
	p->core_info = PALET_CORE_BLUE | PRIORITY_ZAKO;
	p->func_enemy_move = EnemyMoveSearchZ;
	p->func_enemy_tini = EnemyTiniSearchZ;
}



static short EnemyMoveSearchZ (ENEMY * p)
{
    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;

	switch (p->seq) {

	    /* まず突っ込んでくる */
	case 0:
		if (p->m_work++ > 16)
			p->seq++;
		break;

	    /* 減速 */
	case 1:
		p->vy -= 32768;
		if (p->vy <= 32768) {
			p->seq++;
			p->m_work2 = p->m_work2 = 0;
			p->core_info = PALET_CORE_RED | PRIORITY_ZAKO;	/* コア赤くなる */
#define SPEED_SHOT1	15
			if (p->arg) {
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT1, psearch (p->x, p->y), 0);
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
			}
		}
		break;

	    /* 直線運動しつつこちらを向く */
	case 2:
		if (p->m_work++ > 4) {
			p->m_work = 0;
			if (p->m_work2++ < 6) {
			    /* 自機の方を向く */
				if ((unsigned char) (p->angle - psearch (p->x, p->y)) > (unsigned char) 128)
					p->angle += 4;
				else
					p->angle -= 4;
				p->pt = obj_searchz + ((p->angle) >> 3);
			} else {
				if (wabs (player->x - SHORT_LX) > 32)
					p->seq = 3;	/* 自機から遠ければ近付く */
				else
					p->seq = 4;	/* 自機に近ければそのまま逃げる */
				p->m_work = p->m_work2 = 0;
			}
		}
		break;

	    /* 自機に近付く */
	case 3:
		if (p->m_work++ > 4) {
			p->m_work = 0;
		    /* 自機の方を向く */
			if ((unsigned char) (p->angle - psearch (p->x, p->y)) > (unsigned char) 128)
				p->angle += 2;
			else
				p->angle -= 2;
			p->pt = obj_searchz + ((p->angle) >> 3);
		}
		p->m_work2++;
		if ((p->m_work2 < 5) && ((abs (p->vx) < 1.5 * 65536) && (abs (p->vy) < 1.5 * 65536))) {
			p->vx += vxtable[0][p->angle];
			p->vy += vytable[0][p->angle];
		}
		if ((p->m_work2 > 40) && (p->m_work4 < 1)) {
			p->seq = 4;
			p->m_work = p->m_work2 = 0;
			p->m_work4++;
		}
		break;

	    /* 自機から遠ざかる */
	case 4:
		if (p->m_work++ > 4) {
			p->m_work = 0;
		    /* 自機の方を向く */
			if ((unsigned char) (p->angle - psearch (p->x, p->y)) > (unsigned char) 128)
				p->angle += 2;
			else
				p->angle -= 2;
			p->pt = obj_searchz + ((p->angle) >> 3);
		}
		p->m_work2++;
		if ((p->m_work2 < 3) && ((abs (p->vx) < 1.5 * 65536) && (abs (p->vy) < 1.5 * 65536))) {
			p->vx -= vxtable[0][p->angle];
			p->vy -= vytable[0][p->angle];
		}
		if ((p->m_work2 > 40) && (p->m_work4 < 2)) {
			p->seq = 3;
			p->m_work = p->m_work2 = 0;
			p->m_work4++;
		}
		break;

	default:
		break;
	}
    /* 画面外に出たら消去 */
	if ((p->seq > 0)
	    && ((SHORT_LY < -32) || (SHORT_LY > 256 + 32)
		|| (SHORT_LX < -32) || (SHORT_LX > 256 + 32)))
		return (-1);	/* 消去 */


    /* 自機ショットに当たった時の処理 */
    /* ザコは一撃で死ぬのでコア云々は考慮しなくて良い */
	if (p->damage) {
		EffectInit (EFFECT_EXPLZAKO, 0, p->x, p->y);
		SoundSetSE (SE_EXPL);
		if (p->arg == 0) {
			if (points_disp < ZAKO_SCORE_DISP) {
				points_disp++;
				if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
					EffectInit (EFFECT_POINTS_RED, POINTS_200, p->x, p->y);
				else
					EffectInit (EFFECT_POINTS_BLUE, POINTS_100, p->x, p->y);
			} else {
				if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
					EffectInit (EFFECT_POINTS, POINTS_200, p->x, p->y);
				else
					EffectInit (EFFECT_POINTS, POINTS_100, p->x, p->y);
			}
		} else {
			if (points_disp1 < ZAKO_SCORE_DISP) {
				points_disp1++;
				if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
					EffectInit (EFFECT_POINTS_RED, POINTS_200, p->x, p->y);
				else
					EffectInit (EFFECT_POINTS_BLUE, POINTS_100, p->x, p->y);
			} else {
				if (p->core_info == (PALET_CORE_RED | PRIORITY_ZAKO))
					EffectInit (EFFECT_POINTS, POINTS_200, p->x, p->y);
				else
					EffectInit (EFFECT_POINTS, POINTS_100, p->x, p->y);
			}
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



static void EnemyTiniSearchZ (ENEMY * p)
{
}
