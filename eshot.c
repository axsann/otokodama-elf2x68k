#include <xsp2lib.h>

#include "otoko.h"
#include "player.h"
#include "eshot.h"
#include "priority.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define	PALET_ESHOT	0x0300

#define ESHOT_MAX	200	/* �G�e�ő吔 */
static ESHOT eshot[ESHOT_MAX];	/* ���[�N */



/* �N�����ɂP�x�����Ă΂�� */
void EshotInit0 (void)
{
	int i;

    /* ���X�g���Ȃ��� */
	eshot_top = NULL;
	eshot_null_top = eshot;
	for (i = 0; i < ESHOT_MAX; i++)
		eshot[i].next = &eshot[i + 1];

	eshot[ESHOT_MAX - 1].next = NULL;

	eshot_erase = 0;
}



/* �G�e�o�����ɌĂ΂�� */
void EshotInit (short type, signed short x, signed short y, unsigned char speed, unsigned char angle, signed short t)
{
	ESHOT *p;

	if (eshot_null_top == NULL)	/* ���[�N�̋󂫂͂��邩�H */
		return;

	p = eshot_null_top;
	eshot_null_top = p->next;
	p->next = eshot_top;
	eshot_top = p;

	p->lx = (x - 8) << 16;	/* (8,8) ���X�v���C�g���W�̒��S�Ȃ̂ŕ␳ */
	p->ly = (y - 8) << 16;
	p->vx = vxtable[speed][angle];
	p->vy = vytable[speed][angle];
	if (t) {		/* t �t���[����̒l�������l�� */
		p->lx += (p->vx * t);
		p->ly += (p->vy * t);
	}
    /* �����F�G�e�̑��x�Ɠ��蔻��̑傫���ɂ��� */
    /* eshot->hx ���ȉ��̏ꍇ�Aeshot->vx �̍ő�l�͈ȉ��̒ʂ� */
    /* (hx,vx ����) = (2,1.0),(3,3.0),(4,5.0),(5,7.0),(6,9.0) */
    /* �Z�o���͈ȉ��̒ʂ� */
    /* eshot->vx+player->vx > 2*eshot->hx �̎����蔲�������� */
    /* player->vx �̍ő�l�� 3.0�i�����ړ����j */

	switch (p->type = type) {
	case ESHOT_NRG01:	/* �G�l���M�[�e�i�ɏ��j */
		p->hit_p = 2;	/* ���x�� 1.0 �����ɂ��鎖 */
		p->pt = sp_eshot01 + type;
		p->info = PALET_ESHOT | PRIORITY_ESHOT;
		break;

	case ESHOT_NRG02:	/* �@�@ �V �@�@�i���j */
	case ESHOT_NRG03:	/* �@�@ �V �@�@�i���j */
	case ESHOT_NRG04:	/* �@�@ �V �@�@�i��j */
		p->hit_p = 3;	/* ���x�� 3.0 �����ɂ��鎖 */
		p->pt = sp_eshot01 + type;
		p->info = PALET_ESHOT | PRIORITY_ESHOT;
		break;

	case ESHOT_NRG05:	/* �@�@ �V �@�@�i����j */
	case ESHOT_NRG06:	/* �@�@ �V �@�@�i����j */
	case ESHOT_NRG22:	/* �@�@ �V �@�@�i�����ɂQ�j */
	case ESHOT_NRG32:	/* �@�@ �V �@�@�i�����ɂQ�j */
	case ESHOT_NRG23:	/* �@�@ �V �@�@�i���R�p�`�ɂR�j */
	case ESHOT_NRG24:	/* �@�@ �V �@�@�i�������`�ɂS�j */
	case ESHOT_NRG14N:	/* �@�@ �V �@�@�i���΂߂ɂS�j */
	case ESHOT_NRG24N:	/* �@�@ �V �@�@�i���΂߂ɂS�j */
	case ESHOT_NRG34N:	/* �@�@ �V �@�@�i���΂߂ɂS�j */
	case ESHOT_NRG34:	/* �@�@ �V �@�@�i�������`�ɂS�j */
	case ESHOT_NRG15:	/* �@�@ �V �@�@�i���T�j */
	case ESHOT_NRG35:	/* �@�@ �V �@�@�i���Ƒ吳���`�ɂT�j */
		p->hit_p = 4;	/* ���x�� 5.0 �����ɂ��鎖 */
		p->pt = sp_eshot01 + type;
		p->info = 0x0300 | PRIORITY_ESHOT;
		break;

	case ESHOT_LASER01:
		p->hit_p = 3;	/* ���x�� 3.0 �����ɂ��鎖 */
		p->pt = sp_laser01 + 8;
		p->info = PALET_ESHOT | PRIORITY_ESHOT;
		break;

	case ESHOT_LASEREX:
		p->hit_p = 4;	/* ���x�� 5.0 �����ɂ��鎖 */
		p->pt = sp_laser01 + 9;
		p->info = PALET_ESHOT | PRIORITY_ESHOT;
		break;
	default:
		break;
	}
}



