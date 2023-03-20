/* �R�ʃ{�X */
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


#define SPEED_1		3	/* �����x */
#define SPEED_1N	28	/* �����x�𑫂��� */
#define SPEED_2		15	/* �����x */
#define SPEED_2N	10	/* �����x�𑫂��� */

#define SPEED_3		3	/* �R�i�K�ځi�P�ށj�̉����x */
#define SPEED_3N	32	/* �R�i�K�ڂ̉����x�𑫂��� */


#define SPEED_SHOT1	8	/* �P�i�K�ڐ�^�e���x */
#define SPEED_SHOT2	10	/* �P�i�K�ڒe�̑��x */
#define SPEED_SHOT3	14	/* �P�i�K�ڒe�̑��x */
#define SPEED_SHOT4	10	/* ��Q�`�ԁE�O�p�e�̑��x */

#define SPEED_SHOT9_01	14	/* ��Q�`�ԁE�点��e�i�ᑬ�j�̑��x */
#define SPEED_SHOT9_02	20	/* ��Q�`�ԁE�点��e�i�����j�̑��x */

#define SPEED_SHOT9_11	20	/* ��Q�`�ԁE�点��e�i�����j�̑��x */
#define SPEED_SHOT9_12	10	/* ��Q�`�ԁE�点��e�i�����j�̑��x */

#define SPEED_SHOT9_21	8	/* ��Q�`�ԁE�~�`�e�̑��x */
#define SPEED_SHOT9_22	14	/* ��Q�`�ԁE�~�`�e�̑��x */
#define SPEED_SHOT9_23	20	/* ��Q�`�ԁE�~�`�e�̑��x */
#define SPEED_SHOT9_24	26	/* ��Q�`�ԁE�~�`�e�̑��x */


#define HP_0	5000		/* �ϋv�� */

#define TIMER_TIMEOUT	9000	/* ����ȏ㎞�Ԃ��o�߂�����P�� */

#define SEQ_TIMEOUT	10
#define SEQ_DEATH	11



enum {
	MOVETO_INIT = 0,	/* ������� */
	MOVETO_C2L,		/* �������獶�Ɉړ� */
	MOVETO_L2C,
	MOVETO_C2R,
	MOVETO_R2C
};

enum {
	ATTACK_UPPER_CORE = 0,	/* ��̃R�A����U���i�L�т�e�j */
	ATTACK_LOWER_CORE	/* ���̃R�A����U���i�΂�܂��e�j */
};

static short EnemyMoveLBoss03 (ENEMY *);
static void EnemyTiniLBoss03 (ENEMY *);


void EnemyInitLBoss03 (ENEMY * p)
{
	p->vx = 0;
	p->vy = 3 << 16;

    /* ������Ԃ̓��蔻�� */
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

	entry_counter_stop = !0;/* �G���g���[�J�E���^�[��~ */
}



