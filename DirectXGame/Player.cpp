#define NOMINMAX

#include "Player.h"
#include <cassert>
#include <numbers>
#include <algorithm>

// プレイヤー

void Player::Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection, const Vector3& position)
{

	// NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;

	// ワールド変換の初期化
	worldTransform_.Initialize();


	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;


}

void Player::Update()
{
	// キー入力で移動
	InputMove();

	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}



	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	}
	else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuationLanding);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}



	// 移動
	worldTransform_.translation_ += velocity_;

	// 行列を更新
	worldTransform_.UpdateMatrix();


}

void Player::Draw()
{
	// 3Dモデルを描画
	//model_->Draw(worldTransform_, *viewProjection_, textureHandle_);
	model_->Draw(worldTransform_, *viewProjection_);
}

void Player::InputMove()
{
	if (onGround_) {
		// 移動入力
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				// 角度表示
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}
			else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;
				// 角度表示
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
			}


			// 加速／減速
			velocity_ += acceleration;
		}
		else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		// 旋回制御
		if (turnTimer_ > 0.0f)
		{
			//旋回タイマーを1 / 60秒分カウントダウンする
			turnTimer_ -= 1.0f / 60.0f;
			//turnTimer_ = std::max(turnTimer_ - (1.0f / 60.0f), 0.0f);

				// 左右の自キャラ角度テーブル
			float destinationRotationYTable[] = {
				std::numbers::pi_v<float> / 2.0f,
				std::numbers::pi_v<float> *3.0f / 2.0f
			};
			// 状態に応じた目標角度を取得する
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			// 自キャラの角度を設定する
			// 旋回タイマーを使って角度補間;
			worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);

		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_ += Vector3(0, kJumpAcceleration, 0);
			//velocity_ += Vector3(0, kJumpAcceleration / 60.0f, 0);
		}
	}
	// 空中
	else {
		// 落下速度
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

}
