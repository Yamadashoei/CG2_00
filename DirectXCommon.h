#pragma once

#include<d3d12.h>
#include<dxgi1_6.h>

#include <wrl.h>
#include "Logger.h"
#include "StringUtility.h"
#include "WinApp.h"

#include <array>
#include <dxcapi.h>


//04_02_p12,13,17,18,20,2526,27,28,29,30

class DirectXCommon
{

	//デスクリプターヒープを生成する
	ID3D12DescriptorHeap* CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);

public:

	void Initialize(WinApp* winApp);
	//デバイスの生成
	void Device();
	//コマンド関連の生成
	void Command();
	//スワップチェーンの生成
	void SwapChain();
	//深度バッファの生成
	void DepthBuffer();
	//各種デスクリプタヒープの生成
	void DescriptorHeap();
	//レンだーターゲットビューの初期化
	void RTV();
	//深度ステンシルビューの初期化
	void DSV();
	//フエンスの初期化
	void Fence();
	//ビューポート矩形の初期化
	void ViewPort();
	//シザリング矩形
	void ScissoringRect();
	//DXCコンパイルの生成
	void DXCCompiler();
	//ImGuiの初期化
	void ImGui();

	//SRVの指定番号のCPUデスクリプタハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUDescriptorHandle(uint32_t index);
	//SRVの指定番号のGPUデスクリプタハンドルを取得する
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUDescriptorHandle(uint32_t index);

	//RTVの指定番号のCPUデスクリプタハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUDescriptorHandle(uint32_t index);
	//RTVの指定番号のGPUデスクリプタハンドルを取得する
	D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUDescriptorHandle(uint32_t index);

	//DSVの指定番号のCPUデスクリプタハンドルを取得する
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUDescriptorHandle(uint32_t index);
	//DSVの指定番号のGPUデスクリプタハンドルを取得する
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUDescriptorHandle(uint32_t index);


	void SetWinApp(WinApp* winApp) { winApp_ = winApp; }




private:

	HRESULT hr;

	//WindowsAPI
	WinApp* winApp_ = nullptr;
	//DirectX12デバイス
	ID3D12Device* device = nullptr;	//Microsoft::WRL::ComPtr<ID3D12Device>device;
	//DXGIファクトリーの生成
	IDXGIFactory7* dxgiFactory = nullptr;
	//コマンドアロケーターを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	//コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;
	//コマンドキューを生成する
	ID3D12CommandQueue* commandQueue = nullptr;
	//スワップチェインのポインタ
	IDXGISwapChain4* swapChain = nullptr;
	//スワップチェーンリソース
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	static const uint32_t MaxResource = 2;
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	//深度バッファ
	ID3D12Resource* resource = nullptr;

	//デスクリプタヒープ
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	uint32_t descriptorSizeSRV = 0;
	uint32_t descriptorSizeRTV = 0;
	uint32_t descriptorSizeDSV = 0;

	ID3D12DescriptorHeap* rtvDescriptorHeap;	//Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> rtvDescriptorHeap;
	ID3D12DescriptorHeap* srvDescriptorHeap;	//Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> srvDescriptorHeap;
	ID3D12DescriptorHeap* dsvDescriptorHeap;	//Microsoft::WRL::ComPtr < ID3D12DescriptorHeap> dsvDescriptorHeap;

	//RTV
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[MaxResource]; //エラー出る

	//初期値0でFenceを作る
	ID3D12Fence* fence = nullptr;

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//シザリング短径(シザー)
	D3D12_RECT scissoringRect{};
	//DXC
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	IDxcIncludeHandler* includeHandler = nullptr;

	//指定番号のCPUデスクリプタハンドルを取得する
	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);
	//指定番号のGPUデスクリプタハンドルを取得する
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap, uint32_t descriptorSize, uint32_t index);

	//コンパイルシェーダ
	IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

};

