#pragma once
#include "kMath.h"
#include "DirectXCommon.h"
#include <wrl.h>

class SpriteCommon;

//頂点データ
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};
//マテリアルデータ
struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	//Matrix4x4 uvTransform;
};
//座標変換行列データ
struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

class Sprite
{
public: //メンバ関数
	void Initialize(SpriteCommon* spriteCommon); //初期化
	void Update();// 更新
	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU);// 描画
private:
	SpriteCommon* spriteCommon_ = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	//バッファリソースの使い方を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView; //頂点バッファビュー
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//マテリアルにデータを書き込む
	Material* materialData = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	//データを書き込む
	Matrix4x4* transformationMatrixDataSprite = nullptr; //スプライト


	//// VertexResourceの作成
	//	void CreateVertexResource();
	//	// IndexResourceの作成
	//	void CreateIndexResource();
	//	// VertexBufferViewの作成
	//	void CreateVertexbufferView();
	//	// IndexBufferViewの作成
	//	void CreateIndexBufferView();
	//	// MaterialResourceの作成
	//	void CreateMaterialResource();
	//	// MaterialResourceにデータを書きこっむためのアドレスを取得してmaterialDataに割り当てる
	//	void CreateMapMaterialData();
	//	// TransformationMatrixの作成
	//	void CreateTransformationMatrix();
	//	// TransformationMatrixResourceにデータを書き込む溜めのアドレスを取得してTransformationMatrixDataに割り当てる
	//	void CreateMapTransformationMatrixData();









};

