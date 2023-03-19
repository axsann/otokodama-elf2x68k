/* player.h */

typedef struct {
	signed short x, y;	/* ���W */
	short pt;		/* �X�v���C�g�p�^�[���m���D */
	short info;		/* ���]�R�[�h�E�F�E�D��x��\�킷�f�[�^ */
	signed short bank;	/* ���@�̌X�� */
	signed int lx, ly;	/* 32bit X,Y ���W ( l = longword ) */
	char seq;		/* ���@�̏�� */
	char seq2;		/* ���@�̏�ԁi���S���̍ēo��܂ł̃J�E���^�[�j */
	short muteki;		/* ���G���̃J�E���^�[�i>0 �Ŗ��G�j */
	char flash;		/* ���G���̃p���b�g�t���b�V���p�J�E���^�[ */
	char shot_timer;	/* �e�̍L�������v�����邽�߂̃^�C�}�[ */
	char shot_timer_current;/* �a�{�^�����������u�Ԃ� shot_timer */
	char shot_cont;		/* �e��A���I�ɏo�����߂̃J�E���^�[ */
	char high_speed;	/* �����ړ����Ȃ� !0 */
	short a_button;		/* �O��`�{�^���������ꂽ�� */
	short b_button;		/* �O��a�{�^���������ꂽ�� */
	char b_button_next;	/* =!0:�V���b�g���o���I����O�ɂa�{�^���������ꂽ */
	char bomb_type;		/* �{����ށi=0 �Ń{��������Ȃ��j */
	char bomb_flash_state;	/* =0 �Œʏ�A=1 �Ŕ��t���b�V���A=2�Œʏ�p���b�g */
	char bomb_flash_timer;	/* �t���b�V���p�^�C�}�[ */
	short blaser;		/* �{���o�[���[�U�[�̗L������ */
	char bomb_hitcheck;	/* �{���o�[�����蔻��p */
	char bomb_guage_disp;	/* �{���o�[�Q�[�W�\���^��\�� */
	short bomb_hitcheck_time;	/* �{���o�[�����蔻��̎c�莞�� */
	signed int bomb_energy;	/* �{���o�[�G�l���M�[ */
	short rest;		/* �c�@�� */
	char game_over;	/* �Q�[���I�[�o�[�ɂȂ����v�� */
	unsigned int replay_counter;	/* ���v���C�f�[�^���w���J�E���^�[ */
} PLAYER;



#ifdef GLOBAL_DEFINE		/* �O���[�o���ϐ��̒�`�Ɛ錾���P�ɂ܂Ƃ߂�e�N */
#define Extern			/* Extern ���k��������ɒu�� */
#else
#define Extern extern		/* Extern �� extern �ɒu�� */
#endif


/* �O���[�o���ϐ� */
Extern PLAYER player[1];


enum {
	PLAYER_SEQ_ALIVE = 0,	/* �v���C���[�͐����Ă��� */
	PLAYER_SEQ_APPEAR,	/* �o�ꒆ�i��ʉ�����o�Ă���E����s�\�j */
	PLAYER_SEQ_MUTEKI,	/* ���G�i�o�꒼��j */
	PLAYER_SEQ_BARRIER,	/* �o���A�Ŗ��G */
	PLAYER_SEQ_DEAD,	/* ����ł��� */
	PLAYER_SEQ_LAST		/* �I�[���N���A */
};

enum {
	PLAYER_PT_RIGHT2 = 0,	/* ���@����ԉE�ɌX���Ă��鎞�̃p�^�[�� */
	PLAYER_PT_RIGHT1,	/* �@�V�@�����E�Ɂ@�@�@�@�@�V�@�@�@�@�@ */
	PLAYER_PT_CENTER,	/* �@�V�@���ʂ������Ă��鎞�̃p�^�[�� */
	PLAYER_PT_LEFT1,	/* ���@�������E�ɌX���Ă��鎞�̃p�^�[�� */
	PLAYER_PT_LEFT2		/* �@�V�@��ԉE�Ɂ@�@�@�@�@�V�@�@�@�@�@ */
};


enum {
	BOMB_NON = 0,
	BOMB_A,			/* BOMBER */
	BOMB_B,			/* BACK-B */
	BOMB_C			/* LASER */
};

enum {
	BG_FLASH_NON = 0,	/* �w�i�̓t���b�V�����łȂ� */
	BG_FLASH_WHITE,		/* �w�i�͔��t���b�V���� */
	BG_FLASH_NORMAL		/* �w�i�̓t���b�V���������ʏ�p���b�g */
};


enum {
	GAME_PLAY = 0,		/* �Q�[���� */
	GAME_OVER,		/* ����ŃQ�[���I�[�o�[ */
	GAME_COMPLETE,		/* �I�[���N���A */
	GAME_ABORT,		/* [SELECT]+[START] �Œ��f */
};


/* �֐��v���g�^�C�v�錾 */
void PlayerInit0 (void);
void PlayerInit (void);
void PlayerMove (void);
void PlayerTini (void);
void DispRest (void);
