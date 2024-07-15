#include "Boss.h"

void Boss::Initialize(Vector3 pos)
{
	transform_ = { {15.0f,15.0f,15.0f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = std::make_unique<Model>();
	model_->Initialize("cube.obj", transform_);

	worldtransform_.scale = transform_.scale;
	worldtransform_.rotate = transform_.rotate;
	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();

	isDead_ = false;

	hp_ = 10;

	enemyTex = textureManager_->Load("resources/black.png");
}

void Boss::Update()
{
	transform_.translate.z += 0.5f;

	if (hp_ <= 0) {
		isDead_ = true;
	}

	worldtransform_.translate = transform_.translate;
	worldtransform_.UpdateMatrix();
	model_->SetWorldTransform(worldtransform_);
}

void Boss::Draw(Camera* camera)
{
	if (isDead_ == false) {
		model_->Draw(camera, enemyTex);
	}
}
