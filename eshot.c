#include <xsp2lib.h>

#include "otoko.h"
#include "player.h"
#include "eshot.h"
#include "priority.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

#define	PALET_ESHOT	0x0300

#define ESHOT_MAX	200	/* 敵弾最大数 */
static ESHOT eshot[ESHOT_MAX];	/* ワーク */



/* 起動時に１度だけ呼ばれる */
void EshotInit0 (void)
{
	int i;

    /* リストをつなげる */
	eshot_top = NULL;
	eshot_null_top = eshot;
	for (i = 0; i < ESHOT_MAX; i++)
		eshot[i].next = &eshot[i + 1];

	eshot[ESHOT_MAX - 1].next = NULL;

	eshot_erase = 0;
}



/* 敵弾出現時に呼ばれる */
void EshotInit (short type, signed short x, signed short y, unsigned char speed, unsigned char angle, signed short t)
{
	ESHOT *p;

	if (eshot_null_top == NULL)	/* ワークの空きはあるか？ */
		return;

	p = eshot_null_top;
	eshot_null_top = p->next;
	p->next = eshot_top;
	eshot_top = p;

	p->lx = (x - 8) << 16;	/* (8,8) がスプライト座標の中心なので補正 */
	p->ly = (y - 8) << 16;
	p->vx = vxtable[speed][angle];
	p->vy = vytable[speed][angle];
	if (t) {		/* t フレーム後の値を初期値に */
		p->lx += (p->vx * t);
		p->ly += (p->vy * t);
	}
    /* メモ：敵弾の速度と当り判定の大きさについて */
    /* eshot->hx が以下の場合、eshot->vx の最大値は以下の通り */
    /* (hx,vx 未満) = (2,1.0),(3,3.0),(4,5.0),(5,7.0),(6,9.0) */
    /* 算出式は以下の通り */
    /* eshot->vx+player->vx > 2*eshot->hx の時判定抜けが発生 */
    /* player->vx の最大値は 3.0（高速移動時） */

	switch (p->type = type) {
	case ESHOT_NRG01:	/* エネルギー弾（極小） */
		p->hit_p = 2;	/* 速度を 1.0 未満にする事 */
		p->pt = sp_eshot01 + type;
		p->info = PALET_ESHOT | PRIORITY_ESHOT;
		break;

	case ESHOT_NRG02:	/* 　　 〃 　　（小） */
	case ESHOT_NRG03:	/* 　　 〃 　　（中） */
	case ESHOT_NRG04:	/* 　　 〃 　　（大） */
		p->hit_p = 3;	/* 速度を 3.0 未満にする事 */
		p->pt = sp_eshot01 + type;
		p->info = PALET_ESHOT | PRIORITY_ESHOT;
		break;

	case ESHOT_NRG05:	/* 　　 〃 　　（特大） */
	case ESHOT_NRG06:	/* 　　 〃 　　（特大） */
	case ESHOT_NRG22:	/* 　　 〃 　　（小横に２個） */
	case ESHOT_NRG32:	/* 　　 〃 　　（中横に２個） */
	case ESHOT_NRG23:	/* 　　 〃 　　（小３角形に３個） */
	case ESHOT_NRG24:	/* 　　 〃 　　（小正方形に４個） */
	case ESHOT_NRG14N:	/* 　　 〃 　　（小斜めに４個） */
	case ESHOT_NRG24N:	/* 　　 〃 　　（中斜めに４個） */
	case ESHOT_NRG34N:	/* 　　 〃 　　（中斜めに４個） */
	case ESHOT_NRG34:	/* 　　 〃 　　（中正方形に４個） */
	case ESHOT_NRG15:	/* 　　 〃 　　（小５個） */
	case ESHOT_NRG35:	/* 　　 〃 　　（小と大正方形に５個） */
		p->hit_p = 4;	/* 速度を 5.0 未満にする事 */
		p->pt = sp_eshot01 + type;
		p->info = 0x0300 | PRIORITY_ESHOT;
		break;

	case ESHOT_LASER01:
		p->hit_p = 3;	/* 速度を 3.0 未満にする事 */
		p->pt = sp_laser01 + 8;
		p->info = PALET_ESHOT | PRIORITY_ESHOT;
		break;

	case ESHOT_LASEREX:
		p->hit_p = 4;	/* 速度を 5.0 未満にする事 */
		p->pt = sp_laser01 + 9;
		p->info = PALET_ESHOT | PRIORITY_ESHOT;
		break;
	default:
		break;
	}
}



