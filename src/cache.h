#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace epsilon {
template<typename Result, typename... Args>
class LRUCache
{
public:
  using args_tuple_type = std::tuple<std::decay_t<Args>...>;

  LRUCache(int capacity = 50) { capacity_ = capacity; }
  //   LRUCache(const LRUCache &) = delete;
  //   LRUCache &operator=(const LRUCache &) = delete;
  ~LRUCache() = default;

  [[nodiscard]] size_t size() const
  {
    // guard
    return map_.size();
  }

  [[nodiscard]] bool empty() const
  {
    // guard
    return map_.empty();
  }

  void clear()
  {
    // guard
    map_.clear();
    cache_.clear();
  }

  std::optional<Result> get(args_tuple_type key)
  {
    // guard
    const auto map_iter = map_.find(key);
    // If key does not exist
    if (map_iter == map_.cend())
      return std::nullopt;

    // Move this key to the front of the cache
    cache_.splice(cache_.begin(), cache_, map_iter->second);
    return std::make_optional(map_iter->second->second);
  }

  void put(args_tuple_type key, Result value)
  {
    // guard
    const auto map_iter = map_.find(key);

    // Key already exists
    if (map_iter != map_.cend()) {
      // Update the value
      map_iter->second->second = value;
      // Move this entry to the front of the cache
      cache_.splice(cache_.begin(), cache_, map_iter->second);
      return;
    }

    // Reached the capacity, remove the oldest entry
    if (cache_.size() == capacity_) {
      const auto& node = cache_.back();
      map_.erase(node.first);
      cache_.pop_back();
    }

    // Insert the entry to the front of the cache and update mapping.
    cache_.emplace_front(key, value);
    map_[key] = cache_.begin();
  }

private:
  int capacity_;

  //定义缓存，因为缓存是可变得，所以要对它得所有修改进行同步

  std::list<std::pair<args_tuple_type, Result>> cache_;
  std::map<args_tuple_type,
           typename std::list<std::pair<args_tuple_type, Result>>::iterator>
    map_;
  // mutable std::recursive_mutex cache_mutex_;
};

namespace detail {

//构造函数中使用得哑类，避免与拷贝构造函数得重载冲突
class null_param
{};

template<class Sig, class F>
class memoize_helper;

template<class Result, class... Args, class F>
class memoize_helper<Result(Args...), F>
{
public:
  using function_type = F;
  using args_tuple_type = std::tuple<std::decay_t<Args>...>;

  //构造函数需要初始化被包装得函数，可以使用拷贝构造函数拷贝缓存得值
  template<typename Function>
  memoize_helper(Function&& f, null_param)
    : f(f)
  {
  }

  memoize_helper(const memoize_helper& other)
    : f(other.f)
  {
    std::cout << "test" << std::endl;
  }

  template<class... InnerArgs>
  Result operator()(InnerArgs&&... args) const
  {
    std::unique_lock<std::recursive_mutex> lock{ cache_mutex_ };

    //搜索缓存得值
    const auto args_tuple = std::make_tuple(args...);
    const auto cached = cache_.find(args_tuple);

    //如果找到则返回
    if (cached != cache_.end()) {
      return cached->second;

    } else {
      //如果没有找到缓存得值，则调用保存结果，传递
      //*this作为第一个参数，递归调用要使用得函数
      auto&& result = f(*this, std::forward<InnerArgs>(args)...);
      cache_[args_tuple] = result;
      return result;
    }
  }

private:
  function_type f;

  //定义缓存，因为缓存是可变得，所以要对它得所有修改进行同步
  mutable std::map<args_tuple_type, Result> cache_;
  mutable std::recursive_mutex cache_mutex_;
};

} // namespace detail

template<class Sig, class F>
detail::memoize_helper<Sig, std::decay_t<F>>
make_memoized_r(F&& f)
{
  return { std::forward<F>(f), detail::null_param{} };
}

template<typename Result, typename... Args>
auto
make_memoized(Result (*f)(Args...), int capacity)
{
  //创建缓存区把参数元组于计算结果进行映射，如果想要用在多线程中，需要用信号量对它得修改进行同步
  // std::map<std::tuple<Args...>, Result> cache;
  LRUCache<Result, Args...> cache(capacity);

  return [f, cache](Args... args) mutable {
    //获取参数，并检查是否已经存在缓存中
    const auto args_tuple = std::make_tuple(args...);
    auto cached = cache.get(args_tuple);
    if (cached) {
      return *cached;
    } else {
      auto result = f(args...);
      cache.put(args_tuple, result);
      return result;
    }
  };
}
}