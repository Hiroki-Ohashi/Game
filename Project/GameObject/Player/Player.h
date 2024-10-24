#pragma once
#include <TextureManager.h>
#include <Model.h>
#include <Input.h>
#include "PlayerBullet.h"

/// <summary>
/// Player.h
/// player生成のヘッダーファイル
/// </summary>


class Player {
public:
	// デストラクタ
	~Player();

	// 初期化処理
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera* camera_);
	void BulletDraw(Camera* camera_);

	// 当たり判定処理
	void OnCollision() { isHit_ = true; }

	// Getter
	Vector3 GetPos() { return worldtransform_.translate; }
	Vector3 GetReticlePos() { return reticleWorldtransform_.translate; }
	Vector3 Get3DWorldPosition();
	Vector3 GetVelocity() { return velocity_; }
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
	WorldTransform reticleWorldtransform_;
	EulerTransform reticleTransform_;

	// 弾
	std::vector<std::unique_ptr<PlayerBullet>> bullets_;

	// テクスチャ処理
	uint32_t bulletTex;
	uint32_t playerTex;
	uint32_t reticleTex;
	uint32_t hit;

	// 速度
	Vector3 velocity_;

	// タイマー
	int32_t hitTimer_;
	bool isHit_;

	// 条件
	bool isLeft;
	bool isRight;
};