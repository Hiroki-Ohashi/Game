#include "RailCamera.h"
#include "Player/Player.h"

void RailCamera::GameSceneInitialize()
{
	camera_.Initialize();
	camera_.SetFovY(1.0f);

	AttentionObject({ {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{player_->GetPos().x,player_->GetPos().y,player_->GetPos().z} }, { 0.0f, 4.0f, -20.0f });
	RotObject(player_->GetPos(), camera_.cameraTransform.translate, camera_.cameraTransform.rotate);

	frame = 0;
	isFov = true;
}

void RailCamera::Initialize()
{
	camera_.Initialize();
}

void RailCamera::Update()
{
	camera_.Update();
}

void RailCamera::CameraSwing()
{
	// カメラ角度が範囲を超えたら反転
	// X軸
	if (camera_.cameraTransform.rotate.x < kCameraMax.x) {
		cameraSpeedX += cameraMoveSpeed;
	}
	else if (camera_.cameraTransform.rotate.x >= kCameraMax.x) {
		cameraSpeedX -= cameraMoveSpeed;
	}
	// Y軸
	if (camera_.cameraTransform.rotate.y < kCameraMax.y) {
		cameraSpeedY += cameraMoveSpeed;
	}
	else if (camera_.cameraTransform.rotate.y >= kCameraMax.y) {
		cameraSpeedY -= cameraMoveSpeed;
	}
	// カメラスピードを足す
	camera_.cameraTransform.rotate.x += cameraSpeedX;
	camera_.cameraTransform.rotate.y += cameraSpeedY;
}

void RailCamera::startShake()
{
	isShake = true;
	shakeTimer = 40;
	shakeAmplitude = 2.0f;
}

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
	AttentionObject({ {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{player_->GetPos().x,player_->GetPos().y,player_->GetPos().z} }, { 0.0f, 4.0f, -20.0f });
	// カメラをプレイヤーに向ける
	RotObject(player_->GetPos(), camera_.cameraTransform.translate, camera_.cameraTransform.rotate);
}

void RailCamera::AfterStartCamera()
{
	// カメラ位置
	camera_.cameraTransform.translate = { player_->GetPos().x + randX, player_->GetPos().y + cameraOffset.y + randY,  player_->GetPos().z - cameraOffset.z };

	if (isFov) {
		frame++;
		if (frame >= endFrame) {
			isFov = false;
		}
	}

	fov = start + (end - start) * EaseOutQuart(frame / endFrame);
	camera_.SetFovY(fov);

	// カメラをレティクルに向ける
	RotObject(player_->Get3DWorldPosition(), camera_.cameraTransform.translate, camera_.cameraTransform.rotate);
}

void RailCamera::ClearCamera()
{
	camera_.cameraTransform.translate = { player_->GetPos().x, player_->GetPos().y + cameraOffset.y,  98000.0f - cameraOffset.z };

	// カメラをプレイヤーに向ける
	RotObject(player_->Get3DWorldPosition(), camera_.cameraTransform.translate, camera_.cameraTransform.rotate);
}

void RailCamera::ClearCameraMove()
{
	camera_.cameraTransform.rotate.y += 0.01f;

	AttentionObject({ {0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} }, { 0.0f, 13.0f, -25.0f });

	camera_.cameraTransform.rotate.x = 0.4f;
}

void RailCamera::RotObject(Vector3 endObjectPos, Vector3 startObjectPos, Vector3 ObjectRotate)
{
	Vector3 cameraEnd = endObjectPos;
	Vector3 cameraStart = startObjectPos;

	Vector3 diff;
	diff.x = cameraEnd.x - cameraStart.x;
	diff.y = cameraEnd.y - cameraStart.y;
	diff.z = cameraEnd.z - cameraStart.z;

	diff = Normalize(diff);

	Vector3 velocity_(diff.x, diff.y, diff.z);

	// Y軸周り角度（Θy）
	ObjectRotate.y = std::atan2(velocity_.x, velocity_.z);
	float velocityXZ = sqrt((velocity_.x * velocity_.x) + (velocity_.z * velocity_.z));
	ObjectRotate.x = std::atan2(-velocity_.y, velocityXZ);
}

void RailCamera::AttentionObject(EulerTransform origin_, Vector3 offset_)
{
	// cameraInit
	EulerTransform origin = origin_;
	// 追従対象からカメラまでのオフセット
	Vector3 offset = offset_;
	// カメラの角度から回転行列を計算する
	Matrix4x4 worldTransform = MakeRotateYMatrix(camera_.cameraTransform.rotate.y);
	// オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, worldTransform);
	// 座標をコピーしてオフセット分ずらす
	camera_.cameraTransform.translate.x = origin.translate.x + offset.x;
	camera_.cameraTransform.translate.y = origin.translate.y + offset.y;
	camera_.cameraTransform.translate.z = origin.translate.z + offset.z;
}
