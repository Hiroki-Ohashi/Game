#pragma once
#include "Collider.h"
#include "Player.h"
#include "Enemy.h"
#include "GameScene.h"
#include <list>
#include <cstdint>

class Player;
class Enemy;
class GameScene;

class CollisionManeger {
public:
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);
	void CheckAllCollision();

	void SetPlayer(Player* player) { player_ = player; }
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
	void SetGameScene(GameScene* gamescene) { gameScene_ = gamescene; }

private:
	// 自キャラ
	Player* player_ = nullptr;
	// 敵キャラ
	Enemy* enemy_ = nullptr;
	// ゲームシーン
	GameScene* gameScene_ = nullptr;
};