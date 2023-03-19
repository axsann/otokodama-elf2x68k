/* ２面ボス */
#include <xsp2lib.h>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../entry.h"
#include "../sound.h"
#include "../psearch.h"


#define SPEED_LASER		30

#define SPEED_1		3	/* 加速度 */
#define SPEED_1N	28	/* 加速度を足す回数 */

#define SPEED_SHOT1	8	/* １段階目扇型弾速度 */
#define SPEED_SHOT2	10	/* １段階目弾の速度 */
#define SPEED_SHOT3	12	/* １段階目・２列直線弾の一番遅い速度 */
#define SPEED_SHOT4	10	/* 第２形態・三角弾の速度 */
#define SPEED_SHOT5	11	/* 第２形態・なぎ倒し弾の速度 */


#define HP_0	3200		/* 耐久力 */
#define HP_1	HP_0-700	/* 第１形態・耐久力がこれ以下になったら炸裂弾を撃つ */
#define HP_2	HP_1-600	/* 〃   ・耐久力がこれ以下になったら直線弾を撃つ */
#define HP_3	HP_2-500	/* 〃   ・耐久力がこれ以下になったら第２形態へ */

#define TIMER_1	2500		/* これ以上時間が経過したら第２形態へ */
#define TIMER_2	7000		/* これ以上時間が経過したら撤退 */
#define TIMER_3	5000		/* これ以上時間が経過したら第２形態攻撃開始 */

#define SPEED_3		3	/* ３段階目（撤退）の加速度 */
#define SPEED_3N	32	/* ３段階目の加速度を足す回数 */

#define SEQ_DEATH	6



enum {
	MOVETO_INIT = 0,	/* 初期状態 */
	MOVETO_C2L,		/* 中央から左に移動 */
	MOVETO_L2C,
	MOVETO_C2R,
	MOVETO_R2C
};

enum {
	ATTACK_UPPER_CORE = 0,	/* 上のコアから攻撃（伸びる弾） */
	ATTACK_LOWER_CORE	/* 下のコアから攻撃（ばらまき弾） */
};

static short EnemyMoveLBoss02 (ENEMY *);
static void EnemyTiniLBoss02 (ENEMY *);


void EnemyInitLBoss02 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 4 << 16;

	/* 第１形態の当り判定 */
	p->hit_px = 32;
	p->hit_py = 16;
	p->hit_sx = 32;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_lboss02;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveLBoss02;
	p->func_enemy_tini = EnemyTiniLBoss02;

	p->parts1 = 0;
	p->parts2 = 0;
	p->parts1_work = 20;	/* 左翼復活までの時間（ =1 で復活） */
	p->parts1_work2 = 0;	/* 左翼何回復活したか */
	p->parts2_work = 50;	/* 右翼復活までの時間（ =1 で復活） */
	p->parts2_work2 = 0;	/* 右翼何回復活したか */

	p->parts3 = 0;
	p->parts4 = 0;
	p->parts3_work = 0;	/* 耳が出たか */
	p->parts3_work2 = 0;	/* 翼破壊から耳発生までの時間 */

	entry_counter_stop = !0;/* エントリーカウンター停止 */

	if (p->arg == 0)
		p->work4 = PALET_LBOSS02 | PRIORITY_BOSS;
	else
		p->work4 = PALET_LBOSS02_1 | PRIORITY_BOSS;
}



