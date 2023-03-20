/* ���͖��� UPL �ƁA�r�f�I�Q�[�����D�������������ɖ{�������� */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <x68k/dos.h>
#include <x68k/iocs.h>
#include <XSP2lib.H>
#include <PCM8Afnc.H>
#include <apicglib.h>

#define GLOBAL_DEFINE		/* �O���[�o���ϐ����m�ۂ��� */
#include "otoko.h"
#include "player.h"
#include "shot.h"
#include "enemy.h"
#include "eshot.h"
#include "effect.h"
#include "entry.h"
#include "psearch.h"
#include "txfont.h"
#include "gvram.h"
#include "sound.h"


#define	PCG_MAX	668		/* �X�v���C�g�o�b�f�p�^�[���ő�g�p�� */
#define	REF_MAX	872		/* �����X�v���C�g���t�@�����X�f�[�^�ő�g�p�� */
#define	FRM_MAX	22038		/* �����X�v���C�g�t���[���f�[�^�ő�g�p�� */
#define RGB(r,g,b) ((g)<<11|(r)<<6|(b)<<1)


static char pcg_alt[PCG_MAX + 1];	/* �o�b�f�z�u�Ǘ��e�[�u�� */
static char pcg_dat[PCG_MAX * 128];	/* �o�b�f�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
static XOBJ_REF_DAT ref_dat[REF_MAX];	/* ���t�@�����X�f�[�^ */
static XOBJ_FRM_DAT frm_dat[FRM_MAX];	/* �t���[���f�[�^ */
static int old_crtmod;

static int sizeof_pcg = 0;	/* �X�v���C�g�o�b�f�p�^�[���ǂݍ��ݐ� */
static int sizeof_ref = 0;	/* �����X�v���C�g���t�@�����X�f�[�^�ǂݍ��ݐ� */
static int sizeof_frm = 0;	/* �����X�v���C�g�t���[���f�[�^�ǂݍ��ݐ� */


static unsigned short blue_core[28][5];
static unsigned short red_core[28][5];

static int high_score[10];
static char high_score_name[10][4];
static char high_score_date[10][11];	/* yyyy/mm/dd */
static char high_score_changed = 0;	/* �n�C�X�R�A�������o���K�v�����邩 */

static int xsp_mode_num;
static char n_option;
static int y_option;		/* �J�n�ʐ�-1 */
static int z_option;		/* �c�@�� */

#define	SAVE_GRPALET	32	/* �ۑ�����p���b�g���i�^�C�g���́u�j�v�u�e�v�p�j */
static unsigned short grpalet[SAVE_GRPALET];

enum {
	IN_TITLE = 0, IN_GAME, IN_GAME_OVER
};
static char game_state = IN_TITLE;

enum {
	SELECT_START = 0, SELECT_REPLAY_BEFORE, SELECT_REPLAY_HIGH, SELECT_RANKING, SELECT_EXIT
};

static int demo_state = SELECT_RANKING;


#define WAIT_TIME	6
#define WAIT_DEMO	(10*55)	/* �^�C�g����ʂł���ȏ㑀�삳��Ȃ�������f�� */


static void Usage (void)
{
	printf ("�j�̃V���[�e�B���O�E�j�e otoko.x ver1.00\n"
		"		programmed by Mitsuky <FreeSoftware>\n"
		"�g�p�@ : otoko [option]\n"
		"[option]\n"
	/* "	-E : �G���g���[�ҏW���[�h\n"	 */
		"	-S�t�@�C���� : �n�C�X�R�A�ۑ��t�@�C���̎w��\n"
		"	-N : �n�C�X�R�A���t�@�C���ɕۑ����Ȃ�\n"
	  "	-D���l : �\������ �i���l = 0�`2�i���l���傫���قǏd���j, �f�t�H���g = 1�j\n"
		"	-X���l : xsp_mode �i���l = 1�`3, �f�t�H���g = 2�j\n"
		"	-Y���l : �J�n�ʐ�\n�i���l = 1�`4, �f�t�H���g = 1�j"
		"	-Z���l : �c�@���i���l = 1�`9, �f�t�H���g = 3�j\n"
		"�n�C�X�R�A�͕W���ݒ�i�J�n�ʐ� = 1, �c�@�� = 3�j���̂݋L�^����܂�\n"
	);
}



