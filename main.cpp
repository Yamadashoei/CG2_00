#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>
#include <wrl.h>

#include<cassert>
#include<fstream>
#include<sstream>
#include<dxgidebug.h>
#include<dxcapi.h>
#include<d3d12.h>
#include<dxgi1_6.h>

#include "Transform.h"

#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "D3DResourceLeakChecker.h"

#include "SpriteCommon.h"
#include "Sprite.h"

#include "kMath.h"
using namespace kMath;

using namespace Logger;
using namespace StringUtility;


//struct VertexData
//{
//	Vector4 position;
//	Vector2 texcoord;
//	Vector3 normal;
//};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData>vertices;
	MaterialData material;
};

//struct TransformationMatrix {
//	Matrix4x4 WVP;
//	Matrix4x4 World;
//};

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

Transform transform{ {0.5f,0.5f,0.5f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
////CPUで動かす用Transformを作る
Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,-10.0f} };

//Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
//
//Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
//
//Matrix4x4 viewMatrix = Inverse(cameraMatrix);
//
Matrix4x4 projectionMatrix = Matrix4x4::MakePerspectiveMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);
//
//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
//
////Sprite用のWorldViewProjectionMatrixを作る
//Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
//
//Matrix4x4 viewMatrixSprite = MakeIdentity4x4();

//DescriptorHeapの作成関数
ID3D12DescriptorHeap* CreateDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	//ディスクリプタヒープの生成 
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;//レンダーターゲットビュー用
	descriptorHeapDesc.NumDescriptors = numDescriptors;//ダブルバッファ用に2つ。多くても別に構わない
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	//ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));
	return descriptorHeap.Get();
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;//構築するMaterialData
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identiferに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData; //構築するModelData
	std::vector<Vector4>positions; //位置
	std::vector<Vector3>normals; //法線
	std::vector<Vector2>texcoords; //テクスチャ座標
	std::string line; //ファイルから読んだ１行を格納するもの

	std::ifstream file(directoryPath + "/" + filename); //ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;// 先頭の識別子を読む

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置 / UV / 法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');///区切りでインデックスを読んでいく	
					elementIndices[element] = std::stoi(index);
				}
				//要素のIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];

				position.x *= 1.0f;
				normal.x *= -1.0f;
				texcoord.y = 1.0f - texcoord.y;

				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTemplateLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// D3D12ResourceChecker
	D3DResourceLeakChecker d3dResourceLeakChecker;

#pragma region 基盤システムの初期化

	WinApp* winApp = nullptr;
	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

	DirectXCommon* dxCommon = nullptr;
	//DirectX初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	Input* input = nullptr;
	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

	SpriteCommon* spriteCommon = nullptr;
	//スプライト共有部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

