#include "Input.h"

Input* Input::GetInsTance()
{
	static Input instance;
	return &instance;
}

void Input::Initialize(){
	HRESULT result;
	// DirectInputのインスタンスを生成
	result = DirectInput8Create(WinApp::GetInsTance()->GetHInstance().hInstance, DIRECTION_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	// キーボードデバイス生成
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboad, NULL);
	assert(SUCCEEDED(result));

	// 入力データ形式のセット
	result = keyboad->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	// 排他制御レベルのセット
	result = keyboad->SetCooperativeLevel(WinApp::GetInsTance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update(){

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
