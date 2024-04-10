#pragma once

#include "Model.h"
#include "WorldTransform.h"

// プレイヤー
class Player
{

public:

    /// <summary>
    /// 初期化
    /// </summary>
    /// <param name="model">モデル</param>
    /// <param name="textureHandle">テクスチャハンドル</param>
    
    

    /// <summary>
    /// 
    /// </summary>
    /// <param name="model">モデル</param>
    /// <param name="textureHandle">テクスチャハンドル</param>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection);

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();


private:
    // ワールド変換データ
    WorldTransform worldTransform_;
    // モデル
    Model* model_ = nullptr;
    // テクスチャハンドル
    uint32_t textureHandle_ = 0u;

    // ビュープロジェクション
    ViewProjection* viewProjection_ = nullptr;

};

