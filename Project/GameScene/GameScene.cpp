#include "GameScene.h"
#include <time.h>

using namespace Engine;

/// <summary>
/// GameScene.cpp
/// ゲームシーンのソースファイル
/// </summary>

GameScene::~GameScene(){
}

void GameScene::Initialize() {
	camera_.Initialize();
	textureManager_->Initialize();

	// PostEffect
	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(0.0f, 16.0f);
	postProcess_->SetNoise(0.0f, 0.0f);
	postProcess_->SetBlurStrength(0.0f);

	// player
	player_ = std::make_unique<Player>();
	player_->Initialize();

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// Texture
	go = textureManager_->Load("resources/go.png");
	ready = textureManager_->Load("resources/ready.png");
	uv = textureManager_->Load("resources/map.png");
	enemyBulletTex = textureManager_->Load("resources/black.png");
	bossBulletTex = textureManager_->Load("resources/red.png");
	backTitle = textureManager_->Load("resources/backTitle.png");
	retry = textureManager_->Load("resources/retry.png");

	// UI(ready)
	ready_ = std::make_unique<Sprite>();
	ready_->Initialize(Vector2{ 340.0f, 0.0f }, Vector2{ 600.0f, 300.0f }, ready);
	ready_->SetSize({ 600.0f, 300.0f });

	// UI(go)
	transform_ = { {10.0f,10.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,30.0f,-100.0f} };
	go_ = std::make_unique<Model>();
	go_->Initialize("board.obj", transform_);
	go_->SetLight(false);

	// 
	sentaku_ = std::make_unique<Sprite>();
	sentaku_->Initialize(Vector2{ 1050.0f, 560.0f }, Vector2{ 127.0f, 107.0f }, retry);
	sentaku_->SetSize({ 127.0f, 107.0f });

	// Json
    json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("stage");
	json_->Adoption(levelData_, true);
	json_->EnemyAdoption(levelData_, player_.get(), this);
	json_->FixedEnemyAdoption(levelData_, player_.get(), this);

	for (std::unique_ptr<Enemy>& enemy : json_->GetEnemys()) {
		enemy->SetIsDead(false);
	}

	// stage
	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	/*particle_ = std::make_unique<Particles>();
	particle_->Initialize("board.obj", pos_, 80);*/

	// boolInit
	isVignette_ = true;
	isGameClear_ = false;
	isApploach_ = true;
	isGameOver_ = false;
	isPose_ = false;

	// cameraInit
	camera_.cameraTransform.translate = { player_->GetPos().x, player_->GetPos().y + cameraOffset.y,  player_->GetPos().z - cameraOffset.z };

	// EffectInit
	blurStrength_ = 0.3f;
	noiseStrength = 0.0f;
}

