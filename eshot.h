/* eshot.h */

#define ESHOT_ERASE	14	/* �e�����p�萔 */

typedef struct _eshot {
	signed short x, y;	/* ���W */
	short pt;		/* �X�v���C�g�p�^�[���m���D */
	short info;		/* ���]�R�[�h�E�F�E�D��x��\�킷�f�[�^ */
	short type;		/* ��� */
	signed int lx, ly;	/* 32bit X,Y ���W ( l = longword ) */
	signed int vx, vy;	/* ���x */
	signed short hit_p;	/* �΃v���C���[�����蔻��iX,Y ���ʁj */
	unsigned char angle;	/* �p�x (0~255) */
	unsigned char speed;	/* ���x (0~7) */
	struct _eshot *next;	/* ���̍\���̂ւ̃|�C���^ */
} ESHOT;


#ifdef GLOBAL_DEFINE		/* �O���[�o���ϐ��̒�`�Ɛ錾���P�ɂ܂Ƃ߂�e�N */
#define Extern			/* Extern ���k��������ɒu�� */
#else
#define Extern extern		/* Extern �� extern �ɒu�� */
#endif


/* �O���[�o���ϐ� */
Extern ESHOT *eshot_top,	/* �g�p���̃��[�N�̃��X�g */
 *eshot_null_top;		/* ��̃��[�N�̃��X�g */
Extern char eshot_erase;	/* >0 �Ȃ�e������ */


enum {
	ESHOT_NRG01 = 0,	/* �G�l���M�[�e�i�ɏ��j */
	ESHOT_NRG02,		/* �@�@ �V �@�@�i���j */
	ESHOT_NRG03,		/* �@�@ �V �@�@�i���j */
	ESHOT_NRG04,		/* �@�@ �V �@�@�i��j */
	ESHOT_NRG05,		/* �@�@ �V �@�@�i����j */
	ESHOT_NRG06,		/* �@�@ �V �@�@�i����j */
	ESHOT_NRG22,		/* �@�@ �V �@�@�i�����ɂQ�j */
	ESHOT_NRG32,		/* �@�@ �V �@�@�i�����ɂQ�j */
	ESHOT_NRG23,		/* �@�@ �V �@�@�i���R�p�`�ɂR�j */
	ESHOT_NRG24,		/* �@�@ �V �@�@�i�������`�ɂS�j */
	ESHOT_NRG14N,		/* �@�@ �V �@�@�i���΂߂ɂS�j */
	ESHOT_NRG24N,		/* �@�@ �V �@�@�i���΂߂ɂS�j */
	ESHOT_NRG34N,		/* �@�@ �V �@�@�i���΂߂ɂS�j */
	ESHOT_NRG34,		/* �@�@ �V �@�@�i�������`�ɂS�j */
	ESHOT_NRG15,		/* �@�@ �V �@�@�i���T�j */
	ESHOT_NRG35,		/* �@�@ �V �@�@�i���Ƒ吳���`�ɂT�j */
	ESHOT_MIS01,		/* �~�T�C���e�i�����̂����j */
	ESHOT_MIS02,
	ESHOT_LASER01,		/* ���[�U�[ */
	ESHOT_LASEREX,		/* �ɑ����[�U�[ */
	ESHOT_SEARCH		/* �U���e */
};


/* �֐��v���g�^�C�v�錾 */
void EshotInit0 (void);
void EshotInit (short, signed short, signed short, unsigned char, unsigned char, signed short);
void EshotMove (void);
void EshotTini (void);
