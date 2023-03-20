#include <x68k/iocs.h>
#include "txfont.h"

static char *page_ptr;		/* �\����ʂ̍���̃A�h���X */
static short cursor_x = 0, cursor_y = 0;
static char *cursor_ptr;	/* �J�[�\���ʒu�̃A�h���X */



/* �\����ʂ̍���̃A�h���X���w��i0xe00000 �Ƃ��j */
void TxfontPage (char *p)
{
	page_ptr = p;
	cursor_ptr = p + cursor_y * 128 * 8 + cursor_x;
}



void TxfontCursor (short x, short y)
{
	cursor_x = x;
	cursor_y = y;
	cursor_ptr = page_ptr + cursor_y * 128 * 8 + cursor_x;
}


int TxfontInit0 (void)
{
	TxfontPage ((void *) 0xe00000);	/* �f�t�H���g�l */
	return (0);
}



/* �P�����\���i�����p�E�R���g���[���R�[�h�Ȃ��j */
static void disp1 (char c)
{
	char *t;
	unsigned char *p = font_data + (((int) c - 0x20) << 4);

	t = (char *) cursor_ptr;
	*(t + 128 * 0) = *p++;
	*(t + 128 * 1) = *p++;
	*(t + 128 * 2) = *p++;
	*(t + 128 * 3) = *p++;
	*(t + 128 * 4) = *p++;
	*(t + 128 * 5) = *p++;
	*(t + 128 * 6) = *p++;
	*(t + 128 * 7) = *p++;

	t += 0x020000;
	*(t + 128 * 0) = *p++;
	*(t + 128 * 1) = *p++;
	*(t + 128 * 2) = *p++;
	*(t + 128 * 3) = *p++;
	*(t + 128 * 4) = *p++;
	*(t + 128 * 5) = *p++;
	*(t + 128 * 6) = *p++;
	*(t + 128 * 7) = *p++;

	cursor_ptr++;
	cursor_x++;
}



/* �P�����\�� */
void TxfontPutchar (char c)
{
	int sp;

	sp = _iocs_b_super (0);
	switch (c) {
	case 0x09:
		cursor_x = (cursor_x + 8) & 0xfff8;
		cursor_ptr = page_ptr + cursor_y * 128 * 8 + cursor_x;
		break;
	case 0x0d:
		cursor_x = 0;
		cursor_y++;
		cursor_ptr = page_ptr + cursor_y * 128 * 8;
		break;
	case 0x0a:
		break;
	default:
		disp1 (c);
	}
	if (sp > 0)
		_iocs_b_super (sp);
}



/* ������\�� */
void TxfontPuts (char *c)
{
	int sp;

	sp = _iocs_b_super (0);
	while (*c != 0) {
		switch (*c) {
		case 0x09:
			cursor_x = (cursor_x + 8) & 0xfff8;
			cursor_ptr = page_ptr + cursor_y * 128 * 8 + cursor_x;
			break;
		case 0x0d:
			break;
		case 0x0a:
			cursor_x = 0;
			cursor_y++;
			cursor_ptr = page_ptr + cursor_y * 128 * 8;
			break;
		default:
			disp1 (*c);
		}
		c++;
	}
	if (sp > 0)
		_iocs_b_super (sp);
}


void TxfontTini0 (void)
{
}
