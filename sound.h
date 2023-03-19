/* sound.h */

/* ���ʉ��������D�揇�ʁi���ɍs���قǍ����j */
enum {
	SE_SHOT = 1,		/* �v���C���[�̃V���b�g */
	SE_NO_DAMAGE,		/* �G�_���[�W���� */
	SE_DAMAGE,		/* �G�_���[�W */
	SE_CORE_DAMAGE,		/* �G�R�A�_���[�W */
	SE_ESHOT,		/* �G�V���b�g�i���j */
	SE_ESHOT_M,		/* �G�V���b�g�i���j */
	SE_ESHOT_L,		/* �G�V���b�g�i��j */
	SE_EBOMB_M,		/* �G�y���e�i���j */
	SE_EBOMB_L,		/* �G�y���e�i��j */
	SE_LASER,		/* �G���[�U�[ */
	SE_EXLASER,		/* �G�ɑ����[�U�[ */
	SE_EXPL,		/* �G�U�R�̔��� */
	SE_LOCK,		/* �G�L�������̉��i���j */
	SE_LOCK_M,		/* �G�L�������̉��i���j */
	SE_LOCK_L,		/* �G�L�������̉��i��j */
	SE_BOMB_CHARGE,		/* �{���o�[�G�l���M�[�`���[�W�� */
	SE_BOMB,		/* �{���o�[���i�`�{�^�����������u�ԁj */
	SE_BOMB_A,		/* �{���o�[���i�{���o�[�j */
	SE_BOMB_B,		/* �{���o�[���i�o�b�N�{���o�[�j */
	SE_BOMB_C,		/* �{���o�[���i���[�U�[�j */
	SE_EXPL_M,		/* ���{�X���� */
	SE_EXPL_L,		/* �{�X�L�������� */
	SE_PLAYER_EXPL,		/* �v���C���[�̔��� */
	SE_EXTEND,		/* �G�N�X�e���h */
};



/* ���ʉ��������D�揇�ʁi���ɍs���قǍ����j */
enum {
	MUSIC_1 = 1,		/* ���g�p */
	MUSIC_STAGE_CLEAR,
	MUSIC_GAME_OVER,
	MUSIC_RANKING_1ST,
	MUSIC_RANKING_2ND,
	MUSIC_6,
	MUSIC_7,
	MUSIC_BOSS,
	MUSIC_LBOSS,
	MUSIC_10,
	MUSIC_STAGE1,
	MUSIC_STAGE2,
	MUSIC_STAGE3,
	MUSIC_STAGE4,
};


/* �֐��v���g�^�C�v�錾 */
int SoundInit0 (void);
void SoundSetSE (short);
void SoundMove (void);
void SoundMusic (int no);
void SoundFadeOut (int no);
void SoundStop (void);
void SoundTini (void);
void SoundTini0 (void);
