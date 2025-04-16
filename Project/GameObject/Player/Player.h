#pragma once
#include <TextureManager.h>
#include <Model.h>
#include <Input.h>
#include "PlayerBullet.h"
#include <Sprite.h>
#include "Collider.h"
#include "CollisionConfig.h"
#include "PlayerUI/PlayerUI.h"
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
	// 攻撃処理
	void LockOn(Vector3 EnemyPos, Vector3 EnemyPrePos);
	void Attack();
	// 描画処理
	void Draw(Camera* camera_);
	void DrawUI();
	void BulletDraw(Camera* camera_);

	// 当たり判定処理
	void OnCollision() override;
	Vector3 GetWorldPosition() const override;

	// Getter
	Vector3 GetPos() const { return worldtransform_.translate; }
	Vector3 GetReticlePos() { return positionReticle; }
	Vector3 Get3DWorldPosition();
	Vector3 GetVelocity() { return velocity_; }
	int32_t GetHP() { return HP; }
	bool GetIsHit() { return isHit_; }
	
	// 半分の大きさ
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
			worldtransform_.translate.x - worldtransform_.scale.x,
			worldtransform_.translate.y - worldtransform_.scale.y,
			worldtransform_.translate.z - worldtransform_.scale.z
		};
	}

	// AABBの最大座標を取得
	Vector3 GetAABBMax() const {
		return {
			worldtransform_.translate.x + worldtransform_.scale.x,
			worldtransform_.translate.y + worldtransform_.scale.y,
			worldtransform_.translate.z + worldtransform_.scale.z
		};
	}
	
	// 弾リストを取得
	 std::vector<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

	 // Setter
	 void SetGoalLine(bool goal);

private:
	// レティクルの方向にプレイヤーの向きを変える
	void PlayerRot();
	// 移動
	void Move();
	// レティクルの3D座標を2D変換
	void Convert2D(Camera* camera_);
	// 予測射撃
	Vector3 PredictPosition(Vector3 shotPosition, Vector3 targetPosition, Vector3 targetPrePosition, float bulletSpeed);
private:
	// シングルトン呼び出し
	TextureManager* textureManager_ = TextureManager::GetInstance();

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

	// 弾
	std::vector<std::unique_ptr<PlayerBullet>> bullets_;

	// UI
	std::unique_ptr<PlayerUI> uiModel_;

	// テクスチャ処理
	uint32_t playerTex;
	uint32_t reticleTex;
	uint32_t hit;

	// 速度
	Vector3 velocity_;
	float playerSpeed = 30.0f;

	// タイマー
	int32_t hitTimer_;
	bool isHit_;

	// 条件
	bool isLeft;
	bool isRight;

	// HP
	int32_t HP = 5;
	const uint32_t damage_ = 1;

	// 機体制御
	const float kMaxRoll = DirectX::XMConvertToRadians(65.0f); 
	const float kMaxPitch = DirectX::XMConvertToRadians(25.0f); 
	const float kYawSpeed = DirectX::XMConvertToRadians(2.5f);
	const float kRollLerpFactor = 0.2f; 
	const float kPitchLerpFactor = 0.15f;
	const float kYawLerpFactor = 1.0f;
	float yawSpeed_ = 0.0f;
};