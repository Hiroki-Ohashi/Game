#pragma once
#include <Camera.h>

class Player;

class RailCamera {
public:
	// 初期化処理
	void GameSceneInitialize();
	void Initialize();

	// 更新処理
	void Update();

	// カメラの揺れ
	void CameraSwing();

	// シェイクを開始する関数
	void startShake();
	void ShakeCamera();

	// スタート
	void StartCamera();
	void AfterStartCamera();
	// クリア
	void ClearCamera();
	void ClearCameraMove();

	// Getter
	Camera* GetCamera() { return &camera_; }
	Vector3 GetCameraPos() { return camera_.cameraTransform.translate; }
	Vector3 GetCameraRot() { return camera_.cameraTransform.rotate; }
	// Setter
	void SetPlayer(Player* player) { player_ = player; }
	void SetPos(Vector3 cameraPos) { camera_.cameraTransform.translate = cameraPos; }
	void SetRot(Vector3 cameraRot) { camera_.cameraTransform.rotate = cameraRot; }
	void SetGoalLine(float line) { goalLine = line; }
	void SetkCameraMax(Vector2 max_) { kCameraMax = max_; }

private:
	void RotObject(Vector3 startObjectPos, Vector3 endObjectPos, Vector3 ObjectRotate);
	void AttentionObject(EulerTransform origin_, Vector3 offset_);
private:
	// カメラ
	Camera camera_;
	// 呼び出し
	Player* player_ = nullptr;

	// カメラoffset
	Vector3 cameraOffset = { 0.0f,4.0f, 10.0f };

	// カメラシェイク変数
	bool isShake = false;
	int shakeTimer = 0;
	float shakeAmplitude = 1.0f; // 揺れの最大振幅
	float shakeDecay = 0.2f;    // 揺れの減衰率
	float randX = 0.0f;
	float randY = 0.0f;

	// カメライージング変数
	float start = 0.45f;
	float end = 1.2f;
	float frame;
	float endFrame =30.0f;
	float fov = 1.0f;
	bool isFov = false;

	// ゴールライン
	float goalLine;

	// cameraSpeed
	float cameraSpeedX = 0.0001f;
	float cameraSpeedY = 0.0002f;
	float cameraMoveSpeed = 0.000005f;
	Vector2 kCameraMax;
};
