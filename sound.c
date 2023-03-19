#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <x68k/iocs.h>
#include "otoko.h"
#include "zmcall.h"

#define SE_MAX	32
#define MUSIC_MAX	16


static void *se_ptr[SE_MAX];	/* 読み込んだ .PCM データへのポインタ */
static int se_size[SE_MAX];	/* ↑       のサイズ */
static short se_no = 0;		/* 鳴らす効果音の番号 */
static char zmusic_keep = 0;	/* ZMUSIC が常駐しているか */

static void *music_ptr[MUSIC_MAX];	/* 読み込んだ .ZMD データへのポインタ */
static int music_size[MUSIC_MAX];	/* ↑       のサイズ */

extern void Tini (void);

int filelength(FILE *fp)
{
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  
  return size;
}

/* 起動時に１回だけ呼ばれる */
int SoundInit0 (void)
{
	FILE *fp, *fp2;
	int data_no;
	char str1[1024];
	int i;

	for (i = 0; i < SE_MAX; i++)
		se_ptr[i] = NULL;
	for (i = 0; i < MUSIC_MAX; i++)
		music_ptr[i] = NULL;

	/* SOUND.CNF 読み込み */
	if ((fp = fopen ("SOUND.CNF", "r")) == NULL) {
		static char e[256];
		error_level = ERROR_MUSIC;
		error_message = e;
		sprintf (e, "SOUND.CNF が読み込めません\n");
		Tini ();	/* 返ってこない */
	}
	while (fscanf (fp, "%s", str1) != EOF) {
		if (ferror (fp) || feof (fp))
			break;
		if (!strcasecmp (str1, "SE")) {
			fscanf (fp, "%d %s", &data_no, &str1);
			if ((fp2 = fopen (str1, "rb")) == NULL) {
				static char e[256];
				error_level = ERROR_MUSIC;
				error_message = e;
				sprintf (e, "ファイル %s が読み込めません\n", str1);
				Tini ();	/* 返ってこない */
			}
			se_size[data_no] = filelength (fp2);
			if ((se_ptr[data_no] = malloc (se_size[data_no])) == NULL) {
				static char e[256];
				error_level = ERROR_FILE;
				error_message = e;
				sprintf (e, "効果音用メモリが足りません\n");
				Tini ();	/* 返ってこない */
			}
			fread (se_ptr[data_no], se_size[data_no], 1, fp2);
			fclose (fp2);
			fgets (str1, 1024, fp);	/* 以下読み捨てる */
			continue;
		}
		if (!strcasecmp (str1, "MUSIC")) {
			fscanf (fp, "%d %s", &data_no, &str1);
			if ((fp2 = fopen (str1, "rb")) == NULL) {
				static char e[256];
				error_level = ERROR_MUSIC;
				error_message = e;
				sprintf (e, "ファイル %s が読み込めません\n", str1);
				Tini ();	/* 返ってこない */
			}
			music_size[data_no] = filelength (fp2);
			if ((music_ptr[data_no] = malloc (music_size[data_no])) == NULL) {
				static char e[256];
				error_level = ERROR_FILE;
				error_message = e;
				sprintf (e, "音楽用メモリが足りません\n");
				Tini ();	/* 返ってこない */
			}
			fread (music_ptr[data_no], music_size[data_no], 1, fp2);
			fclose (fp2);
			fgets (str1, 1024, fp);	/* 以下読み捨てる */
			continue;
		}
		fgets (str1, 1024, fp);	/* 以下読み捨てる */
		if (ferror (fp) || feof (fp))
			break;
	}
	fclose (fp);

	if (ZmKeepCheck ()){
#if	0
	/* ZMUSIC が常駐していなくても実行できる */
	/* （EX68 対策という話も←よく知らないけど） */
		static char e[256];
		error_level = ERROR_MUSIC;
		error_message = e;
		sprintf (e, "ZMUSIC が常駐していません\n");
		Tini ();	/* 返ってこない */
#endif
	} else {
		zmusic_keep = !0;
		ZmInit ();
	}

	return (0);
}



/* 効果音をセットする（けど鳴らさない） */
void SoundSetSE (short no)
{
	/* 優先順位を判定 */
	if (no > se_no)
		se_no = no;
}


/* 効果音を鳴らす */
void SoundMove (void)
{
	if (se_no) {
		if (se_ptr[se_no]) {
			if (zmusic_keep) {
				ZmAdpcmSE (se_ptr[se_no], se_size[se_no], 4 * 256 + 3, se_no);
			} else {
				if (_iocs_adpcmsns ())	/* 今鳴っていたら止める */
					_iocs_adpcmmod (0);
				_iocs_adpcmout (se_ptr[se_no], 4 * 256 + 3, se_size[se_no]);
			}
		}
		se_no = 0;
	}
}



/* 音楽を鳴らす */
void SoundMusic (int no)
{
	if ((zmusic_keep) && (music_ptr[no]))
		ZmPlay (music_ptr[no], music_size[no]);
}



/* 音楽をフェードアウトする */
void SoundFadeOut (int no)
{
	if (zmusic_keep)
		ZmFadeOut (no);
}



/* 音楽を止める */
void SoundStop (void)
{
	if (zmusic_keep)
		ZmStop ();
}



/* ゲームオーバー時に呼ばれる */
void SoundTini (void)
{
	if (zmusic_keep)
		ZmFadeOut (64);
}



/* 終了時に１回だけ呼ばれる */
void SoundTini0 (void)
{
	int i;

	if (zmusic_keep)
		ZmFadeOut (64);

	for (i = 0; i < SE_MAX; i++) {
		if (se_ptr[i] != NULL)
			free (se_ptr[i]);
	}
	for (i = 0; i < MUSIC_MAX; i++) {
		if (music_ptr[i] != NULL)
			free (music_ptr[i]);
	}
}
