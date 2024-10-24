#pragma once
#include <WorldTransform.h>
#include <Model.h>

class Player;

/// <summary>
/// BossBullet.h
/// ボス弾生成のヘッダーファイル
/// </summary>

// BossBulletクラス
class BossBullet {
public:
	// 初期化処理
	void Initialize(Vector3 pos, Vector3 velocity);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera* camera, uint32_t index);
	// 当たり判定処理
	void OnCollision() { isDead_ = true; }
	// 死亡判定
	bool IsDead() const { return isDead_; }

	// Getter
	Vector3 GetPos() { return worldtransform_.translate; }

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