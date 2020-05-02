#pragma once

#include <cassert>
#include <limits>
#include <memory>
#include <vector>

#include "spot/algorithm.h"
#include "spot/hash.h"

#define INVALID_INDEX std::numeric_limits<size_t>::max()

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
        return Pack<T>::get()[index];
    }

    bool operator==(const Handle<T>& other) const
    {
        return index == other.index && generation == other.generation;
    }

    bool operator!=(const Handle<T>& other) const
    {
        return !(*this == other);
    }

    /// @return The position of the element in the vecto
    size_t get_index() const
    {
        return index;
    }

    /// @return The generation of this element
    size_t get_generation() const
    {
        return generation;
    }

    /// @brief Invalidate this handle and the main handle of the element
    void invalidate();

    /// @brief Creates a copy of the element
    /// @returns A new handle to the new element
    Handle<T> clone() const;

private:
    /// @param i Index to the element pointed to by the handle
    /// @param g Generation of the handle
    Handle(size_t i, size_t g = 0);

    size_t index = INVALID_INDEX;
    size_t generation = 0;

    friend Pack<T>;
};

template<typename T>
Handle<T>::Handle(size_t i, size_t g)
    : index {i}
    , generation {g}
{
    auto& pack = Pack<T>::get();
    assert(index != INVALID_INDEX && i < pack.len() && "Handle index out of vec bounds");
    assert(generation != INVALID_INDEX && "Generation exhausted");
}

template<typename T>
void Handle<T>::invalidate()
{
    // Invalidate only index
    index = INVALID_INDEX;
    // Generation will be incremented when index
    // position in the Pack will be reused
}

template<typename T>
Handle<T> Handle<T>::clone() const
{
    auto& pack = Pack<T>::get();
    if (*this) {
        // Try finding invalid element
        auto it = find_if(pack, [](auto& e) { return !e.handle; });
        if (it == std::end(pack)) {
            // New elements will have generation 0
            auto& copy = pack.vec.emplace_back(**this);
            size_t new_index = pack.len() - 1;
            copy.handle = Handle<T>(new_index);
            return copy.handle;
        } else {
            auto new_gen = it->handle.get_generation() + 1;
            // Overwrite old element
            (*it) = **this;
            auto same_index = it - std::begin(pack.vec);
            it->handle = Handle<T>(same_index, new_gen);
            return it->handle;
        }
    }

    return {};
}

template<typename T>
Handle<T>::operator bool() const
{
    auto& pack = Pack<T>::get();
    return
        // Check it is within boundaries
        index < pack.len() &&
        // Check validity of main handle
        *this == pack[index].handle;
}

template<typename T>
class Pack
{
public:
    static Pack<T>& get();

    size_t len() const;

    Handle<T> find(size_t i) const;

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

private:
    Pack() = default;

    Pack(Pack&) = delete;
    Pack& operator=(Pack&) = delete;

    Pack(Pack&&) = delete;
    Pack& operator=(Pack&&) = delete;

    std::vector<T> vec;

    friend Handle<T>;
};

template<typename T>
Pack<T>& Pack<T>::get()
{
    static Pack<T> pack;
    return pack;
}

template<typename T>
size_t Pack<T>::len() const
{
    return vec.size();
}

template<typename T>
Handle<T> Pack<T>::find(size_t i) const
{
    Handle<T> ret;
    if (i < len()) {
        ret = vec[i].handle;
    }
    return ret;
}

template<typename T>
template<typename... Args>
Handle<T> Pack<T>::push(Args&&... args)
{
    // Try finding invalid element
    auto it = find_if(vec, [](auto& e) { return !e.handle; });
    if (it == std::end(vec)) {
        // New elements will have generation 0
        auto& elem = vec.emplace_back(std::forward<Args>(args)...);
        elem.handle = Handle<T>(len() - 1);
        return elem.handle;
    } else {
        auto new_gen = it->handle.get_generation() + 1;
        // Overwrite old element
        (*it) = std::move(T(std::forward<Args>(args)...));
        auto same_index = it - std::begin(vec);
        it->handle = Handle<T>(same_index, new_gen);
        return it->handle;
    }
}

template<typename T>
class Handled
{
public:
    virtual ~Handled() = default;

    virtual void invalidate()
    {
        handle.invalidate();
    }

    /// @brief Main handle of the element. It becomes invalid when the element can be overwritten
    Handle<T> handle;
};

} // namespace spot

namespace std
{
template<typename T>
struct hash<spot::Handle<T>> {
    size_t operator()(const spot::Handle<T>& handle) const
    {
        return hash_combine(handle.get_index(), handle.get_generation());
    }
};

} // namespace std