/* �����������ƂɌĂ΂�郋�[�`�� */
static void VdispRoutine (void)
{
	static short pal = 0, pal2 = 0;
	static short *b_ptr = &blue_core[0][0], *r_ptr = &red_core[0][0];
	unsigned short *d;
	static int y0, y1;

	switch (game_state) {
	case IN_GAME:
		/* �R�A��_�ł����� */
		pal2 = !pal2;
		if (pal2) {
			d = (unsigned short *) (0xe82200 + 4 * 16 * 2 + 2);
			*d++ = *b_ptr++;
			*d++ = *b_ptr++;
			*d++ = *b_ptr++;
			*d++ = *b_ptr++;
			*d++ = *b_ptr++;
		} else {
			d = (unsigned short *) (0xe82200 + 5 * 16 * 2 + 2);
			*d++ = *r_ptr++;
			*d++ = *r_ptr++;
			*d++ = *r_ptr++;
			*d++ = *r_ptr++;
			*d++ = *r_ptr++;
			pal++;
		}
		if (pal > 27) {
			pal = 0;
			b_ptr = &blue_core[0][0];
			r_ptr = &red_core[0][0];
		}
		if (pause_flag)
			return;	/* �|�[�Y���Ȃ炱���܂� */

		/* �|�[�Y���łȂ���΍s������ */
		*(volatile unsigned int *) 0xe80018 = *(volatile unsigned int *) 0xe8001c = y0;
		y0 -= 2;
		y0 &= 511;

		*(volatile unsigned int *) 0xe80020 = *(volatile unsigned int *) 0xe80024 = y1;
		if (stage != 0) {
			y1 -= 4;
			y1 &= 511;
		} else {
			y1 -= 2;
			y1 &= 511;
		}

		/* �w�i�̃t�F�[�h�C���^�t�F�[�h�A�E�g�����H */
		switch (bg_contrast_control) {
		case BG_NORMAL:/* �ʏ�̃Q�[���� */
		case BG_DRAWING:	/* �w�i�`�撆 */
			break;

		case BG_FADEIN:/* �t�F�[�h�C���� */
			{
				unsigned short *s = bg_contrast_ptr;
				unsigned short *d = (unsigned short *) (0xe82000 + 16 * 2);
				unsigned short h = BG_PALETS / 4;	/* ��������p���b�g�� */
				do {
					*d++ = *s++;
					*d++ = *s++;
					*d++ = *s++;
					*d++ = *s++;
				} while (--h);
				bg_contrast_ptr -= BG_PALETS;
				bg_contrast_num--;
				if (bg_contrast_num <= 0) {
					bg_contrast_control = BG_NORMAL;	/* �t�F�[�h�C���I�� */
					bg_contrast_ptr = &stage_palet_table[stage][0][0];
				} else {
					if (bg_contrast_num == 32)
						y0 = y1 = 0;	/* �X�N���[�����W������ */
				}
			}
			break;
		case BG_FADEOUT:	/* �t�F�[�h�A�E�g�� */
			{
				unsigned short *s = bg_contrast_ptr;
				unsigned short *d = (unsigned short *) (0xe82000 + 16 * 2);
				unsigned short h = BG_PALETS / 4;	/* ��������p���b�g�� */
				do {
					*d++ = *s++;
					*d++ = *s++;
					*d++ = *s++;
					*d++ = *s++;
				} while (--h);
				bg_contrast_ptr += BG_PALETS;
				bg_contrast_num--;
				if (bg_contrast_num <= 0) {
					bg_contrast_control = BG_NORMAL;	/* �t�F�[�h�A�E�g�I�� */
					entry_counter_stop = 0;	/* �G���g���[�ĊJ */
					*(unsigned short *) (0xe82000) = 0;
					d = (unsigned short *) (0xe82000 + 16 * 2);
					h = BG_PALETS / 4;
					do {
						*d++ = 0;
						*d++ = 0;
						*d++ = 0;
						*d++ = 0;
					} while (--h);
				}
			}
			break;
		}

		/* �{���o�[�Ŕw�i�������Ă��鎞�̏��� */
		switch (player->bomb_flash_state) {
		case BG_FLASH_NON:
			break;
		case BG_FLASH_WHITE:	/* ���t���b�V�� */
			{
				unsigned short *d = (unsigned short *) (0xe82000 + 16 * 2);
				unsigned short h = BG_PALETS / 4;	/* ��������p���b�g�� */
				*(unsigned short *) (0xe82000) = RGB (24, 24, 24);	/* �t���b�V���F */
				do {
					*d++ = RGB (24, 24, 24);	/* �t���b�V���F */
					*d++ = RGB (24, 24, 24);
					*d++ = RGB (24, 24, 24);
					*d++ = RGB (24, 24, 24);
				} while (--h);
			}
			break;
		case BG_FLASH_NORMAL:	/* �ʏ�̃p���b�g */
			{
				unsigned short *s;
				unsigned short *d = (unsigned short *) (0xe82000 + 16 * 2);
				unsigned short h = BG_PALETS / 4;	/* ��������p���b�g�� */

				*(unsigned short *) (0xe82000) = 0;
				switch (bg_contrast_control) {
				case BG_NORMAL:
					s = &stage_palet_table[stage][0][0];
					do {
						*d++ = *s++;
						*d++ = *s++;
						*d++ = *s++;
						*d++ = *s++;
					} while (--h);
					break;
				case BG_DRAWING:
					do {
						*d++ = 0;
						*d++ = 0;
						*d++ = 0;
						*d++ = 0;
					} while (--h);
					break;
				case BG_FADEIN:
				case BG_FADEOUT:
					break;	/* ��̕��Őݒ肵�Ă���̂ŕK�v���� */
				}
			}
			break;
		}
		break;

	case IN_TITLE:
	case IN_GAME_OVER:
		break;
	}
}



/* �I�����ɂP�x�����Ă΂��E�G���[��������Ԃ��Ă��Ȃ� */
void Tini (void)
{
	int sp;
	char *error_message_table[]=
	{
		"Thanks for playing !\n",
		"�e�L�X�g VRAM ���g�p���ł�\n",
		"�O���t�B�b�N VRAM ���g�p���ł�\n"
	};

	if (error_level < ERROR_FILE)
		error_message = error_message_table[error_level];

	switch (error_level) {
	case ERROR_NON:
	case ERROR_FILE:
		_iocs_tgusemd (1, 0);
	case ERROR_GUSEMD:
		_iocs_tgusemd (0, 0);
	case ERROR_TUSEMD:
	default:
		break;
	}


	xsp_vsyncint_off ();
	pcm8a_vsyncint_off ();
	xsp_off ();
	SoundStop ();

	sp = _iocs_b_super (0);
	*(unsigned short *) 0xe8e006 &= 0xfffd;	/* HRL �r�b�g */
	_iocs_b_super (sp);

	_iocs_skey_mod (-1, 0, 0);
	_iocs_b_curon ();
	_iocs_crtmod (old_crtmod);

	_dos_kflushio (0xff);	/* �L�[�o�b�t�@���N���A */
	printf ("%s", error_message);

	if (error_level > ERROR_NON)	/* �G���[�����������ꍇ�� */
		exit (-1);	/* �I������ */
}



/* �N�����̃t�@�C���ǂݍ��� */
/* �ǂݍ��݂Ɏ��s�����ꍇ�͂����Ȃ�I������̂Œ��� */
static int LoadFile (char *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int i;

	if ((fp = fopen (fname, "rb")) == NULL) {
		static char e[256];
		error_level = ERROR_FILE;
		error_message = e;
		sprintf (e, "�t�@�C�� %s ���ǂݍ��߂܂���\n", fname);
		Tini ();	/* �Ԃ��Ă��Ȃ� */
	}
	i = fread (ptr, size, n, fp);
	fclose (fp);

	return (i);		/* �Ԃ�l�͓ǂݍ��ݍ��ڐ� */
}



/* SP �f�[�^�ǂݍ��� */
static short LoadSP (char *fname)
{
	int p;
	short sp_no = sizeof_pcg / 128;
	char temp_fname[92];

	strcpy (temp_fname, fname);
	strcat (temp_fname, ".SP");
	p = LoadFile (temp_fname, &pcg_dat[sizeof_pcg], sizeof (char), 128 * PCG_MAX - sizeof_pcg);

	sizeof_pcg += p;

#ifdef DEBUG
	printf ("%s ��ǂݍ��݂܂��� sp_no = %hd\n", fname, sp_no);
#endif
	return (sp_no);
}



