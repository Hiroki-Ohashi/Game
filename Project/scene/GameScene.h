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
#include "Input.h"
#include "imgui.h"

#include "Particle.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")


class GameScene {
public:
	~GameScene();

	// 初期化
	void Initialize();
	// 読み込み
	void Update();
	// 描画
	void Draw();
	// 解放
	void Release();

private:

	// 三角形の数
	static const int Max = 2;

	Input* input_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	Camera* camera_ = nullptr;

	Triangle* triangle_[Max];
	Sprite* sprite_ = nullptr;
	Sphere* sphere_ = nullptr;
	Model* model_ = nullptr;
	Particles* particle_ = nullptr;

	CameraForGpu camera;
	Transform transform;

	uint32_t monsterBall;
	uint32_t moon;
	uint32_t uv;
	uint32_t kusa;
	uint32_t circle;
};
