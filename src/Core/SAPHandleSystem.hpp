#pragma once

#include <cstdint>
#include <vector>

namespace Core {
// Swap And Pop Handle System
class SAPHandleSystem
{
public:
  using size_type = std::uint32_t;

  struct Handle
  {
    size_type index = std::numeric_limits<size_type>::max();
    size_type generation = 0;
  };

public:
  SAPHandleSystem(size_type capacity);
  ~SAPHandleSystem() = default;

  SAPHandleSystem(SAPHandleSystem const&) = delete;
  SAPHandleSystem& operator=(SAPHandleSystem const&) = delete;
  SAPHandleSystem(SAPHandleSystem&&) = delete;
  SAPHandleSystem& operator=(SAPHandleSystem&&) = delete;

  Handle createHandle(size_type dataIdx);
  void notifySwapAndPop(size_type destroyedDataIdx, size_type lastDataIdx);
  size_type toDataIndex(Handle handle) const noexcept;
  bool isValid(Handle handle) const noexcept;

  void resize(size_type newCapacity);
  void reset() noexcept;

public:
  static constexpr size_type INVALID_INDEX = std::numeric_limits<size_type>::max();

private:
  std::vector<size_type> m_sparseToDense; // 映射表, 稀疏数组到数据索引
  std::vector<size_type> m_denseToSparse; // 数据索引到稀疏数组的反向映射
  std::vector<size_type> m_generations;   // 映射表索引对应句柄的代数, 是 m_sparseToDense 的属性
  std::vector<size_type> m_freeIndices;   // 空闲的映射表索引
};
} // namespace Core
