#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "MathFunction.h"
#include <Model.h>
#include <Enemy/Enemy.h>

class Player;
class GameScene;

/// <summary>
/// Josn.h
/// Json生成のソースファイル
/// </summary>

// Jsonクラス
class Json {
public:
	// Json読み込み
	LevelData* LoadJson(const std::string& fileName);
	// オブジェクト名
	void Adoption(LevelData* levelData, bool light);
	// 敵
	void EnemyAdoption(LevelData* levelData, Player* player, GameScene* gamescene);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera& camera, uint32_t index);

	// Getter
	const Camera &GetCamera() { return camera_; }
	std::vector<std::unique_ptr<Enemy>>& GetEnemys() { return enemys_; }
private:
	// model
	std::vector<std::unique_ptr<Model>> objects_;
	// enemy
	std::vector<std::unique_ptr<Enemy>> enemys_;
	Camera camera_;

	// 呼び出し
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
};