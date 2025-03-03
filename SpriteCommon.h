#pragma once
#include "DirectXCommon.h"

class SpriteCommon
{
public: //メンバ関数
	void Initialize(DirectXCommon* dxCommon); //初期化
	DirectXCommon* GetDirectXCommon()const { return dxCommon_; }

	//共通描画設定
	void Settings();

private:
	//PSO
	void RootSignature(); //ルートシグネチャの作成
	void GraphicsPipeline(); //グラフィックスパイプラインの生成

	DirectXCommon* dxCommon_;

	//RootSignature
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};

	//バイナリを元に生成
	//ルートシグネチャ
	Microsoft::WRL::ComPtr < ID3D12RootSignature> rootSignature = nullptr;
	//PSO
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;

};

