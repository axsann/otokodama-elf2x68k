#include <XSP2lib.H>

#include "../otoko.h"
#include "../player.h"
#include "../enemy.h"
#include "../eshot.h"
#include "../effect.h"
#include "../priority.h"
#include "../sound.h"

#define PALET_MVLASERH	0x0400
#define SPEED_LASER		28

static short EnemyMoveMvLaserH (ENEMY *);
static void EnemyTiniMvLaserH (ENEMY *);


void EnemyInitMvLaserH (ENEMY * p)
{
	p->vx = 0;
	p->vy = 0;
	p->hit_px = 24;
	p->hit_py = 16;
	p->hit_sx = 24;
	p->hit_sy = 16;
	p->hit_cx = 6;
	p->damage = 0;
	p->flash = 0;
	p->hp = 200;
	p->pt = obj_mvlaserh;
	p->seq = 0;
	p->m_work = 0;

	p->func_enemy_move = EnemyMoveMvLaserH;
	p->func_enemy_tini = EnemyTiniMvLaserH;
}



static short EnemyMoveMvLaserH (ENEMY * p)
{
	signed short offset_y[20] =
	{-32, -48, -47, -47, -46, -46, -45, -44, -43, -42,
	 -41, -40, -39, -38, -37, -36, -35, -34, -33, -32};

	if ((p->child_kill) || (p->child_death))
		return (-1);	/* 消去 */

	switch (p->seq) {
	case 0:		/* 発射待ち状態 */
		p->x = (p->parent->x) + SHORT_LX;
		p->y = (p->parent->y) + SHORT_LY;
		break;
	case 1:		/* EnemyMoveLaser() の中で 1 になる */
		if (p->m_work == 0) {	/* 最初の１回は何故か２発撃つ（小細工） */
			EshotInit (ESHOT_LASEREX, p->x, p->y + 16, SPEED_LASER, 64, 0);
			SoundSetSE (SE_LASER);	/* レーザー効果音を鳴らす */
		}
		EshotInit (ESHOT_LASEREX, p->x, p->y + 32, SPEED_LASER, 64, 0);
		p->x = (p->parent->x) + SHORT_LX;
		p->y = (p->parent->y) + SHORT_LY + 32 + offset_y[p->m_work];
		if (p->m_work++ > 17) {
			p->seq = 0;
			p->m_work = 0;
		}
		break;
	}

	/* 自機ショットに当たった時の処理 */
	if (p->damage) {
		if (p->damage_core) {
			SoundSetSE (SE_CORE_DAMAGE);
			p->info = PALET_RED | PRIORITY_BOSS_PARTS;
			p->damage_core = 0;
		} else {
			SoundSetSE (SE_DAMAGE);
			p->info = PALET_DAMAGE | PRIORITY_BOSS_PARTS;
		}
		if ((p->hp -= p->damage) <= 0) {
			EffectInit (EFFECT_EXPL, 0, p->x, p->y);
			SoundSetSE (SE_EXPL);	/* ザコ爆発音 */
			p->child_death = !0;	/* 消去準備 */
		}
		p->damage = 0;
		p->flash = TIMER_FLASH_DAMAGE;
	} else {
		if ((p->flash++ > TIMER_FLASH_RED) && (p->hp < 30)) {
			p->info = PALET_RED | PRIORITY_BOSS_PARTS;
			if (p->flash > TIMER_FLASH_NORMAL)
				p->flash = 0;
		} else {
			p->info = PALET_MVLASERH | PRIORITY_BOSS_PARTS;
		}
	}

	xobj_set_st (p);

	return (0);
}



static void EnemyTiniMvLaserH (ENEMY * p)
{
}
