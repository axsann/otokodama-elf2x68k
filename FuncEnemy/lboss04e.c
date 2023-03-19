/* ４面ボス左右耳 (04e = 04 ear) */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"


#define HP_0	200		/* 耐久力 */

enum {
	EAR_LEFT = 0, EAR_RIGHT
};

static short EnemyMoveLBoss04E (ENEMY *);
static void EnemyTiniLBoss04E (ENEMY *);


void EnemyInitLBoss04E (ENEMY * p)
{
	p->hit_px = 32;
	p->hit_py = 16;
	p->hit_sx = 32;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->seq = 0;
	p->m_work = 0;
	p->s_work = p->s_work2 = p->s_work3 = 0;
	p->func_enemy_move = EnemyMoveLBoss04E;
	p->func_enemy_tini = EnemyTiniLBoss04E;

	if (p->lx < 0) {
		p->cwork = EAR_LEFT;	/* 左か右か */
		p->pt = obj_lboss04 + 5;
	} else {
		p->cwork = EAR_RIGHT;
		p->pt = obj_lboss04 + 6;
	}
}



static short EnemyMoveLBoss04E (ENEMY * p)
{
	signed short offset[24][2] =	/* 耳パーツの座標オフセット */
	{
		{-16, -16},
		{-17, -16},
		{-19, -16},
		{-21, -16},
		{-23, -16},
		{-26, -16},
		{-29, -16},
		{-32, -16},
		{-35, -16},
		{-38, -16},
		{-41, -16},
		{-44, -16},
		{-47, -16},
		{-50, -16},
		{-46, -16},
		{-46, -16},
		{-50, -16},
		{-47, -16},
		{-46, -16},
		{-49, -16},
		{-48, -16},
		{-46, -16},
		{-48, -16},
		{-48, -16},
	};

	if ((p->child_kill) || (p->child_death))
		return (-1);	/* 消去 */

	switch (p->seq) {
	case 0:
		p->m_work++;
		if (p->m_work < 24) {
			if (p->cwork == EAR_LEFT)
				p->x = p->parent->x + offset[p->m_work][0];
			else
				p->x = p->parent->x - offset[p->m_work][0];
			p->y = p->parent->y + offset[p->m_work][1];
		} else {
			p->seq++;
		}
		if (p->m_work == 13)
			SoundSetSE (SE_LOCK_M);	/* 「がちゃん」という効果音を鳴らす */
		break;

	case 1:		/* 耳が出た */
		if (p->cwork == EAR_LEFT)
			p->x = p->parent->x - 48;
		else
			p->x = p->parent->x + 48;
		p->y = p->parent->y - 16;

	    /* 残り復活回数により攻撃方法を変える */
		switch (p->arg) {

		    /* なぎ倒し弾 */
		case 12:
			p->s_work2++;
#define INTERVAL12		4
#define SPEED_SHOT12	20
			switch (p->s_work2) {
			case INTERVAL12 * 1:
				if (p->cwork == EAR_LEFT)
					p->angle = 64 + 48;
				else
					p->angle = 64 - 48;
			case INTERVAL12 * 2:
			case INTERVAL12 * 3:
			case INTERVAL12 * 4:
			case INTERVAL12 * 5:
			case INTERVAL12 * 6:
			case INTERVAL12 * 7:
			case INTERVAL12 * 8:
			case INTERVAL12 * 9:
			case INTERVAL12 * 10:
			case INTERVAL12 * 11:
			case INTERVAL12 * 12:
				EshotInit (ESHOT_NRG24N, p->x, p->y, SPEED_SHOT12, p->angle, 3);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				if (p->cwork == EAR_LEFT)
					p->angle -= 8;
				else
					p->angle += 8;
				break;
			case INTERVAL12 * 14:
			case INTERVAL12 * 15:
			case INTERVAL12 * 16:
			case INTERVAL12 * 17:
			case INTERVAL12 * 18:
			case INTERVAL12 * 19:
			case INTERVAL12 * 20:
				EshotInit (ESHOT_NRG24, p->x, p->y, SPEED_SHOT12, p->angle, 3);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				if (p->cwork == EAR_LEFT)
					p->angle += 12;
				else
					p->angle -= 12;
				break;

			case INTERVAL12 * 24:
				p->s_work2 = 0;
				break;
			default:
				break;
			}
			break;


		    /* Ｖ字型 */
		case 11:
			p->s_work2++;
#define INTERVAL11		4
#define SPEED_SHOT11	25
			switch (p->s_work2) {
			case 20 + INTERVAL11 * 0:
				p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
			case 20 + INTERVAL11 * 1:
			case 20 + INTERVAL11 * 2:
			case 20 + INTERVAL11 * 3:
			case 20 + INTERVAL11 * 4:
			case 20 + INTERVAL11 * 5:
			case 20 + INTERVAL11 * 6:
			case 20 + INTERVAL11 * 7:
			case 20 + INTERVAL11 * 8:
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT11, p->s_angle - 12, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT11, p->s_angle + 12, 2);
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				break;
			case 20 + INTERVAL11 * 10:
				p->s_work2 = 0;
				break;
			default:
				break;
			}
			break;


		    /* 炸裂弾連発 */
		case 10:
			p->s_work2++;
#define INTERVAL10	20
#define SPEED_SHOT10	23
			switch (p->s_work2) {
			case INTERVAL10 * 0:
			case INTERVAL10 * 1:
			case INTERVAL10 * 2:
			case INTERVAL10 * 3:
			case INTERVAL10 * 4:
				{
					unsigned char a = 0;
					short h;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT10, a += 16, 2);
				}
				SoundSetSE (SE_EBOMB_M);
				break;
			case INTERVAL10 * 0 + 1:
			case INTERVAL10 * 1 + 1:
			case INTERVAL10 * 2 + 1:
			case INTERVAL10 * 3 + 1:
			case INTERVAL10 * 4 + 1:
				{
					unsigned char a = 8;
					short h;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT10, a += 16, 3);
				}
				break;
			case INTERVAL10 * 7:
				p->s_work2 = 0;
				break;
			default:
				break;
			}
			break;


		    /* ６方向弾 */
		case 9:
			p->s_work2++;
