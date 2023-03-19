#include <stdio.h>
#include <stdlib.h>
#include "otoko.h"
#include "player.h"
#include "enemy.h"
#include "entry.h"
#include "gvram.h"
#include "sound.h"

#ifdef DEBUG
#include "txfont.h"
#endif

#define ENTRY_MUSIC	250
#define ENTRY_MUSIC_FADEOUT	251
#define ENTRY_NON	253
#define ENTRY_START	254
#define ENTRY_END	255


enum {
	WAIT_NON = 0, WAIT_NOW, WAIT_END
};

static unsigned char entry_control_wait;	/* =!0:entry_control が 0 になるまで待つ */



void EntryInit0 (void)
{
}



/* ゲーム開始時に呼ばれる */
void EntryInit (void)
{
	entry_counter = 0;
	entry_timer = 0;
	entry_counter_stop = 0;
	entry_control = 0;
	entry_control_wait = WAIT_NON;
}



void EntryMove (void)
{
#ifdef DEBUG
	{
		char temp_str[64];
		sprintf (temp_str, "FRM %06d", player->replay_counter);
		TxfontCursor (21, 6);
		TxfontPuts (temp_str);
	}
#endif

	if (entry_control_wait == WAIT_NOW) {
		if (entry_control == 0) {
			entry_counter_stop = 0;
			entry_control_wait = WAIT_END;
		}
	}
	if (entry_counter_stop)
		return;		/* エントリーカウンター停止中 */
	if (--entry_timer > 0)
		return;		/* まだ敵は出現しない */

#ifdef DEBUG
	{
		char temp_str[64];
		sprintf (temp_str, "ENTRY %03d", entry_counter);
		TxfontCursor (22, 5);
		TxfontPuts (temp_str);
	}
#endif

	do {
		short type = stage_data[stage][entry_counter].type;
		entry_timer = stage_data[stage][entry_counter].timer;

		switch (type) {
		case ENTRY_START:
			GvramInit ();
			bg_contrast_num = 33;
			entry_counter++;
			break;

		case ENTRY_END:
		    /* ここで停止されたエントリーカウンターは otoko.c VdispRoutine() で */
		    /* フェードアウトが終わると再開される */
			entry_counter_stop = !0;	/* エントリーカウンター停止 */
			bg_contrast_control = BG_FADEOUT;
			bg_contrast_num = 33;
			if (stage < STAGES - 1) {
				stage++;
				entry_counter = 0;
				entry_timer = 1;
			} else {
				entry_counter++;
				entry_timer = 1;
			}
			break;

		case ENTRY_NON:
			entry_counter++;
			break;

		case ENTRY_MUSIC:
			SoundMusic (stage_data[stage][entry_counter].arg);
			entry_counter++;
			break;

		case ENTRY_MUSIC_FADEOUT:
			SoundFadeOut (stage_data[stage][entry_counter].arg);
			entry_counter++;
			break;

		default:
		    /* control = 0 の時は entry_control が 0 になるまで待つ */
			if (entry_control_wait == WAIT_NON) {
				if (stage_data[stage][entry_counter].control == 0) {
					entry_counter_stop = !0;
					entry_control_wait = WAIT_NOW;
					break;
				}
			} else {
				entry_control_wait = WAIT_NON;
			}
			if (entry_control <= stage_data[stage][entry_counter].control) {
				EnemyInit (type,
					   stage_data[stage][entry_counter].x, stage_data[stage][entry_counter].y,
				   stage_data[stage][entry_counter].arg, (struct _enemy *) 0);
			}
			entry_counter++;
			break;
		}
	} while (entry_timer == 0);
}



void EntryTini (void)
{
}
