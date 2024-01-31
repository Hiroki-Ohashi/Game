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

#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"

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

	void CheckAllCollisions();

private:

	// 三角形の数
	static const int Max = 2;

	Input* input_ = nullptr;
	TextureManager* textureManager_ = nullptr;
	Camera* camera_ = nullptr;

	Player* player_ = nullptr;
	Enemy* enemy_ = nullptr;
	Skydome* skydome_ = nullptr;

	Sprite* title_ = nullptr;
	Sprite* kakusi_ = nullptr;
	Sprite* ui_ = nullptr;

	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;

	int uv;
	int kuro;
	int teki;
	int ui;
	int sky;

	int scene;

	float a;
	float b;

	bool isAAA;
	bool isTitle;

	float speed = 0.01f;
	float speed2 = 0.01f;

};
