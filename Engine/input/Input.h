#pragma once
#include <cassert>
#include <wrl.h>
#include <Windows.h>
#include "WinApp.h"
#include <MathFunction.h>

#define DIRECTION_VERSION 0x0800
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib,"xinput.lib")
#pragma comment(lib, "dxguid.lib")

/// <summary>
/// Input.h
/// 入力処理のヘッダーファイル
/// </summary>
namespace Engine
{
	// inputクラス
	class Input {
	public:
		// 入力sトラクタ
		struct ButtonState {
			bool isPressed;
			bool wasPressed;

			ButtonState() : isPressed(false), wasPressed(false) {}
		};

		// シングルトン
		static Input* GetInstance();

		// 初期化
		void Initialize();
		// 読み込み
		void Update();

		// キーの押下をチェック
		bool PushKey(BYTE keyNumber);
		// キーのトリガーをチェック
		bool TriggerKey(BYTE keyNumber);

		bool GetJoystickState(XINPUT_STATE& out) const;

		bool PressedButton(XINPUT_STATE& out, WORD button);

		void UpdateButtonState(ButtonState& state, bool isPressed);

		Vector2 GetLeftStick() const;

	private:
		Microsoft::WRL::ComPtr<IDirectInput8> directInput = nullptr;
		Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboad = nullptr;

		Input::ButtonState state_;

		// 全キーの入力情報を取得する
		BYTE key[256] = {};
		// 前回の全キーの入力情報を取得する
		BYTE keyPre[256] = {};
	};
}