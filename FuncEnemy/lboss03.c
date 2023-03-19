/* ３面ボス */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../entry.h"
#include "../sound.h"
#include "../psearch.h"


#define SPEED_1		3	/* 加速度 */
#define SPEED_1N	28	/* 加速度を足す回数 */
#define SPEED_2		15	/* 加速度 */
#define SPEED_2N	10	/* 加速度を足す回数 */

#define SPEED_3		3	/* ３段階目（撤退）の加速度 */
#define SPEED_3N	32	/* ３段階目の加速度を足す回数 */


#define SPEED_SHOT1	8	/* １段階目扇型弾速度 */
#define SPEED_SHOT2	10	/* １段階目弾の速度 */
#define SPEED_SHOT3	14	/* １段階目弾の速度 */
#define SPEED_SHOT4	10	/* 第２形態・三角弾の速度 */

#define SPEED_SHOT9_01	14	/* 第２形態・らせん弾（低速）の速度 */
#define SPEED_SHOT9_02	20	/* 第２形態・らせん弾（高速）の速度 */

#define SPEED_SHOT9_11	20	/* 第２形態・らせん弾（高速）の速度 */
#define SPEED_SHOT9_12	10	/* 第２形態・らせん弾（高速）の速度 */

#define SPEED_SHOT9_21	8	/* 第２形態・円形弾の速度 */
#define SPEED_SHOT9_22	14	/* 第２形態・円形弾の速度 */
#define SPEED_SHOT9_23	20	/* 第２形態・円形弾の速度 */
#define SPEED_SHOT9_24	26	/* 第２形態・円形弾の速度 */


#define HP_0	5000		/* 耐久力 */

#define TIMER_TIMEOUT	9000	/* これ以上時間が経過したら撤退 */

#define SEQ_TIMEOUT	10
#define SEQ_DEATH	11



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

static short EnemyMoveLBoss03 (ENEMY *);
static void EnemyTiniLBoss03 (ENEMY *);


void EnemyInitLBoss03 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 3 << 16;

    /* 初期状態の当り判定 */
	p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->hp = HP_0;
	p->pt = obj_lboss03;
	p->core_pt = sp_core01;
	p->core_info = PALET_CORE_BLUE | PRIORITY_BOSS_CORE;
	p->seq = 0;
	p->func_enemy_move = EnemyMoveLBoss03;
	p->func_enemy_tini = EnemyTiniLBoss03;

	p->parts1 = p->parts2 = p->parts3 = p->parts4 = 0;

	if (p->arg == 0)
		p->work4 = PALET_LBOSS03 | PRIORITY_BOSS;
	else
		p->work4 = PALET_LBOSS03_1 | PRIORITY_BOSS;

	entry_counter_stop = !0;/* エントリーカウンター停止 */
}



