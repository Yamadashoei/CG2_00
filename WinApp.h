#pragma once
#include <windows.h>

class WinApp
{
public://静的メンバ変数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: //メンバ変数
	//初期化
	void Initialize();
	//更新
	void Update();

public: //定数
	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;
public:
	//getter
	HWND GetHwnd()const { return hwnd; }
	
private:
	//ウィンドウクラスの設定
	WNDCLASS wc{};
public:
	//getter
	HINSTANCE GetHInstance()const { return wc.hInstance; }


};

