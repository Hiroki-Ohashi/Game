#include "Json.h"
#include <numbers>

LevelData* Json::LoadJson(const std::string& fileName)
{
	camera_.Initialize();

	// 連結してフルパスを得る
	const std::string fullpath = "resources/" + fileName + ".json";

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

			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// Translation
			objectData.translation.x = transform["translation"][0].get<float>();
			objectData.translation.y = transform["translation"][2].get<float>();
			objectData.translation.z = transform["translation"][1].get<float>();
			// Rotation
			objectData.rotation.x = transform["rotation"][0].get<float>();
			objectData.rotation.y = (transform["rotation"][2].get<float>() + std::numbers::pi_v<float>);
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
		

		//}

		// TODO: オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		if (object.contains("children")) {

		}
	}

	return levelData;
}

void Json::Adoption(LevelData* levelData)
{
	// レベルデータからオブジェクトを生成、配置
	for (auto& objectData : levelData->objects) {
		// モデルを指定して3Dオブジェクトを生成
		std::unique_ptr<Model> newObject = std::make_unique<Model>();
		newObject->Initialize(objectData.filename + ".obj", { { 1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} });
		newObject->SetPosition(objectData.translation);
		newObject->SetRotation(objectData.rotation);
		newObject->SetScale(objectData.scaling);
		objects_.push_back(std::move(newObject));
	}
}

void Json::Update()
{
	camera_.Update();
}

void Json::Draw(Camera& camera, uint32_t index)
{
	for (auto& object : objects_) {
		object->Draw(&camera, index);
	}
}