/* entry.h */

typedef struct _entry {
	short type;		/* ��� */
	signed short x, y;	/* ���W */
	short arg;		/* �\���̈����� */
	short control;		/* ��Փx�����p */
	short timer;		/* ���̃L�����N�^�[���o�Ă���܂ł̎��� */
} ENTRY;

#define ENTRY_MAX	768


#ifdef GLOBAL_DEFINE		/* �O���[�o���ϐ��̒�`�Ɛ錾���P�ɂ܂Ƃ߂�e�N */
#define Extern			/* Extern ���k��������ɒu�� */
#else
#define Extern extern		/* Extern �� extern �ɒu�� */
#endif


/* �O���[�o���ϐ� */
Extern ENTRY stage_data[STAGES][ENTRY_MAX];
Extern int entry_counter;
Extern signed int entry_timer;
Extern unsigned char entry_counter_stop;	/* =!0:�G���g���[���ɐi�߂Ȃ��i�{�X�퓙�j */
Extern unsigned short entry_control;	/* ���ݏo�����Ă���G�̏d�ݒ������� */


/* �֐��v���g�^�C�v�錾 */
void EntryInit0 (void);
void EntryInit (void);
void EntryMove (void);
void EntryTini (void);
