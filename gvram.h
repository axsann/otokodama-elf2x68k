/* gvram.h */

/* bg_contrast_control �ɓ����l */
enum {
	BG_NORMAL = 0, BG_DRAWING, BG_FADEIN, BG_FADEOUT
};


/* �֐��v���g�^�C�v�錾 */
void GvramInit (void);
void GvramMove (void);
void GvramTini (void);
