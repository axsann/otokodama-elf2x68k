/* gvram.h */

/* bg_contrast_control に入れる値 */
enum {
	BG_NORMAL = 0, BG_DRAWING, BG_FADEIN, BG_FADEOUT
};


/* 関数プロトタイプ宣言 */
void GvramInit (void);
void GvramMove (void);
void GvramTini (void);
