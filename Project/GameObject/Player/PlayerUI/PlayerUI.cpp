#include "PlayerUI.h"

void PlayerUI::Initialize()
{
	textureManager_ = TextureManager::GetInstance();

	// モデルごとの初期パラメータ
	rightTransform_ = { {2.1f,1.3f,1.0f},{0.25f,-0.7f,0.0f},{0.0f,-50.0f,0.0f} };
	leftTransform_ = { {2.1f,1.3f,1.0f},{0.25f,0.7f,0.0f},{0.0f,-50.0f,0.0f} };

	// HPUIモデル
	rightModel_ = std::make_unique<Model>();
	rightModel_->Initialize("board.obj", rightTransform_);
	rightModel_->SetLight(false);
	// HPUI座標
	rightWorldtransform_.Initialize();
	rightWorldtransform_.scale = rightTransform_.scale;
	rightWorldtransform_.rotate = rightTransform_.rotate;
	rightWorldtransform_.translate = rightTransform_.translate;
	rightWorldtransform_.UpdateMatrix();

	// 操作方法モデル
	leftModel_ = std::make_unique<Model>();
	leftModel_->Initialize("board.obj", leftTransform_);
	leftModel_->SetLight(false);
	// 操作方法モデルワールド座標
	leftWorldtransform_.Initialize();
	leftWorldtransform_.scale = leftTransform_.scale;
	leftWorldtransform_.rotate = leftTransform_.rotate;
	leftWorldtransform_.translate = leftTransform_.translate;
	leftWorldtransform_.UpdateMatrix();

	// texture
	HP = textureManager_->Load("resources/hp.png");
	operation = textureManager_->Load("resources/operation.png");

	isEaseStart = true;
	isEaseEnd = false;
}

void PlayerUI::Update()
{
	if (isEaseStart) {
		frame++;
		if (frame >= endFrame) {
			frame = 0.0f;
			isEaseStart = false;
		}

		rightWorldtransform_.scale.y = start + (end - start) * EaseOutQuart(frame / endFrame);
		leftWorldtransform_.scale.y = start + (end - start) * EaseOutQuart(frame / endFrame);
	}
	
	if(!isEaseEnd && !isEaseStart){
		rightWorldtransform_.scale.y = rightTransform_.scale.y;
		leftWorldtransform_.scale.y = leftTransform_.scale.y;
	}

	if (ImGui::TreeNode("right")) {
		ImGui::DragFloat3("Scale", &rightWorldtransform_.scale.x, 0.01f);
		ImGui::DragFloat3("Rotate.y ", &rightWorldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &rightWorldtransform_.translate.x, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("left")) {
		ImGui::DragFloat3("Scale", &leftWorldtransform_.scale.x, 0.01f);
		ImGui::DragFloat3("Rotate.y ", &leftWorldtransform_.rotate.x, 0.01f);
		ImGui::DragFloat3("Transform", &leftWorldtransform_.translate.x, 0.01f);
		ImGui::TreePop();
	}

	rightWorldtransform_.UpdateMatrix();
	leftWorldtransform_.UpdateMatrix();

	rightModel_->SetWorldTransform(rightWorldtransform_);
	leftModel_->SetWorldTransform(leftWorldtransform_);
}

void PlayerUI::Draw(Camera* camera_)
{
	rightModel_->Draw(camera_, operation);
	leftModel_->Draw(camera_, HP);
}

void PlayerUI::SetUIPosition(Vector3 pos)
{
	rightWorldtransform_.translate = { pos.x - 4.0f, pos.y - 1.0f, pos.z };
	leftWorldtransform_.translate = { pos.x + 4.0f, pos.y - 1.0f, pos.z };
	
	rightWorldtransform_.UpdateMatrix();
	leftWorldtransform_.UpdateMatrix();
	rightModel_->SetWorldTransform(rightWorldtransform_);
	leftModel_->SetWorldTransform(leftWorldtransform_);
}

void PlayerUI::SetEaseEnd(bool isEase)
{
	isEaseEnd = isEase;
	
	if (isEaseEnd) {
		frame++;
		if (frame >= endFrame) {
			frame = 0;
			isEaseEnd = false;
		}

		rightWorldtransform_.scale.y = end + (start - end) * EaseOutQuart(frame / endFrame);
		leftWorldtransform_.scale.y = end + (start - end) * EaseOutQuart(frame / endFrame);
	}
}
