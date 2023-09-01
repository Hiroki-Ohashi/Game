#pragma once
#include <Windows.h>
#include <cstdint>
#include "Function.h"

class DirectX;

class Triangle {
public: 
	void Initialize(DirectX* dir_, Vector4* pos);

	void Update();

	void Draw(DirectX* dir_);

	void Release();

	void CreateVertexBufferView(DirectX* dir_, Vector4* pos);

public:
	ID3D12Resource* vertexResource = nullptr;

	Vector4* vertexData = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	static inline HRESULT hr_;
};