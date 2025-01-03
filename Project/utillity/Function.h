#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <dxgidebug.h>
#include <dxcapi.h>
#include "DirectXTex.h"

/// <summary>
/// Funcsion.ｈ
/// logの関数をまとめたヘッダーファイル
/// </summary>

// Convertクラス
class Convert {
public:
	// 文字コンバート
	static std::wstring ConvertString(const std::string& str);
	// 文字変換
	static std::string ConvertString(const std::wstring& str);
	// ログ生成
	static void Log(const std::string& message);
	// シェーダーコンパイル
	static IDxcBlob* CompileShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
};

