/* 今は無き UPL と、ビデオゲームが好きだった自分に本作を捧げる */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <x68k/dos.h>
#include <x68k/iocs.h>
#include <XSP2lib.H>
#include <PCM8Afnc.H>
#include <apicglib.h>

#define GLOBAL_DEFINE		/* グローバル変数を確保する */
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


#define	PCG_MAX	668		/* スプライトＰＣＧパターン最大使用数 */
#define	REF_MAX	872		/* 複合スプライトリファレンスデータ最大使用数 */
#define	FRM_MAX	22038		/* 複合スプライトフレームデータ最大使用数 */
#define RGB(r,g,b) ((g)<<11|(r)<<6|(b)<<1)


static char pcg_alt[PCG_MAX + 1];	/* ＰＣＧ配置管理テーブル */
static char pcg_dat[PCG_MAX * 128];	/* ＰＣＧデータファイル読み込みバッファ */
static XOBJ_REF_DAT ref_dat[REF_MAX];	/* リファレンスデータ */
static XOBJ_FRM_DAT frm_dat[FRM_MAX];	/* フレームデータ */
static int old_crtmod;

static int sizeof_pcg = 0;	/* スプライトＰＣＧパターン読み込み数 */
static int sizeof_ref = 0;	/* 複合スプライトリファレンスデータ読み込み数 */
static int sizeof_frm = 0;	/* 複合スプライトフレームデータ読み込み数 */


static unsigned short blue_core[28][5];
static unsigned short red_core[28][5];

static int high_score[10];
static char high_score_name[10][4];
static char high_score_date[10][11];	/* yyyy/mm/dd */
static char high_score_changed = 0;	/* ハイスコアを書き出す必要があるか */

static int xsp_mode_num;
static char n_option;
static int y_option;		/* 開始面数-1 */
static int z_option;		/* 残機数 */

#define	SAVE_GRPALET	32	/* 保存するパレット数（タイトルの「男」「弾」用） */
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
#define WAIT_DEMO	(10*55)	/* タイトル画面でこれ以上操作されなかったらデモ */


static void Usage (void)
{
	printf ("男のシューティング・男弾 otoko.x ver1.00\n"
		"		programmed by Mitsuky <FreeSoftware>\n"
		"使用法 : otoko [option]\n"
		"[option]\n"
	/* "	-E : エントリー編集モード\n"	 */
		"	-Sファイル名 : ハイスコア保存ファイルの指定\n"
		"	-N : ハイスコアをファイルに保存しない\n"
	  "	-D数値 : 表示負荷 （数値 = 0～2（数値が大きいほど重い）, デフォルト = 1）\n"
		"	-X数値 : xsp_mode （数値 = 1～3, デフォルト = 2）\n"
		"	-Y数値 : 開始面数\n（数値 = 1～4, デフォルト = 1）"
		"	-Z数値 : 残機数（数値 = 1～9, デフォルト = 3）\n"
		"ハイスコアは標準設定（開始面数 = 1, 残機数 = 3）時のみ記録されます\n"
	);
}



