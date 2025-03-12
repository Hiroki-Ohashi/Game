#pragma once
#include "BaseEnemyState.h"

class Enemy;

class EnemyStateApproach : public BaseEnemyState {
public:
	void Update(Enemy* pEnemy);
};