#include "SpriteCommon.h"


void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	RootSignature();
	GraphicsPipeline();
}

void SpriteCommon::RootSignature()
{
}

void SpriteCommon::GraphicsPipeline()
{
}

void SpriteCommon::Settings()
{
	//修正
	//ルートシグネチャをセットするコマンド
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	//グラフィックスパイプラインステートをセットするコマンド
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
	//プリミティブトポロジーをセットするコマンド
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