static short EnemyMoveLBoss03 (ENEMY * p)
{
    /* コアが外側にある時の座標 */
	signed short core_table1[33][2] =
	{
		{0, 45},
		{8, 45},
		{17, 42},
		{25, 38},
		{32, 32},
		{38, 25},
		{42, 17},
		{45, 8},
		{45, 0},
		{45, -8},
		{42, -17},
		{38, -25},
		{32, -32},
		{25, -38},
		{17, -42},
		{8, -45},
		{0, -45},
		{-8, -45},
		{-17, -42},
		{-25, -38},
		{-32, -32},
		{-38, -25},
		{-42, -17},
		{-45, -8},
		{-45, 0},
		{-45, 8},
		{-42, 17},
		{-38, 25},
		{-32, 32},
		{-25, 38},
		{-17, 42},
		{-8, 45},
		{0, 45},
	};
    /* コアが内側にある時の座標 */
	signed short core_table2[33][2] =
	{
		{0, 9},
		{1, 8},
		{3, 8},
		{5, 7},
		{6, 6},
		{7, 5},
		{8, 3},
		{8, 1},
		{9, 0},
		{8, -1},
		{8, -3},
		{7, -5},
		{6, -6},
		{5, -7},
		{3, -8},
		{1, -8},
		{0, -9},
		{-1, -8},
		{-3, -8},
		{-5, -7},
		{-6, -6},
		{-7, -5},
		{-8, -3},
		{-8, -1},
		{-9, 0},
		{-8, 1},
		{-8, 3},
		{-7, 5},
		{-6, 6},
		{-5, 7},
		{-3, 8},
		{-1, 8},
		{0, 9},
	};
    /* ショット用・第２形態円形大 */
	signed short shot_pos1[16][2] =
	{
		{0, 50},
		{19, 46},
		{35, 35},
		{46, 19},
		{50, 0},
		{46, -19},
		{35, -35},
		{19, -46},
		{0, -50},
		{-19, -46},
		{-35, -35},
		{-46, -19},
		{-50, 0},
		{-46, 19},
		{-35, 35},
		{-19, 46},

	};
    /* ショット用・第２形態円形中 */
	signed short shot_pos2[8][2] =
	{
		{0, 35},
		{24, 24},
		{35, 0},
		{24, -24},
		{0, -35},
		{-24, -24},
		{-35, 0},
		{-24, 24},
	};
    /* ショット用・第２形態円形小 */
	signed short shot_pos3[8][2] =
	{
		{0, 20},
		{14, 14},
		{20, 0},
		{14, -14},
		{0, -20},
		{-14, -14},
		{-20, 0},
		{-14, 14},
	};
	short h;


    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
	p->x = (p->lx += p->vx) >> 16;
	p->y = (p->ly += p->vy) >> 16;
	p->timer++;

	switch (p->seq) {
	case 0:
		if (SHORT_LY > 48) {
			p->seq++;
			p->m_work = 0;
		}
		break;
	case 1:		/* 減速 */
		p->vy -= 4096;
		if (p->vy < 4096) {
			p->seq++;
			p->vy = 0;
			p->m_work = MOVETO_INIT;	/* 移動方向 */
		}
		switch (p->m_work++) {
		case 2:
			p->parts1 = EnemyInit (ENEMY_LBOSS03U, 111, 256 + 66, 0 + p->arg * 4, p);	/* パーツ１出現 */
			break;
		case 7:
			p->parts2 = EnemyInit (ENEMY_LBOSS03U, 177, 256 + 66, 1 + p->arg * 4, p);	/* パーツ２出現 */
			break;
		case 5:
			p->parts3 = EnemyInit (ENEMY_LBOSS03U, 111, 256 + 66, 2 + p->arg * 4, p);	/* パーツ３出現 */
			break;
		case 9:
			p->parts4 = EnemyInit (ENEMY_LBOSS03U, 177, 256 + 66, 3 + p->arg * 4, p);	/* パーツ４出現 */
			break;
		}
		break;
	case 2:
		if ((p->parts1->seq == 1) && (p->parts2->seq == 1)
		    && (p->parts3->seq == 1) && (p->parts4->seq == 1)) {
			p->seq++;
			p->parts1->seq++;
			p->parts2->seq++;
			p->parts3->seq++;
			p->parts4->seq++;
			p->pt = obj_lboss03 + 16;
			p->angle = 0;	/* 角度(0~8) */
			SoundSetSE (SE_LOCK_M);	/* 「がちゃん」という効果音を鳴らす */
		}
		break;
	case 3:		/* パーツが「がちゃん」とくっつくアニメーション */
		p->pt++;
		if (p->pt >= obj_lboss03 + 33) {
			p->seq++;
			p->pt = obj_lboss03;
			p->work2 = 0;	/* 角度の加速度に使用 */
			p->work3 = 0;	/* パーツを破壊した数 */
			p->angle = 0;	/* 角度に使用 */
			p->parts1 = EnemyInit (ENEMY_LBOSS03P, 0, 0, 0 + p->arg * 4, p);	/* パーツ１出現 */
			p->parts2 = EnemyInit (ENEMY_LBOSS03P, 0, 0, 1 + p->arg * 4, p);	/* パーツ２出現 */
			p->parts3 = EnemyInit (ENEMY_LBOSS03P, 0, 0, 2 + p->arg * 4, p);	/* パーツ３出現 */
			p->parts4 = EnemyInit (ENEMY_LBOSS03P, 0, 0, 3 + p->arg * 4, p);	/* パーツ４出現 */
			p->s_work = 0;
			SubEnemyMoveToInit (p, 144, 96, SPEED_1, SPEED_1N);
		}
		break;

	case 4:		/* 少し後退 */
		if (SubEnemyMoveTo (p) < 0) {
			p->seq++;
			p->m_work = MOVETO_INIT;	/* 移動方向 */
			p->s_work = 0;
			p->s_work2 = 0;
		}
		break;

	case 5:		/* 第１形態 */
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


	    /* 攻撃＆回転角の処理 */
		p->s_work3 = 0;
		switch (p->s_work) {
		case 0:
			if (p->s_work2++ > 120 * 5) {
				p->s_work++;
				p->s_work3 = !0;
			}
			if (p->work2 < 128)
				p->work2++;
			p->angle += (p->work2 / 32);
			break;
		case 1:
			if (p->s_work2++ > 120 * 4) {
				p->s_work++;
				p->s_work3 = !0;
			}
			if (p->work2 < 128)
				p->work2++;
			p->angle -= (p->work2 / 32);
			break;
		case 2:
			if (p->s_work2++ > 120 * 5) {
				p->s_work++;
				p->s_work3 = !0;
			}
			if (p->work2 < 128)
				p->work2++;
			p->angle += (p->work2 / 32);
			break;
		case 3:
			if (p->s_work2++ > 120 * 6) {
				p->s_work = 0;	/* 最初に戻る */
				p->s_work3 = !0;
			    /* この時点でパーツを１個も破壊していなければコアが赤くなる */
				if (p->work3 == 0)
					p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
			}
			if (p->work2 < 128)
				p->work2++;
			p->angle -= (p->work2 / 32);
			break;
		}
		if (p->s_work3) {
		    /* 今攻撃が変わったところ */
			p->s_work2 = 0;
			if (p->parts1)
				p->parts1->s_work = 0 * 7;
			if (p->parts2)
				p->parts2->s_work = 1 * 7;
			if (p->parts3)
				p->parts3->s_work = 2 * 7;
			if (p->parts4)
				p->parts4->s_work = 3 * 7;
			if (!eshot_erase)
				eshot_erase = ESHOT_ERASE;	/* 弾消し */
		}
		p->pt = obj_lboss03 + ((p->angle & 0x38) >> 3);

	    /* パーツの有無を調べて全部なければ次の形態へ */
		if ((p->parts1) && (p->parts1->child_death)) {
			p->parts1 = 0;	/* これ以降参照しない */
			p->work3++;
		}
		if ((p->parts2) && (p->parts2->child_death)) {
			p->parts2 = 0;	/* これ以降参照しない */
			p->work3++;
		}
		if ((p->parts3) && (p->parts3->child_death)) {
			p->parts3 = 0;	/* これ以降参照しない */
			p->work3++;
		}
		if ((p->parts4) && (p->parts4->child_death)) {
			p->parts4 = 0;	/* これ以降参照しない */
			p->work3++;
		}
		if (p->work3 >= 4)
			p->seq++;
		break;

	case 6:		/* 止まるまで待つ */
		if (SubEnemyMoveToAbort (p) < 0) {
			SubEnemyMoveToInit (p, 144, 96, SPEED_2, SPEED_2N);
			p->seq++;
			p->cwork = p->cwork2 = 0;
		}
		break;

	case 7:		/* 中央に移動 */
		if (p->angle >= 2) {
			p->angle -= 2;
		} else {
			p->angle = 0;
			p->cwork = !0;
		}
		p->pt = obj_lboss03 + ((p->angle & 0x38) >> 3);
	    /* 設定値に従って移動 */
		if (p->cwork2 == 0) {
			if (SubEnemyMoveTo (p) < 0)
				p->cwork2 = !0;
		}
		if ((p->cwork) && (p->cwork2)) {
			p->seq++;
			p->m_work = 32;	/* コアの座標 */
			p->m_work2 = 0;
		}
		break;
	case 8:		/* コア集積 */
		if (p->m_work2++ > 2) {
			p->m_work2 = 0;
			p->m_work--;
			if (p->m_work <= 6) {
				p->seq++;
				SoundSetSE (SE_LOCK_M);	/* 「がちゃん」という効果音を鳴らす */
				p->cwork = 0;	/* コアの角度 */
				p->cwork2 = 0;
				p->m_work = MOVETO_INIT;	/* 移動方向 */
				p->s_work = p->s_work2 = p->s_work3 = p->s_work4 = p->s_angle = p->s_angle2 = 0;
				p->hit_px = 56;
				p->hit_py = 16;
				p->hit_sx = 56;
				p->hit_sy = 16;
				p->hit_cx = 12;
			}
		}
		break;

	case 9:		/* 第２形態 */
		if (p->timer > TIMER_TIMEOUT) {
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
			{0, 54 + 16, 128 + 16, 256 - 54 + 16, 128 + 16};
			short m_work_table[5] =
			{MOVETO_INIT, MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L};

			p->m_work2 = !0;
		    /* 順に左→中央→右→中央→左へ移動 */
			dx = dx_table[p->m_work];
			p->m_work = m_work_table[p->m_work];

		    /* p->x+player->y は乱数の種として使用しています */
			if (SHORT_LY < 80)
				dy = (((unsigned short) rndtable[(p->x + player->y) & 0xff]) / 4 + 48 + 16);
			else
				dy = 64;
		    /* 移動方向を設定 */
			SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
		} else {
		    /* 設定値に従って移動 */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* 移動方向再設定 */
		}

	    /* コアの回転処理 */
		if ((unsigned char) p->cwork2 < 128)
			p->cwork2 += 2;
		p->cwork += ((unsigned char) p->cwork2 / 32);


	    /* 第２形態・攻撃処理 */
		switch (p->s_work) {
		case 0:
			if (p->s_work2++ > 120 * 5) {
				p->s_work++;
				p->s_work2 = p->s_work3 = p->s_work4 = p->s_angle = p->s_angle2 = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* 弾消し */
			}
			if ((p->s_work2 > 20) && (p->s_work2 < 500)) {
				if (p->s_work3++ > 2) {
					p->s_work3 = 0;
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_01, p->s_angle, 3);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_01, p->s_angle + 64, 3);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_01, p->s_angle + 128, 3);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_01, p->s_angle + 192, 3);
					p->s_angle += 5;
					SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				}
			}
			if (((p->s_work2 > 180) && (p->s_work2 < 250)) ||
			    ((p->s_work2 > 380) && (p->s_work2 < 450))) {
				if (p->s_work4++ > 1) {
					p->s_work4 = 0;
					EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_02, p->s_angle2, 3);
					EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_02, p->s_angle2 + 64, 3);
					EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_02, p->s_angle2 + 128, 3);
					EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_02, p->s_angle2 + 192, 3);
					p->s_angle2 += 12;
					SoundSetSE (SE_ESHOT_M);	/* 敵ショット音 */
				}
			}
			break;
		case 1:
