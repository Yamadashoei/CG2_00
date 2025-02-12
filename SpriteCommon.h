#pragma once
#include "DirectXCommon.h"

class SpriteCommon
{
public: //メンバ関数
	void Initialize(DirectXCommon* dxCommon); //初期化

private:
	//PSO
	void RootSignature(); //ルートシグネチャの作成
	void GraphicsPipeline(); //グラフィックスパイプラインの生成

	DirectXCommon* dxCommon_;


//PSO
	Microsoft::WRL::ComPtr < ID3D12PipelineState> graphicsPipelineState = nullptr;

};