#define INTERVAL9	6
#define SPEED_SHOT9	18
			switch (p->s_work2) {
			case INTERVAL9 * 0:
				p->angle = 12;
			case INTERVAL9 * 1:
			case INTERVAL9 * 2:
			case INTERVAL9 * 3:
			case INTERVAL9 * 4:
			case INTERVAL9 * 5:
			case INTERVAL9 * 6:
			case INTERVAL9 * 7:
			case INTERVAL9 * 8:
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 0, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 1, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 2, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 3, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 4, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 5, 3);
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				break;

			case INTERVAL9 * 9:
				{
					unsigned char a = 8;
					short h;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT9, a += 16, 3);
					SoundSetSE (SE_EBOMB_M);
				}
				break;

			case INTERVAL9 * 12:
				p->angle += 43;
			case INTERVAL9 * 13:
			case INTERVAL9 * 14:
			case INTERVAL9 * 15:
			case INTERVAL9 * 16:
			case INTERVAL9 * 17:
			case INTERVAL9 * 18:
			case INTERVAL9 * 19:
			case INTERVAL9 * 20:
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 0, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 1, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 2, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 3, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 4, 3);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT9, p->angle + 43 * 5, 3);
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				break;
			case INTERVAL9 * 21:
				{
					unsigned char a = 8;
					short h;
					for (h = 0; h < 16; h++)
						EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT9, a += 16, 3);
					SoundSetSE (SE_EBOMB_M);
				}
				break;

			case INTERVAL9 * 25:
				p->s_work2 = 0;
				break;
			default:
				break;
			}
			break;


		    /* 扇状ばらまき弾 */
		case 8:
			p->s_work2++;
#define INTERVAL8	6
#define SPEED_SHOT8	15
			if (p->s_work2 > INTERVAL8) {
				short type;
				short type_table[15] =
				{ESHOT_NRG02,
				 ESHOT_NRG02, ESHOT_NRG02, ESHOT_NRG02, ESHOT_NRG02,
				 ESHOT_NRG03, ESHOT_NRG03, ESHOT_NRG03, ESHOT_NRG04,
				 ESHOT_NRG04, ESHOT_NRG04, ESHOT_NRG05, ESHOT_NRG05,
				 ESHOT_NRG06, ESHOT_NRG34N};

				p->s_work2 = 0;
				p->s_work3++;
				if (p->s_work3 == 1) {
					p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
					p->s_angle2 = p->x;	/* 乱数の種として使用 */
					p->s_work4 = SPEED_SHOT8;
				}
				type = type_table[p->s_work3];

				if (p->s_work3 < 15) {
					EshotInit (type, p->x, p->y, p->s_work4, p->s_angle + ((rndtable[p->s_angle2++]) & 63) - 32, 3);
					EshotInit (type, p->x, p->y, p->s_work4, p->s_angle + ((rndtable[p->s_angle2++]) & 63) - 32, 3);
					EshotInit (type, p->x, p->y, p->s_work4++, p->s_angle + ((rndtable[p->s_angle2++]) & 63) - 32, 3);
					SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				}
				if (p->s_work3 >= 20)
					p->s_work3 = 0;
			}
			break;


		    /* ３方向斜め弾 */
		case 7:
			p->s_work2++;
