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
#include "Animation.h"

#include "Particle.h"
#include "IScene.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")


class GameScene : public IScene {
public:
	~GameScene();

	// 初期化
	void Initialize() override;
	// 読み込み
	void Update() override;
	// 描画
	void Draw() override;
	// 解放
	void Release();

private:

	// 三角形の数
	static const int Max = 2;

	TextureManager* textureManager_ = nullptr;
	Camera* camera_ = nullptr;

	std::unique_ptr<Sphere> sphere_;

	//std::unique_ptr<Model> model_;
	//std::unique_ptr<Model> model2_;

	std::unique_ptr<AnimationModel> model_;
	std::unique_ptr<AnimationModel> model2_;
	std::unique_ptr<AnimationModel> model3_;

	std::unique_ptr<Particles> particle_ = nullptr;
	std::unique_ptr<Particles> particle2_ = nullptr;

	CameraForGpu camera;

	EulerTransform transform;
	EulerTransform transform2;
	EulerTransform transform3;

	Vector3 pos = { -2.0f, 0.0f, 10.0f };
	Vector3 pos2 = { 2.0f, 0.0f, 10.0f };

	uint32_t monsterBall;
	uint32_t moon;
	uint32_t uv;
	uint32_t kusa;
	uint32_t circle;
	uint32_t utillity;
	uint32_t white;
};