void GameScene::Update(){

	camera_.Update();

	postProcess_->NoiseUpdate(0.1f);

	stage_->Update();

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInsTance()->GetJoystickState(joyState)) {
		bool currentBackButtonState = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START);

		// ボタンが前フレームで押されておらず、今フレームで押されたらトグル
		if (!prevBackButtonState_ && currentBackButtonState) {
			isPose_ = !isPose_;
		}

		// 現在のボタン状態を次のフレームに持ち越す
		prevBackButtonState_ = currentBackButtonState;
	}

	if (isPose_) {
		postProcess_->SetNoise(0.2f, 1.0f);

		// 十字キーでシーン選択
		if (Input::GetInsTance()->GetJoystickState(joyState)) {
			// 長押し防止
			if (scenePrev == 0) {
				if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_DPAD_DOWN)) {
					scenePrev = 1;
					sentaku_->SetTexture(backTitle);
				}
			}
			else if (scenePrev == 1) {
				if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_DPAD_UP)) {
					scenePrev = 0;
					sentaku_->SetTexture(retry);
				}
			}

			// 選んだシーンをAボタンで遷移開始
			if (scenePrev == 0) {
				if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
					isPose_ = false;
				}
			}
			else if (scenePrev == 1) {
				if (Input::GetInsTance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {
					isVignette_ = true;
				}
			}

		}

		// 選んだシーンでフェードイン
		if (scenePrev == 1) {
			if (isVignette_) {
				postProcess_->VignetteFadeIn(0.1f, 0.1f);
			}

			if (postProcess_->GetVignetteLight() <= 0.0f) {
				isVignette_ = false;
				// タイトルシーンへ
				sceneNo = TITLE;
			}
		}
	}
	else {
		postProcess_->SetLineStrength(0.0f);

		// json更新処理
		json_->Update();
		json_->EnemyUpdate(camera_, player_.get(), this);
		json_->FixedEnemyUpdate(camera_, player_.get(), this);

		// ゲームスタート
		// 画面が切り替わったらvignetteをフェードアウトして明るくする
		if (isVignette_) {
			postProcess_->VignetteFadeOut(0.1f, 0.1f, 16.0f, 0.0f);
		}
		if (postProcess_->GetVignetteShape() <= 0.0f) {
			isVignette_ = false;
		}
		// スタート演出が終わったらプレイヤー更新処理
		if (isApploach_ == false) {
			player_->Update(&camera_);
		}

		// Lockon
		LockOnEnemy();

		// 敵弾更新
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
			bullet->Update();
		}

		// デスフラグの立った敵弾を排除
		enemyBullets_.erase(
			std::remove_if(
				enemyBullets_.begin(),
				enemyBullets_.end(),
				[](const std::unique_ptr<EnemyBullet>& bullet) {
					return bullet->IsDead();
				}
			),
			enemyBullets_.end()
		);

		// カメラシェイク
		ShakeCamera();

		// ゲームスタート演出
		Start();

		// ゲームクリア演出
		Clear();

		// ゲームオーバー演出
		Over();

		// 当たり判定
		CheckAllCollisions();
	}
}

void GameScene::Draw()
{
	// 天球描画
	skydome_->Draw(&camera_);
	// 床描画
	stage_->Draw(&camera_);
	// 敵、ステージ描画
	json_->Draw(camera_, uv);
	// プレイヤーの弾描画
	player_->BulletDraw(&camera_);

	// 敵弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Draw(&camera_, bossBulletTex);
	}

	// プレイヤー描画
	player_->Draw(&camera_);

	if (isApploach_) {
		// readyUI描画
		ready_->Draw();
	}
	else {
		// goUI描画
		go_->Draw(&camera_, go);

		if (isGameClear_ == false) {
			// プレイヤーUI描画
			player_->DrawUI();
		}
	}

	//json_->DrawEnemy(camera_);

	if (isPose_) {
		sentaku_->Draw();
	}
}

void GameScene::PostDraw()
{
	postProcess_->NoiseDraw();
}

