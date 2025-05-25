#pragma once
#include <thread>
#include <vector>
#include <memory>
#include "Sprite.h"
#include "TextureManager.h"
using namespace Engine;

class LoadingManager {
public:
    // ロードの各段階
    enum class Stage {
        L, LO, LOA, LOAD, LOADI, LOADIN,
        LOADING, LOADING1, LOADING2, LOADING3, COMPLETE
    };

    ~LoadingManager();

    // 初期化
    void Initialize(TextureManager* textureManager);
    // ローディング演出開始
    void Start();
    // 更新処理
    void Update();
    // 描画処理
    void Draw();
    // ロード完了判定
    bool IsComplete() const;

private:
    // スレッド処理本体
    void LoadThreadFunc();
    // テクスチャ切り替え
    void UpdateSprite();

private:
    // 現在のローディングステージ
    Stage currentStage_ = Stage::L;
    // 表示用スプライト
    std::unique_ptr<Sprite> sprite_;
    // 各ステージのテクスチャ
    std::vector<uint32_t> textures_;
    // 非同期スレッド
    std::thread loadThread_;
    // スレッド動作中フラグ
    bool threadRunning_ = false;
};