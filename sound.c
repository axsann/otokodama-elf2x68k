#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <x68k/iocs.h>
#include "otoko.h"
#include "zmcall.h"

#define SE_MAX	32
#define MUSIC_MAX	16


static void *se_ptr[SE_MAX];	/* �ǂݍ��� .PCM �f�[�^�ւ̃|�C���^ */
static int se_size[SE_MAX];	/* ��       �̃T�C�Y */
static short se_no = 0;		/* �炷���ʉ��̔ԍ� */
static char zmusic_keep = 0;	/* ZMUSIC ���풓���Ă��邩 */

static void *music_ptr[MUSIC_MAX];	/* �ǂݍ��� .ZMD �f�[�^�ւ̃|�C���^ */
static int music_size[MUSIC_MAX];	/* ��       �̃T�C�Y */

extern void Tini (void);

int filelength(FILE *fp)
{
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  
  return size;
}

/* �N�����ɂP�񂾂��Ă΂�� */
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

	/* SOUND.CNF �ǂݍ��� */
	if ((fp = fopen ("SOUND.CNF", "r")) == NULL) {
		static char e[256];
		error_level = ERROR_MUSIC;
		error_message = e;
		sprintf (e, "SOUND.CNF ���ǂݍ��߂܂���\n");
		Tini ();	/* �Ԃ��Ă��Ȃ� */
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
				sprintf (e, "�t�@�C�� %s ���ǂݍ��߂܂���\n", str1);
				Tini ();	/* �Ԃ��Ă��Ȃ� */
			}
			se_size[data_no] = filelength (fp2);
			if ((se_ptr[data_no] = malloc (se_size[data_no])) == NULL) {
				static char e[256];
				error_level = ERROR_FILE;
				error_message = e;
				sprintf (e, "���ʉ��p������������܂���\n");
				Tini ();	/* �Ԃ��Ă��Ȃ� */
			}
			fread (se_ptr[data_no], se_size[data_no], 1, fp2);
			fclose (fp2);
			fgets (str1, 1024, fp);	/* �ȉ��ǂݎ̂Ă� */
			continue;
		}
		if (!strcasecmp (str1, "MUSIC")) {
			fscanf (fp, "%d %s", &data_no, &str1);
			if ((fp2 = fopen (str1, "rb")) == NULL) {
				static char e[256];
				error_level = ERROR_MUSIC;
				error_message = e;
				sprintf (e, "�t�@�C�� %s ���ǂݍ��߂܂���\n", str1);
				Tini ();	/* �Ԃ��Ă��Ȃ� */
			}
			music_size[data_no] = filelength (fp2);
			if ((music_ptr[data_no] = malloc (music_size[data_no])) == NULL) {
				static char e[256];
				error_level = ERROR_FILE;
				error_message = e;
				sprintf (e, "���y�p������������܂���\n");
				Tini ();	/* �Ԃ��Ă��Ȃ� */
			}
			fread (music_ptr[data_no], music_size[data_no], 1, fp2);
			fclose (fp2);
			fgets (str1, 1024, fp);	/* �ȉ��ǂݎ̂Ă� */
			continue;
		}
		fgets (str1, 1024, fp);	/* �ȉ��ǂݎ̂Ă� */
		if (ferror (fp) || feof (fp))
			break;
	}
	fclose (fp);

	if (ZmKeepCheck ()){
#if	0
	/* ZMUSIC ���풓���Ă��Ȃ��Ă����s�ł��� */
	/* �iEX68 �΍�Ƃ����b�����悭�m��Ȃ����ǁj */
		static char e[256];
		error_level = ERROR_MUSIC;
		error_message = e;
		sprintf (e, "ZMUSIC ���풓���Ă��܂���\n");
		Tini ();	/* �Ԃ��Ă��Ȃ� */
#endif
	} else {
		zmusic_keep = !0;
		ZmInit ();
	}

	return (0);
}



/* ���ʉ����Z�b�g����i���ǖ炳�Ȃ��j */
void SoundSetSE (short no)
{
	/* �D�揇�ʂ𔻒� */
	if (no > se_no)
		se_no = no;
}


/* ���ʉ���炷 */
void SoundMove (void)
{
	if (se_no) {
		if (se_ptr[se_no]) {
			if (zmusic_keep) {
				ZmAdpcmSE (se_ptr[se_no], se_size[se_no], 4 * 256 + 3, se_no);
			} else {
				if (_iocs_adpcmsns ())	/* �����Ă�����~�߂� */
					_iocs_adpcmmod (0);
				_iocs_adpcmout (se_ptr[se_no], 4 * 256 + 3, se_size[se_no]);
			}
		}
		se_no = 0;
	}
}



/* ���y��炷 */
void SoundMusic (int no)
{
	if ((zmusic_keep) && (music_ptr[no]))
		ZmPlay (music_ptr[no], music_size[no]);
}



/* ���y���t�F�[�h�A�E�g���� */
void SoundFadeOut (int no)
{
	if (zmusic_keep)
		ZmFadeOut (no);
}



/* ���y���~�߂� */
void SoundStop (void)
{
	if (zmusic_keep)
		ZmStop ();
}



/* �Q�[���I�[�o�[���ɌĂ΂�� */
void SoundTini (void)
{
	if (zmusic_keep)
		ZmFadeOut (64);
}



/* �I�����ɂP�񂾂��Ă΂�� */
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
