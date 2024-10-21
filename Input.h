#pragma once
#include <Windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

//02_02_p19まで

class Input
{
public: //メンバ関数
	//namespace
	template<class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	//初期化
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	//更新
	void Update();

	bool PushKey(BYTE keyNumber);
	//トリガー
	bool TriggerKey(BYTE keyNumber);

	//DirectInputのインスタンス
	ComPtr<IDirectInput8>  directInput ;

private://メンバ変数
	//キーボードのデバイス
	ComPtr<IDirectInputDevice8>keyboard;
	//全キーの入力情報を取得
	BYTE key[256] = {};
	//前回キーの入力情報を取得
	BYTE keyPre[256] = {};

};

