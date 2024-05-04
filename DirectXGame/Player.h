#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "MathUtilityForText.h"
#include "Input.h"

// 前方参照
class MapChipField;

// プレイヤー
class Player
{

public:
    // 左右
    enum class LRDirection {
        kRight,
        kLeft,
    };


    // 角
    enum Corner {
        kRightBottom,    // 右下
        kLeftBottom,     // 左下
        kRightTop,       // 右上
        kLeftTop,        // 左上

        kNumCorner       // 要素数

    };




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


    void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

private:
    static inline const float kAcceleration = 0.01f;
    static inline const float kAttenuation = 0.05f;
    static inline const float kTimeTurn = 0.3f;

    // 重力加速度（下方向）
    static inline const float kGravityAcceleration = 0.03f;
    // 最大落下速度（下方向）
    static inline const float kLimitFallSpeed = 0.5f;
    // ジャンプ初速（上方向）
    static inline const float kJumpAcceleration = 0.5f;

    static inline const float kAttenuationLanding = 0.0f;
    static inline const float kBlank = 0.04f;



    // マップとの当たり判定情報
    struct CollisionMapInfo {
        bool ceiling = false; // 天井衝突フラグ
        bool landing = false; // 着地フラグ
        bool hitWall = false; // 壁接触フラグ
        Vector3 move;         // 移動量
    };

    // ワールド変換データ
    WorldTransform worldTransform_;
    // モデル
    Model* model_ = nullptr;
    // テクスチャハンドル
    uint32_t textureHandle_ = 0u;

    // ビュープロジェクション
    ViewProjection* viewProjection_ = nullptr;

    Vector3 velocity_ = {};




    LRDirection lrDirection_ = LRDirection::kRight;
    float turnFirstRotationY_ = 0.0f;
    float turnTimer_ = 0.0f;


    // 接地状態フラグ
    bool onGround_ = true;

    // マップチップによるフィールド
    MapChipField* mapChipField_ = nullptr;


    // キャラクターの当たり判定サイズ
    static inline const float kWidth = 0.8f;
    static inline const float kHeight = 0.8f;

    // 移動関数
    void InputMove();
    void CheckMapCollision(CollisionMapInfo& info);
    void CheckMapCollisionUp(CollisionMapInfo& info);

    Vector3 CornerPosition(const Vector3& center, Corner corner);

};