#define INTERVAL7	15
#define SPEED_SHOT7	12
			if (p->s_work2 > INTERVAL7) {
				p->s_work2 = 0;
				p->s_work3++;
				if (p->s_work3 < 5) {
					if (p->cwork == EAR_LEFT) {
						EshotInit (ESHOT_NRG22, p->x, p->y, SPEED_SHOT7, 64 + 9, 3);
						EshotInit (ESHOT_NRG22, p->x, p->y, SPEED_SHOT7 + 4, 64, 3);
						EshotInit (ESHOT_NRG22, p->x, p->y, SPEED_SHOT7 + 8, 64 - 9, 3);
					} else {
						EshotInit (ESHOT_NRG22, p->x, p->y, SPEED_SHOT7, 64 - 9, 3);
						EshotInit (ESHOT_NRG22, p->x, p->y, SPEED_SHOT7 + 4, 64, 3);
						EshotInit (ESHOT_NRG22, p->x, p->y, SPEED_SHOT7 + 8, 64 + 9, 3);
					}
					SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				}
				if (p->s_work3 >= 7)
					p->s_work3 = 0;
			}
			break;


		    /* 半円炸裂弾連発 */
		case 6:
			p->s_work2++;
#define INTERVAL6	18
#define SPEED_SHOT6	13
			if (p->s_work2 > INTERVAL6) {
				p->s_work2 = 0;
				p->s_work3++;

				if (p->s_work3 == 1)
					p->s_work4 = SPEED_SHOT6;
				if (p->s_work3 < 6) {
					unsigned char a = 16;
					short type;
					short type_table[6] =
					{ESHOT_NRG02, ESHOT_NRG02, ESHOT_NRG03, ESHOT_NRG03, ESHOT_NRG04, ESHOT_NRG05,};
					short h;
					type = type_table[p->s_work3];
					for (h = 0; h < 12; h++)
						EshotInit (type, p->x, p->y, p->s_work4, a += 8, 2);
					p->s_work4 += 2;
					SoundSetSE (SE_EBOMB_M);
				}
				if (p->s_work3 >= 7)
					p->s_work3 = 0;
			}
			break;


		    /* 伸びるＶ字弾 */
		case 5:
			p->s_work2++;
#define SPEED_SHOT5	10
			if (p->s_work2 == 12) {
				unsigned char a = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT5, a + 8, 2);
				EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT5, a - 8, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT5 + 3 * 1, a + 8, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT5 + 3 * 1, a - 8, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT5 + 3 * 2, a + 8, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT5 + 3 * 2, a - 8, 2);
				EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT5 + 3 * 3, a + 8, 2);
				EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT5 + 3 * 3, a - 8, 2);
				EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT5 + 3 * 4, a + 8, 2);
				EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT5 + 3 * 4, a - 8, 2);
				EshotInit (ESHOT_NRG35, p->x, p->y, SPEED_SHOT5 + 3 * 5, a + 8, 2);
				EshotInit (ESHOT_NRG35, p->x, p->y, SPEED_SHOT5 + 3 * 5, a - 8, 2);
				SoundSetSE (SE_EBOMB_L);
			}
			if (p->s_work2 >= 36)
				p->s_work2 = 0;
			break;


		    /* たがい違い半円炸裂弾連発 */
		case 4:
			p->s_work2++;
