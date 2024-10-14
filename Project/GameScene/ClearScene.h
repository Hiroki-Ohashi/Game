#pragma once
#include "IScene.h"
#include <TextureManager.h>
#include <Camera.h>
#include <Sprite.h>
#include <PostProcess.h>
#include <Skydome.h>
#include <Json.h>

class ClearScene : public IScene {
public:
	~ClearScene();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void PostDraw() override;

private:
	Camera camera_;
	TextureManager* textureManager_ = TextureManager::GetInstance();

	std::unique_ptr<Sprite> clear_ = nullptr;
	std::unique_ptr<Sprite> clearLog_ = nullptr;
	std::unique_ptr<Sprite> log_ = nullptr;

	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	// 天球
	std::unique_ptr<Skydome> skydome_;

	std::unique_ptr<Json> json_ = nullptr;
	LevelData* levelData_ = nullptr;

	EulerTransform transform;

	uint32_t clear;
	uint32_t clearLog;
	uint32_t log;
	uint32_t player;

	uint32_t timer;
	bool blinking;

	bool isVignette_;
	float noiseStrength = 0.0f;
};