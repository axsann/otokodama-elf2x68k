/* psearch.h */

#ifdef GLOBAL_DEFINE		/* �O���[�o���ϐ��̒�`�Ɛ錾���P�ɂ܂Ƃ߂�e�N */
#define Extern			/* Extern ���k��������ɒu�� */
#else
#define Extern extern		/* Extern �� extern �ɒu�� */
#endif


/* �O���[�o���ϐ� */
Extern unsigned char pstable[256][256];


/* �֐��v���g�^�C�v�錾 */
unsigned char psearch (signed short, signed short);
unsigned char msearch (signed short, signed short, signed short, signed short);
