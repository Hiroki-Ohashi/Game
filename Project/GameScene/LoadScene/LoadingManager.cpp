#include "LoadingManager.h"
#include <chrono>
#include <thread>

LoadingManager::~LoadingManager()
{
    if (loadThread_.joinable()) {
        // スレッドが終了するまで待つ
        loadThread_.join();
    }
}

void LoadingManager::Initialize(TextureManager* textureManager)
{
    // 各段階のテクスチャを順番に読み込み
    textures_.push_back(textureManager->Load("resources/l.png"));
    textures_.push_back(textureManager->Load("resources/lo.png"));
    textures_.push_back(textureManager->Load("resources/loa.png"));
    textures_.push_back(textureManager->Load("resources/load.png"));
    textures_.push_back(textureManager->Load("resources/loadi.png"));
    textures_.push_back(textureManager->Load("resources/loadin.png"));
    textures_.push_back(textureManager->Load("resources/loading.png"));
    textures_.push_back(textureManager->Load("resources/loading..png"));
    textures_.push_back(textureManager->Load("resources/loading...png"));
    textures_.push_back(textureManager->Load("resources/loading....png"));

    // 初期スプライト設定
    sprite_ = std::make_unique<Sprite>();
    sprite_->Initialize({ 0.0f, 0.0f }, { 1280.0f, 720.0f }, textures_[0]);
}

void LoadingManager::Start()
{
    // すでに実行中なら無視
    if (threadRunning_) return;
    threadRunning_ = true;

    loadThread_ = std::thread(&LoadingManager::LoadThreadFunc, this);
}

void LoadingManager::LoadThreadFunc()
{
    /// スレッド内で一定時間ごとにステージを進行
    using namespace std::chrono_literals;
    for (int i = 1; i <= static_cast<int>(Stage::LOADING3); ++i) {
        std::this_thread::sleep_for(300ms);
        currentStage_ = static_cast<Stage>(i);
    }

    std::this_thread::sleep_for(300ms);
    currentStage_ = Stage::COMPLETE;
    threadRunning_ = false;
}

void LoadingManager::Update()
{
    UpdateSprite();
}

void LoadingManager::UpdateSprite()
{
    /// 現在ステージに対応するテクスチャをセット
    int stageIndex = static_cast<int>(currentStage_);
    if (stageIndex < static_cast<int>(Stage::COMPLETE)) {
        sprite_->SetTexture(textures_[stageIndex]);
    }
}

void LoadingManager::Draw()
{
    sprite_->Draw();
}

bool LoadingManager::IsComplete() const
{
    /// ロードが完了したかどうかを返す
    return currentStage_ == Stage::COMPLETE;
}