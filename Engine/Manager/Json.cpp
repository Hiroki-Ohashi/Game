#include "Json.h"
#include <numbers>
#include "Player/Player.h"
#include "GameScene.h"

/// <summary>
/// Json.cpp
/// Json生成のソースファイル
/// </summary>

LevelData* Json::LoadJson(const std::string& fileName)
{
	//camera_.Initialize();

	// 連結してフルパスを得る
	const std::string fullpath = "resources/Json/" + fileName + ".json";

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から回答したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	// レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();

	// "object"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		// 種別を取得
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {
			// 要素追加
			levelData->objects.emplace_back(LevelData::ObjectData{});
			// 今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects.back();

			if (object.contains("file_name")) {
				// ファイル名
				objectData.filename = object["file_name"];
			}

			if (object.contains("colliderSize")) {
				objectData.colliderSize.x = object["colliderSize"][0].get<float>();
				objectData.colliderSize.y = object["colliderSize"][2].get<float>();
				objectData.colliderSize.z = object["colliderSize"][1].get<float>();
			}

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// Translation
			objectData.translation.x = transform["translation"][0].get<float>();
			objectData.translation.y = transform["translation"][2].get<float>();
			objectData.translation.z = transform["translation"][1].get<float>();
			// Rotation
			objectData.rotation.x = transform["rotation"][0].get<float>();
			objectData.rotation.y = (transform["rotation"][2].get<float>() - std::numbers::pi_v<float>);
			objectData.rotation.z = transform["rotation"][1].get<float>();
			// Scaling
			objectData.scaling.x = transform["scaling"][0].get<float>();
			objectData.scaling.y = transform["scaling"][2].get<float>();
			objectData.scaling.z = transform["scaling"][1].get<float>();
		}

		// CAMERA
		if (type.compare("CAMERA") == 0) {

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// Translation
			camera_.cameraTransform.translate.x = transform["translation"][0].get<float>();
			camera_.cameraTransform.translate.y = transform["translation"][2].get<float>();
			camera_.cameraTransform.translate.z = transform["translation"][1].get<float>();
			// Rotation
			camera_.cameraTransform.rotate.x = -(transform["rotation"][0].get<float>() - std::numbers::pi_v<float> / 2.0f);
			camera_.cameraTransform.rotate.y = -transform["rotation"][2].get<float>();
			camera_.cameraTransform.rotate.z = -transform["rotation"][1].get<float>();
			// Scaling
			camera_.cameraTransform.scale.x = transform["scaling"][0].get<float>();
			camera_.cameraTransform.scale.y = transform["scaling"][2].get<float>();
			camera_.cameraTransform.scale.z = transform["scaling"][1].get<float>();
		}

		// Enemy
		if (type.compare("ENEMY") == 0) {
			// 要素追加
			levelData->enemys.emplace_back(LevelData::EnemyData{});
			// 今追加した要素の参照を得る
			LevelData::EnemyData& EnemyData = levelData->enemys.back();

			if (object.contains("file_name")) {
				// ファイル名
				EnemyData.filename = object["file_name"];
			}

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// Translation
			EnemyData.translation.x = transform["translation"][0].get<float>();
			EnemyData.translation.y = transform["translation"][2].get<float>();
			EnemyData.translation.z = transform["translation"][1].get<float>();
			// Rotation
			EnemyData.rotation.x = transform["rotation"][0].get<float>();
			EnemyData.rotation.y = (transform["rotation"][2].get<float>() - std::numbers::pi_v<float>);
			EnemyData.rotation.z = transform["rotation"][1].get<float>();
			// Scaling
			EnemyData.scaling.x = transform["scaling"][0].get<float>();
			EnemyData.scaling.y = transform["scaling"][2].get<float>();
			EnemyData.scaling.z = transform["scaling"][1].get<float>();
		}

		// TODO: オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		if (object.contains("children")) {
		}

		// FixedEnemy
		if (type.compare("FIXED") == 0) {
			// 要素追加
			levelData->fixedEnemys.emplace_back(LevelData::FixedData{});
			// 今追加した要素の参照を得る
			LevelData::FixedData& FixedEnemyData = levelData->fixedEnemys.back();

			if (object.contains("file_name")) {
				// ファイル名
				FixedEnemyData.filename = object["file_name"];
			}

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// Translation
			FixedEnemyData.translation.x = transform["translation"][0].get<float>();
			FixedEnemyData.translation.y = transform["translation"][2].get<float>();
			FixedEnemyData.translation.z = transform["translation"][1].get<float>();
			// Rotation
			FixedEnemyData.rotation.x = transform["rotation"][0].get<float>();
			FixedEnemyData.rotation.y = (transform["rotation"][2].get<float>() - std::numbers::pi_v<float>);
			FixedEnemyData.rotation.z = transform["rotation"][1].get<float>();
			// Scaling
			FixedEnemyData.scaling.x = transform["scaling"][0].get<float>();
			FixedEnemyData.scaling.y = transform["scaling"][2].get<float>();
			FixedEnemyData.scaling.z = transform["scaling"][1].get<float>();
		}

		// TODO: オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		if (object.contains("children")) {
		}
	}

	return levelData;
}

