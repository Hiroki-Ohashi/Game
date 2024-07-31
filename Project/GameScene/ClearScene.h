#pragma once
#include "IScene.h"
#include <TextureManager.h>
#include <Camera.h>
#include <Sprite.h>
#include <PostProcess.h>

class ClearScene : public IScene {
public:
	~ClearScene();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void PostDraw() override;

private:

	TextureManager* textureManager_ = TextureManager::GetInstance();

	std::unique_ptr<Sprite> title_ = nullptr;

	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	EulerTransform transform;

	uint32_t title;

	bool isFade;
};