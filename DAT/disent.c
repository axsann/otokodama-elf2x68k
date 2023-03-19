#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../names.h"

#define ENEMY_MUSIC	250
#define ENEMY_MUSIC_FADEOUT	251
#define ENEMY_NON	253
#define ENEMY_START	254
#define ENEMY_END	255


void usage (void)
{
	printf ("男弾の stageXX.dat を .s 形式に変換する disent.x ver1.00\n"
		"		programmed by Mitsuky <FreeSoftware>\n"
		"使用法 : disent [option] ファイル名...\n"
#if	0
		"[option]\n"
		"	-X : \n"
		"ファイル名は複数指定できます\n"
#endif
	);
}



int disent (char *fname)
{
	FILE *fp;
	int i;
	int file_size;
	void *dat;
	char fname_root[256], fname2[256];
	signed short *p;

	if ((fp = fopen (fname, "rb")) == NULL) {
		printf ("ファイル %s が読み込めません\n", fname);
		return (-1);
	}
	file_size = filelength (fileno (fp));
	if ((int) (dat = malloc (file_size)) < 0) {
		printf ("メモリが足りません\n");
		return (-1);
	}
	fread (dat, 1, file_size, fp);
	fclose (fp);

    /* 拡張子を .s に */
	{
		char *s, *d;
		s = fname;
		d = fname_root;
		while (*s != '.')
			*d++ = *s++;
		*d = '\0';
		strcpy (fname2, fname_root);
		strcat (fname2, ".s");
	}
	if ((fp = fopen (fname2, "wb")) == NULL) {
		printf ("ファイル %s が書き込めません\n", fname2);
		return (-1);
	}
	fprintf (fp, "*	%s\n"
		 "\n"
		 "	.include	names.inc\n"
		 "\n"
		 "	.data\n"
		 "	.even\n"
		 "%s:\n"
		 "		* type,x,y,arg,ctrl,time\n",
		 fname2, fname_root);

	for (i = 0, p = dat; i < file_size / 12; i++) {
		signed short h;
		switch (h = *p++) {
		case ENEMY_MUSIC:
			fprintf (fp, "	.dc.w	ENEMY_MUSIC,");
			break;
		case ENEMY_MUSIC_FADEOUT:
			fprintf (fp, "	.dc.w	ENEMY_MUSIC_FADEOUT,");
			break;
		case ENEMY_NON:
			fprintf (fp, "	.dc.w	ENEMY_NON,");
			break;
		case ENEMY_START:
			fprintf (fp, "	.dc.w	ENEMY_START,");
			break;
		case ENEMY_END:
			fprintf (fp, "	.dc.w	ENEMY_END,");
			break;
		default:
			fprintf (fp, "	.dc.w	%s,", enemy_name[h]);
			break;
		}
		fprintf (fp, "%d,", *p++);
		fprintf (fp, "%d,", *p++);
		fprintf (fp, "%d,", *p++);
		fprintf (fp, "%d,", *p++);
		fprintf (fp, "%d", *p++);

		if ((i % 10) == 0)
			fprintf (fp, "		* %d\n", i);
		else
			fprintf (fp, "\n");
	}
	fclose (fp);

	return (0);
}



int main (int argc, char *argv[])
{
	int i;
	int slash_flag = 0;
	int fname_found_flag = 0;	/* ファイル名が１つでもあったか */

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

			case 's':
			case 'S':
				break;

			default:
				usage ();
				return (-1);
			}
		} else {
			disent (argv[i]);
			fname_found_flag = 1;
		}
	}

	if (fname_found_flag == 0) {
		usage ();
		return (-1);
	}
	return (0);
}
