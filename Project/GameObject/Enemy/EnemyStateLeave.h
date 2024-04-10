#pragma once
#include "BaseEnemyState.h"
#include "Enemy.h"

class Enemy;

class EnemyStateLeave : public BaseEnemyState {
public:
	void Update(Enemy* pEnemy);
};