#pragma endregion 

	//#pragma region RootSignature作成
	//	//RootSignature作成
	//	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//
	//	//DescriptorRange
	//	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//	descriptorRange[0].BaseShaderRegister = 0; //0から始まる
	//	descriptorRange[0].NumDescriptors = 1; //数は1つ
	//	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV; //SRVを使う
	//	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; //Offsetを自動計算
	//
	//	//RootParameter作成
	//	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	//	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	//	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	//	rootParameters[0].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド
	//
	//	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;// DescriptorTableを使う
	//	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;// VertexShaderで使う
	//	rootParameters[1].Descriptor.ShaderRegister = 0; //Object3d.VS.hlsl の b0
	//
	//	//rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // DescriptorTableを使う
	//	//rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;         // VertexShaderで使う
	//	//rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing; // Tableの中身の配列を指定
	//	//rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing); // Tableで利用する数
	//
	//	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	//	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	//	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	//	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	//
	//	descriptionRootSignature.pParameters = rootParameters;              //ルートパラメータ配列へのポインタ
	//	descriptionRootSignature.NumParameters = _countof(rootParameters);  //配列の長さ
	//
	//	//Sampler
	//	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	//	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//~1の範囲外をリピート
	//	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	//	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	//	staticSamplers[0].ShaderRegister = 0;//レジスタ番号®を使う
	//	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	//	descriptionRootSignature.pStaticSamplers = staticSamplers;
	//	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	//
	//	//シリアライズしてバイナリにする
	//	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
	//	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	//	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//	if (FAILED(hr)) {
	//		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	//		assert(false);
	//	}
	//	//バイナリを元に生成
	//	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//	hr = dxCommon->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	//	assert(SUCCEEDED(hr));
	//#pragma endregion
	//
	//#pragma region InputLayout
	//	//InputLayoutの設定 
	//	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//	inputElementDescs[0].SemanticName = "POSITION";
	//	inputElementDescs[0].SemanticIndex = 0;
	//	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//	inputElementDescs[1].SemanticName = "TEXCOORD";
	//	inputElementDescs[1].SemanticIndex = 0;
	//	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//	inputElementDescs[2].SemanticName = "NORMAL";
	//	inputElementDescs[2].SemanticIndex = 0;
	//	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//
	//	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	//	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	//	inputLayoutDesc.NumElements = _countof(inputElementDescs);
	//#pragma endregion
	//
	//#pragma region BlenderStateの設定
	//	//BlendStateの設定
	//	D3D12_BLEND_DESC blendDesc{};
	//	//すべての色要素を書き込む
	//	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//#pragma endregion
	//
	//#pragma region RasiterStateの設定
	//	//RasterizerStateの設定
	//	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//	//裏面を表示しない
	//	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//	//三角形の中を塗りつぶす
	//	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//#pragma endregion
	//
	//#pragma region Shaderのコンパイル
	//	//ShaderをCompileする
	//	Microsoft::WRL::ComPtr<IDxcBlob>  vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3d.VS.hlsl", L"vs_6_0");
	//	assert(vertexShaderBlob != nullptr);
	//
	//	Microsoft::WRL::ComPtr<IDxcBlob>  pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3d.PS.hlsl", L"ps_6_0");
	//	assert(pixelShaderBlob != nullptr);
	//
	//#pragma endregion
	//
	//	//DepthStencilStateの設定
	//	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//	//Depthの機能を有効化
	//	depthStencilDesc.DepthEnable = true;
	//	//書き込む
	//	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//	//比較関数はLessEqual
	//	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//#pragma endregion
	//
	//#pragma region PSOの生成
	//	//PSOを生成する
	//	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();// RootSignature
	//	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;// InputLayout
	//	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };// VertexShader
	//	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };// PixelShader
	//	graphicsPipelineStateDesc.BlendState = blendDesc;// BlendState
	//	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;// RasterizerState
	//	//書き込むRTVの情報
	//	graphicsPipelineStateDesc.NumRenderTargets = 1;
	//	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//	//利用するトポロジ(形状)のタイプ。三角形
	//	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//	//どのように画面に色を打ち込むかの設定(気にしなくて良い)
	//	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	//	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//
	//	//DepthStencilをPSOに代入
	//	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	//実際に生成
	//	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPilelineState = nullptr;
	//	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPilelineState));
	//	assert(SUCCEEDED(hr));
	//#pragma endregion

#pragma region 最初のシーンの初期化
	//スプライトの初期化
	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteCommon);

#pragma endregion

	//モデルデータ読み込み
	ModelData modelData = LoadObjFile("resources", "plane.obj");
	DirectX::ScratchImage mipImages2 = DirectXCommon::LoadTexture(modelData.material.textureFilePath);


	////6頂点までしか確保できない　エラー
	//#pragma region VertexResourceの生成
	//	//実際に頂点リソースを作る
	//	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(VertexData) * modelData.vertices.size());
	//#pragma endregion

	//#pragma region VertexBufferViewを作成
	//	//VertexBufferViewを作成
	//	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//	//リソースの先頭アドレスから使う
	//	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//	//リソースサイズは頂点３つ分
	//	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	//	//1頂点あたりのサイズ
	//	vertexBufferView.StrideInBytes = sizeof(VertexData);
	//#pragma endregion

	//#pragma region VertexBufferViewを作成
	//	//頂点リソースにデータを書き込む
	//	VertexData* vertexData = nullptr;
	//	//書き込むためのアドレスを取得
	//	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	//
	//#pragma endregion

