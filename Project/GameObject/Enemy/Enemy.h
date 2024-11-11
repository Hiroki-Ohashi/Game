#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>
#include "EnemyBullet.h"

class Player;
class GameScene;

/// <summary>
/// Enemy.h
/// 敵生成のヘッダーファイル
/// </summary>

// Enemyクラス
class Enemy {
public:
	// 初期化処理
	void Initialize(Vector3 pos);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera* camera);
	// 攻撃処理
	void Attack();
	// 当たり判定処理
	void OnCollision() { isDead_ = true; }

	// 死亡判定
	void SetIsDead(bool isDead) { isDead_ = isDead; }
	bool IsDead(){ return isDead_; }

	// Getter
	Vector3 GetPos() { return worldtransform_.translate; }

	// Setter
	void SetPosition(Vector3 pos) { worldtransform_.translate = pos; }
	void SetRotation(Vector3 rotate) { worldtransform_.rotate = rotate; }
	void SetScale(Vector3 scale) { worldtransform_.scale = scale; }
	void SetLight(bool isLight) { model_->SetLight(isLight); }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetPlayer(Player* player) { player_ = player; }
private:
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;

	// 呼び出し
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
private:
	// テクスチャ
	uint32_t enemyTex;
	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 180;
	bool isDead_;
};