/* 垂直同期ごとに呼ばれる */
void EshotMove (void)
{
	ESHOT *p, *q;
	signed short pl_x = player->x - 8, pl_y = player->y - 8;
	signed short p_x, p_y;

#ifdef DEBUG
	eshot_sum = 0;
#endif
	p = eshot_top;		/* 現在注目しているワーク */
	q = NULL;		/* １つ前のワーク（ワーク削除時に必要） */

    /* 弾消し中か？（高速化のためやや冗長なコードになってます） */
	if (eshot_erase == 0) {
	    /* 通常の処理（弾消しではない） */
		while (p != NULL) {
#ifdef DEBUG
			eshot_sum++;
#endif
		    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
			p_x = p->x = (p->lx += p->vx) >> 16;
			p_y = p->y = (p->ly += p->vy) >> 16;

		    /* 敵弾が画面外に出たか？ */
		    /* （画面右から出た判定と左から出た判定を１回の比較で行っている） */
			if (((unsigned short) p_x > 256 + 16) || ((unsigned short) p_y > 256 + 16)) {
				if (q == NULL) {	/* リストの一番最初を削除 */
					eshot_top = p->next;
					p->next = eshot_null_top;
					eshot_null_top = p;
					q = NULL;
					p = eshot_top;
				} else {
					q->next = p->next;
					p->next = eshot_null_top;
					eshot_null_top = p;
					p = q->next;
				}
			} else {
			    /* 敵弾と自機の当たり判定 */
			    /* この辺の最適化は -fstrings-reduce が頑張ってくれるハズ */
				signed short t, t2 = p->hit_p;
				if (((t = p_y - t2) <= pl_y)
				    && ((t += (short) (t2 << 1)) >= pl_y)
				    && ((t = p_x + t2) >= pl_x)
				    && ((t -= (short) (t2 << 1)) <= pl_x)) {
					if (player->seq == PLAYER_SEQ_ALIVE)
						player->seq = PLAYER_SEQ_DEAD;
				} else {
					xsp_set_st (p);
				}
				q = p;
				p = p->next;
			}
		}
	} else {
	    /* 弾消し中 */
		char erase_flag;/* キャラクターを消す場合 =!0 */
		unsigned short erase_table[14] =
		{0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6};

		while (p != NULL) {
			if (eshot_erase > 1)
				erase_flag = 0;
			else
				erase_flag = !0;
#ifdef DEBUG
			eshot_sum++;
#endif

		    /* 速度を足して上位ワード（固定整数部）だけ取り出す */
			p_x = p->x = (p->lx += p->vx) >> 16;
			p_y = p->y = (p->ly += p->vy) >> 16;

		    /* 敵弾が画面外に出たか？ */
		    /* （画面右から出た判定と左から出た判定を１回の比較で行っている） */
			if (((unsigned short) p_x > 256 + 16) || ((unsigned short) p_y > 256 + 16)) {
				erase_flag = !0;
			} else {
				p->pt = sp_eshotera + erase_table[eshot_erase];
				xsp_set_st (p);
			}

			if (erase_flag) {
				if (q == NULL) {	/* リストの一番最初を削除 */
					eshot_top = p->next;
					p->next = eshot_null_top;
					eshot_null_top = p;
					q = NULL;
					p = eshot_top;
				} else {
					q->next = p->next;
					p->next = eshot_null_top;
					eshot_null_top = p;
					p = q->next;
				}
			} else {
				q = p;
				p = p->next;
			}
		}
		eshot_erase--;
	}
}



/* 敵弾消去時に呼ばれる（ゲームオーバー時） */
void EshotTini (void)
{
	int i;

    /* リストをつなげる */
	eshot_top = NULL;
	eshot_null_top = eshot;
	for (i = 0; i < ESHOT_MAX; i++)
		eshot[i].next = &eshot[i + 1];

	eshot[ESHOT_MAX - 1].next = NULL;
}
