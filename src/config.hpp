#pragma once

#ifndef ONSCRIPTER_CONFIG_H
#define ONSCRIPTER_CONFIG_H

#include <array>
#include <functional>
#include <list>
#include <memory>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#ifdef INFRA_FORCE_GHC_FS
#include <ghc/filesystem.hpp>
#else
#include <filesystem>
#endif

namespace onscripter {
template <typename T>
using Vector = std::vector<T>;

template <typename T, size_t N = 1>
using Array = std::array<T, N>;

template <typename T>
using Stack = std::stack<T>;

template <typename T>
using List = std::list<T>;

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template <typename Key, typename Value>
using UnorderedMultimap = std::unordered_multimap<Key, Value>;

template <typename Key, typename Value>
using UnorderedMap = std::unordered_map<Key, Value>;

template <typename Key, typename Value>
using SmallUnorderedMap = UnorderedMap<Key, Value>;

template <typename T>
using UnorderedSet = std::unordered_set<T>;

template <typename T>
using SmallUnorderedSet = std::unordered_set<T>;

template <typename T>
using SmallOrderedSet = std::set<T>;

template <typename T>
using Hash = std::hash<T>;

template <typename T>
using Function = std::function<T>;

using String = std::string;

// Smart pointer types.
template <typename T>
using UniquePtr = std::unique_ptr<T>;

template <typename T>
class Releaser;

template <typename T>
using UniqueReleaserPtr = std::unique_ptr<T, Releaser<T>>;

template <typename T>
using SharedPtr = std::shared_ptr<T>;

template <typename T>
using WeakPtr = std::weak_ptr<T>;

template <typename T, typename... Args>
inline SharedPtr<T> MakeShared(Args&&... args) {
    return std::make_shared<T, Args...>(std::forward<Args>(args)...);
}
template <typename T, typename... Args>
inline UniquePtr<T> MakeUnique(Args&&... args) {
    return std::make_unique<T, Args...>(std::forward<Args>(args)...);
}

#define var_defer__(x) defer__ ## x
#define var_defer_(x) var_defer__(x)
#define defer(ops) onscripter::Defer var_defer_(__COUNTER__)(ops)
#define ref_defer(ops) onscripter::Defer var_defer_(__COUNTER__)([&]{ ops; }) // Capture all by ref
#define val_defer(ops) onscripter::Defer var_defer_(__COUNTER__)([=]{ ops; }) // Capture all by val
#define none_defer(ops) onscripter::Defer var_defer_(__COUNTER__)([]{ ops; }) // Capture nothing

struct Defer {
    std::function<void(void)> action;
    Defer(const std::function<void(void)>& act);
    Defer(const std::function<void(void)>&& act);
    Defer(const Defer& act) = delete;
    Defer& operator=(const Defer& act) = delete;
    Defer(Defer&& act) = delete;
    Defer& operator=(Defer&& act) = delete;
    ~Defer();
};

#ifdef INFRA_FORCE_GHC_FS
namespace fs = ghc::filesystem;
#else
namespace fs = std::filesystem;
#endif

};  // namespace onscripter

#endif
