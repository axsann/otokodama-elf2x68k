#include <xsp2lib.h>

#include "otoko.h"
#include "player.h"
#include "shot.h"
#include "priority.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define SHOT_MAX	24	/* �V���b�g�ő吔 */
static SHOT shot[SHOT_MAX];	/* ���[�N */

#define DISP_OFFSET_X	(-8)	/* (x,y)+(-8,-8) �ɃX�v���C�g��\�� */
#define DISP_OFFSET_Y	(-8)

#define SHOT_SPEED	31


/* �N�����ɂP�x�����Ă΂�� */
void ShotInit0 (void)
{
	int i;

    /* ���X�g���Ȃ��� */
	shot_top = NULL;
	shot_null_top = shot;
	for (i = 0; i < SHOT_MAX; i++)
		shot[i].next = &shot[i + 1];

	shot[SHOT_MAX - 1].next = NULL;
}



/* �V���b�g�o�����ɌĂ΂�� */
void ShotInit (unsigned char angle)
{
	SHOT *p;

	if (shot_null_top == NULL)	/* ���[�N�̋󂫂͂��邩�H */
		return;

	p = shot_null_top;
	shot_null_top = p->next;
	p->next = shot_top;
	shot_top = p;

	p->lx = (player->x) << 16;	/* �V���b�g�̍��W�͎��@+(0,-16) */
	p->ly = (player->y + 4) << 16;	/* ���@�̏�����ɏo�� */
	p->pt = sp_shot + pt_rot[angle];
	p->info = 0x0100 | PRIORITY_SHOT | info_rot[angle];
#if	0
	0b xxxx_xxxx
	  0 ~ 63:0
	  64 ~ 127:4
	  128 ~ 191:8
	  192 ~ 255:c
#endif
	  p->damage = 0;
	p->frame = -1;
	p->vx = vxtable[SHOT_SPEED][angle];
	p->vy = vytable[SHOT_SPEED][angle];
}



/* �����������ƂɌĂ΂�� */
void ShotMove (void)
{
	SHOT *p, *q;

	p = shot_top;		/* ���ݒ��ڂ��Ă��郏�[�N */
	q = NULL;		/* �P�O�̃��[�N�i���[�N�폜���ɕK�v�j */
	while (p != NULL) {
	    /* ���x�𑫂��ď�ʃ��[�h�i�Œ萮�����j�������o�� */
		p->x = ((p->lx += p->vx) >> 16) + DISP_OFFSET_X;
		p->y = ((p->ly += p->vy) >> 16) + DISP_OFFSET_Y;

		p->frame++;
	    /* �V���b�g���G�ɓ����������͉�ʊO�ɏo�����H */
	    /* �i��ʉE����o������ƍ�����o��������P��̔�r�ōs���Ă��鎖�ɒ��ځj */
		if ((p->damage)
		    || ((unsigned short) (p->x) > 256 + 16 - DISP_OFFSET_X)
		    || ((unsigned short) (p->y) > 256 + 16 - DISP_OFFSET_Y)) {

			if (q == NULL) {	/* ���X�g�̈�ԍŏ����폜 */
				shot_top = p->next;
				p->next = shot_null_top;
				shot_null_top = p;
				q = NULL;
				p = shot_top;
			} else {
				q->next = p->next;
				p->next = shot_null_top;
				shot_null_top = p;
				p = q->next;
			}
		} else {
			xsp_set_st (p);
			q = p;
			p = p->next;
		}
	}
}



/* �V���b�g�������ɌĂ΂��i�ʃN���A���j */
void ShotTini (void)
{
	int i;

    /* ���X�g���Ȃ��� */
	shot_top = NULL;
	shot_null_top = shot;
	for (i = 0; i < SHOT_MAX; i++)
		shot[i].next = &shot[i + 1];

	shot[SHOT_MAX - 1].next = NULL;
}
