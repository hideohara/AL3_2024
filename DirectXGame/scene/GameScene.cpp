#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete modelBlock_;


	// 自キャラの解放
	delete player_;


	// ブロックの解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;

	// スカイドームの解放
	delete skydome_;

	delete modelSkydome_;


	// マップチップフィールドの解放
	delete mapChipField_;



}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("test.png");

	// 3Dモデルの生成
	model_ = Model::Create();

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();


	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &viewProjection_);

	// ブロックの3Dモデルの生成
	modelBlock_ = Model::CreateFromOBJ("block");



	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// スカイドームの生成
	skydome_ = new Skydome();
	// スカイドームの初期化
	skydome_->Initialize(modelSkydome_, &viewProjection_);


	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");



	GenerateBlocks();


}

void GameScene::Update() {

	// 自キャラの更新
	player_->Update();

	// スカイドームの更新
	skydome_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			if (!worldTransformBlock)
				continue;

			//Matrix4x4 result{ 
			//	1.0f, 0.0f, 0.0f, 0.0f, 
			//	0.0f, 1.0f, 0.0f, 0.0f,
			//	0.0f, 0.0f, 1.0f, 0.0f, 
			//	worldTransformBlock->translation_.x, 
			//	worldTransformBlock->translation_.y, 
			//	worldTransformBlock->translation_.z, 
			//	1.0f };


			//スケーリング行列の作成

			//	X軸周り回転行列の作成
			//	Y軸周り回転行列の作成
			//	Z軸周り回転行列の作成
			//	回転行列の合成(Z回転 * X回転 * Y回転)

			//	平行移動行列の作成

			//worldTransformBlock->matWorld_ = result;
			//worldTransformBlock->matWorld_ = MakeTranslateMatrix(worldTransformBlock->translation_);
			//	スケーリング行列 * 回転行列 * 平行移動行列;

			//worldTransformBlock->matWorld_ = MakeAffineMatrix(
			//		worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
					//worldTransformBlock->translation_);

			//worldTransformBlock->

			// 定数バッファに転送する
			//worldTransformBlock->TransferMatrix();

			//worldTransformBlock->rotation_.y += 0.01f;
			worldTransformBlock->UpdateMatrix();
		}
	}

	// デバッグカメラの更新
	debugCamera_->Update();

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	// カメラの処理
	if (isDebugCameraActive_) {
		//デバッグカメラの更新
		viewProjection_.matView = debugCamera_->GetViewProjection().matView; // デバッグカメラのビュー行列;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection; //デバッグカメラのプロジェクション行列;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	}
	else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}


}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 自キャラの描画
	player_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
		}
	}

	// スカイドームの描画
	skydome_->Draw();

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks()
{
	// 要素数
	//const uint32_t kNumBlockVirtical = 10;
	//const uint32_t kNumBlockHorizontal = 20


	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// ブロック1個分の横幅
	//const float kBlockWidth = 2.0f;
	//const float kBlockHeight = 2.0f;

	// 要素数を変更する
	// 列数を設定（縦方向のブロック数）
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を設定（横方向のブロック数）
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}


	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

}
