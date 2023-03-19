#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RGB(r,g,b) ((g)<<11|(r)<<6|(b)<<1)

#define BG_PALETS	64	/* BGで使用しているパレット数 */

#define SPEED_MAX	32
double speed[SPEED_MAX] =
{0.50, 0.60, 0.70, 0.80, 0.90,
 1.00, 1.10, 1.20, 1.30, 1.40, 1.50, 1.60, 1.70, 1.80, 1.90,
 2.00, 2.10, 2.20, 2.30, 2.40, 2.50, 2.60, 2.70, 2.80, 2.90,
 3.00, 3.10, 3.20,
 4.00, 5.00, 6.00, 7.00};


signed int vxtable[SPEED_MAX][256], vytable[SPEED_MAX][256];
unsigned char pstable[256][256];
unsigned char rndtable[256];

const short pt_rot[256] =
{
	0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8,
	7, 7, 7, 7, 7, 7, 7, 7,
	6, 6, 6, 6, 6, 6, 6, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	4, 4, 4, 4, 4, 4, 4, 4,
	3, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7,
	8, 8, 8, 8, 8, 8, 8, 8,
	7, 7, 7, 7, 7, 7, 7, 7,
	6, 6, 6, 6, 6, 6, 6, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	4, 4, 4, 4, 4, 4, 4, 4,
	3, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0
};
const short info_rot[256] =
{
	0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
	0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
	0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
	0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
	0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
	0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
	0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
	0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000, 0x4000,
	0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
	0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
	0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
	0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
	0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
	0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
	0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
	0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000, 0xc000,
	0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
	0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
	0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
	0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
	0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
	0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
	0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
	0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
	0, 0, 0, 0
};


const unsigned short blue[5][3] =
{
	{0, 0, 18},		/* R,G,B */
	{0, 0, 28},
	{15, 15, 31},
	{20, 20, 31},
	{31, 31, 31}
};

const unsigned short red[5][3] =
{
	{18, 0, 0},		/* R,G,B */
	{28, 0, 0},
	{31, 15, 15},
	{31, 20, 20},
	{31, 31, 31}
};

unsigned short blue_core[28][5];
unsigned short red_core[28][5];



void usage (void)
{
	printf ("OTOKO.X 用テーブルを作成する maketbl.x ver1.00\n"
		"		programmed by Mitsuky <FreeSoftware>\n"
		"以下のテーブルを作成します\n"
		"	TBL/PT_ROT.TBL   : スプライトパターン No.に足すオフセット値\n"
		"	TBL/INFO_ROT.TBL : スプライト info に足すオフセット値\n"
		"	TBL/VXTABLE.TBL	 : cos テーブル\n"
		"	TBL/VYTABLE.TBL	 : sin テーブル\n"
		"	TBL/PSTABLE.TBL	 : 自機サーチテーブル\n"
		"	TBL/RNDTABLE.TBL : 乱数テーブル\n"
		"	TBL/BLUECORE.PAL : 青いコアのパレットテーブル\n"
		"	TBL/REDCORE.PAL	 : 赤いコアのパレットテーブル\n"
	    "	TBL/STAGE01G.PAL	 : １面背景のパレットをグラデーション化したもの\n"
		"	TBL/STAGE02G.PAL	 : ２面					〃\n"
		"角度関係のは全て１周＝256段階で、右向きが０、下向きが64となっております\n"
	);
}