#define INTERVAL4	20
#define SPEED_SHOT4	18
			switch (p->s_work2) {
			case INTERVAL4 * 1:
			case INTERVAL4 * 3:
			case INTERVAL4 * 5:
				{
					unsigned char a = 8;
					short h;
					for (h = 0; h < 7; h++) {
						EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT4, a += 8, 2);
						EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT4, a += 8, 4);
					}
				}
				SoundSetSE (SE_EBOMB_M);
				break;
			case INTERVAL4 * 2:
			case INTERVAL4 * 4:
				{
					unsigned char a = 16;
					short h;
					for (h = 0; h < 6; h++) {
						EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT4 + 2, a += 8, 2);
						EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT4 + 2, a += 8, 4);
					}
				}
				SoundSetSE (SE_EBOMB_M);
				break;
			case INTERVAL4 * 7:
				p->s_work2 = 0;
				break;
			default:
				break;
			}
			break;


		    /* ダイヤ型弾 */
		case 3:
			p->s_work2++;
#define INTERVAL3	18
#define SPEED_SHOT3	22
			if (p->s_work2 > INTERVAL3) {
				char a = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				p->s_work2 = 0;
				p->s_work3++;

				if (p->s_work3 < 7) {
					EshotInit (ESHOT_NRG14N, p->x, p->y - 12, SPEED_SHOT3, a, 0);
					EshotInit (ESHOT_NRG14N, p->x + 12, p->y, SPEED_SHOT3, a, 0);
					EshotInit (ESHOT_NRG14N, p->x, p->y + 12, SPEED_SHOT3, a, 0);
					EshotInit (ESHOT_NRG14N, p->x - 12, p->y, SPEED_SHOT3, a, 0);
					SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				}
				if (p->s_work3 >= 9)
					p->s_work3 = 0;
			}
			break;



		    /* ３方向弾 */
		case 2:
			p->s_work2++;
#define INTERVAL2	7
#define SPEED_SHOT2	20
			switch (p->s_work2) {
			case INTERVAL2 * 1:
				p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
			case INTERVAL2 * 2:
			case INTERVAL2 * 3:
			case INTERVAL2 * 4:
			case INTERVAL2 * 5:
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, p->s_angle - 18, 2);
				EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT2+2, p->s_angle, 2);
				EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, p->s_angle + 18, 2);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				break;
			case INTERVAL2 * 8:
				p->s_work2 = 0;
				break;
			default:
				break;
			}
			break;


		    /* 不等号弾 */
		case 1:
			p->s_work2++;
#define INTERVAL1	6
#define SPEED_SHOT1	22
			switch (p->s_work2) {
			case INTERVAL1 * 1:
				p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT1, p->s_angle, 2);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				break;
			case INTERVAL1 * 2:
				EshotInit (ESHOT_NRG04, p->x+6*1, p->y, SPEED_SHOT1, p->s_angle, 2);
				EshotInit (ESHOT_NRG04, p->x-6*1, p->y, SPEED_SHOT1, p->s_angle, 2);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				break;
			case INTERVAL1 * 3:
				EshotInit (ESHOT_NRG03, p->x+6*2, p->y, SPEED_SHOT1, p->s_angle, 2);
				EshotInit (ESHOT_NRG03, p->x-6*2, p->y, SPEED_SHOT1, p->s_angle, 2);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				break;
			case INTERVAL1 * 4:
				EshotInit (ESHOT_NRG02, p->x+6*3, p->y, SPEED_SHOT1, p->s_angle, 2);
				EshotInit (ESHOT_NRG02, p->x-6*3, p->y, SPEED_SHOT1, p->s_angle, 2);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				break;
			case INTERVAL1 * 8:
				p->s_work2 = 0;
				break;
			default:
				break;
			}
			break;


		default:	/* ここには来ないハズ */
			p->arg = 1;	/* バグ避け */
			break;
		}		/* end of switch(p->arg) */
		break;

	default:
		break;
	}



    /* 自機ショットに当たった時の処理 */
	if (p->damage) {
		SoundSetSE (SE_DAMAGE);
		p->info = PALET_DAMAGE | PRIORITY_BOSS;
		p->damage_core = 0;
		if ((p->hp -= p->damage) <= 0) {
		    /* 死んだ */
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			EffectInit (EFFECT_POINTS_RED, POINTS_1200, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			p->child_death = !0;	/* 消去準備 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 50)) {
			p->info = PALET_RED | PRIORITY_BOSS_PARTS_L;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_LBOSS04 | PRIORITY_BOSS_PARTS_L;
		}
	}
	xobj_set_st (p);


	return (0);
}



static void EnemyTiniLBoss04E (ENEMY * p)
{
}
