#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <json.hpp>
#include "MathFunction.h"
#include <Model.h>

class Json {
public:
	LevelData* LoadJson(const std::string& fileName);
	void Adoption(LevelData* levelData);
	void Update();
	void Draw(Camera& camera, uint32_t index);

	const Camera &GetCamera() { return camera_; }

private:
	std::vector<std::unique_ptr<Model>> objects_;
	Camera camera_;
};