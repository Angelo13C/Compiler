#include "arena_allocator.hpp"

ArenaAllocator::ArenaAllocator(std::size_t blockSize) : blockSize(blockSize), currentOffset(0)
{
    block = static_cast<void*>(new char[blockSize]);
}

ArenaAllocator::ArenaAllocator(ArenaAllocator&& other) noexcept : blockSize { std::exchange(other.blockSize, 0) }, 
    block { std::exchange(other.block, nullptr) }, currentOffset { std::exchange(other.currentOffset, 0) }
{
}

ArenaAllocator::~ArenaAllocator()
{
    delete[] block;
}

size_t ArenaAllocator::remainingFreeSpace()
{
    return blockSize - currentOffset;
}
