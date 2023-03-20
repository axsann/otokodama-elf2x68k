/* �R�ʃ{�X�p�[�c (03p = 03 parts) */
/* �{�X�ɕt���Ă���S�̃p�[�c */
#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"
#include "../psearch.h"


/* �R�ʃ{�X�Ƃ��ēo�ꎞ */
#define HP_0	1200	/* ����̃p�[�c�̑ϋv�� */
#define HP_1	180		/* �E��́@�@�@�V�@�@�@ */
#define HP_2	1100		/* �E���́@�@�@�V�@�@�@ */
#define HP_3	150		/* �����́@�@�@�V�@�@�@ */

/* �S�ʃ{�X���b�V���� */
#define HP_4	1200	/* ����̃p�[�c�̑ϋv�� */
#define HP_5	900		/* �E��́@�@�@�V�@�@�@ */
#define HP_6	1100	/* �E���́@�@�@�V�@�@�@ */
#define HP_7	1000	/* �����́@�@�@�V�@�@�@ */

#define TIMER_TIMEOUT	6000	/* ����ȏ㎞�Ԃ��o�߂����玩�� */

#define SPEED_SHOT1	8	/* �P�i�K�ڐ��e�̑��x */
#define SPEED_SHOT2	12	/* �P�i�K�ڐ��e�̑��x */

#define SPEED_SHOT1_1	14	/* ���܏�e�̑��x */
#define SPEED_SHOT1_2	16	/* �����e�̑��x */

#define SPEED_SHOT2_1	10	/* �~�`�e�̒x���e�̑��x */
#define SPEED_SHOT2_2	14	/* �~�`�e�̑����e�̑��x */

#define SPEED_SHOT3_1	8
#define SPEED_SHOT3_2	17

#define SEQ_DEATH	3


static signed short shot_pos[32][2] =
{
	{0, 24},
	{4, 23},
	{9, 22},
	{13, 19},
	{16, 16},
	{19, 13},
	{22, 9},
	{23, 4},
	{24, 0},
	{23, -4},
	{22, -9},
	{19, -13},
	{16, -16},
	{13, -19},
	{9, -22},
	{4, -23},
	{0, -24},
	{-4, -23},
	{-9, -22},
	{-13, -19},
	{-16, -16},
	{-19, -13},
	{-22, -9},
	{-23, -4},
	{-24, 0},
	{-23, 4},
	{-22, 9},
	{-19, 13},
	{-16, 16},
	{-13, 19},
	{-9, 22},
	{-4, 23},
};

static short EnemyMoveLBoss03P (ENEMY *);
static void EnemyTiniLBoss03P (ENEMY *);


void EnemyInitLBoss03P (ENEMY * p)
{
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->timer = 0;
	p->damage = 0;
	p->flash = 0;
	p->seq = 0;
	p->pt = obj_lboss03 + 8;
	p->m_work = 0;
	p->s_work = p->arg * 7;	/* �V���b�g�̎��������炷 */
	p->s_work2 = 0;
	p->func_enemy_move = EnemyMoveLBoss03P;
	p->func_enemy_tini = EnemyTiniLBoss03P;

	if (p->arg < 4) {
		/* �R�ʃ{�X�Ƃ��ēo�ꎞ */
		signed short hp_table[4] =
		{HP_0, HP_1, HP_2, HP_3};
		p->work4 = PALET_LBOSS03 | PRIORITY_BOSS_PARTS_L;
		p->hp = hp_table[p->arg];	/* ���̓p�[�c�ɂ���đϋv�͂��Ⴄ */
	} else {
		/* �S�ʃ{�X���b�V���� */
		signed short hp_table[4] =
		{HP_4, HP_5, HP_6, HP_7};
		p->arg -= 4;
		p->work4 = PALET_LBOSS03_1 | PRIORITY_BOSS_PARTS_L;
		p->hp = hp_table[p->arg];	/* ���̓p�[�c�ɂ���đϋv�͂��Ⴄ */
	}
}



