#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "MathFunction.h"
#include <Model.h>

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
	// 更新処理
	void Update();
	// 描画処理
	void Draw(Camera& camera, uint32_t index);

	// Getter
	const Camera &GetCamera() { return camera_; }

private:
	// model
	std::vector<std::unique_ptr<Model>> objects_;
	Camera camera_;
};