int maketable (void)
{
	int i, j, k;
	FILE *fp;

	printf ("データテーブルを作成します。\n");

	fp = fopen ("TBL/PT_ROT.TBL", "wb");
	fwrite (pt_rot, 256, sizeof (short), fp);
	fclose (fp);

	fp = fopen ("TBL/INFO_ROT.TBL", "wb");
	fwrite (info_rot, 256, sizeof (short), fp);
	fclose (fp);

	for (i = 0; i < SPEED_MAX; i++) {
		for (j = 0; j < 256; j++) {
			vxtable[i][j] = (signed int) (cos (2.0 * M_PI * (long) j / 256.0) * 65536.0 * speed[i]);
			vytable[i][j] = (signed int) (sin (2.0 * M_PI * (long) j / 256.0) * 65536.0 * speed[i]);
		}
	}
	fp = fopen ("TBL/VXTABLE.TBL", "wb");
	fwrite (vxtable, 256 * SPEED_MAX, sizeof (signed int), fp);
	fclose (fp);
	fp = fopen ("TBL/VYTABLE.TBL", "wb");
	fwrite (vytable, 256 * SPEED_MAX, sizeof (signed int), fp);
	fclose (fp);

	printf ("少々時間が掛かります。申し訳ありません。\n"
		"（ arctan を 256x256 個計算しています。）\n");
	for (i = 0; i < 256; i++) {
		printf ("%3d/256\x1b[0J\n\x0b", i + 1);
		for (j = 0; j < 256; j++) {
			pstable[i][j] = (unsigned char) (atan ((double) j / (double) i) * 256.0 / 2.0 / M_PI);
		}
	}
	fp = fopen ("TBL/PSTABLE.TBL", "wb");
	fwrite (pstable, 256 * 256, sizeof (unsigned char), fp);
	fclose (fp);

	for (i = 0; i < 256; i++)
		rndtable[i] = (unsigned char) ((rand ()/ 4) & 255);
	fp = fopen ("TBL/RNDTABLE.TBL", "wb");
	fwrite (rndtable, 256, sizeof (unsigned char), fp);
	fclose (fp);


	{
		unsigned short *p = &blue_core[0][0];

		for (i = 0; i < 14; i++) {
			for (j = 0; j < 5; j++) {
				unsigned short r, g, b;
				if (blue[j][0] > i)
					r = blue[j][0] - i;
				else
					r = 0;
				if (blue[j][1] > i)
					g = blue[j][1] - i;
				else
					g = 0;
				if (blue[j][2] > i)
					b = blue[j][2] - i;
				else
					b = 0;
				*p++ = ((g << 11) | (r << 6) | (b << 1)) & 0xfffe;
			}
		}
		for (i = 14; i > 0; i--) {
			for (j = 0; j < 5; j++) {
				unsigned short r, g, b;
				if (blue[j][0] > i)
					r = blue[j][0] - i;
				else
					r = 0;
				if (blue[j][1] > i)
					g = blue[j][1] - i;
				else
					g = 0;
				if (blue[j][2] > i)
					b = blue[j][2] - i;
				else
					b = 0;
				*p++ = ((g << 11) | (r << 6) | (b << 1)) & 0xfffe;
			}
		}
	}
	fp = fopen ("TBL/BLUECORE.PAL", "wb");
	fwrite (blue_core, 5 * 28, sizeof (unsigned short), fp);
	fclose (fp);


	{
		unsigned short *p = &red_core[0][0];

		for (i = 0; i < 14; i++) {
			for (j = 0; j < 5; j++) {
				unsigned short r, g, b;
				if (red[j][0] > i)
					r = red[j][0] - i;
				else
					r = 0;
				if (red[j][1] > i)
					g = red[j][1] - i;
				else
					g = 0;
				if (red[j][2] > i)
					b = red[j][2] - i;
				else
					b = 0;
				*p++ = ((g << 11) | (r << 6) | (b << 1)) & 0xfffe;
			}
		}
		for (i = 14; i > 0; i--) {
			for (j = 0; j < 5; j++) {
				unsigned short r, g, b;
				if (red[j][0] > i)
					r = red[j][0] - i;
				else
					r = 0;
				if (red[j][1] > i)
					g = red[j][1] - i;
				else
					g = 0;
				if (red[j][2] > i)
					b = red[j][2] - i;
				else
					b = 0;
				*p++ = ((g << 11) | (r << 6) | (b << 1)) & 0xfffe;
			}
		}
	}
	fp = fopen ("TBL/REDCORE.PAL", "wb");
	fwrite (red_core, 5 * 28, sizeof (unsigned short), fp);
	fclose (fp);


	for (k = 1; k <= 4; k++) {
		unsigned short stage_palet_table[33][BG_PALETS];
		char fname[256];

		{
			short h1, h2;
			for (h1 = 0; h1 < 33; h1++) {
				for (h2 = 0; h2 < BG_PALETS; h2++)
					stage_palet_table[h1][h2] = 0;
			}
		}
		sprintf (fname, "BG/STAGE%02d.PAL", k);
		fp = fopen (fname, "rb");
		fread (stage_palet_table, BG_PALETS, sizeof (unsigned short), fp);
		fclose (fp);

		for (i = 1; i < 33; i++) {
			for (j = 0; j < BG_PALETS; j++) {
				int r, g, b;
				unsigned short r2, g2, b2;
				unsigned short c;

				c = stage_palet_table[0][j];
				g = (c >> 11) & 0x1e;
				r = (c >> 6) & 0x1e;
				b = (c >> 1) & 0x1e;

				g2 = (g * (32 - i) / 32) & 0x1e;
				r2 = (r * (32 - i) / 32) & 0x1e;
				b2 = (b * (32 - i) / 32) & 0x1e;

				stage_palet_table[i][j] = (g2 << 11) | (r2 << 6) | (b2 << 1);
			}
		}
		sprintf (fname, "BG/STAGE%02dG.PAL", k);
		fp = fopen (fname, "wb");
		fwrite (stage_palet_table, BG_PALETS * 33, sizeof (unsigned short), fp);
		fclose (fp);
	}


	printf ("\n終了しました。\n");

	return (0);
}



int main (int argc, char *argv[])
{
	if (argc != 1) {
		usage ();
		return (-1);
	}
	maketable ();

	return (0);
}
