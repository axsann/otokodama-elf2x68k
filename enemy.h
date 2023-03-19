/* enemy.h */

#ifndef SHORT_LX
/* signed int �̍��W�l����Œ菬�����i���16bit�j�����o���}�N�� */
#define SHORT_LX (*((short *) (&p->lx)))
#define SHORT_LY (*((short *) (&p->ly)))
#define SHORT_VX (*((short *) (&p->vx)))
#define SHORT_VY (*((short *) (&p->vy)))
#endif

#define ZAKO_SCORE_DISP	16	/* �U�R�L�����̓��_��\������� */

/* �G�L�����_�Łi�_���[�W���󂯂��A�̗͂����Ȃ��j���̃^�C�}�[�p�萔 */
#define TIMER_FLASH_RED		10	/* �^�C�}�[������ȏ�Ȃ�ԃt���b�V�� */
#define TIMER_FLASH_DAMAGE	14	/* �_���[�W���󂯂���^�C�}�[�����̒l�� */
#define TIMER_FLASH_NORMAL	15	/* ������z������^�C�}�[���O�� */


typedef struct _enemy {
	signed short x, y;	/* ���W */
	short pt;		/* �X�v���C�g�p�^�[���m���D */
	short info;		/* ���]�R�[�h�E�F�E�D��x��\�킷�f�[�^ */
	short type;		/* ��� */
	short arg;		/* �\���̈����� */
	signed int lx, ly;	/* 32bit X,Y ���W ( l = longword ) */
	signed int vx, vy;	/* ���x */
	signed short hit_px, hit_py;	/* �΃v���C���[�����蔻�� */
	signed short hit_sx, hit_sy;	/* �΃V���b�g�����蔻��i�S�́j */
	signed short hit_cx;	/* �΃V���b�g�����蔻��i�R�A�j */
	signed short hp;	/* �ϋv�́i<=0 �Ŕj��j */
	unsigned short timer;	/* ���Ԑ؂�p�^�C�}�[ */
	short damage;		/* ����󂯂��_���[�W�i���@�V���b�g�ɓ������Ă��Ȃ���� = 0�j */
	char damage_core;	/* ����R�A�Ƀ_���[�W���󂯂��� != 0 */
	char flash;		/* �c�� hp �����Ȃ��ăt���b�V������J�E���^�[ */
	unsigned char angle;	/* �p�x (0~255) */
	short (*func_enemy_move) (struct _enemy *);	/* �ړ��֐��ւ̃|�C���^ */
	void (*func_enemy_tini) (struct _enemy *);	/* �I���֐��ւ̃|�C���^ */
	struct _enemy *next;	/* ���̍\���̂ւ̃|�C���^ */


	short work;		/* ���[�N */
	short work2;
	short work3;
	short work4;
	char cwork;		/* ���[�N(char) */
	char cwork2;
	char cwork3;
	char cwork4;
	char cwork5;
	char seq;		/* ���݂̏�� */
	short m_work;		/* �ړ��p���[�N */
	short m_work2;
	short m_work3;
	short m_work4;
	short s_work;		/* �V���b�g�p���[�N */
	short s_work2;
	short s_work3;
	short s_work4;
	unsigned char s_angle;	/* �V���b�g�̊p�x (0~255) */
	unsigned char s_angle2;
	unsigned char s_angle3;
	unsigned char s_angle4;


	/* �ړ��p���[�N�iSubMoveTo() �p�j */
	signed int move_ax, move_ay;	/* �����x */
	unsigned short move_seq;/* �������A�����x�^�����A������ */
	signed short move_count;/* �����x�����񑫂��� */
	signed short move_count2;	/* ��葬�x�Ői�މ� */
	signed short move_count3;	/* ���ۂɉ��������� */
	signed short move_counter;	/* ���񑫂��������� */


	/* �R�A�֌W�̃��[�N */
	signed short core_x, core_y;	/* �R�A���W */
	short core_pt;		/* �X�v���C�g�p�^�[���m���D */
	short core_info;	/* ���]�R�[�h�E�F�E�D��x��\�킷�f�[�^ */


	/* �p�[�c�֌W�̃��[�N */
	struct _enemy *parts1;	/* �p�[�c�ւ̃|�C���^ */
	struct _enemy *parts2;
	struct _enemy *parts3;
	struct _enemy *parts4;
	short parts1_work;	/* ���[�N */
	short parts1_work2;
	short parts2_work;
	short parts2_work2;
	short parts3_work;
	short parts3_work2;

	/* �L�����N�^�[�ԒʐM�p���[�N */
	struct _enemy *parent;	/* �e�̃��[�N */
	unsigned char child_kill;	/* �e���q���E�����i�e���ݒ�j */
	unsigned char child_death;	/* �q�����񂾂��Ƃ�e�ɒm�点��i�q���ݒ�j */
} ENEMY;


