#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "MathFunction.h"
#include <Model.h>
#include <Stage/Object.h>
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
	void FixedEnemyAdoption(LevelData* levelData, Player* player, GameScene* gamescene);
	// 更新処理
	void Update();
	void EnemyUpdate(Camera& camera, Player* player, GameScene* gamescene);
	void FixedEnemyUpdate(Camera& camera, Player* player, GameScene* gamescene);
	// 描画処理
	void Draw(Camera& camera, uint32_t index);
	void DrawEnemy(Camera& camera);

	// Getter
	const Camera &GetCamera() { return camera_; }
	std::vector<std::unique_ptr<Enemy>>& GetEnemys() { return enemys_; }
	std::vector<std::unique_ptr<Enemy>>& GetFixedEnemys() { return fixedEnemys_; }
	std::vector<std::unique_ptr<Object>>& GetObjects() { return objects_; }

private:
	// model
	std::vector<std::unique_ptr<Object>> objects_;
	// enemy
	std::vector<std::unique_ptr<Enemy>> enemys_;
	std::vector<std::unique_ptr<Enemy>> fixedEnemys_;
	Camera camera_;

	// 呼び出し
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
};