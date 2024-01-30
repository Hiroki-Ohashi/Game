#include "PlayerBullet.h"

PlayerBullet::~PlayerBullet(){
	delete model_;
}

void PlayerBullet::Init(Vector3 pos)
{
	transform = { { 0.05f,0.05f,0.3f},{0.0f,0.0f,0.0f},{pos.x,pos.y,pos.z} };

	model_ = new Model();
	model_->Initialize("cube.obj", transform);

}

void PlayerBullet::Update()
{

	model_->worldTransform_.translate.z += 1.0f;

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(Camera* camera, uint32_t index)
{
	model_->Draw(camera, index);
}