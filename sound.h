/* sound.h */

/* 効果音＆発音優先順位（下に行くほど高い） */
enum {
	SE_SHOT = 1,		/* プレイヤーのショット */
	SE_NO_DAMAGE,		/* 敵ダメージ無し */
	SE_DAMAGE,		/* 敵ダメージ */
	SE_CORE_DAMAGE,		/* 敵コアダメージ */
	SE_ESHOT,		/* 敵ショット（小） */
	SE_ESHOT_M,		/* 敵ショット（中） */
	SE_ESHOT_L,		/* 敵ショット（大） */
	SE_EBOMB_M,		/* 敵炸裂弾（中） */
	SE_EBOMB_L,		/* 敵炸裂弾（大） */
	SE_LASER,		/* 敵レーザー */
	SE_EXLASER,		/* 敵極太レーザー */
	SE_EXPL,		/* 敵ザコの爆発 */
	SE_LOCK,		/* 敵キャラ合体音（小） */
	SE_LOCK_M,		/* 敵キャラ合体音（中） */
	SE_LOCK_L,		/* 敵キャラ合体音（大） */
	SE_BOMB_CHARGE,		/* ボンバーエネルギーチャージ音 */
	SE_BOMB,		/* ボンバー音（Ａボタンを押した瞬間） */
	SE_BOMB_A,		/* ボンバー音（ボンバー） */
	SE_BOMB_B,		/* ボンバー音（バックボンバー） */
	SE_BOMB_C,		/* ボンバー音（レーザー） */
	SE_EXPL_M,		/* 中ボス爆発 */
	SE_EXPL_L,		/* ボスキャラ爆発 */
	SE_PLAYER_EXPL,		/* プレイヤーの爆発 */
	SE_EXTEND,		/* エクステンド */
};



/* 効果音＆発音優先順位（下に行くほど高い） */
enum {
	MUSIC_1 = 1,		/* 未使用 */
	MUSIC_STAGE_CLEAR,
	MUSIC_GAME_OVER,
	MUSIC_RANKING_1ST,
	MUSIC_RANKING_2ND,
	MUSIC_6,
	MUSIC_7,
	MUSIC_BOSS,
	MUSIC_LBOSS,
	MUSIC_10,
	MUSIC_STAGE1,
	MUSIC_STAGE2,
	MUSIC_STAGE3,
	MUSIC_STAGE4,
};


/* 関数プロトタイプ宣言 */
int SoundInit0 (void);
void SoundSetSE (short);
void SoundMove (void);
void SoundMusic (int no);
void SoundFadeOut (int no);
void SoundStop (void);
void SoundTini (void);
void SoundTini0 (void);