#pragma region 頂点リソース //まだ書いてない

#pragma endregion

#pragma region Textureを読んで転送
	//Textureを読んで転送する
	DirectX::ScratchImage mipImages = dxCommon->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(metadata); //dxCommon->GetDevice(),
	dxCommon->UploadTextureData(textureResource.Get(), mipImages);
#pragma endregion

#pragma region Textureを読んで転送(2つ目) //まだ書いてない

#pragma endregion

	//#pragma region VertexResourceSpriteの生成
	//	//Sprite用の頂点リソースを作る
	//	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(VertexData) * 6);
	//
	//#pragma endregion

	//#pragma region VertexBufferViewSpriteを作成
	//	//VertexBufferViewを作成
	//	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//	//リソースの先頭アドレスから使う
	//	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	//	//リソースサイズは頂点３つ分
	//	vertexBufferViewSprite.SizeInBytes = UINT(sizeof(VertexData) * 6);
	//	//1頂点あたりのサイズ
	//	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	//#pragma endregion

	//#pragma region Spriteの頂点データ
	//	//頂点リソースにデータを書き込む
	//	VertexData* vertexDataSprite = nullptr;
	//	//書き込むためのアドレスを取得
	//	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//	//一枚目の四角形
	//	//左下
	//	vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
	//	vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
	//	//左上
	//	vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f,1.0f };
	//	vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
	//	//右下
	//	vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	//	vertexDataSprite[2].texcoord = { 1.0f, 1.0f };
	//	//右上
	//	vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	//	vertexDataSprite[3].texcoord = { 1.0f, 0.0f };
	//
	//
	//#pragma endregion

#pragma region SRV (ShaderResourceView)(1つ目)
	// metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);
	//// SRVの生成
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
#pragma endregion

#pragma region SRV (ShaderResourceView)(2つ目) //まだ書いてない

#pragma endregion

	//#pragma region マテリアル
	//	//マテリアル用のリソースを作る
	//	Microsoft::WRL::ComPtr<ID3D12Resource>  materialResource = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4));
	//	//マテリアルにデータを書き込む
	//	Vector4* materialData = nullptr;
	//	//書き込むためのアドレスを取得
	//	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//	//赤を書き込む
	//	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	//#pragma endregion

	//#pragma region WVP (VertexBufferView)
	//	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(Matrix4x4));
	//	//データ書き込み
	//	Matrix4x4* wvpData = nullptr;
	//	//書き込むためのアドレスを取得
	//	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//	//単位行列を書き込んでおく
	//	*wvpData = MakeIdentity4x4();
	//#pragma endregion

#pragma region sprite用のマテリアルリソースを作る //まだ書いてない

#pragma endregion

#pragma region sprite用のWindow //まだ書いてない

#pragma endregion

//#pragma region Sprite用のTransformationMatrix
//	//マテリアル用のリソースを作る
//	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(Matrix4x4));
//	//データを書き込む
//	Matrix4x4* transformationMatrixDataSprite = nullptr;
//	//書き込むためのアドレスを取得
//	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
//	//赤を書き込む
//	*transformationMatrixDataSprite = MakeIdentity4x4();
//#pragma endregion


#pragma region Lighting //まだ書いてない

#pragma endregion

//#pragma region index用
//	//実際に頂点リソースを作る
//	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite = dxCommon->CreateBufferResource(dxCommon->GetDevice(), sizeof(uint32_t) * 6);
//	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
//	//リソースの先頭のアドレスから使う
//	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
//	//使用するリソースのサイズはインデックス６つ分のサイズ
//	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
//	//インデックスをuint32_tとする
//	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
//	//インデックスリソースにデータを書き込む
//	uint32_t* indexDataSprite = nullptr;
//	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
//	indexDataSprite[0] = 0;  indexDataSprite[1] = 1;  indexDataSprite[2] = 2;
//	indexDataSprite[3] = 1;  indexDataSprite[4] = 3;  indexDataSprite[5] = 2;
//#pragma endregion

#pragma region Resourceの作成

