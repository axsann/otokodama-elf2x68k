#include "player.h"
#include "psearch.h"



/* 自機サーチ:敵キャラの座標から見た自機の方向を返す */
unsigned char psearch (signed short ex, signed short ey)
{
	signed short dx, dy;

    /* 安全のため dx,dy 共に -255~255 の範囲に納める */
    /* （そのため座標の差が 256 以上だと誤差が出るが実用上問題ない） */
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



/* 任意サーチ:敵キャラの座標から見た任意の方向を返す */
unsigned char msearch (signed short ex, signed short ey, signed short mx, signed short my)
{
	signed short dx, dy;

    /* 安全のため dx,dy 共に -255~255 の範囲に納める */
    /* （そのため座標の差が 256 以上だと誤差が出るが実用上問題ない） */
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
