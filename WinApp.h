#pragma once
#include <windows.h>
#include <cstdint>

class WinApp
{
public://静的メンバ変数
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: //メンバ変数
	//初期化
	void Initialize();
	//更新
	void Update();
	//終了
	void Finalize();

	//getter
	HWND GetHwnd()const { return hwnd; }
	//getter
	HINSTANCE GetHInstance()const { return wc.hInstance; }

	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;
	//ウィンドウクラスの設定
	WNDCLASS wc{};


};

