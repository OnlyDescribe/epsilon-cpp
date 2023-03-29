#ifndef TRISER_H
#define TRISER_H

#include <array>
#include <bits/utility.h>
#include <functional>
#include <list>
#include <optional>
#include <set>
#include <tuple>
#include <utility>
#include <vector>

#include "canonicalform.h"
#include "cfCharSetsUtil.h"

#include "maple.h"
#include "mplshlib.h"

#include "epsilon.h"
#include "subresultant.h"
#include "tree.h"
#include "triser_util.h"

namespace epsilon {
// 不用inline. 之后cpp改成cxx, 定义放在本文件中
inline namespace epsilon_cpp17 {

using CFSet = std::set<CanonicalForm, std::greater<>>;

CFSet
selectWithLevel(const CFSet& PS, const int level, CanonicalForm& T_mindeg);
std::tuple<CFSet, CFSet, CanonicalForm, bool>
selectWithLevel(const CFSet& PS, const int level);

std::optional<CanonicalForm>
simps1(const CFSet& ps_1);

std::set<std::array<CFSet, 2>>
TriSer_Alg(const CFSet& PS, const CFSet& QS, const std::string& thread_name);

void
TriSer_STD_MultiThread(const CFSet& PS, const CFSet& QS);

// PImpl 放在BASE中
class TriSer
{
public:
  using polynomial_set_t = CFSet;
  using polynomial_system_t = std::tuple<polynomial_set_t, polynomial_set_t>;

  static constexpr bool FACTORIZED = false;

  void set_level(int level) { level_ = level; }

  [[nodiscard]] int get_level() const { return level_; }

  void set_ps(const polynomial_system_t& ps) { std::tie(ts_, us_) = ps; }

  [[nodiscard]] CanonicalForm SelectLevelPoly(const polynomial_set_t& ps) const;

  bool SelectWithLevel();

  bool MakeTriangularSystemFine();

  bool SimplifyTsWithTs1(); // 优化策略可以封装为函数式的命令模式

  // 可能可以优化,观察 nozero.log
  template<typename Container>
  CanonicalForm SplitStrategy1(Container& phi, int k);

  template<typename Container>
  void SplitStrategy2(Container& phi, int k, int r);

  template<typename Container>
  bool EliminateTsK(Container& phi, int k);

  template<typename Container>
  std::optional<std::tuple<CFSet, CFSet>> Eliminate(Container& phi)
  {
    ts_.erase(CanonicalForm(0));
    us_.erase(CanonicalForm(1));

    // T2.2
    for (; level_ > 0; --level_) {
      // std::cout << "level: " << level_ << std::endl;
      if (SelectWithLevel()) {
        // std::cout << "SimplifyTsWithTs1: " << std::endl;
        if (SimplifyTsWithTs1()) {
          // T2.2.1
          // std::cout << "EliminateTsK: " << std::endl;
          if (!EliminateTsK(phi, level_))
            return std::nullopt;
          // T2.2.2
          // std::cout << "MakeTriangularSystemFine: " << std::endl;
          if (!MakeTriangularSystemFine())
            return std::nullopt;
          // T2.2.3
          if (us_.count(0) > 0) {
            return std::nullopt;
          }
          for (const auto& f : ts_) {
            if (f.inQ() && !f.isZero()) {
              return std::nullopt;
            }
          }
          // std::cout << "simplify: " << std::endl;
          ts_ = simplify<true>(ts_, us_);

        } else {
          return std::nullopt;
        }
      } else {
        return std::nullopt;
      }
    }
    // std::cout << "filterResult: " << std::endl;
    return std::make_optional(filter(
      CFSet::key_compare(), ts_.begin(), ts_.end(), us_.begin(), us_.end()));
    // return std::make_optional(std::make_tuple(ts_, us_));
  }

private:
  CFSet ts_, us_;
  CanonicalForm p2_, p1_;
  int level_;
  CFSet ts_k_, ts_1_;
  CFArray hs_, is_;
};

} // namespace epsilon_cpp17

} // namespace epsilon

#endif