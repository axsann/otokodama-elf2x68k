#include "otoko.h"
#include "player.h"
#include "shot.h"
#include "enemy.h"
#include "psearch.h"
#include "damage.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif


#define ENEMY_MAX	32	/* �G�ő吔 */

static ENEMY enemy[ENEMY_MAX];	/* ���[�N */
static ENEMY *enemy_top,	/* �g�p���̃��[�N�̃��X�g */
 *enemy_null_top,		/* ��̃��[�N�̃��X�g */
 *enemy_end;			/* �g�p�����[�N�̃��X�g�̖��� */


/* �V���b�g���G�L�����ɗ^����_���[�W�i�Y�����͏o�����Ă���̃t���[�����j */
static short shot_damage[]=
{
	4, 4, 4, 4, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

/* �V���b�g���G�L�����ɗ^����_���[�W�i�Y�����͏o�����Ă���̃t���[�����j */
/* �R�A�ɓ��Ă��� */
static short shot_damage_core[]=
{
	6, 6, 6, 6, 5, 5, 5, 5, 4, 4,
	4, 4, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};
/* ���Ȃ݂ɃV���b�g�� 7dot/frame �� */

#define BLASER_HITX	16


#define wabs(x) ((x) >= 0 ? (short) (x) : (short) -(x))
#define abs(x)  ((x) >= 0 ? (int) (x) : (int) -(x))


typedef void (*func_init) (struct _enemy *);
func_init FuncEnemyInit[]=
{
	EnemyInitBoss02,
	EnemyInitBoss03,
	EnemyInitCoverF,
	EnemyInitCoverN,
	EnemyInitCoverN2,
	EnemyInitLBoss01,
	EnemyInitLBoss01C,
	EnemyInitLBoss02,
	EnemyInitLBoss02E,
	EnemyInitLBoss02H,
	EnemyInitLBoss02W,
	EnemyInitLBoss03,
	EnemyInitLBoss03G,
	EnemyInitLBoss03P,
	EnemyInitLBoss03U,
	EnemyInitLBoss03Z,
	EnemyInitLBoss04,
	EnemyInitLBoss04C,
	EnemyInitLBoss04E,
	EnemyInitLBoss04S,
	EnemyInitLBoss04V,
	EnemyInitLBoss04W,
	EnemyInitMvLaser,
	EnemyInitMvLaserH,
	EnemyInitOpLaser,
	EnemyInitOpLinear,
	EnemyInitRound4,
	EnemyInitSearchZ,
	EnemyInitSword,
	EnemyInitSword2,
	EnemyInitZako02,
	EnemyInitZako031,
	EnemyInitZako032,
	EnemyInitZako033,
	EnemyInitZako04,
	EnemyInitZako041,
};



/* �N�����ɂP�x�����Ă΂�� */
void EnemyInit0 (void)
{
	int i;

    /* ���X�g���Ȃ��� */
	enemy_top = NULL;
	enemy_end = NULL;
	enemy_null_top = enemy;
	for (i = 0; i < ENEMY_MAX; i++)
		enemy[i].next = &enemy[i + 1];

	enemy[ENEMY_MAX - 1].next = NULL;
}



/* �Q�[���J�n���ɂP�x�����Ă΂�� */
/* �u���߂̂W�@�Ԃ񂾂����_��\���v�Ɏg�p���� static �ϐ������������� */
void EnemyInitStatic (void)
{
	EnemyInitStaticLBoss03G ();
	EnemyInitStaticSearchZ ();
	EnemyInitStaticZako02 ();
	EnemyInitStaticZako031 ();
	EnemyInitStaticZako032 ();
	EnemyInitStaticZako033 ();
	EnemyInitStaticZako04 ();
	EnemyInitStaticZako041 ();
}



/* �G�o�����ɌĂ΂�� */
ENEMY *EnemyInit (short type, signed short x, signed short y,
		  short arg, struct _enemy *parent)
{
	ENEMY *p;

	if (enemy_null_top == NULL)
		return (NULL);	/* �󂫂̃��[�N���Ȃ��i�L�����N�^�[�I�[�o�[�j */

    /* ���X�g�̖����ɐV�����m�[�h��ǉ��i���Ƃ͈Ⴄ�̂Œ��Ӂj */
	p = enemy_null_top;
	enemy_null_top = p->next;
	if (enemy_top == NULL)
		enemy_top = p;
	else
		enemy_end->next = p;
	p->next = NULL;
	enemy_end = p;

	p->type = type;
	p->arg = arg;
	p->lx = x << 16;
	p->ly = y << 16;
	p->parent = parent;
	p->child_kill = p->child_death = 0;
	FuncEnemyInit[type] (p);/* ���������[�`���֔�ԁi�֐��ւ̃|�C���^���ă��c�j */

	return (p);		/* �쐬�������[�N�ւ̃|�C���^��Ԃ� */
}



/* �����������ƂɌĂ΂�� */
void EnemyMove (void)
{
	ENEMY *p, *q;
	signed short pl_x = player->x, pl_y = player->y;

#ifdef DEBUG
	enemy_sum = 0;
#endif

	p = enemy_top;		/* ���ݒ��ڂ��Ă��郏�[�N */
	q = NULL;		/* �P�O�̃��[�N�i���[�N�폜���ɕK�v�j */
	while (p != NULL) {
#ifdef DEBUG
		enemy_sum++;
#endif
	    /* �ړ��֐����Ăԁi�֐��ւ̃|�C���^���ă��c�j */
		if ((p->func_enemy_move) (p)) {	/* �Ԃ�l����O�Ȃ���� */
			(p->func_enemy_tini) (p);	/* �����֐����Ă� */
			if (q == NULL) {	/* ���X�g�̈�ԍŏ����폜 */
				enemy_top = p->next;
				p->next = enemy_null_top;
				enemy_null_top = p;
				q = NULL;
				p = enemy_top;
			} else {
				if (p == enemy_end) {	/* ���X�g�̈�ԍŌ���폜 */
					q->next = NULL;
					enemy_end = q;
					p->next = enemy_null_top;
					enemy_null_top = p;
					p = q->next;
				} else {
					q->next = p->next;
					p->next = enemy_null_top;
					enemy_null_top = p;
					p = q->next;
				}
			}
		} else {
			SHOT *p2 = shot_top;	/* ���ݒ��ڂ��Ă���V���b�g�̃��[�N */
			signed short t;
		    /* �v���C���[�Ƃ̓����蔻�� */
			if (((t = p->x + p->hit_px) > pl_x)
			    && ((t -= (short) (p->hit_px << 1)) < pl_x)
			    && ((t = p->y + p->hit_py) > pl_y)
			    && ((t -= (short) (p->hit_py << 1)) < pl_y)) {
				if (player->seq == PLAYER_SEQ_ALIVE) {
					player->seq = PLAYER_SEQ_DEAD;
				} else {
					short hit_w;
				    /* ���G��Ԃœ������� */
					if (((t = p->x + (hit_w = p->hit_cx)) >= pl_x)
					    && ((t -= (short) (hit_w << 1)) <= pl_x)) {
					    /* �R�A�ɓ������� */
						p->damage += PLAYER_DAMAGE_CORE;
						p->damage_core = !0;
					} else {
					    /* ����ȊO�̕����ɓ������� */
						p->damage += PLAYER_DAMAGE;
					}
				}
			}
		    /* �V���b�g�Ƃ̓����蔻�� */
			while (p2 != NULL) {
				signed short p2_w;
				short hit_w;
				if (((t = p->x + (hit_w = p->hit_sx)) >= (p2_w = *((short *) (&p2->lx))))
				    && ((t -= (short) (hit_w << 1)) <= p2_w)
				    && ((t = p->y + (hit_w = p->hit_sy)) >= (p2_w = *((short *) (&p2->ly))))
				    && ((t -= (short) (hit_w << 1)) <= p2_w)
				    && (p2->damage == 0)) {
					p2->damage = !0;
					if (((t = p->x + (hit_w = p->hit_cx)) >= (p2_w = *((short *) (&p2->lx))))
					    && ((t -= (short) (hit_w << 1)) <= p2_w)) {
					    /* �R�A�ɓ������� */
						p->damage += shot_damage_core[p2->frame];
						p->damage_core = !0;
					} else {
					    /* ����ȊO�̕����ɓ������� */
						p->damage += shot_damage[p2->frame];
					}
				}
				p2 = p2->next;
			}
		    /* �{���o�[�Ƃ̓����蔻�� */
			if (p->hit_sx != 0) {
				switch (player->bomb_hitcheck) {
				case BOMB_NON:
					break;
				case BOMB_A:
					p->damage += BOMBER_DAMAGE;
					break;
				case BOMB_B:
					if (p->y > pl_y)
						p->damage += BACK_B_DAMAGE;
					break;
				case BOMB_C:
					{
						short hit_w;
						if (((t = p->x + (hit_w = p->hit_sx)) >= pl_x)
						    && ((t -= (short) (hit_w << 1)) <= pl_x)
						    && (p->y <= pl_y)) {
							if (((t = p->x + (hit_w = p->hit_cx)) >= pl_x)
							    && ((t -= (short) (hit_w << 1)) <= pl_x)) {
							    /* �R�A�ɓ������� */
								p->damage += BLASER_DAMAGE_CORE;
								p->damage_core = !0;
							} else {
							    /* ����ȊO�̕����ɓ������� */
								p->damage += BLASER_DAMAGE;
							}
						}
					} break;
				}
			}
			q = p;
			p = p->next;
		}
	}
}



/* �G�������ɌĂ΂��i�Q�[���I�[�o�[���j */
void EnemyTini (void)
{
	int i;

    /* ���X�g���Ȃ��� */
	enemy_top = NULL;
	enemy_null_top = enemy;
	for (i = 0; i < ENEMY_MAX; i++)
		enemy[i].next = &enemy[i + 1];

	enemy[ENEMY_MAX - 1].next = NULL;
}



/* �T�[�r�X���[�`�� */

enum {
	MOVESEQ_ADD, MOVESEQ_FIX, MOVESEQ_SUB, MOVESEQ_END
};

/* ��΍��W dx,dy �Ɍ������Ĉړ�����i�������j */
void SubEnemyMoveToInit (ENEMY * p, signed short dest_x, signed short dest_y,
			 unsigned char speed, signed short count)
{
	unsigned char angle;
	signed short sx, sy;

	sx = wabs (dest_x - p->x);
	sy = wabs (dest_y - p->y);
	angle = msearch (p->x, p->y, dest_x, dest_y);	/* �ړ����� */
	p->move_ax = vxtable[speed][angle] / 32;
	p->move_ay = vytable[speed][angle] / 32;

	p->move_counter = p->move_count = count;	/* ��������^�������邩 */

    /* �덷�����Ȃ����邽�߂ɑ傫�����Ōv�Z */
    /* ��������΂� 0 �ɂȂ�Ȃ��悤�ɂ��邽�� +1 ���Ă��܂��i���׍H�j */
	if (sx > sy)
		p->move_count2 = sx * 65536 / (abs (p->move_ax * count) + 1) - count;
	else
		p->move_count2 = sy * 65536 / (abs (p->move_ay * count) + 1) - count;

	p->move_count3 = 0;	/* ���ۂɉ����^���������� */
	p->move_seq = MOVESEQ_ADD;
}



/* ���W dx,dy �Ɍ������Ĉړ����� */
short SubEnemyMoveTo (ENEMY * p)
{
	switch (p->move_seq) {
		case MOVESEQ_ADD:	/* ������ */
		p->vx += p->move_ax;
		p->vy += p->move_ay;
		p->move_count3++;
		if (p->move_counter-- <= 0) {
			p->move_seq++;	/* ���������玟�� */
			p->move_counter = p->move_count2;
		}
		break;
	case MOVESEQ_FIX:	/* �����x�^���� */
		if (p->move_counter-- <= 0) {
			p->move_seq++;	/* ���������玟�� */
			p->move_counter = p->move_count;
		}
		break;
	case MOVESEQ_SUB:	/* ������ */
		p->vx -= p->move_ax;
		p->vy -= p->move_ay;
		p->move_count3--;
		if (p->move_counter-- <= 0)	/* ������x���������玟�� */
			p->move_seq++;
		break;
	case MOVESEQ_END:
		p->vx = p->vy = p->move_ax = p->move_ay = 0;
		return (-1);
	}
	return (0);
}



/* �ړ��𒆒f���� */
short SubEnemyMoveToAbort (ENEMY * p)
{
	if (p->move_count3-- <= 0) {
		p->vx = 0;
		p->vy = 0;
		return (-1);
	}
	p->vx -= p->move_ax;
	p->vy -= p->move_ay;

	return (0);
}