static short EnemyMoveLBoss03 (ENEMY * p)
{
    /* �R�A���O���ɂ��鎞�̍��W */
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
    /* �R�A�������ɂ��鎞�̍��W */
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
    /* �V���b�g�p�E��Q�`�ԉ~�`�� */
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
    /* �V���b�g�p�E��Q�`�ԉ~�`�� */
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
    /* �V���b�g�p�E��Q�`�ԉ~�`�� */
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


    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
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
	case 1:		/* ���� */
		p->vy -= 4096;
		if (p->vy < 4096) {
			p->seq++;
			p->vy = 0;
			p->m_work = MOVETO_INIT;	/* �ړ����� */
		}
		switch (p->m_work++) {
		case 2:
			p->parts1 = EnemyInit (ENEMY_LBOSS03U, 111, 256 + 66, 0 + p->arg * 4, p);	/* �p�[�c�P�o�� */
			break;
		case 7:
			p->parts2 = EnemyInit (ENEMY_LBOSS03U, 177, 256 + 66, 1 + p->arg * 4, p);	/* �p�[�c�Q�o�� */
			break;
		case 5:
			p->parts3 = EnemyInit (ENEMY_LBOSS03U, 111, 256 + 66, 2 + p->arg * 4, p);	/* �p�[�c�R�o�� */
			break;
		case 9:
			p->parts4 = EnemyInit (ENEMY_LBOSS03U, 177, 256 + 66, 3 + p->arg * 4, p);	/* �p�[�c�S�o�� */
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
			p->angle = 0;	/* �p�x(0~8) */
			SoundSetSE (SE_LOCK_M);	/* �u�������v�Ƃ������ʉ���炷 */
		}
		break;
	case 3:		/* �p�[�c���u�������v�Ƃ������A�j���[�V���� */
		p->pt++;
		if (p->pt >= obj_lboss03 + 33) {
			p->seq++;
			p->pt = obj_lboss03;
			p->work2 = 0;	/* �p�x�̉����x�Ɏg�p */
			p->work3 = 0;	/* �p�[�c��j�󂵂��� */
			p->angle = 0;	/* �p�x�Ɏg�p */
			p->parts1 = EnemyInit (ENEMY_LBOSS03P, 0, 0, 0 + p->arg * 4, p);	/* �p�[�c�P�o�� */
			p->parts2 = EnemyInit (ENEMY_LBOSS03P, 0, 0, 1 + p->arg * 4, p);	/* �p�[�c�Q�o�� */
			p->parts3 = EnemyInit (ENEMY_LBOSS03P, 0, 0, 2 + p->arg * 4, p);	/* �p�[�c�R�o�� */
			p->parts4 = EnemyInit (ENEMY_LBOSS03P, 0, 0, 3 + p->arg * 4, p);	/* �p�[�c�S�o�� */
			p->s_work = 0;
			SubEnemyMoveToInit (p, 144, 96, SPEED_1, SPEED_1N);
		}
		break;

	case 4:		/* ������� */
		if (SubEnemyMoveTo (p) < 0) {
			p->seq++;
			p->m_work = MOVETO_INIT;	/* �ړ����� */
			p->s_work = 0;
			p->s_work2 = 0;
		}
		break;

	case 5:		/* ��P�`�� */
	    /* �܂��ړ����� */
		if (p->m_work == MOVETO_INIT) {
		    /* ������ԂȂ� */
			if (p->x > player->x)
				p->m_work = MOVETO_C2L;	/* �ړ����� */
			else
				p->m_work = MOVETO_C2R;	/* �ړ����� */
			p->m_work2 = 0;
		}
		if (p->m_work2 == 0) {
		    /* ����Ȃ� */
			signed short dx, dy;
			signed short dx_table[5] =
			{0, 54 + 16, 128 + 16, 256 - 54 + 16, 128 + 16};
			short m_work_table[5] =
			{MOVETO_INIT, MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L};

			p->m_work2 = !0;
		    /* ���ɍ����������E�����������ֈړ� */
			dx = dx_table[p->m_work];
			p->m_work = m_work_table[p->m_work];

		    /* player->y �͗����̎�Ƃ��Ďg�p���Ă��܂� */
			if (SHORT_LY < 80)
				dy = (((unsigned short) rndtable[(player->y) & 0xff]) / 4 + 32 + 16);
			else
				dy = 64;
		    /* �ړ�������ݒ� */
			SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
		} else {
		    /* �ݒ�l�ɏ]���Ĉړ� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* �ړ������Đݒ� */
		}


	    /* �U������]�p�̏��� */
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
				p->s_work = 0;	/* �ŏ��ɖ߂� */
				p->s_work3 = !0;
			    /* ���̎��_�Ńp�[�c���P���j�󂵂Ă��Ȃ���΃R�A���Ԃ��Ȃ� */
				if (p->work3 == 0)
					p->core_info = PALET_CORE_RED | PRIORITY_BOSS_CORE;
			}
			if (p->work2 < 128)
				p->work2++;
			p->angle -= (p->work2 / 32);
			break;
		}
		if (p->s_work3) {
		    /* ���U�����ς�����Ƃ��� */
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
				eshot_erase = ESHOT_ERASE;	/* �e���� */
		}
		p->pt = obj_lboss03 + ((p->angle & 0x38) >> 3);

	    /* �p�[�c�̗L���𒲂ׂđS���Ȃ���Ύ��̌`�Ԃ� */
		if ((p->parts1) && (p->parts1->child_death)) {
			p->parts1 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
			p->work3++;
		}
		if ((p->parts2) && (p->parts2->child_death)) {
			p->parts2 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
			p->work3++;
		}
		if ((p->parts3) && (p->parts3->child_death)) {
			p->parts3 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
			p->work3++;
		}
		if ((p->parts4) && (p->parts4->child_death)) {
			p->parts4 = 0;	/* ����ȍ~�Q�Ƃ��Ȃ� */
			p->work3++;
		}
		if (p->work3 >= 4)
			p->seq++;
		break;

	case 6:		/* �~�܂�܂ő҂� */
		if (SubEnemyMoveToAbort (p) < 0) {
			SubEnemyMoveToInit (p, 144, 96, SPEED_2, SPEED_2N);
			p->seq++;
			p->cwork = p->cwork2 = 0;
		}
		break;

	case 7:		/* �����Ɉړ� */
		if (p->angle >= 2) {
			p->angle -= 2;
		} else {
			p->angle = 0;
			p->cwork = !0;
		}
		p->pt = obj_lboss03 + ((p->angle & 0x38) >> 3);
	    /* �ݒ�l�ɏ]���Ĉړ� */
		if (p->cwork2 == 0) {
			if (SubEnemyMoveTo (p) < 0)
				p->cwork2 = !0;
		}
		if ((p->cwork) && (p->cwork2)) {
			p->seq++;
			p->m_work = 32;	/* �R�A�̍��W */
			p->m_work2 = 0;
		}
		break;
	case 8:		/* �R�A�W�� */
		if (p->m_work2++ > 2) {
			p->m_work2 = 0;
			p->m_work--;
			if (p->m_work <= 6) {
				p->seq++;
				SoundSetSE (SE_LOCK_M);	/* �u�������v�Ƃ������ʉ���炷 */
				p->cwork = 0;	/* �R�A�̊p�x */
				p->cwork2 = 0;
				p->m_work = MOVETO_INIT;	/* �ړ����� */
				p->s_work = p->s_work2 = p->s_work3 = p->s_work4 = p->s_angle = p->s_angle2 = 0;
				p->hit_px = 56;
				p->hit_py = 16;
				p->hit_sx = 56;
				p->hit_sy = 16;
				p->hit_cx = 12;
			}
		}
		break;

	case 9:		/* ��Q�`�� */
		if (p->timer > TIMER_TIMEOUT) {
			p->seq++;	/* ���Ԑ؂� */
			p->m_work = 0;
			break;
		}
	    /* �܂��ړ����� */
		if (p->m_work == MOVETO_INIT) {
		    /* ������ԂȂ� */
			if (p->x > player->x)
				p->m_work = MOVETO_C2L;	/* �ړ����� */
			else
				p->m_work = MOVETO_C2R;	/* �ړ����� */
			p->m_work2 = 0;
		}
		if (p->m_work2 == 0) {
		    /* ����Ȃ� */
			signed short dx, dy;
			signed short dx_table[5] =
			{0, 54 + 16, 128 + 16, 256 - 54 + 16, 128 + 16};
			short m_work_table[5] =
			{MOVETO_INIT, MOVETO_L2C, MOVETO_C2R, MOVETO_R2C, MOVETO_C2L};

			p->m_work2 = !0;
		    /* ���ɍ����������E�����������ֈړ� */
			dx = dx_table[p->m_work];
			p->m_work = m_work_table[p->m_work];

		    /* p->x+player->y �͗����̎�Ƃ��Ďg�p���Ă��܂� */
			if (SHORT_LY < 80)
				dy = (((unsigned short) rndtable[(p->x + player->y) & 0xff]) / 4 + 48 + 16);
			else
				dy = 64;
		    /* �ړ�������ݒ� */
			SubEnemyMoveToInit (p, dx, dy, SPEED_1, SPEED_1N);
		} else {
		    /* �ݒ�l�ɏ]���Ĉړ� */
			if (SubEnemyMoveTo (p) < 0)
				p->m_work2 = 0;	/* �ړ������Đݒ� */
		}

	    /* �R�A�̉�]���� */
		if ((unsigned char) p->cwork2 < 128)
			p->cwork2 += 2;
		p->cwork += ((unsigned char) p->cwork2 / 32);


	    /* ��Q�`�ԁE�U������ */
		switch (p->s_work) {
		case 0:
			if (p->s_work2++ > 120 * 5) {
				p->s_work++;
				p->s_work2 = p->s_work3 = p->s_work4 = p->s_angle = p->s_angle2 = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			if ((p->s_work2 > 20) && (p->s_work2 < 500)) {
				if (p->s_work3++ > 2) {
					p->s_work3 = 0;
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_01, p->s_angle, 3);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_01, p->s_angle + 64, 3);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_01, p->s_angle + 128, 3);
					EshotInit (ESHOT_NRG02, SHORT_LX, SHORT_LY, SPEED_SHOT9_01, p->s_angle + 192, 3);
					p->s_angle += 5;
					SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
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
					SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
				}
			}
			break;
		case 1:
