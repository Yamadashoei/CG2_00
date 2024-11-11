#pragma once
#include <Windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "WinApp.h"

class Input
{
public: //メンバ関数
	//namespace
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//初期化
	void Initialize(WinApp* winApp);
	//トリガー
	bool TriggerKey(BYTE keyNumber);

	//更新
	void Update();
	bool PushKey(BYTE keyNumber);
	

private://メンバ変数
	//DirectInputのインスタンス
	ComPtr<IDirectInput8>  directInput;
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8>keyboard;
	//全キーの入力情報を取得
	BYTE key[256] = {};
	//前回キーの入力情報を取得
	BYTE keyPre[256] = {};

	//WindowsAPI
	WinApp* winApp_ = nullptr;


};

