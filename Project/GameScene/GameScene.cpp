#include "GameScene.h"
#include <time.h>
#include <limits>

using namespace Engine;

/// <summary>
/// GameScene.cpp
/// ゲームシーンのソースファイル
/// </summary>

GameScene::~GameScene(){
}

void GameScene::Initialize() {
	// PostEffect
	postProcess_ = std::make_unique<PostProcess>();
	postProcess_->Initialize(NOISE);
	postProcess_->SetVignette(0.0f, 16.0f);
	postProcess_->SetNoise(0.0f, 0.0f);
	postProcess_->SetBlurStrength(0.0f);

	// player
	player_ = std::make_unique<Player>();
	player_->Initialize();

	railCamera_ = std::make_unique<RailCamera>();
	railCamera_->SetPlayer(player_.get());
	railCamera_->GameSceneInitialize();

	// skybox
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize();

	// Texture
	go = textureManager_->Load("resources/go.png");
	ready = textureManager_->Load("resources/ready.png");
	uv = textureManager_->Load("resources/map.png");
	enemyBulletTex = textureManager_->Load("resources/yellow.png");
	bossBulletTex = textureManager_->Load("resources/red.png");
	backTitle = textureManager_->Load("resources/backTitle.png");
	retry = textureManager_->Load("resources/retry.png");
	kemuri = textureManager_->Load("resources/kemuri.png");

	// UI(ready)
	ready_ = std::make_unique<Sprite>();
	ready_->Initialize(Vector2{ 340.0f, 0.0f }, Vector2{ 600.0f, 300.0f }, ready);
	ready_->SetSize({ 600.0f, 300.0f });

	// UI(go)
	transform_ = { {10.0f,10.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,30.0f,-100.0f} };
	go_ = std::make_unique<Model>();
	go_->Initialize("board.obj", transform_);
	go_->SetLight(false);

	// UI(sentaku)
	sentaku_ = std::make_unique<Sprite>();
	sentaku_->Initialize(Vector2{ 1050.0f, 560.0f }, Vector2{ 127.0f, 107.0f }, retry);
	sentaku_->SetSize({ 127.0f, 107.0f });

	// Json
    json_ = std::make_unique<Json>();
	levelData_ = json_->LoadJson("stage");
	json_->Adoption(levelData_, true);
	json_->EnemyAdoption(levelData_, player_.get(), this);
	//json_->FixedEnemyAdoption(levelData_, player_.get(), this);

	for (std::unique_ptr<FryEnemy>& enemy : json_->GetEnemys()) {
		enemy->SetIsDead(false);
	}

	// stage
	stage_ = std::make_unique<Stage>();
	stage_->Initialize();

	// boolInit
	isVignette_ = true;
	isGameClear_ = false;
	isApploach_ = true;
	isGameOver_ = false;
	isPose_ = false;
	isGoal_ = false;

	// EffectInit
	blurStrength_ = 0.3f;
	noiseStrength = 0.0f;
}

void GameScene::Update(){

	railCamera_->Update();

	postProcess_->NoiseUpdate(0.1f);

	stage_->Update();

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(joyState)) {
		bool currentBackButtonState = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_START);

		// ボタンが前フレームで押されておらず、今フレームで押されたらトグル
		if (!prevBackButtonState_ && currentBackButtonState) {
			isPose_ = !isPose_;
		}

		// 現在のボタン状態を次のフレームに持ち越す
		prevBackButtonState_ = currentBackButtonState;
	}

	if (isPose_) {
		// ポーズメニュー
		Pose(joyState);
	}
	else {
		postProcess_->SetLineStrength(0.0f);

		// json更新処理
		json_->Update();
		json_->EnemyUpdate(*railCamera_->GetCamera(),player_->GetPos().z);

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
			player_->Update(railCamera_->GetCamera());
			railCamera_->SetPlayer(player_.get());
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
	skydome_->Draw(railCamera_->GetCamera());
	// 床描画
	stage_->Draw(railCamera_->GetCamera());
	// 敵、ステージ描画
	json_->Draw(railCamera_->GetCamera(), uv);
	// プレイヤーの弾描画
	player_->BulletDraw(railCamera_->GetCamera());

	// 敵弾描画
	for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
		bullet->Draw(railCamera_->GetCamera(), enemyBulletTex, kemuri);
	}

	// プレイヤー描画
	player_->Draw(railCamera_->GetCamera());

	if (isApploach_) {
		// readyUI描画
		ready_->Draw();
	}
	else {
		// goUI描画
		go_->Draw(railCamera_->GetCamera(), go);

		if (isGameClear_ == false) {
			// プレイヤーUI描画
			player_->DrawUI();
		}
	}

	json_->DrawEnemy(railCamera_->GetCamera());

	if (isPose_) {
		sentaku_->Draw();
	}
}

void GameScene::PostDraw()
{
	postProcess_->NoiseDraw();
}

void GameScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	// リストに登録する
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GameScene::ShakeCamera()
{
	// 当たったらシェイク
	if (player_->GetIsHit()) {
		railCamera_->startShake();

		// 自キャラの衝突時コールバックを呼び出す
		isNoise = true;
		noiseStrength += kdamageNoise;

	}

	if (isNoise) {
		noiseStrength -= 0.5f;
		if (noiseStrength <= 0.0f) {
			isNoise = false;
		}

		postProcess_->SetNoiseStrength(noiseStrength);
	}

	railCamera_->ShakeCamera();
}

void GameScene::LockOnEnemy()
{
	Vector3 playerPos = player_->GetPos(); // プレイヤーの位置
	FryEnemy* nearestEnemy = nullptr;
	float nearestDistSq = std::numeric_limits<float>::max(); // 最小距離の初期値（大きな数）
	bool hasLockOnTarget = false; // ロックオン対象がいるかどうか

	// 一番近いロックオン対象を探す
	for (const std::unique_ptr<FryEnemy>& enemy : json_->GetEnemys()) {
		if (player_->GetPos().z < enemy->GetPos().z &&
			enemy->GetPos().z - player_->GetPos().z <= 2000.0f) {
			if (!enemy->IsDead() && enemy->GetIsLockOn()) { // ロックオンされた敵のみ対象にする
				Vector3 enemyPos = enemy->GetPos();

				Vector3 diff;
				diff.x = enemyPos.x - playerPos.x;
				diff.y = enemyPos.y - playerPos.y;
				diff.z = enemyPos.z - playerPos.z;

				float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
				if (distSq < nearestDistSq) {
					nearestDistSq = distSq;
					nearestEnemy = enemy.get(); // 最も近いロックオン敵を設定
					hasLockOnTarget = true;
				}
			}
		}
	}

	XINPUT_STATE joyState;
	// 入力状態の取得
	if (Input::GetInstance()->GetJoystickState(joyState)) {
		if (Input::GetInstance()->PressedButton(joyState, XINPUT_GAMEPAD_A)) {

			// ロックオンされている敵がいればロックオン、いなければ攻撃
			if (hasLockOnTarget && nearestEnemy) {
				player_->LockOn(nearestEnemy->GetPos(), nearestEnemy->GetPrePos());
			}
			else {
				player_->Attack(); // ロックオンされていない場合は攻撃
			}
		}
	}
}