/* �����������ƂɌĂ΂�� */
void EshotMove (void)
{
	ESHOT *p, *q;
	signed short pl_x = player->x - 8, pl_y = player->y - 8;
	signed short p_x, p_y;

#ifdef DEBUG
	eshot_sum = 0;
#endif
	p = eshot_top;		/* ���ݒ��ڂ��Ă��郏�[�N */
	q = NULL;		/* �P�O�̃��[�N�i���[�N�폜���ɕK�v�j */

    /* �e���������H�i�������̂��߂��璷�ȃR�[�h�ɂȂ��Ă܂��j */
	if (eshot_erase == 0) {
	    /* �ʏ�̏����i�e�����ł͂Ȃ��j */
		while (p != NULL) {
#ifdef DEBUG
			eshot_sum++;
#endif
		    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
			p_x = p->x = (p->lx += p->vx) >> 16;
			p_y = p->y = (p->ly += p->vy) >> 16;

		    /* �G�e����ʊO�ɏo�����H */
		    /* �i��ʉE����o������ƍ�����o��������P��̔�r�ōs���Ă���j */
			if (((unsigned short) p_x > 256 + 16) || ((unsigned short) p_y > 256 + 16)) {
				if (q == NULL) {	/* ���X�g�̈�ԍŏ����폜 */
					eshot_top = p->next;
					p->next = eshot_null_top;
					eshot_null_top = p;
					q = NULL;
					p = eshot_top;
				} else {
					q->next = p->next;
					p->next = eshot_null_top;
					eshot_null_top = p;
					p = q->next;
				}
			} else {
			    /* �G�e�Ǝ��@�̓����蔻�� */
			    /* ���̕ӂ̍œK���� -fstrings-reduce ���撣���Ă����n�Y */
				signed short t, t2 = p->hit_p;
				if (((t = p_y - t2) <= pl_y)
				    && ((t += (short) (t2 << 1)) >= pl_y)
				    && ((t = p_x + t2) >= pl_x)
				    && ((t -= (short) (t2 << 1)) <= pl_x)) {
					if (player->seq == PLAYER_SEQ_ALIVE)
						player->seq = PLAYER_SEQ_DEAD;
				} else {
					xsp_set_st (p);
				}
				q = p;
				p = p->next;
			}
		}
	} else {
	    /* �e������ */
		char erase_flag;/* �L�����N�^�[�������ꍇ =!0 */
		unsigned short erase_table[14] =
		{0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6};

		while (p != NULL) {
			if (eshot_erase > 1)
				erase_flag = 0;
			else
				erase_flag = !0;
#ifdef DEBUG
			eshot_sum++;
#endif

		    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
			p_x = p->x = (p->lx += p->vx) >> 16;
			p_y = p->y = (p->ly += p->vy) >> 16;

		    /* �G�e����ʊO�ɏo�����H */
		    /* �i��ʉE����o������ƍ�����o��������P��̔�r�ōs���Ă���j */
			if (((unsigned short) p_x > 256 + 16) || ((unsigned short) p_y > 256 + 16)) {
				erase_flag = !0;
			} else {
				p->pt = sp_eshotera + erase_table[eshot_erase];
				xsp_set_st (p);
			}

			if (erase_flag) {
				if (q == NULL) {	/* ���X�g�̈�ԍŏ����폜 */
					eshot_top = p->next;
					p->next = eshot_null_top;
					eshot_null_top = p;
					q = NULL;
					p = eshot_top;
				} else {
					q->next = p->next;
					p->next = eshot_null_top;
					eshot_null_top = p;
					p = q->next;
				}
			} else {
				q = p;
				p = p->next;
			}
		}
		eshot_erase--;
	}
}



/* �G�e�������ɌĂ΂��i�Q�[���I�[�o�[���j */
void EshotTini (void)
{
	int i;

    /* ���X�g���Ȃ��� */
	eshot_top = NULL;
	eshot_null_top = eshot;
	for (i = 0; i < ESHOT_MAX; i++)
		eshot[i].next = &eshot[i + 1];

	eshot[ESHOT_MAX - 1].next = NULL;
}