/* XSP �e��f�[�^�ǂݍ��� */
static short LoadXSP (char *fname)
{
	int i;
	int p, f, r;
	short obj_no = sizeof_ref;
	char temp_fname[92];

	strcpy (temp_fname, fname);
	strcat (temp_fname, ".FRM");
	f = LoadFile (temp_fname, &frm_dat[sizeof_frm], sizeof (XOBJ_FRM_DAT), FRM_MAX - sizeof_frm);

	strcpy (temp_fname, fname);
	strcat (temp_fname, ".REF");
	r = LoadFile (temp_fname, &ref_dat[sizeof_ref], sizeof (XOBJ_REF_DAT), REF_MAX - sizeof_ref);

	strcpy (temp_fname, fname);
	strcat (temp_fname, ".XSP");
	p = LoadFile (temp_fname, &pcg_dat[sizeof_pcg], sizeof (char), 128 * PCG_MAX - sizeof_pcg);

	/* FRM_DAT[].pt �␳ */
	for (i = 0; i < f; i++)
		frm_dat[sizeof_frm + i].pt += sizeof_pcg / 128;

	/* REF_DAT[].ptr �␳ */
	for (i = 0; i < r; i++)
		ref_dat[sizeof_ref + i].ptr = (int) ref_dat[sizeof_ref + i].ptr + (int) (&frm_dat[sizeof_frm]);


	sizeof_pcg += p;
	sizeof_frm += f;
	sizeof_ref += r;

#ifdef DEBUG
	printf ("%s ��ǂݍ��݂܂��� sizeof_ref = %hd\n", fname, obj_no);
#endif

	return (obj_no);
}



