#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "MathUtilityForText.h"
#include "Input.h"

// プレイヤー
class Player
{

public:

    /// <summary>
    /// 
    /// </summary>
    /// <param name="model">モデル</param>
    /// <param name="textureHandle">テクスチャハンドル</param>
    /// <param name="viewProjection">ビュープロジェクション</param>
    void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position);

    /// <summary>
    /// 更新
    /// </summary>
    void Update();

    /// <summary>
    /// 描画
    /// </summary>
    void Draw();

    const WorldTransform& GetWorldTransform() const { return worldTransform_; }
    const Vector3& GetVelocity() const { return velocity_; }

private:
    static inline const float kAcceleration = 0.01f;
    static inline const float kAttenuation = 0.05f;
    static inline const float kTimeTurn = 0.3f;

    // 重力加速度（下方向）
    static inline const float kGravityAcceleration = 0.1f;
    // 最大落下速度（下方向）
    static inline const float kLimitFallSpeed = 0.5f;
    // ジャンプ初速（上方向）
    static inline const float kJumpAcceleration = 1.0f;

    static inline const float kAttenuationLanding = 0.0f;


    // ワールド変換データ
    WorldTransform worldTransform_;
    // モデル
    Model* model_ = nullptr;
    // テクスチャハンドル
    uint32_t textureHandle_ = 0u;

    // ビュープロジェクション
    ViewProjection* viewProjection_ = nullptr;

    Vector3 velocity_ = {};

    // 左右
    enum class LRDirection {
        kRight,
        kLeft,
    };


    LRDirection lrDirection_ = LRDirection::kRight;
    float turnFirstRotationY_ = 0.0f;
    float turnTimer_ = 0.0f;


    // 接地状態フラグ
    bool onGround_ = true;

};

