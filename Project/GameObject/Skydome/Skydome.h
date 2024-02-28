#pragma once
#include "Model.h"
#include "WorldTransform.h"


class Skydome {
public:
	~Skydome();

	void Initialize();

	void Update();

	void Draw(Camera* camera, uint32_t index);

private:
	// ワールド変換データ
	Transform transform;
	// モデル
	std::unique_ptr<Model> model_ = nullptr;

};
