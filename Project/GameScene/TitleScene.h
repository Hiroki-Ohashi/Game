#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>

#include "Function.h"
#include "Triangle.h"
#include "Sprite.h"
#include "ImGuiManeger.h"
#include "MathFunction.h"
#include "Camera.h"
#include "Sphere.h"
#include "Model.h"
#include "TextureManager.h"
#include "imgui.h"

#include "Particle.h"
#include "IScene.h"
#include "PostProcess.h"
#include <Json.h>
#include <Skydome.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")



class TitleScene : public IScene {
public:
	~TitleScene();

	void Initialize() override;
	void Update() override;
	void Draw() override;
	void PostDraw() override;

private:
	Camera camera_;
	TextureManager* textureManager_ = TextureManager::GetInstance();

	std::unique_ptr<Sprite> title_ = nullptr;
	std::unique_ptr<Sprite> startLog_ = nullptr;

	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	std::unique_ptr<Json> json_ = nullptr;
	LevelData* levelData_ = nullptr;

	// 天球
	std::unique_ptr<Skydome> skydome_;
private:
	EulerTransform transform;

	float cameraSpeedX = 0.0001f;
	float cameraSpeedY = 0.0002f;

	uint32_t title;
	uint32_t start;
	uint32_t white;

	uint32_t timer;
	bool blinking;
};