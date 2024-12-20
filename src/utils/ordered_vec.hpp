#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <concepts>
#include <functional>
#include <random>

template <typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
    { a >= b } -> std::convertible_to<bool>;
    { a <= b } -> std::convertible_to<bool>;
};

template <Comparable T>
class ordered_vec {
private:
    std::vector<T> data_;

public:
    ordered_vec() = default;

    explicit ordered_vec(const std::vector<T>& data);
    void insert(const T& value);
    void remove(const T& value);
    T& at(std::size_t index);
    const T& at(std::size_t index) const;
    bool empty() const noexcept;
    T& operator[](std::size_t index);
    const T& operator[](std::size_t index) const;
    std::size_t size() const noexcept;
    void clear() noexcept;
    std::size_t count(const T& value) const;
    bool contains(const T& value) const;
    const std::vector<T>& raw_data() const noexcept;
    void foreach(std::function<void(const T&)> func) const;
    void reverse();
    void shuffle();
    void sort_ascending();
    void sort_descending();
    void swap(std::size_t index1, std::size_t index2);
    void move_to_index(std::size_t from_index, std::size_t to_index);
    void add_at_index(std::size_t index, const T& value);
    void shuffle_items_at_indexes(const std::vector<std::size_t>& indexes);
};