void GameScene::Pose(XINPUT_STATE joyState_)
{
	postProcess_->SetNoise(0.2f, 1.0f);

	// 十字キーでシーン選択
	if (Input::GetInstance()->GetJoystickState(joyState_)) {
		// 長押し防止
		if (scenePrev == 0) {
			if (Input::GetInstance()->PressedButton(joyState_, XINPUT_GAMEPAD_DPAD_DOWN)) {
				scenePrev = 1;
				sentaku_->SetTexture(backTitle);
			}
		}
		else if (scenePrev == 1) {
			if (Input::GetInstance()->PressedButton(joyState_, XINPUT_GAMEPAD_DPAD_UP)) {
				scenePrev = 0;
				sentaku_->SetTexture(retry);
			}
		}

		// 選んだシーンをAボタンで遷移開始
		if (scenePrev == 0) {
			if (Input::GetInstance()->PressedButton(joyState_, XINPUT_GAMEPAD_A)) {
				isPose_ = false;
			}
		}
		else if (scenePrev == 1) {
			if (Input::GetInstance()->PressedButton(joyState_, XINPUT_GAMEPAD_A)) {
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

void GameScene::Start()
{

	// 画面が明るくなったらスタート演出
	if (isVignette_ == false) {
		if (isApploach_) {

			frame++;
			if (frame >= endFrame) {
				frame = 0;
				isApploach_ = false;
				postProcess_->SetBlurStrength(blurStrength_);
			}

			float cameraRot = start + (end - start) * EaseOutQuart(frame / endFrame);
			railCamera_->SetRot({ railCamera_->GetCameraRot().x, cameraRot, railCamera_->GetCameraRot().z });
			railCamera_->StartCamera();
		}
		// 演出が終わったらブラーをかけてプレイヤー発射
		else{

			railCamera_->AfterStartCamera();

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

		isGoal_ = true;
		player_->SetGoalLine(isGoal_);

		railCamera_->ClearCamera();

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
	for (std::unique_ptr<FryEnemy>& enemy : json_->GetEnemys()) {
		enemy->SetRadius(5.0f);
		if (!enemy->IsDead()) {
			colliders_.push_back(std::move(enemy.get()));
		}
	}

	// fixedEnemy
	for (std::unique_ptr<FixedEnemy>& enemy : json_->GetFixedEnemys()) {
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

	// 球判定
	CheckSphereCollisions(colliders_);

	// AABB判定
	CheckAABBCollisionsWithObjects();

	// ロックオン（2D矩形判定）
	CheckReticleLockOn();

	// OBB判定
	CheckOBBCollisionsWithPlayer();
}

void GameScene::CheckSphereCollisions(std::list<Collider*>& colliders) {
	// 球同士の衝突チェック
	std::list<Collider*>::iterator itrA = colliders.begin();

	for (; itrA != colliders.end(); ++itrA) {

		Collider* colliderA = *itrA;

		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders.end(); ++itrB) {
			Collider* colliderB = *itrB;
			//当たり判定処理
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void GameScene::CheckAABBCollisionsWithObjects() {
	// AABBによる衝突チェック（弾 vs オブジェクト）
	for (std::unique_ptr<Object>& obj : json_->GetObjects()) {
		for (std::unique_ptr<PlayerBullet>& bullet : player_->GetBullets()) {
			CheckAABBCollisionPair(bullet.get(), obj.get());
		}
		for (std::unique_ptr<EnemyBullet>& bullet : enemyBullets_) {
			CheckAABBCollisionPair(bullet.get(), obj.get());
		}
	}
}

void GameScene::CheckReticleLockOn() {
	// ロックオン矩形領域との衝突チェック
	const float reticleX = player_->GetReticlePos().x;
	const float reticleY = player_->GetReticlePos().y;
	const float left = reticleX - reticleHalfWidth;
	const float right = reticleX + reticleHalfWidth;
	const float top = reticleY - reticleHalfWidth;
	const float bottom = reticleY + reticleHalfWidth;

	auto checkLock = [&](std::unique_ptr<FryEnemy>& enemy) {
		const float ex = enemy->GetScreenPos().x;
		const float ey = enemy->GetScreenPos().y;
		const float eLeft = ex - enemyHalfWidth;
		const float eRight = ex + enemyHalfWidth;
		const float eTop = ey - enemyHalfWidth;
		const float eBottom = ey + enemyHalfWidth;

		bool isHit = (left < eRight && right > eLeft && top < eBottom && bottom > eTop);
		enemy->SetisLockOn(isHit);
		};

	for (auto& enemy : json_->GetEnemys()) {
		checkLock(enemy);
	}

	/*for (auto& enemy : json_->GetFixedEnemys()) {
		checkLock(enemy);
	}*/
}

void GameScene::CheckOBBCollisionsWithPlayer() {
	// プレイヤーのOBBとオブジェクトのOBB衝突チェック
	const OBB playerOBB = player_->GetOBB();

	for (auto& obj : json_->GetObjects()) {
		if (IsOBBColliding(playerOBB, obj->GetOBB())) {
			player_->OnCollision();
			obj->OnCollision();
		}
	}
}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB)
{
	// 同一オブジェクトは判定不要
	if (colliderA == colliderB) return;


	// 衝突フィルタリング
	if ((colliderA->GetCollosionAttribute() & colliderB->GetCollisionMask()) == 0 ||
		(colliderA->GetCollisionMask() & colliderB->GetCollosionAttribute()) == 0) {
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

bool GameScene::IsOBBColliding(const OBB& a, const OBB& b) {
	const float EPSILON = 1e-5f;

	// 各軸：aの3軸 + bの3軸 + a×bの交差軸 = 15本
	Vector3 axis[15];

	// aのローカル軸
	for (int i = 0; i < 3; i++) {
		axis[i] = a.axis[i];
	}

	// bのローカル軸
	for (int i = 0; i < 3; i++) {
		axis[i + 3] = b.axis[i];
	}

	// a×b の交差軸
	int index = 6;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			axis[index++] = Normalize(Cross(a.axis[i], b.axis[j]));
		}
	}

	// 中心間ベクトル
	Vector3 t;
	t.x = b.center.x - a.center.x;
	t.y = b.center.y - a.center.y;
	t.z = b.center.z - a.center.z;

	// 全軸で投影をチェック
	for (int i = 0; i < 15; i++) {
		const Vector3& L = axis[i];
		if (Length(L) < EPSILON) continue; // 無効軸スキップ

		// aの投影幅
		float ra =
			std::abs(Dot(a.axis[0], L)) * a.halfSize.x +
			std::abs(Dot(a.axis[1], L)) * a.halfSize.y +
			std::abs(Dot(a.axis[2], L)) * a.halfSize.z;

		// bの投影幅
		float rb =
			std::abs(Dot(b.axis[0], L)) * b.halfSize.x +
			std::abs(Dot(b.axis[1], L)) * b.halfSize.y +
			std::abs(Dot(b.axis[2], L)) * b.halfSize.z;

		// 中心差の投影
		float dist = std::abs(Dot(t, L));

		if (dist > ra + rb) {
			// この軸で分離がある → 衝突してない
			return false;
		}
	}

	// 全軸で分離がなかった → 衝突している
	return true;
}
