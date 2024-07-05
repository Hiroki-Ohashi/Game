#pragma once
#include "IScene.h"
#include <TextureManager.h>
#include <Camera.h>

class ClearScene : public IScene {
public:
	~ClearScene();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void PostDraw() override;

private:

	TextureManager* textureManager_ = TextureManager::GetInstance();
	Camera* camera_ = nullptr;

	EulerTransform transform;
	EulerTransform transform2;

	uint32_t uv;
};