#define	INTERVAL9_11	20	/* �����y���e�����Ԋu */
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

#define	INTERVAL9_12	27	/* �x���y���e�����Ԋu */
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
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			break;
		case 2:
#define	INTERVAL9_2		90	/* �~�`�e�����Ԋu */
			switch (p->s_work2) {
			case INTERVAL9_2 * 1:
			case INTERVAL9_2 * 2:
			case INTERVAL9_2 * 3:
			case INTERVAL9_2 * 4:
			case INTERVAL9_2 * 5:
			case INTERVAL9_2 * 6:
				p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
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
			    /* �ŏ��ɖ߂� */
				p->s_work = p->s_work2 = p->s_work3 = p->s_work4 = p->s_angle = p->s_angle2 = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
			}
			break;
		default:	/* �����ɗ�����o�O */
			p->s_work = 0;	/* �o�O�悯 */
			break;
		}


		break;

	case SEQ_TIMEOUT:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				SubEnemyMoveToInit (p, 128 + 16, 160, SPEED_3, SPEED_3N);
			}
			break;
		case 1:	/* ��U�����܂ňړ����Ă���P�� */
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				SubEnemyMoveToInit (p, p->x, -128, SPEED_3, SPEED_3N);
			}
			break;
		case 2:
			if (SubEnemyMoveTo (p) < 0) {	/* �~�܂�܂ő҂� */
				return (-1);	/* ���� */
			}
			break;
		}
		break;

	case SEQ_DEATH:
		switch (p->m_work) {
		case 0:
			if (SubEnemyMoveToAbort (p) < 0) {	/* �~�܂�܂ő҂� */
				p->m_work++;
				p->m_work2 = 0;
				p->m_work3 = 0;
			    /* �����蔻��𖳂��� */
				p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
				if (!eshot_erase)
					eshot_erase = ESHOT_ERASE;	/* �e���� */
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
				    /* p->timer �͗����̎�Ƃ��Ďg�p���Ă��܂� */
					EffectInit (EFFECT_EXPL, 0, x / 2 + p->x, y / 4 + p->y);
					SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
				}
				if (p->m_work3 > 20) {
					short h;
					for (h = 0; h < 6; h++) {
						EffectInit (EFFECT_EXPL, 0,
							    (signed short) ((rndtable[h] - 128) / 2) + p->x,
							    (signed short) ((rndtable[h + 1] - 128) / 2) + p->y);
					}
					SoundSetSE (SE_EXPL_L);	/* �{�X�L���������� */
					if (p->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE))
						EffectInit (EFFECT_POINTS_RED, POINTS_20000, p->x, p->y);
					else
						EffectInit (EFFECT_POINTS_BLUE, POINTS_9000, p->x, p->y);
					EffectInit (EFFECT_HAHEN, 0, p->x, p->y);

					return (-1);	/* ���� */
				}
			}
			break;
		}
		break;
	default:
		break;
	}


    /* ��ʊO�ɏo������� */
	if (SHORT_LY > 256 + 32)
		return (-1);	/* ���� */

    /* ���@�V���b�g�ɓ����������̏��� */
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
		    /* ���� */
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

    /* �R�A�̕\�� */
	switch (p->seq) {
	case 3:		/* �R�A�O���� */
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
	case 8:		/* �R�A�W�� */
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
	case 9:		/* �R�A������ */
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
    /* �p�[�c�����݂��Ă���������悤�Ɏw�� */
	if (p->parts1)
		p->parts1->child_kill = !0;
	if (p->parts2)
		p->parts2->child_kill = !0;
	if (p->parts3)
		p->parts3->child_kill = !0;
	if (p->parts4)
		p->parts4->child_kill = !0;
	entry_counter_stop = 0;	/* �G���g���[�J�E���^�[��~���� */
}
