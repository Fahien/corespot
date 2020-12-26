#pragma once

#include <algorithm>
#include <limits>
#include <memory>
#include <vector>

namespace spot
{
/// @brief The handle class is used to solve the problem of using
/// pointers to elements of a vector which can become dangling when
/// the vector gets resized and the element gets moved somwhere else.
/// The handle stores a reference of that vector and the index of the
/// elements it should point to. Using the index means that elements
/// of the vector should never be erased, or a handle might end up
/// referring to a different element.
template<typename T>
class Handle
{
public:
    /// @return The vector containing all the handled elements
    static std::vector<T>& get_vec();

    /// @brief Constucts an invalid handle
    Handle() = default;

    /// @param i Index to the element pointed to by the handle
    Handle(size_t i, size_t g = 0);

    /// @brief Invalidate this handle and the main handle of the element
    void invalidate();

    /// @return Whether the handle is valid or not
    explicit operator bool() const;

    T* operator->() const
    {
        return &**this;
    }
    T& operator*() const
    {
        assert(*this && "Handle is not valid");
        return get_vec()[index];
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

private:
    size_t index = std::numeric_limits<size_t>::max();
    size_t generation = 0;
};

template<typename T>
std::vector<T>& Handle<T>::get_vec()
{
    static std::vector<T> vec;
    return vec;
}

template<typename T>
Handle<T>::Handle(size_t i, size_t g)
    : index {i}
    , generation {g}
{
    assert(index != std::numeric_limits<size_t>::max() && i < get_vec().size() &&
        "Handle index out of vec bounds");
    assert(generation != std::numeric_limits<size_t>::max() && "Generation exhausted");
}

template<typename T>
void Handle<T>::invalidate()
{
    if (*this) {
        (*this)->handle.index = std::numeric_limits<size_t>::max();
        index = std::numeric_limits<size_t>::max();
    }
}

template<typename T>
Handle<T>::operator bool() const
{
    return index != std::numeric_limits<size_t>::max() && index < get_vec().size() &&
        // Check validity of main handle
        *this == get_vec()[index].handle;
}

template<typename T>
class Handled
{
public:
    using Handle = Handle<T>;

    template<class... Args>
    static Handle create(Args&&... args)
    {
        auto& vec = Handle::get_vec();
        // Try finding invalid element
        auto it = std::find_if(std::begin(vec), std::end(vec), [](auto& e) { return !e.handle; });
        if (it == std::end(vec)) {
            // New elements will have generation 0
            auto& elem = vec.emplace_back(std::forward<Args>(args)...);
            elem.handle = Handle(vec.size() - 1);
            return elem.handle;
        } else {
            auto new_gen = it->handle.get_generation() + 1;
            // Overwrite old element
            (*it) = T(std::forward<Args>(args)...);
            auto same_index = it - std::begin(vec);
            it->handle = Handle(same_index, new_gen);
            return it->handle;
        }
    }

    virtual ~Handled() = default;

    /// @brief Main handle of the element. It becomes invalid when the element can be overwritten
    Handle handle;
};

} // namespace spot

namespace std
{
template<typename T>
struct hash<spot::Handle<T>> {
    size_t operator()(const spot::Handle<T>& handle) const
    {
        return handle.get_index() ^ handle.get_generation();
    }
};

} // namespace std
