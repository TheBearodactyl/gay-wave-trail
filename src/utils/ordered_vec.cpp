#include "ordered_vec.hpp"
#include <random>

#if defined(__APPLE__)
    #include <TargetConditionals.h>
#endif

#if defined(__ANDROID__)
    #include <android/log.h>
#endif

template <Comparable T>
ordered_vec<T>::ordered_vec(const std::vector<T> &data) {
  data_ = data;
  std::sort(data_.begin(), data_.end());
}

template <Comparable T>
void ordered_vec<T>::insert(const T &value) {
  auto it = std::lower_bound(data_.begin(), data_.end(), value);
  data_.insert(it, value);
}

template <Comparable T>
void ordered_vec<T>::remove(const T &value) {
  auto it = std::lower_bound(data_.begin(), data_.end(), value);
  if (it != data_.end() && *it == value) {
    data_.erase(it);
  } else {
    throw std::runtime_error("Value not found in ordered_vec");
  }
}

template <Comparable T>
T &ordered_vec<T>::at(std::size_t index) {
  if (index >= data_.size()) {
    throw std::out_of_range("Index out of range");
  }

  return data_.at(index);
}

template <Comparable T>
const T &ordered_vec<T>::at(std::size_t index) const {
  if (index >= data_.size()) {
    throw std::out_of_range("Index out of range");
  }

  return data_.at(index);
}

template <Comparable T>
T &ordered_vec<T>::operator[](std::size_t index) {
  if (index >= data_.size()) {
    throw std::out_of_range("Index out of range");
  }

  return data_[index];
}

template <Comparable T>
const T &ordered_vec<T>::operator[](std::size_t index) const {
  if (index >= data_.size()) {
    throw std::out_of_range("Index out of range");
  }

  return data_[index];
}

template <Comparable T>
std::size_t ordered_vec<T>::size() const noexcept {
  return data_.size();
}

template <Comparable T>
bool ordered_vec<T>::empty() const noexcept {
  return data_.empty();
}

template <Comparable T>
void ordered_vec<T>::clear() noexcept {
  data_.clear();
}

template <Comparable T>
std::size_t ordered_vec<T>::count(const T &value) const {
  return std::count(data_.begin(), data_.end(), value);
}

template <Comparable T>
bool ordered_vec<T>::contains(const T &value) const {
  return std::binary_search(data_.begin(), data_.end(), value);
}

template <Comparable T>
const std::vector<T> &ordered_vec<T>::raw_data() const noexcept {
  return data_;
}

template <Comparable T>
void ordered_vec<T>::foreach (std::function<void(const T &)> func) const {
  for (const auto &item: data_) {
    func(item);
  }
}

template <Comparable T>
void ordered_vec<T>::reverse() {
  std::reverse(data_.begin(), data_.end());
}

template <Comparable T>
void ordered_vec<T>::shuffle() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(data_.begin(), data_.end(), gen);
}

template <Comparable T>
void ordered_vec<T>::sort_ascending() {
  std::sort(data_.begin(), data_.end());
}

template <Comparable T>
void ordered_vec<T>::sort_descending() {
  std::sort(data_.begin(), data_.end(), std::greater<T>());
}

template <Comparable T>
void ordered_vec<T>::swap(std::size_t index1, std::size_t index2) {
  if (index1 >= data_.size() || index2 >= data_.size()) {
    throw std::out_of_range("Index out of range");
  }
  std::swap(data_[index1], data_[index2]);
}

template <Comparable T>
void ordered_vec<T>::move_to_index(std::size_t from_index, std::size_t to_index) {
  if (from_index >= data_.size() || to_index >= data_.size()) {
    throw std::out_of_range("Index out of range");
  }
  T value = data_[from_index];
  data_.erase(data_.begin() + from_index);
  data_.insert(data_.begin() + to_index, value);
}

template <Comparable T>
void ordered_vec<T>::add_at_index(std::size_t index, const T &value) {
  if (index > data_.size()) {
    throw std::out_of_range("Index out of range");
  }
  data_.insert(data_.begin() + index, value);
}

template <Comparable T>
void ordered_vec<T>::shuffle_items_at_indexes(const std::vector<std::size_t> &indexes) {
  std::vector<T> temp;
  for (auto index: indexes) {
    if (index >= data_.size()) {
      throw std::out_of_range("Index out of range");
    }
    temp.push_back(data_[index]);
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(temp.begin(), temp.end(), gen);
  for (std::size_t i = 0; i < indexes.size(); ++i) {
    data_[indexes[i]] = temp[i];
  }
}