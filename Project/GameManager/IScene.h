#pragma once
#include "Input.h"

// シーン名
enum SCENE {
	TITLE,
	STAGE,
	CLEAR,
	OVER
};

/// <summary>
/// IScene.h
/// 現在Sceneのヘッダーファイル
/// </summary>

// ISceneクラス
class IScene {
protected:
	// シーン番号
	static int sceneNo;
	// シングルトン呼び出し
	Input* input_ = Input::GetInsTance();

public:
	// デストラクタ
	virtual ~IScene();

	// 初期化処理
	virtual void Initialize() = 0;
	// 更新処理
	virtual void Update() = 0;
	// 描画処理
	virtual void Draw() = 0;
	virtual void PostDraw() = 0;

	// Getter
	int GetSceneNo();
};