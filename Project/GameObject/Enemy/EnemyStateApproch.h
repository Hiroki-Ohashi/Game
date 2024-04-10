#pragma once
#include "BaseEnemyState.h"
#include "Enemy.h"
#include "EnemyStateLeave.h"

class Enemy;

class EnemyStateApproach : public BaseEnemyState {
public:
	void Update(Enemy* pEnemy);
};