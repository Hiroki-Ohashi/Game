#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>
#include "EnemyBullet.h"
#include "Collider.h"
#include "Particle.h"
#include "EnemyBulletPool.h"
#include "Sprite.h"
#include "CollisionConfig.h"

class Enemy;

class BaseEnemyState {
public:
	virtual void Update(Enemy* pEnemy) = 0;
};