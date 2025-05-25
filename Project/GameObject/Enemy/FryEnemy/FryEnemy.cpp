#include "FryEnemy.h"
#include "player/Player.h"
#include "GameScene.h"


FryEnemy::FryEnemy()
{
}

void FryEnemy::Initialize(Vector3 pos) {
	// 初期トランスフォーム（スケール・回転・座標）を設定
	transform_ = {
		{2.0f, 2.0f, 2.0f},    // スケール：大きさ
		{0.0f, 0.0f, 0.0f},    // 回転：初期は回転なし
		{pos.x, pos.y, pos.z}  // 位置：引数から受け取る
	};

	// モデルデータを読み込んで初期化（使用する .obj ファイル名を指定）
	model_ = std::make_unique<Model>();
	model_->Initialize("doron.obj", transform_);

	// モデルのワールド変換情報を設定
	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;

	// モデルにワールド変換を反映
	model_->SetWorldTransform(worldtransform_);
	worldtransform_.UpdateMatrix(); // 行列の更新

	// ロックオン時のテクスチャや弾などのリソースをロード
	lockTex = textureManager_->Load("resources/Lock.png");        // ロックオン中のアイコン
	lockOnTex = textureManager_->Load("resources/reticle.png");     // ロックオン候補のレティクル
	enemyTex = textureManager_->Load("resources/white.png");       // 敵の基本テクスチャ
	enemyBulletTex = textureManager_->Load("resources/red.png");         // 敵弾のテクスチャ
	bakuhatuTex = textureManager_->Load("resources/bakuhatu.png");    // 爆発エフェクトのテクスチャ

	// ロックオン対象スプライト（2D）を初期化
	enemySprite_ = std::make_unique<Sprite>();
	enemySprite_->Initialize({ 590.0f, 310.0f }, { 35.0f, 35.0f }, lockOnTex);
	enemySprite_->SetRotation({ 0.0f, 0.0f, -0.8f });

	// パーティクルの発生情報を設定
	emitter.count = 10;               // 同時発生数
	emitter.frequency = 0.5f;        // 発生頻度
	emitter.frequencyTime = 0.0f;    // 経過時間初期化
	emitter.transform.translate = { 0.0f, 0.0f, 0.0f }; // 相対位置
	emitter.transform.rotate = { 0.0f, 0.0f, 0.0f };    // 相対回転
	emitter.transform.scale = { 1.0f, 1.0f, 1.0f };     // スケール

	// パーティクル本体を初期化し、エミッタを設定
	particle_ = std::make_unique<Particles>();
	particle_->Initialize("board.obj", worldtransform_.translate, emitter); // モデルと発生座標
	particle_->SetEmitter(emitter); // エミッタ設定

	// 状態フラグを初期化
	isDead_ = false;             // 生存中
	isLockOn_ = false;           // ロックオンされていない
	isPossibillityLock = false;  // ロックオン可能状態ではない

	// 攻撃タイマー初期値
	attackTimer = 10;

	// 衝突判定の属性・マスク設定（当たる相手の指定）
	SetCollosionAttribute(kcollisionAttributeEnemy);     // 自身は敵属性
	SetCollisionMask(kcollisionAttributePlayer);         // プレイヤーにのみ当たる
}

void FryEnemy::Update(Camera* camera) {
	// ワールド変換行列を更新（前回の回転・移動反映）
	worldtransform_.UpdateMatrix();

	// 攻撃タイマーと攻撃判定処理
	HandleAttack();

	// プレイヤーに近づいたら移動を開始
	HandleMovement();

	// プレイヤーの方向に向きを合わせる（Y軸・X軸回転）
	UpdateOrientationToPlayer();

	// 向きを更新した後、ワールド行列を再更新
	worldtransform_.UpdateMatrix();
	model_->SetWorldTransform(worldtransform_);

	// レティクル（照準）のスクリーン座標変換と描画設定
	UpdateReticle(camera);

	// ロックオン状態によってスプライトのテクスチャを切り替える
	UpdateLockOnTexture();

	// 外部用に現在位置を更新
	UpdatePosition(worldtransform_.translate);
}

void FryEnemy::HandleAttack() {
	attackTimer--;

	if (attackTimer <= 0 && !isDead_) {
		// Z距離が2500以内なら攻撃
		float distanceZ = worldtransform_.translate.z - player_->GetPos().z;
		if (distanceZ <= 2500.0f) {
			Attack();
		}

		// タイマーリセット
		attackTimer = kFireInterval;
	}
}

void FryEnemy::HandleMovement() {
	float distanceZ = worldtransform_.translate.z - player_->GetPos().z;

	// プレイヤーが近づいたら移動開始
	if (distanceZ <= kMaxAttack && !isDead_) {
		// Z方向前進（限界位置まで）
		if (worldtransform_.translate.z <= 99600.0f) {
			worldtransform_.translate.z += enemySpeed.z;
		}

		// Y方向上下移動（指定目標高さ posParam.y に向けて）
		const float kMoveSpeed = 0.005f;
		speedY += (worldtransform_.translate.y < posParam.y) ? kMoveSpeed : -kMoveSpeed;
		worldtransform_.translate.y += speedY;
	}
}

void FryEnemy::UpdateOrientationToPlayer() {
	Vector3 start = worldtransform_.translate;
	Vector3 end = player_->GetPos();

	// プレイヤーへのベクトルを正規化
	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z - start.z;

	diff = Normalize(diff);

	// 回転角計算
	float velocityXZ = std::sqrt(diff.x * diff.x + diff.z * diff.z);
	worldtransform_.rotate.y = std::atan2(diff.x, diff.z);            // Y軸回転（左右）
	worldtransform_.rotate.x = std::atan2(-diff.y, velocityXZ);       // X軸回転（上下）
}

void FryEnemy::UpdateReticle(Camera* camera) {
	// レティクル位置（ワールド座標）
	positionReticle = GetPos();

	// ビューポート行列（画面サイズに合わせた座標変換）
	Matrix4x4 matViewport = MakeViewportMatrix(
		0, 0,
		(float)WinApp::GetKClientWidth(),
		(float)WinApp::GetKClientHeight(),
		0, 1
	);

	// ワールド→ビュー→プロジェクション→ビューポート の合成行列
	Matrix4x4 matVPV = Multiply(
		Multiply(camera->viewMatrix, camera->projectionMatrix),
		matViewport
	);

	// ワールド座標をスクリーン座標に変換
	positionReticle = Transform(positionReticle, matVPV);

	// レティクルスプライトの位置を設定（中心からずらして表示）
	enemySprite_->SetPosition(Vector2(positionReticle.x - 25.0f, positionReticle.y));
}

void FryEnemy::UpdateLockOnTexture() {
	enemySprite_->SetTexture(isLockOn_ ? lockTex : lockOnTex);
}
