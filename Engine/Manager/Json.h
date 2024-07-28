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
	void Draw(Camera& camera, uint32_t index);

private:
	std::vector<std::unique_ptr<Model>> objects_;
};