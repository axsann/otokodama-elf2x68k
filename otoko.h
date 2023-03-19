/* otoko.h */

/* #define DEBUG *//* �f�o�b�O���[�h */


#define STAGES	4		/* ���ʐ� */
#define SPEED_MAX	32
#define EXTEND_EVERY	100000	/* �G�u���ݒ� */
#define BG_PALETS	64	/* BG�Ŏg�p���Ă���p���b�g�� */
#define REPLAY_DATA_SIZE_MAX	131072	/* ���v���C�f�[�^�̃T�C�Y�̍ő�l�i16�̔{���ɂ��邱�Ɓj */


#ifdef GLOBAL_DEFINE		/* �O���[�o���ϐ��̒�`�Ɛ錾���P�ɂ܂Ƃ߂�e�N */
#define Extern			/* Extern ���k��������ɒu�� */
#else
#define Extern extern		/* Extern �� extern �ɒu�� */
#endif


/* �O���[�o���ϐ� */
Extern char stage;		/* �ʐ��i�P��=0�j */
Extern char pause_flag;		/* =!0 �Ȃ�ꎞ��~�� */
Extern signed int score;	/* ���_ */
Extern signed int extend_score;	/* �G�N�X�e���h���链�_ */

Extern short error_level;	/* �N�����̃G���[�`�F�b�N */
Extern char *error_message;

Extern signed int vxtable[SPEED_MAX][256], vytable[SPEED_MAX][256];
Extern short pt_rot[256];
Extern short info_rot[256];
Extern unsigned char rndtable[256];

Extern short obj_player, obj_oplaser, obj_coverf, obj_covern, obj_round4, sp_eshot01, sp_eshotera,
  obj_explall, obj_bombstr, obj_blaser, obj_hahen, obj_points, obj_searchz, obj_sword, obj_zako02,
  obj_zako03, obj_zako04, obj_mvlaser, obj_mvlaserh, obj_lboss01, obj_lboss02, obj_lboss03,
  sp_lboss03g, obj_lboss04, sp_core01, sp_shot, sp_laser01, sp_cursor, obj_titspark, obj_titmoji;

Extern unsigned char bg_map[STAGES][30 + 32 * 128 * 2];
Extern unsigned char bg_sp[STAGES][32 * 256];
Extern unsigned short stage_palet_table[STAGES][33][BG_PALETS];
Extern signed short bg_contrast_control;	/* BG_xxx ������ */
Extern unsigned short *bg_contrast_ptr;
Extern unsigned short bg_contrast_num;	/* ���i�K�̃R���g���X�g�����邩�i33�ɌŒ�j */

Extern char disp_level;		/* �\�����׃��x�� */
Extern char e_option;
Extern char standard_play;	/* �W���ݒ�i�c�@�R�A�P�ʂ���J�n�j���H */
Extern char replay_flag;	/* ���v���C�����H */

Extern unsigned char replay_data[REPLAY_DATA_SIZE_MAX];	/* ���v���C�f�[�^ */
Extern int replay_data_size;	/* ���v���C�f�[�^�̑傫�� */
Extern unsigned char high_score_replay_data[REPLAY_DATA_SIZE_MAX];	/* �n�C�X�R�A�̃��v���C�f
									   �[�^ */
Extern int high_score_replay_data_size;	/* �n�C�X�R�A�̃��v���C�f�[�^�̑傫�� */


#ifdef DEBUG
Extern short eshot_sum;
Extern short enemy_sum;
#endif


enum {
	ERROR_NON = 0,		/* �G���[�Ȃ� */
	ERROR_TUSEMD,		/* �e�L�X�g VRAM ���g�p�� */
	ERROR_GUSEMD,		/* �O���t�B�b�N VRAM ���g�p�� */
	ERROR_FILE,		/* �t�@�C�����ǂݍ��߂Ȃ����� */
	ERROR_PIC_MEMORY,	/* .PIC �W�J�p������������Ȃ����� */
	ERROR_PIC_FILE,		/* .PIC �t�@�C�����ǂݍ��߂Ȃ����� */
	ERROR_MUSIC,		/* ���y�֌W */
};


enum {
	DISP_LEVEL_LOW = 0,	/* �\�����ׂ��y�ڂ� */
	DISP_LEVEL_MID,
	DISP_LEVEL_HIGH,
};


enum {
	REPLAY_NON = 0,		/* ���v���C���ł͂Ȃ��i�ʏ�̃Q�[�����j */
	REPLAY_BEFORE,		/* ���O�̃Q�[���̃��v���C�� */
	REPLAY_HIGH,		/* �n�C�X�R�A�̃��v���C�� */
};
