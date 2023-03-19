#include "otoko.h"
#include "player.h"
#include "shot.h"
#include "enemy.h"
#include "psearch.h"
#include "damage.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif


#define ENEMY_MAX	32	/* 敵最大数 */

static ENEMY enemy[ENEMY_MAX];	/* ワーク */
static ENEMY *enemy_top,	/* 使用中のワークのリスト */
 *enemy_null_top,		/* 空のワークのリスト */
 *enemy_end;			/* 使用中ワークのリストの末尾 */


/* ショットが敵キャラに与えるダメージ（添え時は出現してからのフレーム数） */
static short shot_damage[]=
{
	4, 4, 4, 4, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

/* ショットが敵キャラに与えるダメージ（添え時は出現してからのフレーム数） */
/* コアに当てた時 */
static short shot_damage_core[]=
{
	6, 6, 6, 6, 5, 5, 5, 5, 4, 4,
	4, 4, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3, 3, 3, 3, 3
};
/* ちなみにショットは 7dot/frame だ */

#define BLASER_HITX	16


#define wabs(x) ((x) >= 0 ? (short) (x) : (short) -(x))
#define abs(x)  ((x) >= 0 ? (int) (x) : (int) -(x))


typedef void (*func_init) (struct _enemy *);
func_init FuncEnemyInit[]=
{
	EnemyInitBoss02,
	EnemyInitBoss03,
	EnemyInitCoverF,
	EnemyInitCoverN,
	EnemyInitCoverN2,
	EnemyInitLBoss01,
	EnemyInitLBoss01C,
	EnemyInitLBoss02,
	EnemyInitLBoss02E,
	EnemyInitLBoss02H,
	EnemyInitLBoss02W,
	EnemyInitLBoss03,
	EnemyInitLBoss03G,
	EnemyInitLBoss03P,
	EnemyInitLBoss03U,
	EnemyInitLBoss03Z,
	EnemyInitLBoss04,
	EnemyInitLBoss04C,
	EnemyInitLBoss04E,
	EnemyInitLBoss04S,
	EnemyInitLBoss04V,
	EnemyInitLBoss04W,
	EnemyInitMvLaser,
	EnemyInitMvLaserH,
	EnemyInitOpLaser,
	EnemyInitOpLinear,
	EnemyInitRound4,
	EnemyInitSearchZ,
	EnemyInitSword,
	EnemyInitSword2,
	EnemyInitZako02,
	EnemyInitZako031,
	EnemyInitZako032,
	EnemyInitZako033,
	EnemyInitZako04,
	EnemyInitZako041,
};



/* 起動時に１度だけ呼ばれる */
void EnemyInit0 (void)
{
	int i;

    /* リストをつなげる */
	enemy_top = NULL;
	enemy_end = NULL;
	enemy_null_top = enemy;
	for (i = 0; i < ENEMY_MAX; i++)
		enemy[i].next = &enemy[i + 1];

	enemy[ENEMY_MAX - 1].next = NULL;
}



/* ゲーム開始時に１度だけ呼ばれる */
/* 「初めの８機ぶんだけ得点を表示」に使用する static 変数を初期化する */
void EnemyInitStatic (void)
{
	EnemyInitStaticLBoss03G ();
	EnemyInitStaticSearchZ ();
	EnemyInitStaticZako02 ();
	EnemyInitStaticZako031 ();
	EnemyInitStaticZako032 ();
	EnemyInitStaticZako033 ();
	EnemyInitStaticZako04 ();
	EnemyInitStaticZako041 ();
}



/* 敵出現時に呼ばれる */
ENEMY *EnemyInit (short type, signed short x, signed short y,
		  short arg, struct _enemy *parent)
{
	ENEMY *p;

	if (enemy_null_top == NULL)
		return (NULL);	/* 空きのワークがない（キャラクターオーバー） */

    /* リストの末尾に新しいノードを追加（他とは違うので注意） */
	p = enemy_null_top;
	enemy_null_top = p->next;
	if (enemy_top == NULL)
		enemy_top = p;
	else
		enemy_end->next = p;
	p->next = NULL;
	enemy_end = p;

	p->type = type;
	p->arg = arg;
	p->lx = x << 16;
	p->ly = y << 16;
	p->parent = parent;
	p->child_kill = p->child_death = 0;
	FuncEnemyInit[type] (p);/* 初期化ルーチンへ飛ぶ（関数へのポインタってヤツ） */

	return (p);		/* 作成したワークへのポインタを返す */
}



/* 垂直同期ごとに呼ばれる */
void EnemyMove (void)
{
	ENEMY *p, *q;
	signed short pl_x = player->x, pl_y = player->y;

#ifdef DEBUG
	enemy_sum = 0;
#endif

	p = enemy_top;		/* 現在注目しているワーク */
	q = NULL;		/* １つ前のワーク（ワーク削除時に必要） */
	while (p != NULL) {
#ifdef DEBUG
		enemy_sum++;
#endif
	    /* 移動関数を呼ぶ（関数へのポインタってヤツ） */
		if ((p->func_enemy_move) (p)) {	/* 返り値が非０なら消去 */
			(p->func_enemy_tini) (p);	/* 消去関数を呼ぶ */
			if (q == NULL) {	/* リストの一番最初を削除 */
				enemy_top = p->next;
				p->next = enemy_null_top;
				enemy_null_top = p;
				q = NULL;
				p = enemy_top;
			} else {
				if (p == enemy_end) {	/* リストの一番最後を削除 */
					q->next = NULL;
					enemy_end = q;
					p->next = enemy_null_top;
					enemy_null_top = p;
					p = q->next;
				} else {
					q->next = p->next;
					p->next = enemy_null_top;
					enemy_null_top = p;
					p = q->next;
				}
			}
		} else {
			SHOT *p2 = shot_top;	/* 現在注目しているショットのワーク */
			signed short t;
		    /* プレイヤーとの当たり判定 */
			if (((t = p->x + p->hit_px) > pl_x)
			    && ((t -= (short) (p->hit_px << 1)) < pl_x)
			    && ((t = p->y + p->hit_py) > pl_y)
			    && ((t -= (short) (p->hit_py << 1)) < pl_y)) {
				if (player->seq == PLAYER_SEQ_ALIVE) {
					player->seq = PLAYER_SEQ_DEAD;
				} else {
					short hit_w;
				    /* 無敵状態で当たった */
					if (((t = p->x + (hit_w = p->hit_cx)) >= pl_x)
					    && ((t -= (short) (hit_w << 1)) <= pl_x)) {
					    /* コアに当たった */
						p->damage += PLAYER_DAMAGE_CORE;
						p->damage_core = !0;
					} else {
					    /* それ以外の部分に当たった */
						p->damage += PLAYER_DAMAGE;
					}
				}
			}
		    /* ショットとの当たり判定 */
			while (p2 != NULL) {
				signed short p2_w;
				short hit_w;
				if (((t = p->x + (hit_w = p->hit_sx)) >= (p2_w = *((short *) (&p2->lx))))
				    && ((t -= (short) (hit_w << 1)) <= p2_w)
				    && ((t = p->y + (hit_w = p->hit_sy)) >= (p2_w = *((short *) (&p2->ly))))
				    && ((t -= (short) (hit_w << 1)) <= p2_w)
				    && (p2->damage == 0)) {
					p2->damage = !0;
					if (((t = p->x + (hit_w = p->hit_cx)) >= (p2_w = *((short *) (&p2->lx))))
					    && ((t -= (short) (hit_w << 1)) <= p2_w)) {
					    /* コアに当たった */
						p->damage += shot_damage_core[p2->frame];
						p->damage_core = !0;
					} else {
					    /* それ以外の部分に当たった */
						p->damage += shot_damage[p2->frame];
					}
				}
				p2 = p2->next;
			}
		    /* ボンバーとの当たり判定 */
			if (p->hit_sx != 0) {
				switch (player->bomb_hitcheck) {
				case BOMB_NON:
					break;
				case BOMB_A:
					p->damage += BOMBER_DAMAGE;
					break;
				case BOMB_B:
					if (p->y > pl_y)
						p->damage += BACK_B_DAMAGE;
					break;
				case BOMB_C:
					{
						short hit_w;
						if (((t = p->x + (hit_w = p->hit_sx)) >= pl_x)
						    && ((t -= (short) (hit_w << 1)) <= pl_x)
						    && (p->y <= pl_y)) {
							if (((t = p->x + (hit_w = p->hit_cx)) >= pl_x)
							    && ((t -= (short) (hit_w << 1)) <= pl_x)) {
							    /* コアに当たった */
								p->damage += BLASER_DAMAGE_CORE;
								p->damage_core = !0;
							} else {
							    /* それ以外の部分に当たった */
								p->damage += BLASER_DAMAGE;
							}
						}
					} break;
				}
			}
			q = p;
			p = p->next;
		}
	}
}



/* 敵消去時に呼ばれる（ゲームオーバー時） */
void EnemyTini (void)
{
	int i;

    /* リストをつなげる */
	enemy_top = NULL;
	enemy_null_top = enemy;
	for (i = 0; i < ENEMY_MAX; i++)
		enemy[i].next = &enemy[i + 1];

	enemy[ENEMY_MAX - 1].next = NULL;
}



/* サービスルーチン */

enum {
	MOVESEQ_ADD, MOVESEQ_FIX, MOVESEQ_SUB, MOVESEQ_END
};

/* 絶対座標 dx,dy に向かって移動する（初期化） */
void SubEnemyMoveToInit (ENEMY * p, signed short dest_x, signed short dest_y,
			 unsigned char speed, signed short count)
{
	unsigned char angle;
	signed short sx, sy;

	sx = wabs (dest_x - p->x);
	sy = wabs (dest_y - p->y);
	angle = msearch (p->x, p->y, dest_x, dest_y);	/* 移動方向 */
	p->move_ax = vxtable[speed][angle] / 32;
	p->move_ay = vytable[speed][angle] / 32;

	p->move_counter = p->move_count = count;	/* 何回加速／減速するか */

    /* 誤差を少なくするために大きい方で計算 */
    /* 除数が絶対に 0 にならないようにするため +1 しています（小細工） */
	if (sx > sy)
		p->move_count2 = sx * 65536 / (abs (p->move_ax * count) + 1) - count;
	else
		p->move_count2 = sy * 65536 / (abs (p->move_ay * count) + 1) - count;

	p->move_count3 = 0;	/* 実際に加速／減速した回数 */
	p->move_seq = MOVESEQ_ADD;
}



/* 座標 dx,dy に向かって移動する */
short SubEnemyMoveTo (ENEMY * p)
{
	switch (p->move_seq) {
		case MOVESEQ_ADD:	/* 加速中 */
		p->vx += p->move_ax;
		p->vy += p->move_ay;
		p->move_count3++;
		if (p->move_counter-- <= 0) {
			p->move_seq++;	/* 加速したら次へ */
			p->move_counter = p->move_count2;
		}
		break;
	case MOVESEQ_FIX:	/* 等速度運動中 */
		if (p->move_counter-- <= 0) {
			p->move_seq++;	/* 加速したら次へ */
			p->move_counter = p->move_count;
		}
		break;
	case MOVESEQ_SUB:	/* 減速中 */
		p->vx -= p->move_ax;
		p->vy -= p->move_ay;
		p->move_count3--;
		if (p->move_counter-- <= 0)	/* ある程度減速したら次へ */
			p->move_seq++;
		break;
	case MOVESEQ_END:
		p->vx = p->vy = p->move_ax = p->move_ay = 0;
		return (-1);
	}
	return (0);
}



/* 移動を中断する */
short SubEnemyMoveToAbort (ENEMY * p)
{
	if (p->move_count3-- <= 0) {
		p->vx = 0;
		p->vy = 0;
		return (-1);
	}
	p->vx -= p->move_ax;
	p->vy -= p->move_ay;

	return (0);
}
