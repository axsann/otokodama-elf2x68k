/* txfont.h */

#ifdef GLOBAL_DEFINE		/* �O���[�o���ϐ��̒�`�Ɛ錾���P�ɂ܂Ƃ߂�e�N */
#define Extern			/* Extern ���k��������ɒu�� */
#else
#define Extern extern		/* Extern �� extern �ɒu�� */
#endif


/* �O���[�o���ϐ� */
Extern unsigned char font_data[2048];	/* �ǂݍ��񂾃t�H���g�f�[�^ */


/* �֐��v���g�^�C�v�錾 */
void TxfontPage (char *);
void TxfontCursor (short, short);
int TxfontInit0 (void);
void TxfontPutchar (char);
void TxfontPuts (char *);
void TxfontTini0 (void);
