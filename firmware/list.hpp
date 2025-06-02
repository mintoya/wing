#ifndef LISTPLUX_HPP
#define LISTPLUX_HPP

#include "list.h"
#include <stdexcept>
#include <functional>
#include <type_traits>
// completeley written by chatgpt so idk

template <typename T>
class ListPlux {
private:
    ::List* c_list;  // Pointer to underlying C List structure

    // Validate index and return pointer to element
    T* ptr_at(size_t index) const {
        if (index >= c_list->length) {
            throw std::out_of_range("ListPlux index out of range");
        }
        return static_cast<T*>(static_cast<void*>(
            static_cast<char*>(c_list->head) + index * c_list->width));
    }

public:
    // Constructors
    explicit ListPlux(size_t initialSize = 0) {
        c_list = List_new(sizeof(T));
        if (initialSize > 0) {
            resize(initialSize);
        }
    }

    // Move constructor
    ListPlux(ListPlux&& other) noexcept : c_list(other.c_list) {
        other.c_list = nullptr;
    }

    // Destructor
    ~ListPlux() {
        if (c_list) List_delete(c_list);
    }

    // Disallow copying
    ListPlux(const ListPlux&) = delete;
    ListPlux& operator=(const ListPlux&) = delete;

    // Move assignment
    ListPlux& operator=(ListPlux&& other) noexcept {
        if (this != &other) {
            if (c_list) List_delete(c_list);
            c_list = other.c_list;
            other.c_list = nullptr;
        }
        return *this;
    }

    // Element access
    T& operator[](size_t index) { 
        return *ptr_at(index);
    }

    const T& operator[](size_t index) const { 
        return *ptr_at(index);
    }

    T& at(size_t index) { 
        return *ptr_at(index);
    }

    const T& at(size_t index) const { 
        return *ptr_at(index);
    }

    // Capacity
    size_t size() const noexcept { 
        return c_list ? c_list->length : 0; 
    }
    
    size_t capacity() const noexcept { 
        return c_list ? c_list->size : 0; 
    }
    
    bool empty() const noexcept { 
        return size() == 0; 
    }

    // Modifiers
    void resize(size_t newSize) {
        List_resize(c_list, newSize);
    }

    void push_back(const T& value) {
        List_append(c_list, const_cast<void*>(static_cast<const void*>(&value)));
    }

    void insert(size_t index, const T& value) {
        List_insert(c_list, index, const_cast<void*>(static_cast<const void*>(&value)));
    }

    void erase(size_t index) {
        List_remove(c_list, index);
    }

    void clear() {
        resize(0);
    }

    void zeroOut() {
        List_zeroOut(c_list);
    }

    // Iterators
    T* begin() noexcept { 
        return static_cast<T*>(c_list->head);
    }

    const T* begin() const noexcept { 
        return static_cast<const T*>(c_list->head);
    }

    T* end() noexcept { 
        return begin() + size();
    }

    const T* end() const noexcept { 
        return begin() + size();
    }

    // Functional operations
    template <typename UnaryPredicate>
    void filter(UnaryPredicate predicate) {
        size_t write_index = 0;
        for (size_t i = 0; i < size(); ++i) {
            if (predicate((*this)[i])) {
                if (write_index != i) {
                    (*this)[write_index] = (*this)[i];
                }
                ++write_index;
            }
        }
        resize(write_index);
    }

    // Debug/utility
    void prettyPrint() const {
        List_prettyPrint(c_list);
    }

    void printAsString() const {
        List_print_as_string(c_list);
    }

    // Raw access (use with caution)
    ::List* raw_list() noexcept { 
        return c_list; 
    }
    
    const ::List* raw_list() const noexcept { 
        return c_list; 
    }
};

// Range-based for loop support
template <typename T>
T* begin(ListPlux<T>& list) { return list.begin(); }

template <typename T>
const T* begin(const ListPlux<T>& list) { return list.begin(); }

template <typename T>
T* end(ListPlux<T>& list) { return list.end(); }

template <typename T>
const T* end(const ListPlux<T>& list) { return list.end(); }

#endif // LISTPLUX_HPP