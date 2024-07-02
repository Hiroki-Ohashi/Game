#pragma once
#include <TextureManager.h>
#include <Model.h>
#include <Input.h>
#include "PlayerBullet.h"

class Player {
public:
	~Player();

	void Initialize();
	void Update();
	void Draw(Camera* camera_, uint32_t index);

	Vector3 GetPos() { return transform_.translate; }
private:
	void Attack();
private:
	Input* input_ = Input::GetInsTance();
	WorldTransform worldtransform_;
	EulerTransform transform_;
	std::unique_ptr<Model> model_;
	std::list<PlayerBullet*> bullets_;
	float speed = 0.2f;
};