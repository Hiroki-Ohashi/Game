#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "MathFunction.h"
#include <Model.h>
#include <Stage/Object.h>
#include <Enemy/FryEnemy/FryEnemy.h>
#include <Enemy/FixedEnemy/FixedEnemy.h>

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
	void EnemyUpdate(Camera& camera,float distance);
	void FixedEnemyUpdate(Camera& camera, float distance);
	// 描画処理
	void Draw(Camera* camera, uint32_t index);
	void DrawEnemy(Camera* camera);

	// Getter
	const Camera &GetCamera() { return camera_; }
	std::vector<std::unique_ptr<FryEnemy>>& GetEnemys() { return enemys_; }
	std::vector<std::unique_ptr<FixedEnemy>>& GetFixedEnemys() { return fixedEnemys_; }
	std::vector<std::unique_ptr<Object>>& GetObjects() { return objects_; }

private:
	// model
	std::vector<std::unique_ptr<Object>> objects_;
	// enemy
	std::vector<std::unique_ptr<FryEnemy>> enemys_;
	std::vector<std::unique_ptr<FixedEnemy>> fixedEnemys_;
	Camera camera_;

	// 呼び出し
	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr;
};