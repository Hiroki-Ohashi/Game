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
#include <PostProcess.h>
#include "Animation.h"
#include "SkyBox.h"

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

	void PostDraw() override;
  
	// 解放
	void Release();

private:
	TextureManager* textureManager_ = nullptr;
	Camera* camera_ = nullptr;
  
	std::unique_ptr<AnimationModel> model_;

	std::unique_ptr<PostProcess> postProcess_ = nullptr;

	EulerTransform transform;

	uint32_t uv;
};
