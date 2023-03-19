/* entry.h */

typedef struct _entry {
	short type;		/* 種類 */
	signed short x, y;	/* 座標 */
	short arg;		/* 予備の引き数 */
	short control;		/* 難易度調整用 */
	short timer;		/* 次のキャラクターが出てくるまでの時間 */
} ENTRY;

#define ENTRY_MAX	768


#ifdef GLOBAL_DEFINE		/* グローバル変数の定義と宣言を１つにまとめるテク */
#define Extern			/* Extern をヌル文字列に置換 */
#else
#define Extern extern		/* Extern を extern に置換 */
#endif


/* グローバル変数 */
Extern ENTRY stage_data[STAGES][ENTRY_MAX];
Extern int entry_counter;
Extern signed int entry_timer;
Extern unsigned char entry_counter_stop;	/* =!0:エントリーを先に進めない（ボス戦等） */
Extern unsigned short entry_control;	/* 現在出現している敵の重み着き総数 */


/* 関数プロトタイプ宣言 */
void EntryInit0 (void);
void EntryInit (void);
void EntryMove (void);
void EntryTini (void);