void GameScene::CheckAllCollisions()
{

	// 自弾リストの取得
	std::vector<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	std::vector<std::unique_ptr<EnemyBullet>>& enemyBullets = enemyBullets_;

	// コライダー
	std::list<Collider*> colliders_;

	// 登録
	// player
	colliders_.push_back(std::move(player_.get()));

	// enemy
	for (std::unique_ptr<Enemy>& enemy : json_->GetEnemys()) {
		enemy->SetRadius(5.0f);
		colliders_.push_back(std::move(enemy.get()));
	}

	// fixedEnemy
	for (std::unique_ptr<Enemy>& enemy : json_->GetFixedEnemys()) {
		enemy->SetRadius(5.0f);
		colliders_.push_back(std::move(enemy.get()));
	}

	// playerBullet
	for (std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
		bullet->SetRadius(5.0f);
		colliders_.push_back(std::move(bullet.get()));
	}

	// enemyBullet
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
		colliders_.push_back(std::move(bullet.get()));
	}

	// Object
	for (std::unique_ptr<Object>& objects : json_->GetObjects()) {
		colliders_.push_back(std::move(objects.get()));
	}


	std::list<Collider*>::iterator itrA = colliders_.begin();

	for (; itrA != colliders_.end(); ++itrA) {

		Collider* colliderA = *itrA;

		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;
			//当たり判定処理
			CheckCollisionPair(colliderA, colliderB);
		}
	}

	for (std::unique_ptr<Object>& objects : json_->GetObjects()) {
		for (std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
			// AABBの最小/最大座標を取得
			Vector3 AMin = bullet->GetAABBMin();
			Vector3 AMax = bullet->GetAABBMax();
			Vector3 BMin = objects->GetAABBMin();
			Vector3 BMax = objects->GetAABBMax();

			// AABBの重なりを判定
			bool isColliding =
				(AMax.x >= BMin.x && AMin.x <= BMax.x) &&
				(AMax.y >= BMin.y && AMin.y <= BMax.y) &&
				(AMax.z >= BMin.z && AMin.z <= BMax.z);

			if (isColliding) {
				bullet->OnCollision();
				objects->OnCollision();
			}
		}
	}

	for (std::unique_ptr<Object>& objects : json_->GetObjects()) {
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
			// AABBの最小/最大座標を取得
			Vector3 AMin = bullet->GetAABBMin();
			Vector3 AMax = bullet->GetAABBMax();
			Vector3 BMin = objects->GetAABBMin();
			Vector3 BMax = objects->GetAABBMax();

			// AABBの重なりを判定
			bool isColliding =
				(AMax.x >= BMin.x && AMin.x <= BMax.x) &&
				(AMax.y >= BMin.y && AMin.y <= BMax.y) &&
				(AMax.z >= BMin.z && AMin.z <= BMax.z);

			if (isColliding) {
				bullet->OnCollision();
				objects->OnCollision();
			}
		}
	}

	for (std::unique_ptr<Object>& objects : json_->GetObjects()) {
		// AABBの最小/最大座標を取得
		Vector3 AMin = player_->GetAABBMin();
		Vector3 AMax = player_->GetAABBMax();
		Vector3 BMin = objects->GetAABBMin();
		Vector3 BMax = objects->GetAABBMax();

		// AABBの重なりを判定
		bool isColliding =
			(AMax.x >= BMin.x && AMin.x <= BMax.x) &&
			(AMax.y >= BMin.y && AMin.y <= BMax.y) &&
			(AMax.z >= BMin.z && AMin.z <= BMax.z);

		if (isColliding) {
			player_->OnCollision();
			objects->OnCollision();
		}
	}

	for (std::unique_ptr<Enemy>& enemy : json_->GetEnemys()) {

		// プレイヤーの照準（レティクル）の矩形情報
		float reticleX = player_->GetReticlePos().x;
		float reticleY = player_->GetReticlePos().y;
		float reticleHalfWidth = 50.0f;
		float reticleHalfHeight = 50.0f;

		float reticleLeft = reticleX - reticleHalfWidth;
		float reticleRight = reticleX + reticleHalfWidth;
		float reticleTop = reticleY - reticleHalfHeight;
		float reticleBottom = reticleY + reticleHalfHeight;

		// 敵の矩形情報
		float enemyX = enemy->GetScreenPos().x;
		float enemyY = enemy->GetScreenPos().y;
		float enemyHalfWidth = 25.0f;
		float enemyHalfHeight = 25.0f;

		float enemyLeft = enemyX - enemyHalfWidth;
		float enemyRight = enemyX + enemyHalfWidth;
		float enemyTop = enemyY - enemyHalfHeight;
		float enemyBottom = enemyY + enemyHalfHeight;

		// 矩形同士の当たり判定（AABB）
		if (reticleLeft < enemyRight &&
			reticleRight > enemyLeft &&
			reticleTop < enemyBottom &&
			reticleBottom > enemyTop) {
			// ロックオン状態にする
			if (!enemy->GetIsLockOn()) {
				enemy->SetisLockOn(true);
			}
		}
		else {
			// ロックオン解除
			if (enemy->GetIsLockOn()) { // 状態が変わる場合のみ更新
				enemy->SetisLockOn(false);
			}
		}
	}

	for (std::unique_ptr<Enemy>& enemy : json_->GetFixedEnemys()) {
		// プレイヤーの照準（レティクル）の矩形情報
		float reticleX = player_->GetReticlePos().x;
		float reticleY = player_->GetReticlePos().y;
		float reticleHalfWidth = 50.0f;
		float reticleHalfHeight = 50.0f;

		float reticleLeft = reticleX - reticleHalfWidth;
		float reticleRight = reticleX + reticleHalfWidth;
		float reticleTop = reticleY - reticleHalfHeight;
		float reticleBottom = reticleY + reticleHalfHeight;

		// 敵の矩形情報
		float enemyX = enemy->GetScreenPos().x;
		float enemyY = enemy->GetScreenPos().y;
		float enemyHalfWidth = 25.0f;
		float enemyHalfHeight = 25.0f;

		float enemyLeft = enemyX - enemyHalfWidth;
		float enemyRight = enemyX + enemyHalfWidth;
		float enemyTop = enemyY - enemyHalfHeight;
		float enemyBottom = enemyY + enemyHalfHeight;

		// 矩形同士の当たり判定（AABB）
		if (reticleLeft < enemyRight &&
			reticleRight > enemyLeft &&
			reticleTop < enemyBottom &&
			reticleBottom > enemyTop) {
			// ロックオン状態にする
			if (!enemy->GetIsLockOn()) {
				enemy->SetisLockOn(true);
			}
		}
		else {
			// ロックオン解除
			if (enemy->GetIsLockOn()) { // 状態が変わる場合のみ更新
				enemy->SetisLockOn(false);
			}
		}
	}
}

void GameScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	// リストに登録する
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GameScene::startShake(int duration, float amplitude)
{
	isShake = true;
	shakeTimer = duration;
	shakeAmplitude = amplitude;
}

void GameScene::ShakeCamera()
{
	// 当たったらシェイク
	if (player_->GetIsHit()) {
		shakeTimer = 30;
		startShake(40, 2.0f);

		// 自キャラの衝突時コールバックを呼び出す
		noiseStrength += kdamageNoise;
		postProcess_->SetNoiseStrength(noiseStrength);
	}

	/// 更新処理
	if (isShake) {
		// ランダムな振れ幅を計算
		randX = ((rand() % 200) / 100.0f - 1.0f) * shakeAmplitude;
		randY = ((rand() % 200) / 100.0f - 1.0f) * shakeAmplitude;

		// 振幅を減衰
		shakeAmplitude *= shakeDecay;

		// タイマーの減少
		shakeTimer -= 1;

		if (shakeTimer <= 0 || shakeAmplitude < 0.1f) {
			isShake = false;
			randX = 0;
			randY = 0;
		}
	}
	else {
		randX = 0;
		randY = 0;
	}

	// カメラ位置
	camera_.cameraTransform.translate = { player_->GetPos().x + randX, player_->GetPos().y + cameraOffset.y + randY,  player_->GetPos().z - cameraOffset.z };
}

void GameScene::LockOnEnemy()
{
	// fryEnemyLockOn
	for (std::unique_ptr<Enemy>& enemy : json_->GetEnemys()) {
		if (enemy->GetIsLockOn()) {
			player_->LockOn(enemy->GetPos());
		}
		else{
			player_->Attack();
		}
	}

	// fixedEnemyLockOn
	for (std::unique_ptr<Enemy>& enemy : json_->GetFixedEnemys()) {
		if (player_->Get3DWorldPosition().z < enemy->GetPos().z &&
			enemy->GetPos().z - player_->GetPos().z <= 600.0f) {
			player_->LockOn(enemy->GetPos());
		}
	}
}

void GameScene::Start()
{

	// 画面が明るくなったらスタート演出
	if (isVignette_ == false) {
		if (isApploach_) {
			float kRotateCameraSpeed = 0.035f;
			camera_.cameraTransform.rotate.y += kRotateCameraSpeed;

			EulerTransform origin = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{player_->GetPos().x,player_->GetPos().y,player_->GetPos().z} };
			// 追従対象からカメラまでのオフセット
			Vector3 offset = { 0.0f, 1.5f, -20.0f };
			// カメラの角度から回転行列を計算する
			Matrix4x4 worldTransform = MakeRotateYMatrix(camera_.cameraTransform.rotate.y);
			// オフセットをカメラの回転に合わせて回転させる
			offset = TransformNormal(offset, worldTransform);
			// 座標をコピーしてオフセット分ずらす
			camera_.cameraTransform.translate.x = origin.translate.x + offset.x;
			camera_.cameraTransform.translate.y = origin.translate.y + offset.y;
			camera_.cameraTransform.translate.z = origin.translate.z + offset.z;

			time_ += timerSpeed;

			if (time_ >= kMaxTime) {
				time_ = 0;
				isApploach_ = false;
				postProcess_->SetBlurStrength(blurStrength_);
			}
		}
		// 演出が終わったらブラーをかけてプレイヤー発射
		else if (isApploach_ == false) {

			// カメラ位置
			camera_.cameraTransform.translate = { player_->GetPos().x + randX, player_->GetPos().y + cameraOffset.y + randY,  player_->GetPos().z - cameraOffset.z };

			// ブラー
			blurStrength_ -= minusBlurStrength_;
			if (blurStrength_ <= kDefaultBlurStrength_) {
				blurStrength_ = kDefaultBlurStrength_;
			}

			postProcess_->SetBlurStrength(blurStrength_);
		}
	}
}

