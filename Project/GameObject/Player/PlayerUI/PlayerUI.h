#pragma once
#include <Model.h>
#include <Camera.h>
#include <MathFunction.h>
using namespace Engine;

class PlayerUI {
public:
	void Initialize();
	void Update(Camera* camera_);
	void Draw(Camera* camera_, int32_t hp);

	void SetUIPosition(Vector3 pos);
	void SetEaseEnd(bool isEase);

	// カメラの方向を向くように回転調整
	void RotObject(Vector3 startObjectPos, Vector3 endObjectPos, Vector3 ObjectRotate);
private:

	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<Model> rightModel_;
	WorldTransform rightWorldtransform_;
	EulerTransform rightTransform_;

	std::unique_ptr<Model> leftModel_;
	WorldTransform leftWorldtransform_;
	EulerTransform leftTransform_;

	// texture
	uint32_t HP100;
	uint32_t HP80;
	uint32_t HP60;
	uint32_t HP40;
	uint32_t HP20;
	uint32_t HP0;

	uint32_t operation;

	// イージング変数
	float start = 0.0f;
	float end = 1.3f;
	float frame;
	float endFrame = 60.0f;
	bool isEaseStart;
	bool isEaseEnd;
};