static short EnemyMoveLBoss03P (ENEMY * p)
{
	short h;

	if ((p->child_kill) || (p->child_death))
		return (-1);	/* ���� */
	p->timer++;
	if (p->timer > TIMER_TIMEOUT) {
		EffectInit (EFFECT_EXPL, 0, p->x, p->y);
		SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
		p->child_death = !0;	/* �������� */
	} {
		unsigned char a;
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
		/* �\�����W�̏��� */
		switch (p->arg) {
		case 0:
			a = p->parent->angle + 32 + 64 * 0;
			a /= 8;
			p->x = p->parent->x + core_table1[a][0];
			p->y = p->parent->y - core_table1[a][1] - 8;
			break;
		case 1:
			a = p->parent->angle + 32 + 64 * 1;
			a /= 8;
			p->x = p->parent->x + core_table1[a][0];
			p->y = p->parent->y - core_table1[a][1] - 8;
			break;
		case 2:
			a = p->parent->angle + 32 + 64 * 2;
			a /= 8;
			p->x = p->parent->x + core_table1[a][0];
			p->y = p->parent->y - core_table1[a][1] - 8;
			break;
		case 3:
			a = p->parent->angle + 32 + 64 * 3;
			a /= 8;
			p->x = p->parent->x + core_table1[a][0];
			p->y = p->parent->y - core_table1[a][1] - 8;
			break;
		}
		p->pt = obj_lboss03 + 8 + ((p->parent->angle & 0x38) >> 3);
	}


	/* �U������ */
	switch (p->parent->s_work) {
	case 0:		/* ���e */
		p->s_work++;
		switch (p->s_work) {
#define ANGLE1	12
			/* case �̏��Ԃɒ��� */
		case 62:
			p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
			EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, p->s_angle - ANGLE1 * 3, 2);
			EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, p->s_angle - ANGLE1 * 1, 2);
			EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, p->s_angle + ANGLE1 * 1, 2);
			EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT2, p->s_angle + ANGLE1 * 3, 2);
			/* ������ break ���Ȃ����ɒ��� */
		case 2:
			p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT1, p->s_angle - ANGLE1 * 3, 2);
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT1, p->s_angle - ANGLE1 * 2, 2);
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT1, p->s_angle - ANGLE1 * 1, 2);
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT1, p->s_angle + ANGLE1 * 0, 2);
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT1, p->s_angle + ANGLE1 * 1, 2);
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT1, p->s_angle + ANGLE1 * 2, 2);
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT1, p->s_angle + ANGLE1 * 3, 2);
			SoundSetSE (SE_EBOMB_L);
			break;
		case 120:
			p->s_work = 0;
			break;
		default:
			break;
		}
		break;

	case 1:		/* ���܏�e */
		p->s_work++;
		switch (p->s_work) {
#define INTERVAL2	3
			/* case �̏��Ԃɒ��� */
		case 2 + INTERVAL2 * 0:
			p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
			p->s_work2 = SPEED_SHOT1_1;
		case 2 + INTERVAL2 * 1:
		case 2 + INTERVAL2 * 2:
		case 2 + INTERVAL2 * 3:
		case 2 + INTERVAL2 * 4:
		case 2 + INTERVAL2 * 5:
		case 2 + INTERVAL2 * 6:
		case 2 + INTERVAL2 * 7:
		case 2 + INTERVAL2 * 8:
		case 2 + INTERVAL2 * 9:
		case 2 + INTERVAL2 * 10:
			EshotInit (ESHOT_NRG03, p->x, p->y, p->s_work2, p->s_angle, 2);
			SoundSetSE (SE_ESHOT);	/* �G�V���b�g�� */
			p->s_work2 += 1;
			break;
		case 80:
			p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
			EshotInit (ESHOT_NRG24N, p->x, p->y, SPEED_SHOT1_2 + 4 * 0, p->s_angle, 2);
			EshotInit (ESHOT_NRG24N, p->x, p->y, SPEED_SHOT1_2 + 4 * 1, p->s_angle, 2);
			//EshotInit (ESHOT_NRG24N, p->x, p->y, SPEED_SHOT1_2 + 4 * 2, p->s_angle, 2);
			SoundSetSE (SE_ESHOT_M);	/* �G�V���b�g�� */
			break;
		case 120:
			p->s_work = 0;
			break;
		default:
			break;
		}
		break;


	case 2:		/* �~�`�e */
		p->s_work++;
		switch (p->s_work) {
			/* case �̏��Ԃɒ��� */
		case 20:
			p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
			for (h = 0; h < 32; h += 4)
				EshotInit (ESHOT_NRG02, p->x + shot_pos[h][0], p->y + shot_pos[h][1],
					   SPEED_SHOT2_1, p->s_angle, 2);
			SoundSetSE (SE_EBOMB_M);
			break;
		case 50:
			p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
			for (h = 0; h < 32; h += 4)
				EshotInit (ESHOT_NRG02, p->x + shot_pos[h][0], p->y + shot_pos[h][1],
					   SPEED_SHOT2_2, p->s_angle + 16, 2);
			SoundSetSE (SE_EBOMB_M);
			break;
		case 80:
			p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
			for (h = 0; h < 32; h += 4)
				EshotInit (ESHOT_NRG02, p->x + shot_pos[h][0], p->y + shot_pos[h][1],
					   SPEED_SHOT2_1, p->s_angle - 16, 2);
			SoundSetSE (SE_EBOMB_M);
			break;
		case 150:
			for (h = 0; h < 32; h += 4)
				EshotInit (ESHOT_NRG02, p->x + shot_pos[h][0], p->y + shot_pos[h][1],
					   SPEED_SHOT2_2, 64, 2);
			SoundSetSE (SE_EBOMB_M);
			break;
		case 180:
			for (h = 0; h < 32; h += 4)
				EshotInit (ESHOT_NRG02, p->x + shot_pos[h][0], p->y + shot_pos[h][1],
					   SPEED_SHOT2_2, 64 + 16, 2);
			SoundSetSE (SE_EBOMB_M);
			break;
		case 210:
			for (h = 0; h < 32; h += 4)
				EshotInit (ESHOT_NRG02, p->x + shot_pos[h][0], p->y + shot_pos[h][1],
					   SPEED_SHOT2_2, 64 - 16, 2);
			SoundSetSE (SE_EBOMB_M);
			break;
		case 240:
			p->s_work = 0;
			break;
		default:
			break;
		}
		break;

	case 3:		/* �y���e */
		p->s_work++;
		switch (p->s_work) {
			/* case �̏��Ԃɒ��� */
		case 20:
			p->s_angle = psearch (p->x, p->y);	/* ���@�̕������T�[�` */
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT3_1 + 2 * 0, p->s_angle, 2);
			EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT3_1 + 2 * 1, p->s_angle, 2);
			EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT3_1 + 2 * 2, p->s_angle, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 3, p->s_angle, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 4, p->s_angle, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 5, p->s_angle, 2);
			EshotInit (ESHOT_NRG06, p->x, p->y, SPEED_SHOT3_1 + 2 * 6, p->s_angle, 2);
			EshotInit (ESHOT_NRG35, p->x, p->y, SPEED_SHOT3_1 + 2 * 7, p->s_angle, 2);
			EshotInit (ESHOT_NRG35, p->x, p->y, SPEED_SHOT3_1 + 2 * 8, p->s_angle, 2);
			EshotInit (ESHOT_NRG35, p->x, p->y, SPEED_SHOT3_1 + 2 * 9, p->s_angle, 2);
			SoundSetSE (SE_EXLASER);
			break;
		case 50:
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT3_1 + 2 * 0, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT3_1 + 2 * 1, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT3_1 + 2 * 2, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 3, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 4, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 5, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG06, p->x, p->y, SPEED_SHOT3_1 + 2 * 6, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG34N, p->x, p->y, SPEED_SHOT3_1 + 2 * 7, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG34N, p->x, p->y, SPEED_SHOT3_1 + 2 * 8, p->s_angle + 32, 2);
			EshotInit (ESHOT_NRG34N, p->x, p->y, SPEED_SHOT3_1 + 2 * 9, p->s_angle + 32, 2);
			SoundSetSE (SE_EXLASER);
			break;
		case 80:
			EshotInit (ESHOT_NRG02, p->x, p->y, SPEED_SHOT3_1 + 2 * 0, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG03, p->x, p->y, SPEED_SHOT3_1 + 2 * 1, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG04, p->x, p->y, SPEED_SHOT3_1 + 2 * 2, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 3, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 4, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG05, p->x, p->y, SPEED_SHOT3_1 + 2 * 5, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG06, p->x, p->y, SPEED_SHOT3_1 + 2 * 6, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG34N, p->x, p->y, SPEED_SHOT3_1 + 2 * 7, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG34N, p->x, p->y, SPEED_SHOT3_1 + 2 * 8, p->s_angle - 32, 2);
			EshotInit (ESHOT_NRG34N, p->x, p->y, SPEED_SHOT3_1 + 2 * 9, p->s_angle - 32, 2);
			SoundSetSE (SE_EXLASER);
			break;
		case 120:
			p->s_work = 0;
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}



	/* ���@�V���b�g�ɓ����������̏��� */
	if (p->damage) {
		p->info = PALET_RED | PRIORITY_BOSS_PARTS_L;
		p->damage_core = 0;
		if ((p->hp -= p->damage) <= 0) {
			/* ���� */
			p->m_work = 0;
			p->hit_px = p->hit_py = p->hit_sx = p->hit_sy = p->hit_cx = 0;
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL_M);	/* ���{�X������ */
			if (p->parent->core_info == (PALET_CORE_RED | PRIORITY_BOSS_CORE))
				EffectInit (EFFECT_POINTS_RED, POINTS_5000, p->x, p->y);
			else
				EffectInit (EFFECT_POINTS_BLUE, POINTS_3000, p->x, p->y);
			if (disp_level == DISP_LEVEL_HIGH)
				EffectInit (EFFECT_HAHENMINI, 0, p->x, p->y);
			p->child_death = !0;	/* �������� */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if (((p->flash++ > TIMER_FLASH_RED) && (p->hp < 100)) && (p->seq != SEQ_DEATH)) {
			p->info = PALET_RED | PRIORITY_BOSS_PARTS_L;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = p->work4;
		}
	}

	xobj_set_st (p);

	return (0);
}



static void EnemyTiniLBoss03P (ENEMY * p)
{
}