static short EnemyMoveLBoss02 (ENEMY * p)
{
	/* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->timer++;

	switch (p->seq) {
	case 0:
		if (SHORT_LY > 48)
			p->seq++;
		break;
	case 1:		/* 減速 */
		p->vy -= 8192;
		if (p->vy < 8192) {
			p->seq++;
			p->vy = 0;
			p->m_work = MOVETO_INIT;	/* 移動方向 */
			p->m_work3 = 0;	/* 第１段階の中での攻撃方法 */
			p->s_work = 0;
			p->s_work3 = 0;	/* 直線弾用 */
		}
		break;

		/* 第１形態 */
	case 2:		/* 左右に移動しつつ扇状弾 */
		/* まず移動処理 */
		if (p->m_work == MOVETO_INIT) {
			/* 初期状態なら */
			if (p->x > player->x)
				p->m_work = MOVETO_C2L;	/* 移動方向 */
			else
				p->m_work = MOVETO_C2R;	/* 移動方向 */
			p->m_work2 = 0;
		}
		if (p->m_work2 == 0) {
			/* 初回なら */
			signed short dx, dy;
			signed short dx_table[5] =
			{0, 54 + 16, 128 + 16, 256 - 54 + 16, 128 + 16};
			short m_work_table[5] =
			{MOVETO_INIT, MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L};

			p->m_work2 = !0;
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
		} else {
			/* 設定値に従って移動 */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* 移動方向再設定 */
		}

		/* 次に攻撃処理 */
		if (p->s_work++ > 100)
			p->s_work = 0;

		switch (p->m_work3) {
		case 0:
			if (p->hp < HP_1) {
				p->m_work3++;
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			}
			break;
		case 1:
			if (p->hp < HP_2) {
				p->m_work3++;
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			}
			break;
		case 2:
			if (p->hp < HP_3) {
				p->m_work3++;
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
			}
			break;
		default:
			break;
		}
		if (p->timer > TIMER_1)
			p->m_work3 = 4;

		switch (p->m_work3) {
			/* case の並び方に注目 */
		case 2:	/* ２列直線弾 */
			switch (p->s_work3++) {
			case 0:
				p->s_angle3 = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				p->s_work4 = SPEED_SHOT3;
				break;
			case 20 + 6 * 0:
			case 20 + 6 * 1:
			case 20 + 6 * 2:
			case 20 + 6 * 3:
			case 20 + 6 * 4:
			case 20 + 6 * 5:
				EshotInit (ESHOT_NRG32, SHORT_LX, SHORT_LY + 8, p->s_work4, p->s_angle3, 3);
				SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				p->s_work4 += 3;
				break;
			case 20 + 6 * 11:
				p->s_work3 = 0;
				break;
			default:
				break;
			}
			/* ここに break が無いことに注意 */
		case 1:	/* 炸裂弾 */
			if (p->s_work == 25) {
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 0, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 1, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 2, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 3, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 4, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 5, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 6, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 7, 3);
				EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY + 8, SPEED_SHOT2, 16 + 32 * 8, 3);
				SoundSetSE (SE_EBOMB_M);	/* 敵炸裂弾音 */
			}
			if ((p->s_work == 76) && (p->y + 128 > player->y)) {
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 0, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 1, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 2, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 3, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 4, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 5, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 6, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 7, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT3, 32 * 8, 3);
				SoundSetSE (SE_EBOMB_M);	/* 敵炸裂弾音 */
			}
			/* ここに break が無いことに注意 */
		case 0:	/* なぎ倒し弾 */
			if (p->s_work == 1) {
				p->s_angle = 16;
				p->s_angle2 = 128 - 16;
				p->s_work2 = 0;
			}
			if ((p->s_work > 6) && (p->s_work < 60)) {
				p->s_work2++;
				if (p->s_work2 > 3) {
					p->s_work2 = 0;
					EshotInit (ESHOT_NRG02, SHORT_LX - 29, SHORT_LY - 23, SPEED_SHOT1, p->s_angle, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX + 29, SHORT_LY - 23, SPEED_SHOT1, p->s_angle2, 0);
					p->s_angle += 6;
					p->s_angle2 -= 6;
					SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				}
			}
			break;
		case 3:	/* 頭部を破壊した */
		case 4:	/* 頭部を破棄した */
			{
				short arg;
				if (p->m_work3 == 3)
					arg = 0;
				else
					arg = 1;
				if (p->arg)
					arg += 2;
				EnemyInit (ENEMY_LBOSS02H, SHORT_LX, SHORT_LY, arg, 0);	/* 頭を分離 */
			}
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* 弾消し */
			p->pt = obj_lboss02 + 2;	/* 本体部分だけになる */
			p->seq++;
			p->m_work = 0;
			p->m_work2 = 0;
			p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
			break;
		}
		break;


	case 3:		/* 合体！ */
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0) {	/* 止まるまで待つ */
				p->m_work++;
				/* 移動方向を設定 */
				SubEnemyMoveToInit (p, 128 + 16, 128, SPEED_1, SPEED_1N);
			}
			break;
		case 1:
			if (SubEnemyMoveTo (p) < 0)	/* 止まるまで待つ */
				p->m_work++;
			break;
		case 2:
			if (p->m_work2 == 43)
				SoundSetSE (SE_LOCK_M);	/* 「がちゃん」という効果音を鳴らす */
			if (p->m_work2 == 53) {
				p->pt = obj_lboss02 + 4;	/* 本体＋大パーツを本体とする */
				p->seq++;
				p->cwork = ATTACK_UPPER_CORE;
				p->m_work = MOVETO_INIT;	/* 移動方向 */
				p->s_work = 0;
				p->s_work2 = 0;
				p->s_work3 = -110;
				p->s_angle3 = 0;

				/* 第２形態の当り判定 */
				p->hit_px = 64;
				p->hit_py = 16;
				p->hit_sx = 64;
				p->hit_sy = 16;
				p->hit_cx = 16;
			}
			/* 大パーツ部の処理 */
			if (p->m_work2 < 53) {
				signed short offset_y[53] =	/* 大パーツの座標オフセット */
				{
					185, 179, 172, 166, 159, 153, 147, 140, 134, 127,
					121, 115, 108, 102, 95, 89, 83, 76, 70, 63,
					57, 51, 44, 38, 31, 26, 21, 17, 14, 11,
					10, 9, 8, 7, 7, 6, 5, 4, 3, 3,
					2, 1, 0, -1, -1, 0, 1, 0, -1, -1,
					0, 1, 0
				};
				if (p->arg == 0)
					xobj_set (SHORT_LX, SHORT_LY + offset_y[p->m_work2],
						  obj_lboss02 + 3, PALET_LBOSS02 | PRIORITY_BOSS_PARTS_L);
				else
					xobj_set (SHORT_LX, SHORT_LY + offset_y[p->m_work2],
						  obj_lboss02 + 3, PALET_LBOSS02_1 | PRIORITY_BOSS_PARTS_L);
				xsp_set (SHORT_LX - 16, SHORT_LY + offset_y[p->m_work2] - 16,
					 sp_core01, PALET_CORE_BLUE | PRIORITY_BOSS_CORE);
				xsp_set (SHORT_LX, SHORT_LY + offset_y[p->m_work2] - 16,
					 sp_core01, PALET_CORE_BLUE | PRIORITY_BOSS_CORE);
			}
			p->m_work2++;
			break;
		}
		break;


		/* 第２形態 */
	case 4:
		if (p->timer > TIMER_2) {
			p->seq++;	/* 時間切れ */
			p->m_work = 0;
			break;
		}
		/* まず移動処理 */
		if (p->m_work == MOVETO_INIT) {
			/* 初期状態なら */
			if (p->x > player->x)
				p->m_work = MOVETO_C2L;	/* 移動方向 */
			else
				p->m_work = MOVETO_C2R;	/* 移動方向 */
			p->m_work2 = 0;
		}
		if (p->m_work2 == 0) {
			/* 初回なら */
			signed short dx, dy;
			signed short dx_table[5] =
			{0, 72 + 16, 128 + 16, 256 - 72 + 16, 128 + 16};
			short m_work_table[5] =
			{MOVETO_INIT, MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L};

			p->m_work2 = !0;
			/* 順に左→中央→右→中央→左へ移動 */
			dx = dx_table[p->m_work];
			p->m_work = m_work_table[p->m_work];

			/* player->y は乱数の種として使用しています */
			if (SHORT_LY < 80)
				dy = (((unsigned short) rndtable[(player->y) & 0xff]) / 4 + 48 + 16);
			else
				dy = 64;
			/* 移動方向を設定 */
			SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
		} else {
			/* 設定値に従って移動 */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* 移動方向再設定 */
		}


		/* 翼パーツが消えたら復活までの時間を設定 */
		if ((p->parts1) && (p->parts1->child_death)) {
			p->parts1 = 0;	/* これ以降参照しない */
			p->parts1_work = 90;
		}
		if ((p->parts2) && (p->parts2->child_death)) {
			p->parts2 = 0;	/* これ以降参照しない */
			p->parts2_work = 70;	/* 実は右翼の方が復活が早い */
		}
		/* 翼パーツ発生・復活処理 */
		if (p->parts1_work) {
			if (p->parts1_work == 1)
				p->parts1 = EnemyInit (ENEMY_LBOSS02W, -80, 0, p->parts1_work2++, p);	/* 左翼 */
			p->parts1_work--;
		}
		if (p->parts2_work) {
			if (p->parts2_work == 1)
				p->parts2 = EnemyInit (ENEMY_LBOSS02W, 80, 0, p->parts2_work2++, p);	/* 右翼 */
			p->parts2_work--;
		}
		/* 耳を出すか */
		if (p->parts3_work == 0) {
			/* まだ耳が出ていない */
			if (p->parts1_work2 + p->parts2_work2 >= 6) {	/* 翼が出た回数 */
				p->parts3_work = !0;
				p->parts3_work2 = 60;
			}
		} else {
			if (p->parts3_work2) {
				p->parts3_work2--;
				if (p->parts3_work2 == 1) {
					p->parts3 = EnemyInit (ENEMY_LBOSS02E, 40, 0, 0, p);	/* 左耳 */
					p->parts4 = EnemyInit (ENEMY_LBOSS02E, -40, 0, 0, p);	/* 右耳 */
				}
			} else {
				/* 耳がまだあるか */
				if ((p->parts3) && (p->parts3->child_death))
					p->parts3 = 0;	/* これ以降参照しない */
				if ((p->parts4) && (p->parts4->child_death))
					p->parts4 = 0;	/* これ以降参照しない */
				/* 耳破壊済み */
				if ((p->parts3 == 0) && (p->parts4 == 0))
					p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
			}
		}

		/* 次に攻撃処理 */
		switch (p->s_work) {
		case 0:
			if (p->timer > TIMER_3) {
				p->s_work++;
				p->s_work2 = 0;
			}
			break;
		case 1:
			if (p->s_work2++ == 10) {
				EshotInit (ESHOT_NRG23, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT4, 149, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT4, 234, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX, SHORT_LY + 8, SPEED_SHOT4, 64, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT4 + 4, 149, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT4 + 4, 234, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX, SHORT_LY + 8, SPEED_SHOT4 + 4, 64, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT4 + 8, 149, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT4 + 8, 234, 0);
				EshotInit (ESHOT_NRG23, SHORT_LX, SHORT_LY + 8, SPEED_SHOT4 + 8, 64, 0);
				SoundSetSE (SE_LASER);	/* 敵ショット音 */
			}
			if (p->s_work2 > 100)
				p->s_work2 = 0;
			break;
		default:
			break;
		}

		/* なぎ倒し弾 */
		if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE)) {
			switch (p->s_work3++) {
			case 0:
				if (p->arg == 0) {
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT5, p->s_angle3, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT5, p->s_angle3 + 85, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT5, p->s_angle3 + 170, 0);
					p->s_angle3 += 12;
				} else {
					EshotInit (ESHOT_NRG02, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3 + 85, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX - 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3 + 170, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3 + 85, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX + 16, SHORT_LY - 8, SPEED_SHOT5, p->s_angle3 + 170, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT5, p->s_angle3, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT5, p->s_angle3 + 85, 0);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 8, SPEED_SHOT5, p->s_angle3 + 170, 0);
					p->s_angle3 += 12;
				}
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				break;
			case 12:
				p->s_work3 = 0;
				break;
			default:
				break;
			}
		}
		break;


		/* 時間切れ撤退 */
	case 5:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0) {	/* 止まるまで待つ */
				p->m_work++;
				SubEnemyMoveToInit (p, 128 + 16, 160, SPEED_3, SPEED_3N);
			}
			break;
		case 1:	/* 一旦中央まで移動してから撤退 */
			if (SubEnemyMoveTo (p) < 0) {	/* 止まるまで待つ */
				p->m_work++;
				SubEnemyMoveToInit (p, p->x, -128, SPEED_3, SPEED_3N);
			}
			break;
		case 2:
			if (SubEnemyMoveTo (p) < 0) {	/* 止まるまで待つ */
				return (-1);	/* 消去 */
			}
			break;
		}
		break;

		/* 死んだ */
	case SEQ_DEATH:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveToAbort (p) < 0) {	/* 止まるまで待つ */
				p->m_work++;
				p->m_work2 = 0;
				p->m_work3 = 0;
				/* 当たり判定を無くす */
				p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* 弾消し */
			}
			break;
		case 1:
			if (p->m_work2++ > 3) {
				p->m_work2 = 0;
				p->m_work3++;
				if (p->m_work3 < 15) {
					signed short x, y;
					x = rndtable[(p->timer) & 0xff ^ 0x55] - 128;
					y = rndtable[(p->timer) & 0xff ^ 0xaa] - 128;
					/* p->timer は乱数の種として使用しています */
					EffectInit (EFFECT_EXPL, 0, x / 2 + p->x, y / 4 + p->y);
					SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
				}
				if (p->m_work3 > 20) {
					short h;
					for (h = 0; h < 6; h++) {
						EffectInit (EFFECT_EXPL, 0,
							    (signed short) ((rndtable[h] - 128) / 2) + p->x,
							    (signed short) ((rndtable[h + 1] - 128) / 4) + p->y + 16);
					}
					SoundSetSE (SE_EXPL_L);	/* ボスキャラ爆発音 */
					if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE))
						EffectInit (EFFECT_POINTS_RED, POINTS_15000, p->x, p->y);
					else
						EffectInit (EFFECT_POINTS_BLUE, POINTS_7000, p->x, p->y);
					EffectInit (EFFECT_HAHEN, 0, p->x, p->y);

					return (-1);	/* 消去 */
				}
			}
			break;
		}
		break;
	default:
		break;
	}
	/* 画面外に出たら消去 */
	if (SHORT_LY > 256 + 32)
		return (-1);	/* 消去 */


	/* 自機ショットに当たった時の処理 */
	if (p->damage) {
		if (p->damage_core) {
			p->info = PALET_RED | PRIORITY_BOSS;
			p->damage_core = 0;
		} else {
			p->info = PALET_DAMAGE | PRIORITY_BOSS;
		}
		if ((p->hp -= p->damage) <= 0) {
			/* 死んだ */
			p->seq = SEQ_DEATH;
			p->m_work = 0;
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 300)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = p->work4;
		}
	}

	xobj_set_st (p);

	/* コアの表示 */
	p->core_x = p->x - 8;	/* コアの中心は (-8,-8) ドットずれる */
	p->core_y = p->y;
	xsp_set_st (&(p->core_x));
	if (p->seq >= 4) {
		/* 第２形態以降なら２つ目、３つ目のコアを表示 */
		p->core_x = p->x - 16;
		p->core_y = p->y - 16;
		xsp_set_st (&(p->core_x));
		p->core_x = p->x;
		p->core_y = p->y - 16;
		xsp_set_st (&(p->core_x));
	}
	return (0);
}



static void EnemyTiniLBoss02 (ENEMY * p)
{
	/* パーツが存在していたら消すように指示 */
	if (p->parts1)
		p->parts1->child_kill = !0;
	if (p->parts2)
		p->parts2->child_kill = !0;
	if (p->parts3)
		p->parts3->child_kill = !0;
	if (p->parts4)
		p->parts4->child_kill = !0;
	entry_counter_stop = 0;	/* エントリーカウンター停止解除 */
}
