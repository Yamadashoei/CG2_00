#include "Sprite.h"
#include "SpriteCommon.h"
using namespace kMath;

void Sprite::Initialize(SpriteCommon* spriteCommon)
{
	this->spriteCommon_ = spriteCommon;

	////p10やる
	//textureSrvHandleCPU = spriteCommon_->GetDirectXCommon()->GetSRVCPUDescriptorHandle(1);
	//textureSrvHandleGPU = spriteCommon_->GetDirectXCommon()->GetSRVGPUDescriptorHandle(1);


	//Sprite
	vertexResource = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(VertexData) * 4); //modelData.vertices.size()

#pragma region VertexBufferViewを作成
	//リソースの先頭アドレス
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size()); //sizeof(VertexData) * 4;
	//頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	//左下
	vertexData[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
	vertexData[0].texcoord = { 0.0f, 1.0f };
	//左上
	vertexData[1].position = { 0.0f, 0.0f, 0.0f,1.0f };
	vertexData[1].texcoord = { 0.0f, 0.0f };
	//右下
	vertexData[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexData[2].texcoord = { 1.0f, 1.0f };
	//右上
	vertexData[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vertexData[3].texcoord = { 1.0f, 0.0f };
#pragma endregion

	//Index
	indexResource = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(uint32_t) * 6);
#pragma region index用
	//リソースの先頭アドレス
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//使用するリソースサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//頂点サイズ
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;
#pragma endregion

#pragma region マテリアル
	//spriteのリソース
	materialResource = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(Material));//Vector4

	//書き込むためのアドレス
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//色の設定
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//materialData->enableLighting = false;
	//materialData->uvTransform = MakeIdentity4x4();

#pragma endregion

#pragma region Sprite用のTransformationMatrix
	//座標変換行列リソースを作る
	transformationMatrixResource = spriteCommon_->GetDirectXCommon()->CreateBufferResource(sizeof(TransformationMatrix));//Matrix4x4
	//書き込むためのアドレスを取得
	transformationMatrixResource->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));
	//単位行列を書き込んでおく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
#pragma endregion

}

void Sprite::Update()
{

	Transform transform{ {0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	//CPUで動かす用Transformを作る
	Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);

	Matrix4x4 viewMatrix = Inverse(cameraMatrix);

	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, (float)WinApp::kClientWidth, (float)WinApp::kClientHeight, 0.0f, 100.0f);

	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));

	//Sprite用のWorldViewProjectionMatrixを作る
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);

	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
}



void Sprite::Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU)
{

	//VBVを設定
	spriteCommon_->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//マテリアルCBufferの場所を設定
	spriteCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

	//TransformationMatrixBufferの場所を設定
	spriteCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource->GetGPUVirtualAddress());

	//wvp用のCBufferの場所を設定 定数バッファビューを0番目のパラメータで設定
	//spriteCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

	//instancing用のDataを読むためにStructuredBufferのSRVを設定する
	spriteCommon_->GetDirectXCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	//描画！（DrawCall/ドローコール）
	//spriteCommon_->GetDirectXCommon()->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

	//spriteの描画。変更が必要なものだけ変更する
	//spriteCommon_->GetDirectXCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);

	//IndexBufferView 
	spriteCommon_->GetDirectXCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);//indexBufferViewSprite

	spriteCommon_->GetDirectXCommon()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);


	//IndexBufferView 
	//spriteCommon_->GetDirectXCommon()->GetCommandList()->IASetIndexBuffer(&indexBufferView);


}