/* 垂直同期ごとに呼ばれるルーチン */
static void VdispRoutine (void)
{
	static short pal = 0, pal2 = 0;
	static short *b_ptr = &blue_core[0][0], *r_ptr = &red_core[0][0];
	unsigned short *d;
	static int y0, y1;

	switch (game_state) {
	case IN_GAME:
		/* コアを点滅させる */
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
			return;	/* ポーズ中ならここまで */

		/* ポーズ中でなければ行う処理 */
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

		/* 背景のフェードイン／フェードアウト中か？ */
		switch (bg_contrast_control) {
		case BG_NORMAL:/* 通常のゲーム時 */
		case BG_DRAWING:	/* 背景描画中 */
			break;

		case BG_FADEIN:/* フェードイン中 */
			{
				unsigned short *s = bg_contrast_ptr;
				unsigned short *d = (unsigned short *) (0xe82000 + 16 * 2);
				unsigned short h = BG_PALETS / 4;	/* 処理するパレット数 */
				do {
					*d++ = *s++;
					*d++ = *s++;
					*d++ = *s++;
					*d++ = *s++;
				} while (--h);
				bg_contrast_ptr -= BG_PALETS;
				bg_contrast_num--;
				if (bg_contrast_num <= 0) {
					bg_contrast_control = BG_NORMAL;	/* フェードイン終了 */
					bg_contrast_ptr = &stage_palet_table[stage][0][0];
				} else {
					if (bg_contrast_num == 32)
						y0 = y1 = 0;	/* スクロール座標初期化 */
				}
			}
			break;
		case BG_FADEOUT:	/* フェードアウト中 */
			{
				unsigned short *s = bg_contrast_ptr;
				unsigned short *d = (unsigned short *) (0xe82000 + 16 * 2);
				unsigned short h = BG_PALETS / 4;	/* 処理するパレット数 */
				do {
					*d++ = *s++;
					*d++ = *s++;
					*d++ = *s++;
					*d++ = *s++;
				} while (--h);
				bg_contrast_ptr += BG_PALETS;
				bg_contrast_num--;
				if (bg_contrast_num <= 0) {
					bg_contrast_control = BG_NORMAL;	/* フェードアウト終了 */
					entry_counter_stop = 0;	/* エントリー再開 */
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

		/* ボンバーで背景が光っている時の処理 */
		switch (player->bomb_flash_state) {
		case BG_FLASH_NON:
			break;
		case BG_FLASH_WHITE:	/* 白フラッシュ */
			{
				unsigned short *d = (unsigned short *) (0xe82000 + 16 * 2);
				unsigned short h = BG_PALETS / 4;	/* 処理するパレット数 */
				*(unsigned short *) (0xe82000) = RGB (24, 24, 24);	/* フラッシュ色 */
				do {
					*d++ = RGB (24, 24, 24);	/* フラッシュ色 */
					*d++ = RGB (24, 24, 24);
					*d++ = RGB (24, 24, 24);
					*d++ = RGB (24, 24, 24);
				} while (--h);
			}
			break;
		case BG_FLASH_NORMAL:	/* 通常のパレット */
			{
				unsigned short *s;
				unsigned short *d = (unsigned short *) (0xe82000 + 16 * 2);
				unsigned short h = BG_PALETS / 4;	/* 処理するパレット数 */

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
					break;	/* 上の方で設定しているので必要無し */
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



/* 終了時に１度だけ呼ばれる・エラーだったら返ってこない */
void Tini (void)
{
	int sp;
	char *error_message_table[]=
	{
		"Thanks for playing !\n",
		"テキスト VRAM が使用中です\n",
		"グラフィック VRAM が使用中です\n"
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
	*(unsigned short *) 0xe8e006 &= 0xfffd;	/* HRL ビット */
	_iocs_b_super (sp);

	_iocs_skey_mod (-1, 0, 0);
	_iocs_b_curon ();
	_iocs_crtmod (old_crtmod);

	_dos_kflushio (0xff);	/* キーバッファをクリア */
	printf ("%s", error_message);

	if (error_level > ERROR_NON)	/* エラーが発生した場合は */
		exit (-1);	/* 終了する */
}



/* 起動時のファイル読み込み */
/* 読み込みに失敗した場合はいきなり終了するので注意 */
static int LoadFile (char *fname, void *ptr, size_t size, size_t n)
{
	FILE *fp;
	int i;

	if ((fp = fopen (fname, "rb")) == NULL) {
		static char e[256];
		error_level = ERROR_FILE;
		error_message = e;
		sprintf (e, "ファイル %s が読み込めません\n", fname);
		Tini ();	/* 返ってこない */
	}
	i = fread (ptr, size, n, fp);
	fclose (fp);

	return (i);		/* 返り値は読み込み項目数 */
}



/* SP データ読み込み */
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
	printf ("%s を読み込みました sp_no = %hd\n", fname, sp_no);
#endif
	return (sp_no);
}



/* XSP 各種データ読み込み */
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

	/* FRM_DAT[].pt 補正 */
	for (i = 0; i < f; i++)
		frm_dat[sizeof_frm + i].pt += sizeof_pcg / 128;

	/* REF_DAT[].ptr 補正 */
	for (i = 0; i < r; i++)
		ref_dat[sizeof_ref + i].ptr = (int) ref_dat[sizeof_ref + i].ptr + (int) (&frm_dat[sizeof_frm]);


	sizeof_pcg += p;
	sizeof_frm += f;
	sizeof_ref += r;

#ifdef DEBUG
	printf ("%s を読み込みました sizeof_ref = %hd\n", fname, obj_no);
#endif

	return (obj_no);
}



/* 起動時に１度だけ呼ばれる */
static void Init (void)
{
	int i, sp;
	unsigned short *s, *d;
	unsigned short pal_dat[16 * 15];	/* パレットデータファイル読み込みバッファ */
	unsigned short crtcdata[9] =
	{69, 6, 11 + 8, 59 - 8, 567, 5, 40, 552, 0x0111};
	unsigned short textpalet[16] =
	{0, RGB (16, 16, 20), RGB (12, 12, 20), RGB (28, 28, 31), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	old_crtmod = _iocs_crtmod (-1);
	error_message = NULL;

	error_level = ERROR_TUSEMD;
	if (_iocs_tgusemd (1, -1) == 2)	/* テキスト画面の使用状況をチェック */
		Tini ();

	error_level = ERROR_GUSEMD;
	if (_iocs_tgusemd (0, -1))	/* グラフィック画面の使用状況をチェック */
		Tini ();
	_iocs_tgusemd (0, 2);
	_iocs_tgusemd (1, 2);

	/* CD-ROM からの読み込みを速くするためにアルファベット順にソートしてたりして */
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
	printf ("	PCG 使用数 = %d\n	REF 使用数 = %d\n	FRM 使用数 = %d\n",
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


	/* 画面モード設定 */
	_iocs_crtmod (10);	/* 256*256dot 31kHz 256色 */
	_iocs_g_clr_on ();
	_iocs_b_curoff ();
	_iocs_ms_curof ();
	_iocs_skey_mod (0, 0, 0);

	_iocs_sp_init ();
	_iocs_sp_on ();		/* スプライト表示をＯＮ */
	_iocs_bgctrlst (0, 0, 0);	/* ＢＧ０表示ＯＦＦ */
	_iocs_bgctrlst (1, 1, 0);	/* ＢＧ１表示ＯＦＦ */

	/* 256x256 正方形モードを作る */
	sp = _iocs_b_super (0);
	*(unsigned short *) 0xe80028 = crtcdata[8];
	d = (unsigned short *) 0xe80002;
	for (i = 1; i < 8; i++)
		*d++ = crtcdata[i];
	*(unsigned short *) 0xe80000 = crtcdata[0];

	*(unsigned short *) 0xe8e006 |= 0b10;	/* HRL ビット */
	*(unsigned short *) 0xeb080c = crtcdata[2] + 4;
	*(unsigned short *) 0xeb080a = crtcdata[0];	/* スプライト */
	*(unsigned short *) 0xeb080e = crtcdata[6];
	*(volatile unsigned int *) 0xe80018 = *(volatile unsigned int *) 0xe8001c
		= *(volatile unsigned int *) 0xe80020 = *(volatile unsigned int *) 0xe80024
		= (256 << 16) + 256;

	*(unsigned short *) 0xe82500 = 0b01001001001110;	/* 優先順位 TX>SP>GR */


	/* タイトルの.PIC ファイルを読み込む */
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
			printf ("メモリエラー\n");
			error_level = ERROR_PIC_MEMORY;
			error_message = e;
			strcpy (e, ".PIC 展開用メモリが足りません\n");
			Tini ();/* 返ってこない */
		} else {
			error_level = ERROR_PIC_FILE;
			r = APICLOAD ((unsigned short *) 0xc00000, "SP/OTOKO.PIC", 256, 0,
				      file_buf, PIC_FILE_BUF_SIZE, APF_NOINITCRT | APF_NOCLRBUF | APF_NOPRFC, work_buf);
			if (r < 0) {
				static char e[256];
				error_level = ERROR_PIC_FILE;
				error_message = e;
				strcpy (e, "SP/OTOKO.PIC が読み込めません\n");
				Tini ();	/* 返ってこない */
			}
			r = APICLOAD ((unsigned short *) 0xc00000, "SP/DAMA.PIC",
			/* ロード座標をこうする事により 256色モードのページ１に読み込む */
			/* （素直にアドレスを 0xc80000 にすると読めないみたい） */
				      256, 512 + 128,	/* ロードＸ／Ｙ座標 */
				      file_buf, PIC_FILE_BUF_SIZE, APF_NOINITCRT | APF_NOCLRBUF | APF_NOPRFC, work_buf);
			if (r < 0) {
				static char e[256];
				error_level = ERROR_PIC_FILE;
				error_message = e;
				strcpy (e, "SP/DAMA.PIC が読み込めません\n");
				Tini ();	/* 返ってこない */
			}
			_dos_mfree (work_buf);
			_dos_mfree (file_buf);
		}
	}

	/* グラフィック画面のパレットを保存 */
	{
		unsigned short *p = (unsigned short *) (0xe82000 + (256 - SAVE_GRPALET) * 2);
		for (i = 0; i < SAVE_GRPALET; i++)
			grpalet[i] = *p++;
	}

	error_level = ERROR_MUSIC;

	/* パレットに転送 */
	s = (unsigned short *) 0xe82200;
	for (i = 0; i < 16; i++)
		*s++ = textpalet[i];
	d = pal_dat;
	for (i = 0; i < 15 * 16; i++)
		*s++ = *d++;
	_iocs_b_super (sp);

	/* 一般的な初期化 */
	EntryInit0 ();
	PlayerInit0 ();
	ShotInit0 ();
	EnemyInit0 ();
	EshotInit0 ();
	EffectInit0 ();
	TxfontInit0 ();
	SoundInit0 ();


	_dos_breakck (2);	/* BREAK チェックを殺す */
	_dos_intvcs (0xfff1, Tini);	/* _CTRLVC */
	_dos_intvcs (0xfff2, Tini);	/* _ERRJVC */

	xsp_on ();
	xsp_mode (xsp_mode_num);
	xsp_pcgdat_set (pcg_dat, pcg_alt, sizeof (pcg_alt));
	xsp_objdat_set (ref_dat);
	xsp_vsyncint_on (&VdispRoutine);
	pcm8a_vsyncint_on ();

	sp = _iocs_b_super (0);
	*(unsigned short *) 0xe82600 |= 0x007f;	/* 画面表示オン */
	_iocs_b_super (sp);



	error_level = ERROR_NON;
	return;
}




static void TextClear (void)
{
	_iocs_txrascpy (256 / 4 * 256, 256 / 4, 0b1111);	/* テキスト画面クリア */
}


/* ボタンが押されるか timer フレーム経つまで待つ */
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


/* グラフィック画面のパレットを半分の明るさに */
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


/* ハイスコア表示 */
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



/* ハイスコアネームエントリー */
static void NameEntry (short rank)
{
	char temp[4];
	char ch = 'A';
	int cx = 0;		/* カーソル位置 */
	time_t now;
	struct tm *tloc;

	enum {
		JOY_NON = 0, JOY_LEFT, JOY_RIGHT
	};

	if (rank != 9) {
		short h;
		/* ハイスコアを１つづつずらす */
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
		SoundMusic (MUSIC_RANKING_1ST);	/* １位の時 */
		/* 今回のリプレイデータをハイスコアのリプレイデータにコピー */
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
			if ((t & 0b0100) == 0) {	/* 左 */
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
			if ((t & 0b1000) == 0) {	/* 右 */
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
	int sprites;		/* スプライト枚数 */
	int old_score = -1;	/* 初回は必ず表示されるように */

	TextClear ();
	EntryInit ();
	PlayerInit ();
	EnemyInitStatic ();
	SoundStop ();

	if (standard_play)
		sprintf (temp_str, "HI %06d", high_score[0]);
	else
		sprintf (temp_str, "HI ------");	/* ハイスコア集計なし */
	TxfontCursor (21, 0);
	TxfontPuts (temp_str);

	score = 0;
	extend_score = EXTEND_EVERY;
	stage = y_option;
	player->rest = z_option;
	pause_flag = 0;
	game_state = IN_GAME;
	DispRest ();		/* 残機数表示 */

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
		if (score != old_score) {	/* 更新された時のみ表示 */
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
			score += 5000;	/* 残機１機につき 5000pts */
			DispRest ();	/* 残機数表示 */
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
	xsp_out ();		/* スプライトを消去 */

	/* 標準設定の時のみハイスコアを集計 */
	if ((replay_flag == REPLAY_NON) && (standard_play)) {
		replay_data_size = player->replay_counter;

		if ((player->game_over == GAME_OVER) || (player->game_over == GAME_COMPLETE)) {
			short h, rank = 0;

			for (h = 0; h < 10; h++) {
				if (score <= high_score[h])
					rank = h + 1;
			}
			/* １位なら rank=0, 11位なら rank=10 */
			if (rank < 10)
				NameEntry (rank);
		}
	}
	return;
}



/* タイトル画面の表示 */
static int Title (void)
{
	int cy = 0;		/* カーソル位置 */
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

	/* グラフィック画面のパレットを元に戻す */
	{
		unsigned short *p = (unsigned short *) (0xe82000 + (256 - SAVE_GRPALET) * 2);
		int i;
		sp = _iocs_b_super (0);
		xsp_vsync (1);
		*(unsigned short *) 0xe82600 &= 0xff80;	/* 画面表示オフ */
		*(volatile unsigned int *) 0xe80018 = *(volatile unsigned int *) 0xe8001c
			= *(volatile unsigned int *) 0xe80020 = *(volatile unsigned int *) 0xe80024
			= (256 << 16) + 256;
		for (i = 0; i < SAVE_GRPALET; i++)
			*p++ = grpalet[i];
		xsp_vsync (1);
		*(unsigned short *) 0xe82600 |= 0x007f;	/* 画面表示オン */
		_iocs_b_super (sp);
	}
	TextClear ();

	/* 男と弾の合体 */
	{
		short seq = 0;
		char exit_flag = 0;
		short h = 0;

		do {
			xsp_out ();
			if ((_iocs_joyget (0) & 0x60) != 0x60)
				break;
			switch (seq) {
			case 0:/* 「男」が落ちてくる */
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
			case 1:/* 「弾」が上がってくる */
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
			case 2:/* 左右から「OTOKO」「DAMA」が */
				if (h++ > 7) {
					seq++;
					for (h = 0; h < 6; h++)
						EffectInit (EFFECT_EXPL, 0, rndtable[h] / 2 + 64 + 16, rndtable[h + 128] / 2 + 64 + 16);
					EffectInit (EFFECT_EXPL, 0, 128 + 16, 128 + 16);
					SoundSetSE (SE_EXPL_L);	/* ボスキャラ爆発音 */
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
					/* パレットを白くフラッシュ */
					sp = _iocs_b_super (0);
					*(unsigned short *) (0xe82000 + FLASH_PALET * 2) = RGB (24, 24, 24);
					_iocs_b_super (sp);
					break;
				case 0 + 1:
				case 3 + 1:
				case 6 + 1:
				case 9 + 1:
					/* パレットを復帰 */
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

#define	START_X	10		/* "START" を表示する座標 */
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
		if (((t & 0b0001) == 0) && (cy > 0)) {	/* 上 */
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
		if (((t & 0b0010) == 0) && (cy < 4)) {	/* 下 */
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
		if ((standard_play) && (ct2++ > WAIT_DEMO)) {	/* 一定時間操作が無ければデモ */
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



/* ハイスコアデータの読み込み（なければ初期化） */
static void ReadHighScore (char *fname)
{
	int i;
	FILE *fp;

	replay_data_size = high_score_replay_data_size = 0;
	if ((fp = fopen (fname, "rb")) == NULL) {
		/* 読み込めなかった時は新規に */
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



/* ハイスコアデータの書き込み */
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
	z_option = 3;		/* デフォルトの残機数 */
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