#define	INTERVAL9_11	20	/* 速い炸裂弾を撃つ間隔 */
			switch (p->s_work2) {
			case INTERVAL9_11 * 1:
			case INTERVAL9_11 * 2:
			case INTERVAL9_11 * 3:
			case INTERVAL9_11 * 4:
			case INTERVAL9_11 * 5:
			case INTERVAL9_11 * 11:
			case INTERVAL9_11 * 12:
			case INTERVAL9_11 * 13:
			case INTERVAL9_11 * 14:
			case INTERVAL9_11 * 15:
			case INTERVAL9_11 * 16:
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 0, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 2, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 4, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 6, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 8, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 10, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 12, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 14, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 16, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 18, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 20, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 22, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 24, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 26, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 28, 3);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 30, 3);
				SoundSetSE (SE_EBOMB_L);
				break;
			case INTERVAL9_11 * 1 + 1:
			case INTERVAL9_11 * 2 + 1:
			case INTERVAL9_11 * 3 + 1:
			case INTERVAL9_11 * 4 + 1:
			case INTERVAL9_11 * 5 + 1:
			case INTERVAL9_11 * 11 + 1:
			case INTERVAL9_11 * 12 + 1:
			case INTERVAL9_11 * 13 + 1:
			case INTERVAL9_11 * 14 + 1:
			case INTERVAL9_11 * 15 + 1:
			case INTERVAL9_11 * 16 + 1:
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 1, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 3, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 5, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 7, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 9, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 11, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 13, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 15, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 17, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 19, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 21, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 23, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 25, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 27, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 29, 4);
				EshotInit (ESHOT_NRG03, SHORT_LX, SHORT_LY, SPEED_SHOT9_11, 8 * 31, 4);
				break;
			default:
				break;
			}