/* �֐��v���g�^�C�v�錾 */
void EnemyInit0 (void);
void EnemyInitStatic (void);
ENEMY *EnemyInit (short, signed short, signed short, short, struct _enemy *);
void EnemyMove (void);
void EnemyTini (void);

/* �T�[�r�X���[�`�� */
void SubEnemyMoveToInit (ENEMY *, signed short, signed short,
			 unsigned char, signed short);
short SubEnemyMoveTo (ENEMY *);
short SubEnemyMoveToAbort (ENEMY *);


/* FuncEnemy/ �ȉ��̓G�ړ����[�`�� */
void EnemyInitBoss02 (ENEMY *);
void EnemyInitBoss03 (ENEMY *);
void EnemyInitCoverF (ENEMY *);
void EnemyInitCoverN (ENEMY *);
void EnemyInitCoverN2 (ENEMY *);

void EnemyInitLBoss01 (ENEMY *);
void EnemyInitLBoss01C (ENEMY *);

void EnemyInitLBoss02 (ENEMY *);
void EnemyInitLBoss02E (ENEMY *);
void EnemyInitLBoss02H (ENEMY *);
void EnemyInitLBoss02W (ENEMY *);

void EnemyInitLBoss03 (ENEMY *);
void EnemyInitStaticLBoss03G (void);
void EnemyInitLBoss03G (ENEMY *);
void EnemyInitLBoss03P (ENEMY *);
void EnemyInitLBoss03U (ENEMY *);
void EnemyInitLBoss03Z (ENEMY *);

void EnemyInitLBoss04 (ENEMY *);
void EnemyInitLBoss04C (ENEMY *);
void EnemyInitLBoss04E (ENEMY *);
void EnemyInitLBoss04S (ENEMY *);
void EnemyInitLBoss04V (ENEMY *);
void EnemyInitLBoss04W (ENEMY *);

void EnemyInitMvLaser (ENEMY *);
void EnemyInitMvLaserH (ENEMY *);
void EnemyInitOpLaser (ENEMY *);
void EnemyInitOpLinear (ENEMY *);
void EnemyInitRound4 (ENEMY *);

void EnemyInitStaticSearchZ (void);
void EnemyInitSearchZ (ENEMY *);

void EnemyInitSword (ENEMY *);
void EnemyInitSword2 (ENEMY *);

void EnemyInitStaticZako02 (void);
void EnemyInitZako02 (ENEMY *);

void EnemyInitStaticZako031 (void);
void EnemyInitZako031 (ENEMY *);

void EnemyInitStaticZako032 (void);
void EnemyInitZako032 (ENEMY *);

void EnemyInitStaticZako033 (void);
void EnemyInitZako033 (ENEMY *);

void EnemyInitStaticZako04 (void);
void EnemyInitZako04 (ENEMY *);

void EnemyInitStaticZako041 (void);
void EnemyInitZako041 (ENEMY *);


enum {
	ENEMY_BOSS02 = 0,
	ENEMY_BOSS03,
	ENEMY_COVERF,
	ENEMY_COVERN,
	ENEMY_COVERN2,
	ENEMY_LBOSS01,
	ENEMY_LBOSS01C,
	ENEMY_LBOSS02,
	ENEMY_LBOSS02E,
	ENEMY_LBOSS02H,
	ENEMY_LBOSS02W,
	ENEMY_LBOSS03,
	ENEMY_LBOSS03G,
	ENEMY_LBOSS03P,
	ENEMY_LBOSS03U,
	ENEMY_LBOSS03Z,
	ENEMY_LBOSS04,
	ENEMY_LBOSS04C,
	ENEMY_LBOSS04E,
	ENEMY_LBOSS04S,
	ENEMY_LBOSS04V,
	ENEMY_LBOSS04W,
	ENEMY_MVLASER,
	ENEMY_MVLASERH,
	ENEMY_OPLASER,
	ENEMY_OPLINEAR,
	ENEMY_ROUND4,
	ENEMY_SEARCHZ,
	ENEMY_SWORD,
	ENEMY_SWORD2,
	ENEMY_ZAKO02,
	ENEMY_ZAKO031,
	ENEMY_ZAKO032,
	ENEMY_ZAKO033,
	ENEMY_ZAKO04,
	ENEMY_ZAKO041,
};