void Json::Adoption(LevelData* levelData, bool light)
{
	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		// モデルを指定して3Dオブジェクトを生成
		std::unique_ptr<Object> newObject = std::make_unique<Object>();
		newObject->Initialize(objectData.filename + ".obj", { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} });
		newObject->SetPosition(objectData.translation);
		newObject->SetRotation(objectData.rotation);
		newObject->SetScale(objectData.scaling);
		newObject->SetLight(light);
		objects_.push_back(std::move(newObject));
	}
}

void Json::EnemyAdoption(LevelData* levelData, Player* player, GameScene* gamescene)
{
	// レベルデータからオブジェクトを生成、配置
	for (auto& EnemyData : levelData->enemys) {
		// モデルを指定してEnemyを生成
		std::unique_ptr<Enemy> newObject = std::make_unique<Enemy>();
		newObject->Initialize({ 0.0f,0.0f,0.0f }, FRY);
		newObject->SetPosition(EnemyData.translation);
		newObject->SetRotation(EnemyData.rotation);
		newObject->SetScale(Vector3{ 1.0f, 1.0f, 1.0f });
		// 敵キャラに自キャラのアドレスを渡す
		newObject->SetPlayer(player);
		// 敵キャラにゲームシーンを渡す
		newObject->SetGameScene(gamescene);
		newObject->SetIsDead(false);
		enemys_.push_back(std::move(newObject));
	}
}

void Json::FixedEnemyAdoption(LevelData* levelData, Player* player, GameScene* gamescene)
{
	// レベルデータからオブジェクトを生成、配置
	for (auto& FixedEnemyData : levelData->fixedEnemys) {
		// モデルを指定してEnemyを生成
		std::unique_ptr<Enemy> newObject = std::make_unique<Enemy>();
		newObject->Initialize({ 0.0f,0.0f,0.0f }, FIXEDENEMY);
		newObject->SetPosition(FixedEnemyData.translation);
		newObject->SetRotation(FixedEnemyData.rotation);
		// 敵キャラに自キャラのアドレスを渡す
		newObject->SetPlayer(player);
		// 敵キャラにゲームシーンを渡す
		newObject->SetGameScene(gamescene);
		newObject->SetIsDead(false);
		fixedEnemys_.push_back(std::move(newObject));
	}
}

void Json::Update()
{
	//camera_.Update();

	for (std::unique_ptr<Object>& object : objects_) {
		object->Update();
	}
}

void Json::EnemyUpdate(Camera& camera, Player* player, GameScene* gamescene)
{
	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		// 敵キャラに自キャラのアドレスを渡す
		enemy->SetPlayer(player);
		// 敵キャラにゲームシーンを渡す
		enemy->SetGameScene(gamescene);
		enemy->Update(FRY, &camera);
	}
}

void Json::FixedEnemyUpdate(Camera& camera, Player* player, GameScene* gamescene)
{
	for (std::unique_ptr<Enemy>& fixedEnemy : fixedEnemys_) {
		// 敵キャラに自キャラのアドレスを渡す
		fixedEnemy->SetPlayer(player);
		// 敵キャラにゲームシーンを渡す
		fixedEnemy->SetGameScene(gamescene);
		fixedEnemy->Update(FIXEDENEMY, &camera);
	}
}

void Json::Draw(Camera& camera, uint32_t index)
{
	for (std::unique_ptr<Object>& object : objects_) {
		object->Draw(&camera, index);
	}

	for (std::unique_ptr<Enemy>& enemy : enemys_) {
		enemy->Draw(&camera);
		enemy->DrawUI();
	}

	for (std::unique_ptr<Enemy>& fixedEnemy : fixedEnemys_) {
		fixedEnemy->Draw(&camera);
		fixedEnemy->DrawUI();
	}
}
