#include "EnemyStateLeave.h"

void EnemyStateLeave::Update(Enemy* pEnemy) {
	pEnemy->SetVelocity(-0.01f, 0.01f, 0.0f);

	// 移動 (ベクトルを加算)
	pEnemy->Move();
}