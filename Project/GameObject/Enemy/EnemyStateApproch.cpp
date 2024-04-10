#include "EnemyStateApproch.h"

void EnemyStateApproach::Update(Enemy* pEnemy) {
	pEnemy->Enemy::SetVelocity(0.0f, 0.0f, -0.05f);
	// 移動 (ベクトルを加算)
	pEnemy->Move();
	// 既定の位置に達したら離脱
	if (pEnemy->GetPosition().z < -10.0f) {
		pEnemy->ChangeState(new EnemyStateLeave());
	}
}