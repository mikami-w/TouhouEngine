#include "SAPHandleSystem.hpp"

#include <numeric>

#include "Logger.hpp"

namespace Core {

SAPHandleSystem::SAPHandleSystem(size_type capacity)
{
  m_sparseToDense.resize(capacity);
  m_denseToSparse.resize(capacity);
  m_generations.resize(capacity, 1);
  m_freeIndices.reserve(capacity);
  for (size_type i = 0; i < capacity; ++i) {
    // 逆序填入 capacity - 1, ..., 1, 0
    m_freeIndices.push_back(capacity - 1 - i);
  }
}

SAPHandleSystem::Handle SAPHandleSystem::createHandle(size_type dataIdx)
{
  // 找到一个映射表中可用的位置, 优先复用空闲索引, 否则在末尾创建新条目
  size_type logicalIdx = m_freeIndices.back();
  m_freeIndices.pop_back();

  // 建立双向映射
  m_sparseToDense[logicalIdx] = dataIdx;
  m_denseToSparse[dataIdx] = logicalIdx;

  return { logicalIdx, m_generations[logicalIdx] };
}

void SAPHandleSystem::notifySwapAndPop(size_type destroyedDataIdx, size_type lastDataIdx)
{
  // 作废被删除的句柄
  size_type deadLogicalIdx = m_denseToSparse[destroyedDataIdx];
  ++m_generations[deadLogicalIdx]; // 增加代数使旧句柄失效
  m_freeIndices.push_back(deadLogicalIdx);

  // 如果发生了移动 (即删除的不是数组最后一个元素)
  if (destroyedDataIdx != lastDataIdx) {
    size_type movedLogicalIdx = m_denseToSparse[lastDataIdx]; // 找到被移动数据的映射表索引
    m_sparseToDense[movedLogicalIdx] = destroyedDataIdx;      // 更新它的新物理住址
    m_denseToSparse[destroyedDataIdx] = movedLogicalIdx;      // 更新物理数组对应的逻辑柜号
  }
}

SAPHandleSystem::size_type SAPHandleSystem::toDataIndex(Handle handle) const noexcept
{
#if defined(_DEBUG)
  if (!isValid(handle)) {
    LOG_WARN("Invalid handle was attempted to be accessed.");
  }
#endif

  return isValid(handle) ? m_sparseToDense[handle.index] : INVALID_INDEX;
}

bool SAPHandleSystem::isValid(Handle handle) const noexcept
{
  return handle.index < m_sparseToDense.size()                // 索引有效
         && m_generations[handle.index] == handle.generation; // 代数匹配
}

void SAPHandleSystem::resize(size_type newCapacity)
{
  size_type oldCapacity = m_sparseToDense.size();
  if (newCapacity <= oldCapacity) {
    return; // 不允许缩小容量
  }

  m_sparseToDense.resize(newCapacity);
  m_denseToSparse.resize(newCapacity);
  m_generations.resize(newCapacity, 1);

  std::vector<size_type> newFreeIndices;
  newFreeIndices.reserve(newCapacity);
  // 逆序填充新增部分索引
  for (size_type i = 0; i < newCapacity - oldCapacity; ++i) {
    newFreeIndices.push_back(newCapacity - 1 - i);
  }
  newFreeIndices.insert(newFreeIndices.end(), m_freeIndices.begin(), m_freeIndices.end());
  m_freeIndices = std::move(newFreeIndices);
}

void SAPHandleSystem::reset() noexcept
{
  std::iota(m_freeIndices.rbegin(), m_freeIndices.rend(), 0); // 重新逆向填充空闲索引
  for (size_type i = 0; i < m_sparseToDense.size(); ++i) {
    ++m_generations[i]; // 增加代数，确保持有的旧 Handle 全部失效
  }
}
} // namespace Core