/* �N�����ɂP�x�����Ă΂�� */
static void Init (void)
{
	int i, sp;
	unsigned short *s, *d;
	unsigned short pal_dat[16 * 15];	/* �p���b�g�f�[�^�t�@�C���ǂݍ��݃o�b�t�@ */
	unsigned short crtcdata[9] =
	{69, 6, 11 + 8, 59 - 8, 567, 5, 40, 552, 0x0111};
	unsigned short textpalet[16] =
	{0, RGB (16, 16, 20), RGB (12, 12, 20), RGB (28, 28, 31), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	old_crtmod = _iocs_crtmod (-1);
	error_message = NULL;

	error_level = ERROR_TUSEMD;
	if (_iocs_tgusemd (1, -1) == 2)	/* �e�L�X�g��ʂ̎g�p�󋵂��`�F�b�N */
		Tini ();

	error_level = ERROR_GUSEMD;
	if (_iocs_tgusemd (0, -1))	/* �O���t�B�b�N��ʂ̎g�p�󋵂��`�F�b�N */
		Tini ();
	_iocs_tgusemd (0, 2);
	_iocs_tgusemd (1, 2);

	/* CD-ROM ����̓ǂݍ��݂𑬂����邽�߂ɃA���t�@�x�b�g���Ƀ\�[�g���Ă��肵�� */
	LoadFile ("DAT/ALPHA2.DAT", font_data, sizeof (char), 2048);
	LoadFile ("DAT/STAGE01.DAT", &stage_data[0][0], sizeof (ENTRY), ENTRY_MAX);
	LoadFile ("DAT/STAGE02.DAT", &stage_data[1][0], sizeof (ENTRY), ENTRY_MAX);
	LoadFile ("DAT/STAGE03.DAT", &stage_data[2][0], sizeof (ENTRY), ENTRY_MAX);
	LoadFile ("DAT/STAGE04.DAT", &stage_data[3][0], sizeof (ENTRY), ENTRY_MAX);

	LoadFile ("BG/STAGE01.MAP", &bg_map[0][0], sizeof (char), 30 + 32 * 128 * 2);
	LoadFile ("BG/STAGE01.SP", &bg_sp[0][0], sizeof (char), 32 * 256);
	LoadFile ("BG/STAGE01G.PAL", &stage_palet_table[0][0][0], sizeof (unsigned short), BG_PALETS * 33);
	LoadFile ("BG/STAGE02.MAP", &bg_map[1][0], sizeof (char), 30 + 32 * 128 * 2);
	LoadFile ("BG/STAGE02.SP", &bg_sp[1][0], sizeof (char), 32 * 256);
	LoadFile ("BG/STAGE02G.PAL", &stage_palet_table[1][0][0], sizeof (unsigned short), BG_PALETS * 33);
	LoadFile ("BG/STAGE03.MAP", &bg_map[2][0], sizeof (char), 30 + 32 * 128 * 2);
	LoadFile ("BG/STAGE03.SP", &bg_sp[2][0], sizeof (char), 32 * 256);
	LoadFile ("BG/STAGE03G.PAL", &stage_palet_table[2][0][0], sizeof (unsigned short), BG_PALETS * 33);
	LoadFile ("BG/STAGE04.MAP", &bg_map[3][0], sizeof (char), 30 + 32 * 128 * 2);
	LoadFile ("BG/STAGE04.SP", &bg_sp[3][0], sizeof (char), 32 * 256);
	LoadFile ("BG/STAGE04G.PAL", &stage_palet_table[3][0][0], sizeof (unsigned short), BG_PALETS * 33);

	obj_bombstr = LoadXSP ("SP/BOMBSTR");
	obj_blaser = LoadXSP ("SP/BLASER");
	sp_core01 = LoadSP ("SP/CORE01");
	obj_coverf = LoadXSP ("SP/COVERF");
	obj_covern = LoadXSP ("SP/COVERN");
	sp_cursor = LoadSP ("SP/CURSOR");
	sp_eshot01 = LoadSP ("SP/ESHOT01");
	sp_eshotera = LoadSP ("SP/ESHOTERA");
	obj_explall = LoadXSP ("SP/EXPLALL");
	obj_hahen = LoadXSP ("SP/HAHEN");
	sp_laser01 = LoadSP ("SP/LASER01");
	obj_lboss01 = LoadXSP ("SP/LBOSS01");
	obj_lboss02 = LoadXSP ("SP/LBOSS02");
	obj_lboss03 = LoadXSP ("SP/LBOSS03");
	obj_lboss04 = LoadXSP ("SP/LBOSS04");
	sp_lboss03g = LoadSP ("SP/LBOSS03G");
	obj_mvlaser = LoadXSP ("SP/MVLASER");
	obj_mvlaserh = LoadXSP ("SP/MVLASERH");
	obj_oplaser = LoadXSP ("SP/OPLASER");
	obj_player = LoadXSP ("SP/PLAYER");
	obj_points = LoadXSP ("SP/POINTS");
	obj_round4 = LoadXSP ("SP/ROUND4");
	obj_searchz = LoadXSP ("SP/SEARCHZ");
	obj_sword = LoadXSP ("SP/SWORD");
	sp_shot = LoadSP ("SP/SHOT");
	obj_titmoji = LoadXSP ("SP/TITMOJI");
	obj_titspark = LoadXSP ("SP/TITSPARK");
	obj_zako02 = LoadXSP ("SP/ZAKO02");
	obj_zako03 = LoadXSP ("SP/ZAKO03");
	obj_zako04 = LoadXSP ("SP/ZAKO04");

#ifdef DEBUG
	printf ("	PCG �g�p�� = %d\n	REF �g�p�� = %d\n	FRM �g�p�� = %d\n",
		sizeof_pcg / 128, sizeof_ref, sizeof_frm);
#endif

	LoadFile ("SP/OTOKO.PAL", pal_dat, 2, 16 * 15);

	LoadFile ("TBL/BLUECORE.PAL", blue_core, sizeof (unsigned short), 28 * 5);
	LoadFile ("TBL/INFO_ROT.TBL", info_rot, sizeof (short), 256);
	LoadFile ("TBL/PSTABLE.TBL", pstable, sizeof (unsigned char), 256 * 256);
	LoadFile ("TBL/RNDTABLE.TBL", rndtable, sizeof (unsigned char), 256);
	LoadFile ("TBL/REDCORE.PAL", red_core, sizeof (unsigned short), 28 * 5);
	LoadFile ("TBL/PT_ROT.TBL", pt_rot, sizeof (short), 256);
	LoadFile ("TBL/VXTABLE.TBL", vxtable, sizeof (signed int), 256 * SPEED_MAX);
	LoadFile ("TBL/VYTABLE.TBL", vytable, sizeof (signed int), 256 * SPEED_MAX);


	/* ��ʃ��[�h�ݒ� */
	_iocs_crtmod (10);	/* 256*256dot 31kHz 256�F */
	_iocs_g_clr_on ();
	_iocs_b_curoff ();
	_iocs_ms_curof ();
	_iocs_skey_mod (0, 0, 0);

	_iocs_sp_init ();
	_iocs_sp_on ();		/* �X�v���C�g�\�����n�m */
	_iocs_bgctrlst (0, 0, 0);	/* �a�f�O�\���n�e�e */
	_iocs_bgctrlst (1, 1, 0);	/* �a�f�P�\���n�e�e */

	/* 256x256 �����`���[�h����� */
	sp = _iocs_b_super (0);
	*(unsigned short *) 0xe80028 = crtcdata[8];
	d = (unsigned short *) 0xe80002;
	for (i = 1; i < 8; i++)
		*d++ = crtcdata[i];
	*(unsigned short *) 0xe80000 = crtcdata[0];

	*(unsigned short *) 0xe8e006 |= 0b10;	/* HRL �r�b�g */
	*(unsigned short *) 0xeb080c = crtcdata[2] + 4;
	*(unsigned short *) 0xeb080a = crtcdata[0];	/* �X�v���C�g */
	*(unsigned short *) 0xeb080e = crtcdata[6];
	*(volatile unsigned int *) 0xe80018 = *(volatile unsigned int *) 0xe8001c
		= *(volatile unsigned int *) 0xe80020 = *(volatile unsigned int *) 0xe80024
		= (256 << 16) + 256;

	*(unsigned short *) 0xe82500 = 0b01001001001110;	/* �D�揇�� TX>SP>GR */


	/* �^�C�g����.PIC �t�@�C����ǂݍ��� */
	{
		unsigned char *file_buf, *work_buf;
		int r;
		error_level = ERROR_PIC_MEMORY;
#define	PIC_FILE_BUF_SIZE	(16*1024)
		file_buf = _dos_malloc (PIC_FILE_BUF_SIZE);
		//work_buf = _dos_malloc (512 * 1024);
		work_buf = _dos_malloc (128 * 256);
		if (((int) file_buf < 0) || ((int) work_buf < 0)) {
			static char e[256];
			printf ("�������G���[\n");
			error_level = ERROR_PIC_MEMORY;
			error_message = e;
			strcpy (e, ".PIC �W�J�p������������܂���\n");
			Tini ();/* �Ԃ��Ă��Ȃ� */
		} else {
			error_level = ERROR_PIC_FILE;
			r = APICLOAD ((unsigned short *) 0xc00000, "SP/OTOKO.PIC", 256, 0,
				      file_buf, PIC_FILE_BUF_SIZE, APF_NOINITCRT | APF_NOCLRBUF | APF_NOPRFC, work_buf);
			if (r < 0) {
				static char e[256];
				error_level = ERROR_PIC_FILE;
				error_message = e;
				strcpy (e, "SP/OTOKO.PIC ���ǂݍ��߂܂���\n");
				Tini ();	/* �Ԃ��Ă��Ȃ� */
			}
			r = APICLOAD ((unsigned short *) 0xc00000, "SP/DAMA.PIC",
			/* ���[�h���W���������鎖�ɂ�� 256�F���[�h�̃y�[�W�P�ɓǂݍ��� */
			/* �i�f���ɃA�h���X�� 0xc80000 �ɂ���Ɠǂ߂Ȃ��݂����j */
				      256, 512 + 128,	/* ���[�h�w�^�x���W */
				      file_buf, PIC_FILE_BUF_SIZE, APF_NOINITCRT | APF_NOCLRBUF | APF_NOPRFC, work_buf);
			if (r < 0) {
				static char e[256];
				error_level = ERROR_PIC_FILE;
				error_message = e;
				strcpy (e, "SP/DAMA.PIC ���ǂݍ��߂܂���\n");
				Tini ();	/* �Ԃ��Ă��Ȃ� */
			}
			_dos_mfree (work_buf);
			_dos_mfree (file_buf);
		}
	}

	/* �O���t�B�b�N��ʂ̃p���b�g��ۑ� */
	{
		unsigned short *p = (unsigned short *) (0xe82000 + (256 - SAVE_GRPALET) * 2);
		for (i = 0; i < SAVE_GRPALET; i++)
			grpalet[i] = *p++;
	}

	error_level = ERROR_MUSIC;

	/* �p���b�g�ɓ]�� */
	s = (unsigned short *) 0xe82200;
	for (i = 0; i < 16; i++)
		*s++ = textpalet[i];
	d = pal_dat;
	for (i = 0; i < 15 * 16; i++)
		*s++ = *d++;
	_iocs_b_super (sp);

	/* ��ʓI�ȏ����� */
	EntryInit0 ();
	PlayerInit0 ();
	ShotInit0 ();
	EnemyInit0 ();
	EshotInit0 ();
	EffectInit0 ();
	TxfontInit0 ();
	SoundInit0 ();


	_dos_breakck (2);	/* BREAK �`�F�b�N���E�� */
	_dos_intvcs (0xfff1, Tini);	/* _CTRLVC */
	_dos_intvcs (0xfff2, Tini);	/* _ERRJVC */

	xsp_on ();
	xsp_mode (xsp_mode_num);
	xsp_pcgdat_set (pcg_dat, pcg_alt, sizeof (pcg_alt));
	xsp_objdat_set (ref_dat);
	xsp_vsyncint_on (&VdispRoutine);
	pcm8a_vsyncint_on ();

	sp = _iocs_b_super (0);
	*(unsigned short *) 0xe82600 |= 0x007f;	/* ��ʕ\���I�� */
	_iocs_b_super (sp);



	error_level = ERROR_NON;
	return;
}




static void TextClear (void)
{
	_iocs_txrascpy (256 / 4 * 256, 256 / 4, 0b1111);	/* �e�L�X�g��ʃN���A */
}


/* �{�^����������邩 timer �t���[���o�܂ő҂� */
static void WaitTime (int timer)
{
	int t = timer;
	while ((_iocs_joyget (0) & 0x60) != 0x60);
	do {
		xsp_vsync (1);
		if ((_iocs_joyget (0) & 0x60) != 0x60)
			break;
	} while (t--);
	while ((_iocs_joyget (0) & 0x60) != 0x60);
}


/* �O���t�B�b�N��ʂ̃p���b�g�𔼕��̖��邳�� */
static void HalfColor (void)
{
	unsigned short *p = (unsigned short *) (0xe82000 + (256 - SAVE_GRPALET) * 2);
	int i, sp;

	sp = _iocs_b_super (0);
	for (i = 0; i < SAVE_GRPALET; i++) {
		unsigned short c, r, g, b;
		c = grpalet[i];
		r = ((c >> 6) & 31) / 2;
		g = ((c >> 11) & 31) / 2;
		b = ((c >> 1) & 31) / 2;
		*p++ = (((((g << 5) | r) << 5) | b) << 1);
	}
	_iocs_b_super (sp);
}


/* �n�C�X�R�A�\�� */
static void Ranking (void)
{
	short h;
	char temp_str[64];

	TextClear ();
	TxfontCursor (8, 3);
	TxfontPuts ("HARDCORE SHOOTERS");

#define	RANK_X	3
#define	RANK_Y	7
	for (h = 0; h < 10; h++) {
		sprintf (temp_str, "%2hd  %s %06d  %s",
			 h + 1, &high_score_name[h], high_score[h], &high_score_date[h]);
		TxfontCursor (RANK_X, h * 2 + RANK_Y);
		TxfontPuts (temp_str);
	}
}



/* �n�C�X�R�A�l�[���G���g���[ */
static void NameEntry (short rank)
{
	char temp[4];
	char ch = 'A';
	int cx = 0;		/* �J�[�\���ʒu */
	time_t now;
	struct tm *tloc;

	enum {
		JOY_NON = 0, JOY_LEFT, JOY_RIGHT
	};

	if (rank != 9) {
		short h;
		/* �n�C�X�R�A���P�Â��炷 */
		for (h = 9; h > rank; h--) {
			high_score[h] = high_score[h - 1];
			strcpy (high_score_name[h], high_score_name[h - 1]);
			strcpy (high_score_date[h], high_score_date[h - 1]);
		}
	}
	high_score[rank] = score;
	strcpy (high_score_name[rank], "A__");
	now = time (NULL);
	tloc = localtime (&now);
	sprintf (high_score_date[rank], "%4d/%02d/%02d",
		 tloc->tm_year + 1900, tloc->tm_mon + 1, tloc->tm_mday);

	Ranking ();
	HalfColor ();

	high_score_changed = !0;
	if (rank == 0) {
		int i;
		int *p1 = (unsigned int *) (&replay_data[0]), *p2 = (unsigned int *) (&high_score_replay_data[0]);
		SoundMusic (MUSIC_RANKING_1ST);	/* �P�ʂ̎� */
		/* ����̃��v���C�f�[�^���n�C�X�R�A�̃��v���C�f�[�^�ɃR�s�[ */
		high_score_replay_data_size = player->replay_counter;
		for (i = 0; i < (high_score_replay_data_size + 15) / 16; i++) {
			*p2++ = *p1++;
			*p2++ = *p1++;
			*p2++ = *p1++;
			*p2++ = *p1++;
		}
	} else {
		SoundMusic (MUSIC_RANKING_2ND);
	}

#define RANK_NAME_X	(RANK_X+4)

	for (cx = 0; cx < 3; cx++) {
		int old_t = JOY_NON, ct = 0, ct2 = 0;
		TxfontCursor (RANK_NAME_X + cx, RANK_Y + rank * 2);
		TxfontPutchar (ch);
		for (;;) {
			int t;
			xsp_vsync (1);
			t = _iocs_joyget (0);
			if ((t & 0b0100) == 0) {	/* �� */
				if (old_t != JOY_LEFT) {
					if (--ch < ' ')
						ch = '_';
					TxfontCursor (RANK_NAME_X + cx, RANK_Y + rank * 2);
					TxfontPutchar (ch);
					old_t = JOY_LEFT;
					ct = ct2 = 0;
				} else {
					if (ct++ > WAIT_TIME) {
						old_t = JOY_NON;
						ct = 0;
					}
				}
			}
			if ((t & 0b1000) == 0) {	/* �E */
				if (old_t != JOY_RIGHT) {
					if (++ch > 'Z')
						ch = ' ';
					TxfontCursor (RANK_NAME_X + cx, RANK_Y + rank * 2);
					TxfontPutchar (ch);
					old_t = JOY_RIGHT;
					ct = ct2 = 0;
				} else {
					if (ct++ > WAIT_TIME) {
						old_t = JOY_NON;
						ct = 0;
					}
				}
			}
			if ((t & 0b1111) == 0b1111) {
				old_t = JOY_NON;
				ct = 0;
				ct2++;
				switch (ct2) {
				case 30:
					TxfontCursor (RANK_NAME_X + cx, RANK_Y + rank * 2);
					TxfontPutchar ('_');
					break;
				case 50:
					TxfontCursor (RANK_NAME_X + cx, RANK_Y + rank * 2);
					TxfontPutchar (ch);
					break;
				case 60:
					ct2 = 0;
					break;
				default:
					break;
				}
			}
			if ((t & 0b01000000) == 0) {	/* button B */
				while ((_iocs_joyget (0) & 0b01000000) == 0);
				TxfontCursor (RANK_NAME_X + cx, RANK_Y + rank * 2);
				TxfontPutchar (ch);
				temp[cx] = ch;
				ct2 = 0;
				break;
			}
			if ((t & 0b00100000) == 0) {	/* button A */
				while ((_iocs_joyget (0) & 0b00100000) == 0);
				if (cx != 0) {
					TxfontCursor (RANK_NAME_X + cx, RANK_Y + rank * 2);
					TxfontPutchar ('_');
					cx--;
					ch = temp[cx];
					TxfontCursor (RANK_NAME_X + cx, RANK_Y + rank * 2);
					TxfontPutchar (ch);
					ct2 = 0;
				}
			}
		}
	}
	temp[3] = 0;
	strcpy (high_score_name[rank], temp);
	SoundFadeOut (32);
	WaitTime (55 * 5);
}




static void Game (void)
{
	char temp_str[32];
	int sprites;		/* �X�v���C�g���� */
	int old_score = -1;	/* ����͕K���\�������悤�� */

	TextClear ();
	EntryInit ();
	PlayerInit ();
	EnemyInitStatic ();
	SoundStop ();

	if (standard_play)
		sprintf (temp_str, "HI %06d", high_score[0]);
	else
		sprintf (temp_str, "HI ------");	/* �n�C�X�R�A�W�v�Ȃ� */
	TxfontCursor (21, 0);
	TxfontPuts (temp_str);

	score = 0;
	extend_score = EXTEND_EVERY;
	stage = y_option;
	player->rest = z_option;
	pause_flag = 0;
	game_state = IN_GAME;
	DispRest ();		/* �c�@���\�� */

	do {
		EntryMove ();
		PlayerMove ();
		ShotMove ();
		EnemyMove ();
		EshotMove ();
		EffectMove ();
		GvramMove ();
		SoundMove ();
		sprites = xsp_out ();

#define	SCORE_X	2
#define	SCORE_Y	0
		if (score != old_score) {	/* �X�V���ꂽ���̂ݕ\�� */
			old_score = score;
			sprintf (temp_str, "SCORE %06d", score);
			TxfontCursor (SCORE_X, SCORE_Y);
			TxfontPuts (temp_str);
		}
#ifdef DEBUG
		//sprintf (temp_str, "SPRITES %06d", sprites);
		//TxfontCursor (0, 1);
		//TxfontPuts (temp_str);
		sprintf (temp_str, "ESHOT  %03hd", eshot_sum);
		TxfontCursor (0, 2);
		TxfontPuts (temp_str);
		sprintf (temp_str, "ENEMY  %03hd", enemy_sum);
		TxfontCursor (0, 3);
		TxfontPuts (temp_str);
#endif
	} while (player->game_over == GAME_PLAY);
	EntryTini ();
	PlayerTini ();
	ShotTini ();
	EnemyTini ();
	EshotTini ();
	EffectTini ();
	GvramTini ();
	SoundTini ();

	game_state = IN_GAME_OVER;
	switch (player->game_over) {
	case GAME_OVER:
		TxfontCursor (11, 14);
		TxfontPuts ("GAME OVER");
		SoundMusic (MUSIC_GAME_OVER);
		WaitTime (55 * 10);
		break;
	case GAME_COMPLETE:
		while (--player->rest > 0) {
			score += 5000;	/* �c�@�P�@�ɂ� 5000pts */
			DispRest ();	/* �c�@���\�� */
			sprintf (temp_str, "SCORE %06d", score);
			TxfontCursor (SCORE_X, SCORE_Y);
			TxfontPuts (temp_str);
			if (player->rest > 1)
				WaitTime (30);
			xsp_out ();
		}
		TxfontCursor (11, 14);
		TxfontPuts (" COMPLETE");
		SoundMusic (MUSIC_GAME_OVER);
		WaitTime (55 * 10);
		break;
	case GAME_ABORT:
	default:
		break;
	}
	xsp_out ();		/* �X�v���C�g������ */

	/* �W���ݒ�̎��̂݃n�C�X�R�A���W�v */
	if ((replay_flag == REPLAY_NON) && (standard_play)) {
		replay_data_size = player->replay_counter;

		if ((player->game_over == GAME_OVER) || (player->game_over == GAME_COMPLETE)) {
			short h, rank = 0;

			for (h = 0; h < 10; h++) {
				if (score <= high_score[h])
					rank = h + 1;
			}
			/* �P�ʂȂ� rank=0, 11�ʂȂ� rank=10 */
			if (rank < 10)
				NameEntry (rank);
		}
	}
	return;
}



/* �^�C�g����ʂ̕\�� */
static int Title (void)
{
	int cy = 0;		/* �J�[�\���ʒu */
	int old_t, ct, ct2;
	int t;
	int sp;
	short y_table[20] =
	{
		112, 96, 80, 64, 48, 32, 16, 0, 0, 3, 3, 1, 1, 2, 1, 0, 1, 1, 0, 0
	};
	enum {
		JOY_NON = 0, JOY_UP, JOY_DOWN
	};

	game_state = IN_TITLE;

	/* �O���t�B�b�N��ʂ̃p���b�g�����ɖ߂� */
	{
		unsigned short *p = (unsigned short *) (0xe82000 + (256 - SAVE_GRPALET) * 2);
		int i;
		sp = _iocs_b_super (0);
		xsp_vsync (1);
		*(unsigned short *) 0xe82600 &= 0xff80;	/* ��ʕ\���I�t */
		*(volatile unsigned int *) 0xe80018 = *(volatile unsigned int *) 0xe8001c
			= *(volatile unsigned int *) 0xe80020 = *(volatile unsigned int *) 0xe80024
			= (256 << 16) + 256;
		for (i = 0; i < SAVE_GRPALET; i++)
			*p++ = grpalet[i];
		xsp_vsync (1);
		*(unsigned short *) 0xe82600 |= 0x007f;	/* ��ʕ\���I�� */
		_iocs_b_super (sp);
	}
	TextClear ();

	/* �j�ƒe�̍��� */
	{
		short seq = 0;
		char exit_flag = 0;
		short h = 0;

		do {
			xsp_out ();
			if ((_iocs_joyget (0) & 0x60) != 0x60)
				break;
			switch (seq) {
			case 0:/* �u�j�v�������Ă��� */
				sp = _iocs_b_super (0);
				*(volatile unsigned int *) 0xe80018 = *(volatile unsigned int *) 0xe8001c
					= (256 << 16) + y_table[h];
				*(volatile unsigned int *) 0xe80020 = *(volatile unsigned int *) 0xe80024
					= (256 << 16) + 256;
				_iocs_b_super (sp);
				h++;
				if (h == 7) {
					EffectInit (EFFECT_TITSPARK, 0, 128 + 16, 128 + 16);
					SoundSetSE (SE_LOCK_M);
				}
				if (h > 18) {
					seq++;
					h = 0;
				}
				break;
			case 1:/* �u�e�v���オ���Ă��� */
				sp = _iocs_b_super (0);
				*(volatile unsigned int *) 0xe80020 = *(volatile unsigned int *) 0xe80024
					= (256 << 16) + ((-y_table[h]) & 511);
				_iocs_b_super (sp);
				h++;
				if (h == 7) {
					EffectInit (EFFECT_TITSPARK, 1, 128 + 16, 128 + 16);
					SoundSetSE (SE_LOCK_M);
				}
				if (h > 18) {
					seq++;
					h = 0;
					EffectInit (EFFECT_TITMOJI, 0, 0, 0);
				}
				break;
			case 2:/* ���E����uOTOKO�v�uDAMA�v�� */
				if (h++ > 7) {
					seq++;
					for (h = 0; h < 6; h++)
						EffectInit (EFFECT_EXPL, 0, rndtable[h] / 2 + 64 + 16, rndtable[h + 128] / 2 + 64 + 16);
					EffectInit (EFFECT_EXPL, 0, 128 + 16, 128 + 16);
					SoundSetSE (SE_EXPL_L);	/* �{�X�L���������� */
					h = 0;
				}
				break;
			case 3:
				switch (h++) {
				case 0:
				case 3:
				case 6:
				case 9:
#define FLASH_PALET	224
					/* �p���b�g�𔒂��t���b�V�� */
					sp = _iocs_b_super (0);
					*(unsigned short *) (0xe82000 + FLASH_PALET * 2) = RGB (24, 24, 24);
					_iocs_b_super (sp);
					break;
				case 0 + 1:
				case 3 + 1:
				case 6 + 1:
				case 9 + 1:
					/* �p���b�g�𕜋A */
					sp = _iocs_b_super (0);
					*(unsigned short *) (0xe82000 + FLASH_PALET * 2) = RGB (0, 0, 0);
					_iocs_b_super (sp);
					break;
				case 56:
					exit_flag = !0;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			EffectMove ();
			SoundMove ();
			if (seq > 2) {
				xobj_set (128 + 16 + 16, 128 + 16, obj_titmoji, 0x0130);	/* OTOKO */
				xobj_set (128 + 16 + 16, 128 + 16, obj_titmoji + 1, 0x0130);	/* DAMA */
			}
		} while (exit_flag == 0);
		xsp_out ();
		if (exit_flag != 0)
			WaitTime (3 * 55);
	}
	xobj_set (128 + 16 + 16, 128 + 16, obj_titmoji, 0x0130);	/* OTOKO */
	xobj_set (128 + 16 + 16, 128 + 16, obj_titmoji + 1, 0x0130);	/* DAMA */
	SoundMove ();
	EffectTini ();
	xsp_out ();
	sp = _iocs_b_super (0);
	*(volatile unsigned int *) 0xe80018 = *(volatile unsigned int *) 0xe8001c
		= *(volatile unsigned int *) 0xe80020 = *(volatile unsigned int *) 0xe80024
		= (256 << 16) + 0;
	*(unsigned short *) (0xe82000 + FLASH_PALET * 2) = RGB (0, 0, 0);
	_iocs_b_super (sp);
	while ((_iocs_joyget (0) & 0x60) != 0x60);


	HalfColor ();

#define	START_X	10		/* "START" ��\��������W */
#define	START_Y	20

	TxfontCursor (0, 26);
	TxfontPuts (
			   " (C)PROGRAMMED BY MITSUKY 1999\n"
			   "     GRAPHICS BY H.SUZUKI\n\n"
			   " FOR GEKIKOH-DENNOU CLUB VOL.7\n");
	TxfontCursor (0, START_Y);
	TxfontPuts (
			   "	    START\n"
			   "	    REPLAY\n"
			   "	    REPLAY HIGH\n"
			   "	    RANKING\n"
			   "	    EXIT\n"
		);
	TxfontCursor (START_X, START_Y + cy);
	TxfontPutchar ('>');

	old_t = JOY_NON;
	ct = ct2 = 0;
	do {
		xsp_vsync (1);
		t = _iocs_joyget (0);
		if (((t & 0b0001) == 0) && (cy > 0)) {	/* �� */
			ct2 = 0;
			if (old_t != JOY_UP) {
				TxfontCursor (START_X, START_Y + cy);
				TxfontPutchar (' ');
				if (standard_play) {
					cy--;
				} else {
					if (cy == SELECT_RANKING)
						cy = SELECT_START;
					else
						cy--;
				}
				TxfontCursor (START_X, START_Y + cy);
				TxfontPutchar ('>');
				old_t = JOY_UP;
				ct = 0;
			} else {
				if (ct++ > WAIT_TIME) {
					old_t = JOY_NON;
					ct = 0;
				}
			}
		}
		if (((t & 0b0010) == 0) && (cy < 4)) {	/* �� */
			ct2 = 0;
			if (old_t != JOY_DOWN) {
				TxfontCursor (START_X, START_Y + cy);
				TxfontPutchar (' ');
				if (standard_play) {
					cy++;
				} else {
					if (cy == SELECT_START)
						cy = SELECT_RANKING;
					else
						cy++;
				}
				TxfontCursor (START_X, START_Y + cy);
				TxfontPutchar ('>');
				old_t = JOY_DOWN;
				ct = 0;
			} else {
				if (ct++ > WAIT_TIME) {
					old_t = JOY_NON;
					ct = 0;
				}
			}
		}
		if ((t & 0b1111) == 0b1111) {
			old_t = JOY_NON;
			ct = 0;
		}
		if ((standard_play) && (ct2++ > WAIT_DEMO)) {	/* ��莞�ԑ��삪������΃f�� */
			ct2 = 0;
			cy = demo_state;
			demo_state++;
			if (demo_state > SELECT_RANKING)
				demo_state = SELECT_REPLAY_BEFORE;
			break;
		}
		if (_iocs_bitsns (0x0c) & 0b10) {	/* [BREAK] */
			while (_iocs_bitsns (0x0c) & 0b10);
			cy = SELECT_EXIT;
			break;
		}
	} while ((t & 0b01000000) != 0);
	while ((_iocs_joyget (0) & 0b01000000) == 0);
	xsp_out ();

	return (cy);
}



/* �n�C�X�R�A�f�[�^�̓ǂݍ��݁i�Ȃ���Ώ������j */
static void ReadHighScore (char *fname)
{
	int i;
	FILE *fp;

	replay_data_size = high_score_replay_data_size = 0;
	if ((fp = fopen (fname, "rb")) == NULL) {
		/* �ǂݍ��߂Ȃ��������͐V�K�� */
		for (i = 0; i < 10; i++) {
			high_score[i] = (10 - i) * 1000;
			strcpy (high_score_name[i], "X68");
			sprintf (high_score_date[i], "1999/06/14");
		}
	} else {
		for (i = 0; i < 10; i++) {
			fread (&high_score[i], sizeof (int), 1, fp);
			fread (&high_score_name[i], sizeof (char), 4, fp);
			fread (&high_score_date[i], sizeof (char), 11, fp);
		}
		fread (&high_score_replay_data_size, sizeof (int), 1, fp);
		fread (&high_score_replay_data[0], sizeof (unsigned char), high_score_replay_data_size, fp);
		fclose (fp);
	}
}



/* �n�C�X�R�A�f�[�^�̏������� */
static void WriteHighScore (char *fname)
{
	int i;
	FILE *fp;

	if ((n_option == 0) && (high_score_changed) &&
	    ((fp = fopen (fname, "wb")) != NULL)) {
		for (i = 0; i < 10; i++) {
			fwrite (&high_score[i], sizeof (int), 1, fp);
			fwrite (&high_score_name[i], sizeof (char), 4, fp);
			fwrite (&high_score_date[i], sizeof (char), 11, fp);
		}
		fwrite (&high_score_replay_data_size, sizeof (int), 1, fp);
		fwrite (&high_score_replay_data[0], sizeof (unsigned char), high_score_replay_data_size, fp);
		fclose (fp);
	}
}



int main (int argc, char *argv[])
{
	int i;
	int slash_flag = 0, exit_flag = 0;
	static char *save_file = "OTOKO.DAT";

	xsp_mode_num = 2;
	disp_level = 1;
	y_option = 0;
	z_option = 3;		/* �f�t�H���g�̎c�@�� */
	e_option = 0;
	n_option = 0;

	{
		char *temp;

		temp = getenv ("SLASH");
		if ((temp != NULL) && (*temp == '/')) {
			slash_flag = 1;
		}
	}

	for (i = 1; i < argc; i++) {
		if (('-' == *argv[i]) || ((slash_flag == 0) && ('/' == *argv[i]))) {
			switch (*(argv[i] + 1)) {

			case 'd':
			case 'D':
				disp_level = atoi (argv[i] + 2);
				break;
#if	0
			case 'e':
			case 'E':
				e_option = !0;
				break;
#endif
			case 'n':
			case 'N':
				n_option = !0;
				break;

			case 's':
			case 'S':
				save_file = argv[i] + 2;
				break;

			case 'x':
			case 'X':
				xsp_mode_num = atoi (argv[i] + 2);
				break;

			case 'y':
			case 'Y':
				y_option = atoi (argv[i] + 2) - 1;
				if (y_option + 1 > STAGES)
					y_option = STAGES - 1;
				break;

			case 'z':
			case 'Z':
				z_option = atoi (argv[i] + 2);
				if (z_option < 1)
					z_option = 1;
#ifndef DEBUG
				if (z_option > 9)
					z_option = 9;
#endif
				break;

			default:
				Usage ();
				return (-1);
			}
		} else {
		}
	}


	if ((y_option == 0) && (z_option == 3))
		standard_play = !0;
	else
		standard_play = 0;

	ReadHighScore (save_file);
	Init ();

	do {
		switch (Title ()){
		case SELECT_START:
			replay_flag = REPLAY_NON;
			Game ();
			break;
		case SELECT_REPLAY_BEFORE:
			if ((standard_play) && (replay_data_size != 0)) {
				replay_flag = REPLAY_BEFORE;
				Game ();
			}
			break;
		case SELECT_REPLAY_HIGH:
			if ((standard_play) && (high_score_replay_data_size != 0)) {
				replay_flag = REPLAY_HIGH;
				Game ();
			}
			break;
		case SELECT_RANKING:
			Ranking ();
			WaitTime (55 * 10);
			break;
		case SELECT_EXIT:
		default:
			exit_flag = !0;
			break;
		}
	} while (exit_flag == 0);

	Tini ();
	WriteHighScore (save_file);

	return (0);
}
