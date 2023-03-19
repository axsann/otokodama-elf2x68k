/* １面ボス */
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

#define SPEED_SHOT1	8	/* １段階目扇型ショット速度 */
#define SPEED_SHOT2	10	/* ２段階目伸びるショットの一番遅い弾の速度 */
#define SPEED_SHOT3	12	/* ２段階目なぎ倒し弾の速度 */

#define HP_0	3000		/* 耐久力 */
#define HP_1	2300		/* 耐久力がこれ以下になったら２段階目へ */
#define HP_2	800		/* 耐久力がこれ以下になったら炸裂弾 */
#define TIMER_1	1500		/* これ以上時間が経過したら２段階目へ */
#define TIMER_2	3500		/* これ以上時間が経過したら撤退 */

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
static short EnemyMoveLBoss01 (ENEMY *);
static void EnemyTiniLBoss01 (ENEMY *);


void EnemyInitLBoss01 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 2 << 16;
	p->hit_px = 54;
	p->hit_py = 16;
	p->hit_sx = 54;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_lboss01 + 1;
	p->core_pt = sp_core01;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveLBoss01;
	p->func_enemy_tini = EnemyTiniLBoss01;

	if (p->arg == 0)
		p->work4 = PALET_LBOSS01 | PRIORITY_BOSS;
	else
		p->work4 = PALET_LBOSS01_1 | PRIORITY_BOSS;

	entry_counter_stop = !0;/* エントリーカウンター停止 */
}



static short EnemyMoveLBoss01 (ENEMY * p)
{
	/* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->timer++;

	switch (p->seq) {
	case 0:
		if (SHORT_LY > 64)
			p->seq++;
		break;
	case 1:		/* 減速 */
		p->vy -= 8192;
		if (p->vy < 8192) {
			p->seq++;
			p->vy = 0;
			p->m_work = MOVETO_INIT;	/* 移動方向 */
			p->s_work = 0;
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
		p->s_work++;
		if (p->s_work == 1) {
			p->s_angle = 16;
			p->s_angle2 = 128 - 16;
			p->s_work2 = 0;
		}
		if ((p->s_work > 6) && (p->s_work < 60)) {
			p->s_work2++;
			if (p->s_work2 > 3) {
				p->s_work2 = 0;
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1, p->s_angle, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT1, p->s_angle2, 3);
				p->s_angle += 6;
				p->s_angle2 -= 6;
				SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
			}
		}
		if (p->s_work > 100)
			p->s_work = 0;

		/* ある程度ダメージを受けた or 時間切れ */
		if ((p->hp < HP_1) || (p->timer > TIMER_1)) {
			/* カバー破棄 */
			if (p->hp < HP_1)
				p->parts1 = EnemyInit (ENEMY_LBOSS01C, SHORT_LX, SHORT_LY, 0 + p->arg * 2, p);	/* カバーを分離 */
			else
				p->parts1 = EnemyInit (ENEMY_LBOSS01C, SHORT_LX, SHORT_LY, 1 + p->arg * 2, p);	/* カバーを分離 */
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* 弾消し */
			p->pt++;
			p->seq++;
			p->m_work = 0;
			p->m_work2 = 0;
			p->hit_px = p->hit_sx = p->hit_py = p->hit_sy = 0;
		}
		break;
	case 3:		/* アニメーション開始！ */
		p->m_work2++;
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0)	/* 止まるまで待つ */
				p->m_work++;
			break;
		case 1:
			if (p->m_work2 < 100)
				break;
			if (p->pt < obj_lboss01 + 50) {
				p->pt++;
			} else {
				p->seq++;
				p->cwork = ATTACK_UPPER_CORE;
				p->m_work = MOVETO_INIT;	/* 移動方向 */
				p->s_work = 0;
				p->s_work2 = 0;
				p->s_work3 = 0;	/* なぎ倒し弾用 */
				p->s_work4 = 0;	/* 炸裂弾用 */
				p->s_angle3 = 0;	/* なぎ倒し弾用 */
				p->hit_px = p->hit_sx = 70;
				p->hit_py = p->hit_sy = 16;
			}
			if ((p->pt == obj_lboss01 + 9) || (p->pt == obj_lboss01 + 30) || (p->pt == obj_lboss01 + 46))
				SoundSetSE (SE_LOCK_M);	/* 「がちゃん」という効果音を鳴らす */
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
			{0, 32 + 16, 128 + 16, 256 - 32 + 16, 128 + 16};
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
		if (p->cwork == ATTACK_UPPER_CORE) {
			/* 上のコアから攻撃（伸びる弾） */
			p->s_work++;
			switch (p->s_work) {
			case 1:
				p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				if (p->x > player->x) {
					p->s_work2 = 0;
					p->s_angle -= 32;
				} else {
					p->s_work2 = !0;
					p->s_angle += 32;
				}
				break;
			case 10 + 20 * 0:
			case 10 + 20 * 1:
			case 10 + 20 * 2:
			case 10 + 20 * 3:
			case 10 + 20 * 4:
			case 10 + 20 * 5:
			case 10 + 20 * 6:
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 0, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 1, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 2, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 3, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 4, p->s_angle, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY + 16, SPEED_SHOT2 + 3 * 5, p->s_angle, 3);
				SoundSetSE (SE_LASER);	/* 敵ショット音 */
				if (p->s_work2 == 0)
					p->s_angle += 8;
				else
					p->s_angle -= 8;
				break;
			case 10 + 20 * 9:
				p->cwork = ATTACK_LOWER_CORE;
				p->s_work = 0;
				p->s_work2 = 0;
				break;
			default:
				break;
			}
		} else {
			/* 下のコアから攻撃（扇状弾） */
			p->s_work++;
			if (p->s_work < 400) {
				if (p->s_work2++ > 1) {
					p->s_work2 = 0;
					EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY, 7, rndtable[p->angle++] & 0xfc, 8);
					EshotInit (ESHOT_NRG04, SHORT_LX, SHORT_LY, 7, rndtable[p->angle++] & 0xfc, 8);
					SoundSetSE (SE_ESHOT);	/* 敵ショット音 */
				}
			}
			if (p->s_work == 480) {
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* 弾消し */
			}
			if (p->s_work > 500) {
				p->cwork = ATTACK_UPPER_CORE;
				p->s_work = 0;
				p->s_work2 = 0;
			}
		}
		/* なぎ倒し弾 */
		if (p->hp < HP_2) {
			switch (p->s_work3++) {
			case 0:
				EffectInit (EFFECT_EXPL, 0, p->x, p->y);
				SoundSetSE (SE_EXPL_M);	/* 中ボス爆発音 */
				break;
			case 1:
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT3, p->s_angle3, 0);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT3, p->s_angle3 + 128, 0);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, SPEED_SHOT3, p->s_angle3 + 64, 0);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY + 16, SPEED_SHOT3, p->s_angle3 + 192, 0);
				SoundSetSE (SE_ESHOT);
				p->s_angle3 += 16;
				break;
			case 11:
				p->s_work3 = 1;
				break;
			default:
				break;
			}
		}
		/* ボスラッシュ時 */
