#pragma once
#include <Model.h>
#include <Camera.h>
#include <MathFunction.h>
using namespace Engine;

class PlayerUI {
public:
	void Initialize();
	void Update();
	void Draw(Camera* camera_);

	void SetUIPosition(Vector3 pos);
	void SetEaseEnd(bool isEase);
private:

	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<Model> rightModel_;
	WorldTransform rightWorldtransform_;
	EulerTransform rightTransform_;

	std::unique_ptr<Model> leftModel_;
	WorldTransform leftWorldtransform_;
	EulerTransform leftTransform_;

	// texture
	uint32_t HP;
	uint32_t operation;

	// イージング変数
	float start = 0.0f;
	float end = 1.3f;
	float frame;
	float endFrame = 20.0f;
	bool isEaseStart;
	bool isEaseEnd;
};