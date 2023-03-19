/* effect.h */

typedef struct _effect {
	signed short x, y;	/* ���W */
	short pt;		/* �X�v���C�g�p�^�[���m���D */
	short info;		/* ���]�R�[�h�E�F�E�D��x��\�킷�f�[�^ */
	short type;		/* ��� */
	short type2;		/* ��ނ��̂Q */
	signed int lx, ly;	/* 32bit X,Y ���W ( l = longword ) */
	signed int vx, vy;	/* ���x */
	short seq;		/* ���݂̏�� */
	short seq2;
	short seq3;
	short (*func_effect_move) (struct _effect *);	/* �ړ��֐��ւ̃|�C���^ */
	struct _effect *next;	/* ���̍\���̂ւ̃|�C���^ */
} EFFECT;


#ifdef GLOBAL_DEFINE		/* �O���[�o���ϐ��̒�`�Ɛ錾���P�ɂ܂Ƃ߂�e�N */
#define Extern			/* Extern ���k��������ɒu�� */
#else
#define Extern extern		/* Extern �� extern �ɒu�� */
#endif


/* �O���[�o���ϐ� */
Extern int *points;


enum {
	EFFECT_EXPL = 0,
	EFFECT_EXPLZAKO,
	EFFECT_EXPLMINI,
	EFFECT_EXPLPLAYER,
	EFFECT_BOMBSTRA,
	EFFECT_BOMBSTRB,
	EFFECT_BOMBSTRC,
	EFFECT_HAHEN,
	EFFECT_HAHENMINI,
	EFFECT_POINTS_RED,
	EFFECT_POINTS_BLUE,
	EFFECT_POINTS,
	EFFECT_BOMBER,
	EFFECT_BACKB,
	EFFECT_BLASER,
	EFFECT_TITSPARK,
	EFFECT_TITMOJI,
	EFFECT_LASTLASER,
	EFFECT_LASTPLAYER,
};


enum {
	POINTS_100 = 0,
	POINTS_200,
	POINTS_300,
	POINTS_400,
	POINTS_500,
	POINTS_600,
	POINTS_700,
	POINTS_800,
	POINTS_900,
	POINTS_1000,
	POINTS_1200,
	POINTS_1500,
	POINTS_1800,
	POINTS_2000,
	POINTS_2500,
	POINTS_3000,
	POINTS_4000,
	POINTS_5000,
	POINTS_6000,
	POINTS_7000,
	POINTS_8000,
	POINTS_9000,
	POINTS_10000,
	POINTS_12000,
	POINTS_15000,
	POINTS_18000,
	POINTS_20000,
	POINTS_25000,
	POINTS_28000,
	POINTS_30000
};



/* �֐��v���g�^�C�v�錾 */
void EffectInit0 (void);
void EffectInit (short, short, short, short);
void EffectMove (void);
void EffectTini (void);


void EffectInitExpl (EFFECT *);
void EffectInitExplZako (EFFECT *);
void EffectInitExplMini (EFFECT *);
void EffectInitExplPlayer (EFFECT *);
void EffectInitBombstr (EFFECT *);
void EffectInitHahen (EFFECT *);
void EffectInitHahenMini (EFFECT *);
void EffectInitPointsRed (EFFECT *);
void EffectInitPointsBlue (EFFECT *);
void EffectInitPoints (EFFECT *);
void EffectInitBomber (EFFECT *);
void EffectInitBackB (EFFECT *);
void EffectInitBLaser (EFFECT *);
void EffectInitTitSpark (EFFECT *);
void EffectInitTitMoji (EFFECT *);
void EffectInitLastLaser (EFFECT *);
void EffectInitLastPlayer (EFFECT *);
