/* スプライト表示優先順位、パレット定義 */

#ifndef PRIORITY

#define PRIORITY

#define PRIORITY_CURSOR		0x3f	/* エントリー編集モード用カーソル */

#define PRIORITY_ESHOT		0x3f	/* 敵弾 */
#define PRIORITY_ELASER		0x3e	/* 敵レーザー */

#define PRIORITY_ZAKO		0x3d	/* ザコ */
#define PRIORITY_ZAKO_CORE	0x3c	/* ザココア */
#define PRIORITY_BARRIER	0x3b	/* 自機バリア */
#define PRIORITY_PLAYER_EXPL	0x3b	/* 自機爆風 */
#define PRIORITY_PLAYER		0x3a	/* 自機 */
#define PRIORITY_SHOT		0x39	/* 自機ショット */
#define PRIORITY_BOMB_STR	0x38	/* ボム文字列 */
#define PRIORITY_POINTS		0x38	/* 得点表示 */
#define PRIORITY_BOMBER		0x37	/* ボンバー爆風 */
#define PRIORITY_ZAKO_EXPL	0x36	/* ザコ爆風 */
#define PRIORITY_BOSS_EXPL	0x35	/* ボス爆風 */
#define PRIORITY_BOSS_PARTS	0x34	/* ボスパーツ（上） */
#define PRIORITY_BOSS		0x33	/* ボス本体 */
#define PRIORITY_BOSS_CORE	0x32	/* ボスコア */
#define PRIORITY_BOSS_PARTS_L	0x31	/* ボスパーツ（下） */
#define PRIORITY_HAHEN		0x31	/* 破片 */


/* 以下パレット */
#define PALET_CURSOR	0x0400

#define PALET_CORE_BLUE	0x0400
#define PALET_CORE_RED	0x0500
//#define PALET_CORE_BLUE		0x0D00
//#define PALET_CORE_RED		0x0E00
#define PALET_POINTS_BLUE	0x0800
#define PALET_POINTS_RED	0x0700
#define PALET_PLAYER		0x0700
#define PALET_PLAYER_FLASH	0x0D00
#define PALET_RED			0x0E00	/* 赤フラッシュ */
#define PALET_DAMAGE		0x0F00	/* 白フラッシュ */


#define PALET_LBOSS01		0x0600
#define PALET_LBOSS01_1		0x0700	/* arg = 1 の時のパレット */

#define PALET_LBOSS02		0x0900
#define PALET_LBOSS02_1		0x0300	/* arg = 1 の時のパレット */

#define PALET_LBOSS03		0x0900
#define PALET_LBOSS03_1		0x0300	/* arg = 1 の時のパレット */

#define PALET_LBOSS04		0x0900

#endif
