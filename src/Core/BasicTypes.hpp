#pragma once

namespace DirectX {
struct XMFLOAT2;
struct XMFLOAT3;
struct XMFLOAT4;
}

namespace Core {

using Vector2 = DirectX::XMFLOAT2;
using Vector3 = DirectX::XMFLOAT3;
using Vector4 = DirectX::XMFLOAT4;

using Position = Vector2;
}
