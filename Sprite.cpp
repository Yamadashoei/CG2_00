#include "Sprite.h"
#include "SpriteCommon.h"
using namespace kMath;

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	this->spriteCommon_ = spriteCommon;

	//p10やる




	//spriteのリソース
	materialResource = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(Vector4));

	//書き込むためのアドレス
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//色の設定
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = false;
	//materialData->uvTransform = MakeIdentity4x4();

	//座標変換行列リソースを作る
	transformationMatrixResource = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(Matrix4x4));
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	//単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();

}

void Sprite::Update()
{

	//p20のぶんの直し
	//頂点リソースにデータを書き込む
	vertexData[0].position = { 0.0f, 0.0f, 0.0f, 1.0f }; // 左上
	vertexData[0].texcoord = { 0.0f, 0.0f };
	vertexData[1].position = { 640.0f, 0.0f, 0.0f, 1.0f }; // 右上
	vertexData[1].texcoord = { 1.0f, 0.0f };
	vertexData[2].position = { 640.0f, 360.0f, 0.0f, 1.0f }; // 右下
	vertexData[2].texcoord = { 1.0f, 1.0f };
	vertexData[3].position = { 0.0f, 360.0f, 0.0f, 1.0f }; // 左下
	vertexData[3].texcoord = { 0.0f, 1.0f };

	//インデックスリソースにデータを書き込む
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 3;
	indexData[3] = 3;
	indexData[4] = 1;
	indexData[5] = 2;

	// Transform変数を作る
	Transform transform{
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};

	Transform cameraTransform{
		{1.0f, 1.0f, 1.0f },
		{0.0f, 0.0f, 0.0f },
		{0.0f, 0.0f, -5.0f}
	};
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.1f, 100.0f);

	transformationMatrixData->WVP = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	//transformationMatrixData->World = worldMatrix;

}

void Sprite::Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU)
{
	// VertexBufferViewを設定
	CreateVertexbufferView();
	spriteBase_->GetDxBase()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView); // VBVを設定
	// IndexBufferViewを設定
	CreateIndexBufferView();
	spriteBase_->GetDxBase()->GetCommandList()->IASetIndexBuffer(&indexbufferView); // VBVを設定

	// マテリアルCBufferの場所を設定
	spriteBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	// 座標変換行列CBufferの場所を設定
	// wvp用のCBufferの場所を設定
	spriteBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	// SRVのDescriptorTableの先頭を設定。2はrootParameter[2]である。
	//spriteBase_->GetDxBase()->GetCommandList()->SetGraphicsRootDescriptorTable(2, spriteBase_->GetDxBase()->GetSRVGPUDescriptorHandle(1));
	spriteBase_->GetDxBase()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	// 描画! (DrawCall)
	spriteBase_->GetDxBase()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}

