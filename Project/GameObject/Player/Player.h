#pragma once
#include "Model.h"
#include "MathFunction.h"
#include "Camera.h"
#include "Input.h"
#include "WorldTransform.h"
#include "TextureManager.h"

#include "PlayerBullet.h"

class Player {
public:
	~Player();

	void Init(const std::string filename);
	void Update();
	void Draw(uint32_t index, Camera* camera);

	void Attack();

public:
	Model* model_;
	Input* input_ = Input::GetInsTance();
	TextureManager* textureManager_ = TextureManager::GetInstance();

	Transform transform;

	float speed = 0.02f;

	bool isAttack;

	std::list<PlayerBullet*> bullets_;

	int bulletTex;
};