#include "CameraController.h"

#include "Player.h"
#include <algorithm>
#include <functional>
#include <iostream>

void CameraController::Initialize()
{
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void CameraController::Update()
{
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象とオフセットからカメラの座標を計算
	//viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;

	// 追従対象の座標、移動速度とオフセットで目標座標を計算
	//destination_ = targetWorldTransform.translation_ + targetVelocity * kVelocityBias + targetOffset_;
	//destination_ = targetWorldTransform.translation_ + targetVelocity * kVelocityBias + targetOffset_;
	// 追従対象とオフセットからカメラの目標座標を計算
	//destination_ = targetWorldTransform.translation_ + targetOffset_;
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	destination_ = targetWorldTransform.translation_ + targetOffset_ + targetVelocity * kVelocityBias;

	// 座標補間によりゆったり追従
	viewProjection_.translation_ = Lerp(viewProjection_.translation_, destination_, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	viewProjection_.translation_.x = (std::max)(viewProjection_.translation_.x, targetWorldTransform.translation_.x + targetMargin.left);
	viewProjection_.translation_.x = (std::min)(viewProjection_.translation_.x, targetWorldTransform.translation_.x + targetMargin.right);
	viewProjection_.translation_.y = (std::max)(viewProjection_.translation_.y, targetWorldTransform.translation_.y + targetMargin.bottom);
	viewProjection_.translation_.y = (std::min)(viewProjection_.translation_.y, targetWorldTransform.translation_.y + targetMargin.top);

	// 移動範囲制限
	viewProjection_.translation_.x = (std::max)(viewProjection_.translation_.x, movableArea_.left);
	viewProjection_.translation_.x = (std::min)(viewProjection_.translation_.x, movableArea_.right);
	viewProjection_.translation_.y = (std::max)(viewProjection_.translation_.y, movableArea_.bottom);
	viewProjection_.translation_.y = (std::min)(viewProjection_.translation_.y, movableArea_.top);


	// 行列を更新する
	viewProjection_.UpdateMatrix();
}

void CameraController::Reset()
{
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;


}
