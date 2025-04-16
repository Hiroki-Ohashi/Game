#pragma once
#include <Camera.h>

class Player;

class RailCamera {
public:
	void Initialize();
	void Update();

	// シェイクを開始する関数
	void startShake(int duration, float amplitude);
	void ShakeCamera();

	// スタート
	void StartCamera();
	void ClearCamera();

	// Getter
	Camera GetCamera() { return camera_; }
	// Setter
	void SetPlayer(Player* player) { player_ = player; }

private:
	void RotObject(Vector3 startObjectPos, Vector3 endObjectPos, Vector3 ObjectRotate);
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

	// ノイズ
	float noiseStrength;
	const float kdamageNoise = 3.0f;
	const float kMaxNoiseStrength = 100.0f;
	const float plusNoiseStrength = 1.0f;
};