void GameScene::Clear()
{
	// クリア条件
	if (player_->GetPos().z >= goalline) {
		camera_.cameraTransform.translate = { player_->GetPos().x, player_->GetPos().y + cameraOffset.y,  goalline - cameraOffset.z };

		// カメラをプレイヤーに向ける
		Vector3 end = player_->GetPos();
		Vector3 start = camera_.cameraTransform.translate;

		Vector3 diff;
		diff.x = end.x - start.x;
		diff.y = end.y - start.y;
		diff.z = end.z - start.z;

		diff = Normalize(diff);

		Vector3 velocity_(diff.x, diff.y, diff.z);

		// Y軸周り角度（Θy）
		camera_.cameraTransform.rotate.y = std::atan2(velocity_.x, velocity_.z);
		float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
		camera_.cameraTransform.rotate.x = std::atan2(-velocity_.y, velocityXZ);

		isGameClear_ = true;
	}

	if (isGameClear_) {
		postProcess_->VignetteFadeIn(0.1f, 0.1f);

		if (postProcess_->GetVignetteLight() <= 0.0f) {
			isGameClear_ = false;
			sceneNo = CLEAR;
		}
	}
}

void GameScene::Over()
{
	// ゲームオーバー条件
	if (player_->GetHP() <= 0) {
		isGameOver_ = true;
	}

	if (isGameOver_) {
		if (noiseStrength <= kMaxNoiseStrength) {
			noiseStrength += plusNoiseStrength;
		}

		if (postProcess_->GetNoiseStrength() >= kMaxNoiseStrength) {
			isGameOver_ = false;
			sceneNo = OVER;
		}
	}

	// ダメージを受けた時のノイズ設定
	postProcess_->SetNoiseStrength(noiseStrength);
}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	// 衝突フィルタリング
	if (colliderA->GetCollosionAttribute() != colliderB->GetCollisionMask() &&
		colliderB->GetCollosionAttribute() != colliderA->GetCollisionMask()) {
		return;
	}

	//当たり判定の計算開始
	Vector3 Apos = colliderA->GetWorldPosition();
	Vector3 Bpos = colliderB->GetWorldPosition();
	
	float ARadious = colliderA->GetRadius();
	float BRadious = colliderB->GetRadius();

	float a2bX = (Bpos.x - Apos.x) * (Bpos.x - Apos.x); float a2bY = (Bpos.y - Apos.y) * (Bpos.y - Apos.y);
	float a2bZ = (Bpos.z - Apos.z) * (Bpos.z - Apos.z); float L = (ARadious + BRadious) * (ARadious + BRadious);

	if (a2bX + a2bY + a2bZ <= L) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}

void GameScene::CheckAABBCollisionPair(Collider* colliderA, Collider* colliderB)
{
	// フィルタリング（同じ属性かつマスクが一致しない場合は判定をスキップ）
	if ((colliderA->GetCollosionAttribute() & colliderB->GetCollisionMask()) == 0 ||
		(colliderA->GetCollisionMask() & colliderB->GetCollosionAttribute()) == 0) {
		return;
	}

	// AABBの最小/最大座標を取得
	Vector3 AMin = colliderA->GetAABBMin();
	Vector3 AMax = colliderA->GetAABBMax();
	Vector3 BMin = colliderB->GetAABBMin();
	Vector3 BMax = colliderB->GetAABBMax();

	// AABBの重なりを判定
	bool isColliding =
		(AMax.x >= BMin.x && AMin.x <= BMax.x) &&
		(AMax.y >= BMin.y && AMin.y <= BMax.y) &&
		(AMax.z >= BMin.z && AMin.z <= BMax.z);

	if (isColliding) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}
