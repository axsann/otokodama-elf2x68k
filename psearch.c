#include "player.h"
#include "psearch.h"



/* ���@�T�[�`:�G�L�����̍��W���猩�����@�̕�����Ԃ� */
unsigned char psearch (signed short ex, signed short ey)
{
	signed short dx, dy;

    /* ���S�̂��� dx,dy ���� -255~255 �͈̔͂ɔ[�߂� */
    /* �i���̂��ߍ��W�̍��� 256 �ȏゾ�ƌ덷���o�邪���p����Ȃ��j */
	dx = player->x - ex;
	if (dx > 255) {
		dx = 255;
	} else {
		if (dx < -255) {
			dx = -255;
		}
	}
	dy = player->y - ey;
	if (dy > 255) {
		dy = 255;
	} else {
		if (dy < -255) {
			dy = -255;
		}
	}

	if (dx > 0) {
		if (dy > 0) {
			return (pstable[dx][dy]);
		} else {
			return (256 - pstable[dx][-dy]);
		}
	} else {
		if (dy > 0) {
			return (128 - pstable[-dx][dy]);
		} else {
			return (128 + pstable[-dx][-dy]);
		}
	}
}



/* �C�ӃT�[�`:�G�L�����̍��W���猩���C�ӂ̕�����Ԃ� */
unsigned char msearch (signed short ex, signed short ey, signed short mx, signed short my)
{
	signed short dx, dy;

    /* ���S�̂��� dx,dy ���� -255~255 �͈̔͂ɔ[�߂� */
    /* �i���̂��ߍ��W�̍��� 256 �ȏゾ�ƌ덷���o�邪���p����Ȃ��j */
	dx = mx - ex;
	if (dx > 255) {
		dx = 255;
	} else {
		if (dx < -255) {
			dx = -255;
		}
	}
	dy = my - ey;
	if (dy > 255) {
		dy = 255;
	} else {
		if (dy < -255) {
			dy = -255;
		}
	}

	if (dx > 0) {
		if (dy > 0) {
			return (pstable[dx][dy]);
		} else {
			return (256 - pstable[dx][-dy]);
		}
	} else {
		if (dy > 0) {
			return (128 - pstable[-dx][dy]);
		} else {
			return (128 + pstable[-dx][-dy]);
		}
	}
}