#pragma endregion


	MSG msg{};

	//ウィンドウの×ボタンが押されるまでメインループ
	while (true) {
		//WINDOWにメッセージが来てたら最優先で処理させる
		if (winApp->ProcessMessage()) {
			//ゲームループを抜ける
			break;
		}
		else {

			sprite->Update();

			//ゲームの処理↓
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame();
			//開発用のUIの処理

			//ImGui 1枚目
			ImGui::Begin("Model");
			//ImGui::DragFloat3("color", &materialData->x, 0.01f);
			ImGui::DragFloat3("scale", &transform.scale.x, 0.01f);
			ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
			ImGui::DragFloat3("translate", &transform.translate.x, 0.01f);
			//色変え
			//ImGui::ColorEdit3("color", &materialData->x);
			ImGui::End();

			//ImGui 2枚目
			ImGui::Begin("Window");
			//ImGui::DragFloat3("spriteColor", &materialData->x, 0.01f);
			ImGui::DragFloat3("spriteScale", &transformSprite.scale.x, 0.01f);
			ImGui::DragFloat3("spriteRotate", &transformSprite.rotate.x, 0.01f);
			ImGui::DragFloat3("spriteTranslate", &transformSprite.translate.x, 0.01f);
			//色変え
			//ImGui::ColorEdit3("spriteColor", &materialData->x);
			ImGui::End();

			////これから書き込むバックバッファのインデックスを取得
			//backBufferIndex = swapChain->GetCurrentBackBufferIndex();

			transform.rotate.y += 0.03f;
			Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate); //*wvpData = worldMatrix;
			Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
			Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, float(kClientWidth), float(kClientHeight), 0.0f, 100.0f);

			Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite)); //*transformationMatrixDataSprite = worldViewProjectionMatrixSprite;

			////コメ解除
			//Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
			//// WVP等を計算して、Resourceに書き込む。メインループの中で行う
			//for (uint32_t index = 0; index < kNumInstance; ++index) {
			//	Matrix4x4 worldMatrix = MakeAffineMatrix(transforms[index].scale, transforms[index].rotate, transforms[index].translate);
			//	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, viewProjectionMatrix);
			//	instancingData[index].WVP = worldViewProjectionMatrix;
			//	instancingData[index].World = worldMatrix;
			//}

			dxCommon->PreDraw();

			//スプライト
			spriteCommon->Settings();

			////RootSignatureを設定。PS0に設定しているけど別途設定が必要
			//dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
			//// PSOを設定
			//dxCommon->GetCommandList()->SetPipelineState(graphicsPilelineState.Get());
			////VBVを設定
			//dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
			////
			////形状を設定。PSOに設定しているものとはまた別。同じものを設定すると考えておけば良い
			//dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			////マテリアルCBufferの場所を設定
			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());

			////wvp用のCBufferの場所を設定
			//// 定数バッファビューを0番目のパラメータで設定
			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			////dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());

			////instancing用のDataを読むためにStructuredBufferのSRVを設定する
			////dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU);
			//dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

			////描画！（DrawCall/ドローコール）
			////dxCommon->GetCommandList()->DrawInstanced(6, 1, 0, 0);
			////描画！6頂点の板ポリゴンを、kNumInstance（今回は10）だけInstance描画を行う

			////パーティクル
			//dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);


			////spriteの描画。変更が必要なものだけ変更する
			//dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			////IndexBufferView 
			//dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
			////TransformationMatrixBufferの場所を設定
			//dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
			//dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);


			//ImGuiの内部コマンド生成 
			ImGui::Render();
			ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList().Get());

			//spriteDraw
			sprite->Draw(textureSrvHandleGPU);

			dxCommon->PostDraw();

		}
	}
	////出力ウィンドウへの文字出力
	//OutputDebugStringA("Hello,DirectX!\n");

#ifdef _DEBUG


#endif

	//dxCommonの終了処理
	dxCommon->Finalize();
	delete input;
	//WindowsAPIの終了処理
	winApp->Finalize();
	//WindowsAPI解放
	delete winApp;
	//DirectX解放
	delete dxCommon;
	//スプライトの共有部の解放
	delete spriteCommon;
	//スプライトの解放
	delete sprite;

	return 0;
}