#define	INTERVAL9_12	27	/* 遅い炸裂弾を撃つ間隔 */
			switch (p->s_work2) {
			case INTERVAL9_12 * 1:
			case INTERVAL9_12 * 2:
			case INTERVAL9_12 * 3:
			case INTERVAL9_12 * 4:
			case INTERVAL9_12 * 8:
			case INTERVAL9_12 * 9:
			case INTERVAL9_12 * 10:
			case INTERVAL9_12 * 11:
			case INTERVAL9_12 * 12:
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 0, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 2, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 4, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 6, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 8, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 10, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 12, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 14, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 16, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 18, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 20, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 22, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 24, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 26, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 28, 3);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 30, 3);
				SoundSetSE (SE_EBOMB_M);
				break;
			case INTERVAL9_12 * 1 + 1:
			case INTERVAL9_12 * 2 + 1:
			case INTERVAL9_12 * 3 + 1:
			case INTERVAL9_12 * 4 + 1:
			case INTERVAL9_12 * 8 + 1:
			case INTERVAL9_12 * 9 + 1:
			case INTERVAL9_12 * 10 + 1:
			case INTERVAL9_12 * 11 + 1:
			case INTERVAL9_12 * 12 + 1:
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 1, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 3, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 5, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 7, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 9, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 11, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 13, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 15, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 17, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 19, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 21, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 23, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 25, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 27, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 29, 4);
				EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_12, p->s_angle2 + 8 * 31, 4);
				p->s_angle2 += 2;
				break;
			default:
				break;
			}
			if (p->s_work2++ > 120 * 4) {
				p->s_work++;
				p->s_work2 = p->s_work3 = p->s_work4 = p->s_angle = p->s_angle2 = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* 弾消し */
			}
			break;
		case 2:
