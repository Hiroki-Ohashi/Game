#include "FixedEnemy.h"
#include "player/Player.h"
#include "GameScene.h"

FixedEnemy::FixedEnemy()
{
}

void FixedEnemy::Initialize(Vector3 pos)
{
	// 初期トランスフォームの設定（スケール・回転・位置）
	transform_ = {
		{2.0f, 2.0f, 2.0f},          // スケール：サイズ
		{0.0f, 0.0f, 0.0f},          // 回転：初期は未回転
		{pos.x, pos.y, pos.z}        // 位置：初期位置
	};

	// モデルを生成し、モデルデータ（fixed.obj）を読み込む
	model_ = std::make_unique<Model>();
	model_->Initialize("fixed.obj", transform_);

	// ワールド変換にトランスフォームを反映
	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	model_->SetWorldTransform(worldtransform_);
	worldtransform_.UpdateMatrix(); // 行列を更新して変換を反映

	// ロック中のテクスチャを読み込む
	lockTex = textureManager_->Load("resources/Lock.png");

	// 2Dスプライト（ロックオンレティクル）を作成・初期化
	enemySprite_ = std::make_unique<Sprite>();
	enemySprite_->Initialize({ 590.0f, 310.0f }, { 35.0f, 35.0f }, lockOnTex); // 位置・サイズ・テクスチャ
	enemySprite_->SetRotation({ 0.0f, 0.0f, -0.8f });

	// パーティクルのエミッタ初期化
	emitter.count = 10;                      // 一度に発生させるパーティクル数
	emitter.frequency = 0.5f;               // 発生間隔（秒）
	emitter.frequencyTime = 0.0f;           // 経過時間初期化
	emitter.transform.translate = { 0.0f, 0.0f, 0.0f }; // 相対位置
	emitter.transform.rotate = { 0.0f, 0.0f, 0.0f };    // 回転
	emitter.transform.scale = { 1.0f, 1.0f, 1.0f };     // サイズ

	// パーティクル本体を生成し、初期化・エミッタを設定
	particle_ = std::make_unique<Particles>();
	particle_->Initialize("board.obj", worldtransform_.translate, emitter);
	particle_->SetEmitter(emitter);

	// 状態初期化
	isDead_ = false;               // 敵はまだ死んでいない
	isLockOn_ = false;             // ロックオンされていない状態
	isPossibillityLock = false;    // ロックオン候補ではない

	// 各種テクスチャを読み込む
	lockOnTex = textureManager_->Load("resources/reticle.png");   // ロックオン候補表示用
	enemyTex = textureManager_->Load("resources/white.png");     // 敵の見た目テクスチャ
	enemyBulletTex = textureManager_->Load("resources/red.png");       // 弾の見た目テクスチャ
	bakuhatuTex = textureManager_->Load("resources/bakuhatu.png");  // 爆発エフェクト

	// 衝突属性・対象の設定
	SetCollosionAttribute(kcollisionAttributeEnemy); // 自分は敵属性
	SetCollisionMask(kcollisionAttributePlayer);     // 衝突対象はプレイヤーのみ
}

void FixedEnemy::Update(Camera* camera)
{
	// ワールド行列を更新
	model_->SetWorldTransform(worldtransform_);
	worldtransform_.UpdateMatrix();

	// 攻撃のタイミング処理（連射や発射間隔）
	HandleAttackLogic();

	// プレイヤーの方向を向くように回転を更新
	UpdateRotationToPlayer();

	// 3D座標からスクリーン座標への変換とスプライト位置設定
	UpdateReticlePosition(camera);

	// ロックオン状態に応じてレティクルのテクスチャを変更
	UpdateReticleTexture();

	// 位置情報の更新）
	UpdatePosition(worldtransform_.translate);
}

void FixedEnemy::HandleAttackLogic() {
	attackTimer--;

	if (attackTimer <= 0 && !isDead_) {
		rensya--;

		// 連射間隔を満たし、プレイヤーが射程内なら攻撃
		if (rensya < 0 && worldtransform_.translate.z - player_->GetPos().z <= kMaxAttack) {
			Attack();
			rensyanum += rensyaNumSpeed;
		}

		// まだ最大連射数に達していないなら、再度発射までのカウントをリセット
		if (rensyanum < kMaxRensyaNum) {
			rensya = 10;
		}

		// 連射上限に達したらクールタイム
		if (rensyanum >= kMaxRensyaNum) {
			attackTimer = kFireInterval;
			rensyanum = 0;
		}
	}
}

// プレイヤーの方向を向くように敵の回転を調整
void FixedEnemy::UpdateRotationToPlayer() {
	Vector3 end = player_->GetPos();
	Vector3 start = worldtransform_.translate;

	// プレイヤーとの方向ベクトルを正規化
	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z - start.z;

	diff = Normalize(diff);
	Vector3 velocity(diff.x, diff.y, diff.z);

	// Y軸回転：プレイヤー方向を向く
	worldtransform_.rotate.y = std::atan2(velocity.x, velocity.z);

	// X軸回転：上下方向の調整
	float velocityXZ = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
	worldtransform_.rotate.x = std::atan2(-velocity.y, velocityXZ);

	worldtransform_.UpdateMatrix();
}

void FixedEnemy::UpdateReticlePosition(Camera* camera) {
	positionReticle = GetPos();

	// ビューポート変換行列の作成
	Matrix4x4 matViewport = MakeViewportMatrix(
		0, 0,
		static_cast<float>(WinApp::GetKClientWidth()),
		static_cast<float>(WinApp::GetKClientHeight()),
		0, 1
	);

	// ビュー x プロジェクション x ビューポートの合成行列
	Matrix4x4 matVPV = Multiply(
		Multiply(camera->viewMatrix, camera->projectionMatrix),
		matViewport
	);

	// ワールド座標 → スクリーン座標へ変換
	positionReticle = Transform(positionReticle, matVPV);

	// スプライトの位置をスクリーン座標に合わせる
	enemySprite_->SetPosition(Vector2(positionReticle.x - 35.0f, positionReticle.y));
}

// ロックオン状態に応じてスプライトのテクスチャを切り替える
void FixedEnemy::UpdateReticleTexture() {
	if (isLockOn_) {
		enemySprite_->SetTexture(lockTex);
	}
	else {
		enemySprite_->SetTexture(lockOnTex);
	}
}
