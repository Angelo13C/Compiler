#pragma once

#include <cstdlib>
#include <utility>
#include <algorithm>
#include <memory>

class ArenaAllocator
{
public:
    ArenaAllocator(std::size_t blockSize);
    ArenaAllocator(ArenaAllocator&& other) noexcept;
    ArenaAllocator& operator=(ArenaAllocator&& other) noexcept
    {
        std::swap(blockSize, other.blockSize);
        std::swap(block, other.block);
        std::swap(currentOffset, other.currentOffset);
        return *this;
    }

    ArenaAllocator(const ArenaAllocator&) = delete;
    ArenaAllocator& operator= (const ArenaAllocator&) = delete;

    ~ArenaAllocator();

    template<typename T>
    T* allocate()
    {
        auto pointer = static_cast<void*>((char*) block + currentOffset);
        auto space = remainingFreeSpace();
        if (std::align(alignof(T), sizeof(T), pointer, space)) {
            currentOffset = static_cast<char*>(pointer) - static_cast<char*>(block) + sizeof(T);
            return static_cast<T*>(pointer);
        }
        return nullptr;
    }

    template<typename T, typename... Args>
    T* allocate_and_initialize(Args&&... args)
    {
        T* allocatedData = allocate<T>();
        if(allocatedData != nullptr)
            new (allocatedData) T(std::forward<Args>(args)...);

        return allocatedData;
    }

private:
    size_t remainingFreeSpace();

private:
    std::size_t blockSize;
    void* block;
    std::size_t currentOffset;
};