#include "effect.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define EFFECT_MAX	32	/* �G�t�F�N�g�ő吔 */
static EFFECT effect[EFFECT_MAX];	/* ���[�N */
static EFFECT *effect_top,	/* �g�p���̃��[�N�̃��X�g */
 *effect_null_top;		/* ��̃��[�N�̃��X�g */

typedef void (*func_init) (struct _effect *);
func_init FuncEffectInit[]=
{
	EffectInitExpl,
	EffectInitExplZako,
	EffectInitExplMini,
	EffectInitExplPlayer,
	EffectInitBombstr,
	EffectInitBombstr,
	EffectInitBombstr,
	EffectInitHahen,
	EffectInitHahenMini,
	EffectInitPointsRed,
	EffectInitPointsBlue,
	EffectInitPoints,
	EffectInitBomber,
	EffectInitBackB,
	EffectInitBLaser,
	EffectInitTitSpark,
	EffectInitTitMoji,
	EffectInitLastLaser,
	EffectInitLastPlayer,
};


static int points_table[]=
{
	100, 200, 300, 400, 500, 600, 700, 800, 900, 1000,
	1200, 1500, 1800, 2000, 2500, 3000, 4000, 5000, 6000, 7000,
	8000, 9000, 10000, 12000, 15000, 18000, 20000, 25000, 28000, 30000
};


/* �N�����ɂP�x�����Ă΂�� */
void EffectInit0 (void)
{
	int i;

    /* ���X�g���Ȃ��� */
	effect_top = NULL;
	effect_null_top = effect;
	for (i = 0; i < EFFECT_MAX; i++)
		effect[i].next = &effect[i + 1];

	effect[EFFECT_MAX - 1].next = NULL;

	points = &points_table[0];
}



/* �G�t�F�N�g�o�����ɌĂ΂�� */
void EffectInit (short type, short type2, short x, short y)
{
	EFFECT *p;

	if (effect_null_top == NULL)
		return;		/* �󂫂̃��[�N���Ȃ��i�L�����N�^�[�I�[�o�[�j */

	p = effect_null_top;
	effect_null_top = p->next;
	p->next = effect_top;
	effect_top = p;

	p->type = type;
	p->type2 = type2;
	p->x = x;
	p->y = y;
	FuncEffectInit[type] (p);	/* ���������[�`���֔�ԁi�֐��ւ̃|�C���^���ă��c�j */
}



/* �����������ƂɌĂ΂�� */
void EffectMove (void)
{
	EFFECT *p, *q;

	p = effect_top;		/* ���ݒ��ڂ��Ă��郏�[�N */
	q = NULL;		/* �P�O�̃��[�N�i���[�N�폜���ɕK�v�j */
	while (p != NULL) {
	    /* �ړ����[�`���֔�ԁi�֐��ւ̃|�C���^���ă��c�j */
		if ((p->func_effect_move) (p)) {	/* �Ԃ�l����O�Ȃ���� */
			if (q == NULL) {	/* ���X�g�̈�ԍŏ����폜 */
				effect_top = p->next;
				p->next = effect_null_top;
				effect_null_top = p;
				q = NULL;
				p = effect_top;
			} else {
				q->next = p->next;
				p->next = effect_null_top;
				effect_null_top = p;
				p = q->next;
			}
		} else {
			q = p;
			p = p->next;
		}
	}
}



/* �G�t�F�N�g�������ɌĂ΂��i�ʃN���A���j */
void EffectTini (void)
{
	int i;

    /* ���X�g���Ȃ��� */
	effect_top = NULL;
	effect_null_top = effect;
	for (i = 0; i < EFFECT_MAX; i++)
		effect[i].next = &effect[i + 1];

	effect[EFFECT_MAX - 1].next = NULL;
}
