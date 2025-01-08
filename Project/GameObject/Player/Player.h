#pragma once
#include <TextureManager.h>
#include <Model.h>
#include <Input.h>
#include "PlayerBullet.h"
#include <Sprite.h>
#include "Collider.h"
#include "CollisionConfig.h"
using namespace Engine;

/// <summary>
/// Player.h
/// player生成のヘッダーファイル
/// </summary>

// Playerクラス
class Player : public Collider {
public:
	// デストラクタ
	~Player();

	// 初期化処理
	void Initialize();
	// 更新処理
	void Update(Camera* camera_);
	void LockOn(bool isLockOn, Vector3 EnemyPos);
	// 描画処理
	void Draw(Camera* camera_);
	void DrawUI();
	void BulletDraw(Camera* camera_);

	// 当たり判定処理
	void OnCollision() override;
	Vector3 GetWorldPosition() const override;

	// Getter
	Vector3 GetPos() { return worldtransform_.translate; }
	Vector3 GetReticlePos() { return positionReticle; }
	Vector3 Get3DWorldPosition();
	Vector3 GetVelocity() { return velocity_; }
	int32_t GetHP() { return HP; }
	bool GetIsHit() { return isHit_; }
	
	Vector3 GetHalfSize() const {
		return {
			worldtransform_.scale.x / 2.0f,
			worldtransform_.scale.y / 2.0f,
			worldtransform_.scale.z / 2.0f
		};
	}

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

	
	// 弾リストを取得
	 std::vector<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }
private:
	// 攻撃処理
	void Attack();
private:
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();
	Input* input_ = Input::GetInsTance();


	// プレイヤー
	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;

	// レティクル
	std::unique_ptr<Model> reticleModel_;
	std::unique_ptr<Sprite> reticleSprite_;
	WorldTransform reticleWorldtransform_;
	EulerTransform reticleTransform_;
	Vector3 positionReticle;

	// UI
	std::unique_ptr<Sprite> hp0_;
	std::unique_ptr<Sprite> hp1_;
	std::unique_ptr<Sprite> hp2_;
	std::unique_ptr<Sprite> hp3_;
	std::unique_ptr<Sprite> hp4_;
	std::unique_ptr<Sprite> hp5_;

	// 弾
	std::vector<std::unique_ptr<PlayerBullet>> bullets_;

	// テクスチャ処理
	uint32_t playerTex;
	uint32_t reticleTex;
	uint32_t hit;

	uint32_t hp5;
	uint32_t hp4;
	uint32_t hp3;
	uint32_t hp2;
	uint32_t hp1;
	uint32_t hp0;

	// 速度
	Vector3 velocity_;

	// タイマー
	int32_t hitTimer_;
	bool isHit_;

	// 条件
	bool isLeft;
	bool isRight;

	// HP
	int32_t HP = 5;
	const uint32_t damage_ = 1;
};