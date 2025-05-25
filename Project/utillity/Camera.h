#pragma once
#include "Function.h"
#include "MathFunction.h"
#include "WinApp.h"

/// <summary>
/// Camera.h
/// カメラ生成のヘッダーファイル
/// </summary>

// Cameraクラス
struct Camera {
	// 初期化処理
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw();

	void SetFovY(float fov_) { fov = fov_; }

	// シングルトン呼び出し
	static WinApp* winapp_;

	// cameraPamameter
	EulerTransform cameraTransform;
	Matrix4x4 worldMatrix = {};
	Matrix4x4 cameraMatrix;
	Matrix4x4 viewMatrix;
	Matrix4x4 projectionMatrix;
	Matrix4x4 worldViewProjectionMatrix;
	Matrix4x4* transformationMatrixData;

	Matrix4x4 sMatView = {};
	Matrix4x4 sMatProjection = {};

	float fov = 0.45f;
};
