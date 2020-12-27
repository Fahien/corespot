#pragma once

#include <cassert>
#include <limits>
#include <memory>
#include <vector>

#include "spot/algorithm.h"
#include "spot/hash.h"

namespace spot
{
template<typename T>
class Pack;

/// @brief The handle class is used to solve the problem of using
/// pointers to elements of a vector which can become dangling when
/// the vector gets resized and the element gets moved somwhere else.
/// The handle stores a reference of that vector and the index of the
/// element it should point to. Using the index means that elements
/// of the vector should never be erased, or a handle might end up
/// referring to a different element.
template<typename T>
class Handle
{
public:
    /// @brief Constucts an invalid handle
    Handle() = default;

    /// @return Whether the handle is valid or not
    explicit operator bool() const;

    T* operator->() const
    {
        return &**this;
    }

    T& operator*() const
    {
        auto& pack = Pack<T>::get();
        auto& gen = pack.indices[id];
        return pack.vec[gen.index];
    }

    bool operator==(const Handle<T>& other) const
    {
        return id == other.id && generation == other.generation;
    }

    bool operator!=(const Handle<T>& other) const
    {
        return !(*this == other);
    }

    /// @return The id of this handle
    size_t get_id() const
    {
        return id;
    }

    size_t get_generation() const
    {
        return generation;
    }

    /// @brief Invalidate this handle and the main handle of the element
    void invalidate();

    /// @brief Creates a copy of the element
    /// @return A new handle to the new element
    Handle<T> clone() const;

private:
    /// @param id ID of this handle is its position within Pack indices
    Handle(size_t id, size_t generation = 0);

    /// ID is its index within the list of handles of a Pack
    size_t id = ULONG_MAX;

    size_t generation = 0;

    friend Pack<T>;
};

template<typename T>
Handle<T>::Handle(size_t id, size_t generation)
    : id {id}
    , generation {generation}
{
    auto& pack = Pack<T>::get();
    assert(id != ULONG_MAX && id < pack.indices.size() + 1 && "Handle ID out of indices bounds");
}

template<typename T>
void Handle<T>::invalidate()
{
    // Invalidate main handle
    auto& pack = Pack<T>::get();
    // Remove element
    pack.remove(*this);
}

template<typename T>
Handle<T> Handle<T>::clone() const
{
    return Pack<T>::get().push(**this);
}

template<typename T>
Handle<T>::operator bool() const
{
    auto& pack = Pack<T>::get();
    return
        // Check it is within indices boundaries
        id < pack.indices.size() &&
        // Check validity of generational index
        pack.indices[id] &&
        // Check they are same generation
        generation == pack.indices[id].generation;
}

struct Gen {
    size_t index = ULONG_MAX;
    size_t generation = 0;

    explicit operator bool() const;
};

Gen::operator bool() const
{
    return index != ULONG_MAX;
}

template<typename T>
class Pack
{
public:
    static Pack<T>& get();

    size_t size() const;

    /// @return Whether the current id is pointing to a valid generational index
    Handle<T> find(size_t id) const;

    typename std::vector<T>::iterator begin()
    {
        return vec.begin();
    }

    typename std::vector<T>::iterator end()
    {
        return vec.end();
    }

    typename std::vector<T>::const_iterator begin() const
    {
        return vec.begin();
    }

    typename std::vector<T>::const_iterator end() const
    {
        return vec.end();
    }

    template<class... Args>
    Handle<T> push(Args&&... args);

    T& operator[](size_t index)
    {
        return vec[index];
    }

    const T& operator[](size_t index) const
    {
        return vec[index];
    }

    void remove(const Handle<T>& handle);

private:
    Pack() = default;

    Pack(Pack&) = delete;
    Pack& operator=(Pack&) = delete;

    Pack(Pack&&) = delete;
    Pack& operator=(Pack&&) = delete;

    /// List of contiguous elements
    std::vector<T> vec;

    /// List of generational indices to elements
    std::vector<Gen> indices;

    /// List of positions to free indices
    std::vector<size_t> free;

    friend Handle<T>;
};

template<typename T>
Pack<T>& Pack<T>::get()
{
    static Pack<T> pack;
    return pack;
}

template<typename T>
size_t Pack<T>::size() const
{
    return vec.size();
}

template<typename T>
Handle<T> Pack<T>::find(size_t id) const
{
    return Handle<T>(id);
}

template<typename T>
template<typename... Args>
Handle<T> Pack<T>::push(Args&&... args)
{
    auto index = vec.size();
    auto& elem = vec.emplace_back(std::forward<Args>(args)...);

    size_t id;
    if (free.empty()) {
        id = indices.size();
        auto gen = Gen {index, 0};
        // Id is an index into the indices vector
        indices.emplace_back(std::move(gen));
    } else {
        id = free.back();
        free.pop_back();
        // Update the index pointed to by this handle
        // @todo Move generation here?
        indices[id].index = index;
        indices[id].generation++;
    }

    return Handle<T>(id, indices[id].generation);
}

template<typename T>
void Pack<T>::remove(const Handle<T>& handle)
{
    auto& gen = indices[handle.id];
    auto last_index = vec.size() - 1;
    std::swap(vec[gen.index], vec[last_index]);
    vec.pop_back();

    // Update index that was pointing to last element
    // We do not know where it is, therefore let us find it
    auto it = find_if(indices, [last_index](Gen& g) { return g.index == last_index; });
    assert(it != std::end(indices) && "Failed to update index for swapped element");
    it->index = gen.index;

    // Invalidate index only as generation will be
    // incremented when index position in the Pack will be reused
    gen.index = ULONG_MAX;

    // Index of the removed element can be added to free list
    free.emplace_back(handle.id);
}

} // namespace spot

namespace std
{
template<typename T>
struct hash<spot::Handle<T>> {
    size_t operator()(const spot::Handle<T>& handle) const
    {
        return hash_combine(handle.get_id(), handle.get_generation());
    }
};

} // namespace std
