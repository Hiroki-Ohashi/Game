#include "Input.h"

/// <summary>
/// Input.cpp
/// 入力処理のソースファイル
/// </summary>
namespace Engine
{
	Input* Input::GetInstance()
	{
		static Input instance;
		return &instance;
	}

	void Input::Initialize() {
		HRESULT result;
		// DirectInputのインスタンスを生成
		result = DirectInput8Create(WinApp::GetInstance()->GetHInstance().hInstance, DIRECTION_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
		assert(SUCCEEDED(result));

		// キーボードデバイス生成
		result = directInput->CreateDevice(GUID_SysKeyboard, &keyboad, NULL);
		assert(SUCCEEDED(result));

		// 入力データ形式のセット
		result = keyboad->SetDataFormat(&c_dfDIKeyboard);
		assert(SUCCEEDED(result));

		// 排他制御レベルのセット
		result = keyboad->SetCooperativeLevel(WinApp::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		assert(SUCCEEDED(result));
	}

	void Input::Update() {

		// 前回のキー入力を保存
		memcpy(keyPre, key, sizeof(key));

		// キーボード情報の取得
		keyboad->Acquire();
		keyboad->GetDeviceState(sizeof(key), key);
	}

	bool Input::PushKey(BYTE keyNumber)
	{
		// 指定キーを押していればtrueを返す
		if (key[keyNumber]) {
			return true;
		}

		return false;
	}

	bool Input::TriggerKey(BYTE keyNumber)
	{
		if (keyPre[keyNumber] != 0 && key[keyNumber] == 0)
		{
			return true;
		}

		return false;
	}

	bool Input::GetJoystickState(XINPUT_STATE& out) const
	{
		DWORD dwResult = XInputGetState(0, &out);
		if (dwResult == ERROR_SUCCESS) {
			return true;
		}

		return false;
	}

	bool Input::PressedButton(XINPUT_STATE& out, WORD button)
	{
		Input::GetInstance()->UpdateButtonState(state_, out.Gamepad.wButtons & button);

		return (state_.isPressed && !state_.wasPressed);
	}

	void Input::UpdateButtonState(ButtonState& state, bool isPressed)
	{
		state.wasPressed = state.isPressed;
		state.isPressed = isPressed;
	}

	Vector2 Input::GetLeftStick() const
	{
		XINPUT_STATE state{};
		if (XInputGetState(0, &state) != ERROR_SUCCESS) {
			return { 0.0f, 0.0f };
		}

		const float DEAD_ZONE = 0.2f;

		float lx = static_cast<float>(state.Gamepad.sThumbLX) / 32767.0f;
		float ly = static_cast<float>(state.Gamepad.sThumbLY) / 32767.0f;

		float magnitude = std::sqrt(lx * lx + ly * ly);

		if (magnitude < DEAD_ZONE) {
			return { 0.0f, 0.0f };
		}

		// デッドゾーン補正
		float scale = (magnitude - DEAD_ZONE) / (1.0f - DEAD_ZONE);
		lx = (lx / magnitude) * scale;
		ly = (ly / magnitude) * scale;

		return { lx, ly };
	}

}