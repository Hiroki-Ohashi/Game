#include "RailCamera.h"
#include "Player/Player.h"

void RailCamera::Initialize()
{
	camera_.Initialize();

	// cameraInit
	EulerTransform origin = { {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{player_->GetPos().x,player_->GetPos().y,player_->GetPos().z} };
	// 追従対象からカメラまでのオフセット
	Vector3 offset = { 0.0f, 4.0f, -20.0f };
	// カメラの角度から回転行列を計算する
	Matrix4x4 worldTransform = MakeRotateYMatrix(camera_.cameraTransform.rotate.y);
	// オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, worldTransform);
	// 座標をコピーしてオフセット分ずらす
	camera_.cameraTransform.translate.x = origin.translate.x + offset.x;
	camera_.cameraTransform.translate.y = origin.translate.y + offset.y;
	camera_.cameraTransform.translate.z = origin.translate.z + offset.z;

	RotObject(player_->GetPos(), camera_.cameraTransform.translate, camera_.cameraTransform.rotate);
}

void RailCamera::Update()
{
	camera_.Update();
}

//void RailCamera::startShake(int duration, float amplitude)
//{
//}

void RailCamera::ShakeCamera()
{
	/// 更新処理
	if (isShake) {
		// ランダムな振れ幅を計算
		randX = ((rand() % 200) / 100.0f - 1.0f) * shakeAmplitude;
		randY = ((rand() % 200) / 100.0f - 1.0f) * shakeAmplitude;

		// 振幅を減衰
		shakeAmplitude *= shakeDecay;

		// タイマーの減少
		shakeTimer -= 1;

		if (shakeTimer <= 0 || shakeAmplitude < 0.1f) {
			isShake = false;
			randX = 0;
			randY = 0;
		}
	}
	else {
		randX = 0;
		randY = 0;
	}

	// カメラ位置
	camera_.cameraTransform.translate.x += randX;
	camera_.cameraTransform.translate.y += randY;
}

void RailCamera::StartCamera()
{
}

void RailCamera::ClearCamera()
{
}

void RailCamera::RotObject(Vector3 endObjectPos, Vector3 startObjectPos, Vector3 ObjectRotate)
{
	Vector3 end = endObjectPos;
	Vector3 start = startObjectPos;

	Vector3 diff;
	diff.x = end.x - start.x;
	diff.y = end.y - start.y;
	diff.z = end.z - start.z;

	diff = Normalize(diff);

	Vector3 velocity_(diff.x, diff.y, diff.z);

	// Y軸周り角度（Θy）
	ObjectRotate.y = std::atan2(velocity_.x, velocity_.z);
	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	ObjectRotate.x = std::atan2(-velocity_.y, velocityXZ);
}
