#pragma once
#include <Camera.h>
#include <TextureManager.h>
#include <WorldTransform.h>
#include <Model.h>
#include "BossBullet.h"
#include "Collider.h"
using namespace Engine;

/// <summary>
/// Boss.h
/// Boss生成のヘッダーファイル
/// </summary>

class Player;
class GameScene;

// Bossクラス
class Boss : public Collider {
public:
	// 初期化処理
	void Initialize(Vector3 pos);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera* camera);

	// 出現処理
	void approach();
	// 攻撃処理
	void Attack();

	// 当たり判定処理
	void OnCollision() override;
	Vector3 GetWorldPosition() const override;

	// 死亡判定
	bool IsDead() const { return isDead_; }

	// Setter
	void SetIsDead(bool isDead) { isDead_ = isDead; }

	// Getter
	Vector3 GetPos() { return transform_.translate; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }
	void SetPlayer(Player* player) { player_ = player; }
private:
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();
	WorldTransform worldtransform_;
	EulerTransform transform_;

	// model
	std::unique_ptr<Model> model_;

	// 呼び出し
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
private:
	// Bossパラメータ
	uint32_t hp_;
	uint32_t enemyTex;
	uint32_t hit;

	// 移動速度
	float speedX = 0.5f;
	float speedY = 0.3f;

	float downSpeedY = 0.5f;
	float rotSpeedY = 0.3f;
	bool isApproach;

	// 発射タイマー
	int32_t attackTimer = 10;
	static const int kFireInterval = 120;
	bool isDead_;

	// ヒットタイマー
	int32_t hitTimer_;
	bool isHit_;
};