#define	INTERVAL9_2		90	/* 円形弾を撃つ間隔 */
			switch (p->s_work2) {
			case INTERVAL9_2 * 1:
			case INTERVAL9_2 * 2:
			case INTERVAL9_2 * 3:
			case INTERVAL9_2 * 4:
			case INTERVAL9_2 * 5:
			case INTERVAL9_2 * 6:
				p->s_angle = psearch (p->x, p->y);	/* 自機の方向をサーチ */
				for (h = 0; h < 16; h++)
					EshotInit (ESHOT_NRG02, p->x + shot_pos1[h][0], p->y + shot_pos1[h][1],
						   SPEED_SHOT9_21, p->s_angle, 2);
				SoundSetSE (SE_EBOMB_M);
				break;
			case INTERVAL9_2 * 1 + 1:
			case INTERVAL9_2 * 2 + 1:
			case INTERVAL9_2 * 3 + 1:
			case INTERVAL9_2 * 4 + 1:
			case INTERVAL9_2 * 5 + 1:
			case INTERVAL9_2 * 6 + 1:
				for (h = 0; h < 8; h++)
					EshotInit (ESHOT_NRG03, p->x + shot_pos2[h][0], p->y + shot_pos2[h][1],
						   SPEED_SHOT9_22, p->s_angle, 2);
				break;
			case INTERVAL9_2 * 1 + 2:
			case INTERVAL9_2 * 2 + 2:
			case INTERVAL9_2 * 3 + 2:
			case INTERVAL9_2 * 4 + 2:
			case INTERVAL9_2 * 5 + 2:
			case INTERVAL9_2 * 6 + 2:
				for (h = 0; h < 8; h++)
					EshotInit (ESHOT_NRG04, p->x + shot_pos3[h][0], p->y + shot_pos3[h][1],
						   SPEED_SHOT9_23, p->s_angle, 2);
				break;
			case INTERVAL9_2 * 1 + 3:
			case INTERVAL9_2 * 2 + 3:
			case INTERVAL9_2 * 3 + 3:
			case INTERVAL9_2 * 4 + 3:
			case INTERVAL9_2 * 5 + 3:
			case INTERVAL9_2 * 6 + 3:
				EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT9_24, p->s_angle, 2);
				break;
			default:
				break;
			}
			if (p->s_work2++ > 120 * 5) {
			    /* 最初に戻る */
				p->s_work = p->s_work2 = p->s_work3 = p->s_work4 = p->s_angle = p->s_angle2 = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* 弾消し */
			}
			break;
		default:	/* ここに来たらバグ */
			p->s_work = 0;	/* バグよけ */
			break;
		}


		break;

	case SEQ_TIMEOUT:
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
							    (signed short) ((rndtable[h + 1] - 128) / 2) + p->y);
					}
					SoundSetSE (SE_EXPL_L);	/* ボスキャラ爆発音 */
					if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE))
						EffectInit (EFFECT_POINTS_RED, POINTS_20000, p->x, p->y);
					else
						EffectInit (EFFECT_POINTS_BLUE, POINTS_9000, p->x, p->y);
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
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 500)) {
			p->info = PALET_RED | PRIORITY_BOSS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = p->work4;
		}
	}
	xobj_set_st (p);

    /* コアの表示 */
	switch (p->seq) {
	case 3:		/* コア外側に */
	case 4:
	case 5:
	case 6:
	case 7:
		{
			unsigned char a;
			a = p->angle + 32 + 64 * 0;
			a /= 8;
			p->core_x = p->x + core_table1[a][0] - 8;
			p->core_y = p->y - core_table1[a][1] - 8;
			xsp_set_st (&(p->core_x));
			a = p->angle + 32 + 64 * 1;
			a /= 8;
			p->core_x = p->x + core_table1[a][0] - 8;
			p->core_y = p->y - core_table1[a][1] - 8;
			xsp_set_st (&(p->core_x));
			a = p->angle + 32 + 64 * 2;
			a /= 8;
			p->core_x = p->x + core_table1[a][0] - 8;
			p->core_y = p->y - core_table1[a][1] - 8;
			xsp_set_st (&(p->core_x));
			a = p->angle + 32 + 64 * 3;
			a /= 8;
			p->core_x = p->x + core_table1[a][0] - 8;
			p->core_y = p->y - core_table1[a][1] - 8;
			xsp_set_st (&(p->core_x));
		}
		break;
	case 8:		/* コア集積 */
		p->core_x = p->x - p->m_work - 8;
		p->core_y = p->y - p->m_work - 8;
		xsp_set_st (&(p->core_x));
		p->core_x = p->x + p->m_work - 8;
		xsp_set_st (&(p->core_x));
		p->core_y = p->y + p->m_work - 8;
		xsp_set_st (&(p->core_x));
		p->core_x = p->x - p->m_work - 8;
		xsp_set_st (&(p->core_x));
		break;
	case 9:		/* コア内側に */
	case SEQ_TIMEOUT:
	case SEQ_DEATH:
		{
			unsigned char a;
			a = p->cwork + 32 + 64 * 0;
			a /= 8;
			p->core_x = p->x + core_table2[a][0] - 8;
			p->core_y = p->y - core_table2[a][1] - 8;
			xsp_set_st (&(p->core_x));
			a = p->cwork + 32 + 64 * 1;
			a /= 8;
			p->core_x = p->x + core_table2[a][0] - 8;
			p->core_y = p->y - core_table2[a][1] - 8;
			xsp_set_st (&(p->core_x));
			a = p->cwork + 32 + 64 * 2;
			a /= 8;
			p->core_x = p->x + core_table2[a][0] - 8;
			p->core_y = p->y - core_table2[a][1] - 8;
			xsp_set_st (&(p->core_x));
			a = p->cwork + 32 + 64 * 3;
			a /= 8;
			p->core_x = p->x + core_table2[a][0] - 8;
			p->core_y = p->y - core_table2[a][1] - 8;
			xsp_set_st (&(p->core_x));
		}
		break;
	default:
		break;
	}
	return (0);
}



static void EnemyTiniLBoss03 (ENEMY * p)
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
