#pragma once
#include <cassert>
#include <wrl.h>
#include <Windows.h>
#define DIRECTION_VERSION 0x0800
#include <dinput.h>
#include "WinApp.h"

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


class Input {
public:

	static Input* GetInsTance();

	// 初期化
	void Initialize();
	// 読み込み
	void Update();

	// キーの押下をチェック
	bool PushKey(BYTE keyNumber);
	// キーのトリガーをチェック
	bool TriggerKey(BYTE keyNumber);

private:
	Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboad = nullptr;

	// 全キーの入力情報を取得する
	BYTE key[256] = {};
	// 前回の全キーの入力情報を取得する
	BYTE keyPre[256] = {};
};