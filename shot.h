/* shot.h */

typedef struct _shot {
	signed short x, y;	/* ���W */
	short pt;		/* �X�v���C�g�p�^�[���m���D */
	short info;		/* ���]�R�[�h�E�F�E�D��x��\�킷�f�[�^ */
	short type;		/* ��� */
	signed int lx, ly;	/* 32bit X,Y ���W ( l = longword ) */
	signed int vx, vy;	/* ���x */
	short damage;		/* �G�ɓ��������� = !0 */
	char frame;		/* �o�����Ă��牽�t���[���o������ */
	struct _shot *next;	/* ���̍\���̂ւ̃|�C���^ */
} SHOT;


#ifdef GLOBAL_DEFINE		/* �O���[�o���ϐ��̒�`�Ɛ錾���P�ɂ܂Ƃ߂�e�N */
#define Extern			/* Extern ���k��������ɒu�� */
#else
#define Extern extern		/* Extern �� extern �ɒu�� */
#endif

Extern SHOT *shot_top,		/* �g�p���̃��[�N�̃��X�g */
 *shot_null_top;		/* ��̃��[�N�̃��X�g */


/* �֐��v���g�^�C�v�錾 */
void ShotInit0 (void);
void ShotInit (unsigned char);
void ShotMove (void);
void ShotTini (void);
