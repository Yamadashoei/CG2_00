#include "Input.h"
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")


void Input::Initialize(WinApp* winApp)
{
	HRESULT hr;
	//DirectionInput
	//IDirectInputDevice8* keyboard = nullptr;

	hr = DirectInput8Create(
		winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(hr));

	//キーボードデバイスの生成
	hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(hr));
	//入力データ形式のセット
	hr = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//排他制御レベルのセット
	hr = keyboard->SetCooperativeLevel(
		winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	//借りてきたWinAppのインスタンスを記録
	this->winApp_ = winApp;

}

bool Input::TriggerKey(BYTE keyNumber)
{
	if (key[keyNumber] && !keyPre[keyNumber]) {
		return true;
	}
	return false;
}

void Input::Update()
{
	//前回のキー入力を保存 
	memcpy(keyPre, key, sizeof(key));
	//キーボード情報の取得開始
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(key), key);



}

bool Input::PushKey(BYTE keyNumber)
{
	//指定キーを押してればtrueを返す
	if (key[keyNumber]) {
		return true;
	}
	//そうでなければfalseを返す
	return false;
}


