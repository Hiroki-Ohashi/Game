#pragma once
#include <WorldTransform.h>
#include <Model.h>
#include "Collider.h"
using namespace Engine;

class Player;

/// <summary>
/// EnemyBullet.h
/// 敵弾生成のヘッダーファイル
/// </summary>

// EnemyBulletクラス
class EnemyBullet : public Collider {
public:
	// 初期化処理
	void Initialize(Vector3 pos, Vector3 velocity);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera* camera, uint32_t index);
	// 当たり判定処理
	void OnCollision() override;
	Vector3 GetWorldPosition() const override;
	// 死亡判定
	bool IsDead() const { return isDead_; }

	// Getter
	Vector3 GetPos() { return worldtransform_.translate; }

	// AABBの最小座標を取得
	Vector3 GetAABBMin() const {
		return {
			worldtransform_.translate.x - (worldtransform_.scale.x / 1.0f),
			worldtransform_.translate.y - (worldtransform_.scale.y / 1.0f),
			worldtransform_.translate.z - (worldtransform_.scale.z / 1.0f)
		};
	}

	// AABBの最大座標を取得
	Vector3 GetAABBMax() const {
		return {
			worldtransform_.translate.x + (worldtransform_.scale.x / 1.0f),
			worldtransform_.translate.y + (worldtransform_.scale.y / 1.0f),
			worldtransform_.translate.z + (worldtransform_.scale.z / 1.0f)
		};
	}
	// Setter
	void SetPlayer(Player* player) { player_ = player; }
private:
	// model
	std::unique_ptr<Model> model_;
	EulerTransform transform;
	WorldTransform worldtransform_;
	// 自機
	Player* player_ = nullptr;
	// 速度
	Vector3 velocity_;

	// デスタイマー
	static const int32_t kLifeTime = 60 * 5;
	int32_t deathTimer_ = kLifeTime;
	bool isDead_ = false;
};