#define INTERVAL4_3	30
#define SPEED_SHOT4_3	22
		if (p->arg) {
			if (p->s_work4++ > INTERVAL4_3) {
				short h;
				char a = 0;
				for (h = 0; h < 16; h++)
					EshotInit (ESHOT_NRG24N, SHORT_LX, SHORT_LY + 8, SPEED_SHOT4_3, a += 16, 3);
				SoundSetSE (SE_EBOMB_M);	/* 敵炸裂弾音 */
				p->s_work4 = 0;
			}
		}
		break;

		/* 時間切れ撤退 */
	case 5:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0) {	/* 止まるまで待つ */
				p->m_work++;
				SubEnemyMoveToInit (p, p->x, -128, SPEED_3, SPEED_3N);
			}
			break;
		case 1:
			if (SubEnemyMoveTo (p) < 0) {	/* 止まるまで待つ */
				entry_counter_stop = 0;	/* エントリーカウンター停止解除 */
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
					EffectInit (EFFECT_EXPL, 0, x / 2 + p->x, y / 4 + p->y + 16);
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
						EffectInit (EFFECT_POINTS_RED, POINTS_10000, p->x, p->y);
					else
						EffectInit (EFFECT_POINTS_BLUE, POINTS_5000, p->x, p->y);
					EffectInit (EFFECT_HAHEN, 0, p->x, p->y);

					entry_counter_stop = 0;	/* エントリーカウンター停止解除 */
					return (-1);	/* 消去 */
				}
			}
			break;
		}
	default:
		break;
	}

	/* 画面外に出たら消去 */
	if (SHORT_LY > 256 + 32)
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
	p->core_y = p->y - 8;
	if (p->seq != SEQ_DEATH) {	/* 死んだらコア状態は変化させない */
		if (player->y < p->y + 96)	/* 自機との座標の差が小さければ赤 */
			p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
		else
			p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;
	}
	xsp_set_st (&(p->core_x));
	/* コア２つ目 */
	p->core_x = p->x - 8;	/* コアの中心は (-8,+8) ドットずれる */
	p->core_y = p->y + 8;
	xsp_set_st (&(p->core_x));

	return (0);
}


static void EnemyTiniLBoss01 (ENEMY * p)
{
}
