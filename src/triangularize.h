#ifndef TRIANGULARIZE_H
#define TRIANGULARIZE_H
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
#include "triser_util.h"

#include "tree.h"
#include "triser.h"

namespace epsilon {
// 不要用inline
inline namespace epsilon_cpp17 {

Variable
get_max_var(const CFSet& PS);

template<typename DecompositionAlgorithm>
class Triangularize
{
public:
  using polynomial_set_t = CFSet;
  using polynomial_system_t = std::tuple<polynomial_set_t, polynomial_set_t>;

  [[nodiscard]] auto& get_psi() { return psi_; }
  [[nodiscard]] auto get_psi() const { return psi_; }

  [[nodiscard]] auto& get_alg_impl() { return alg_impl_; }
  [[nodiscard]] auto get_alg_impl() const { return alg_impl_; }

private:
  DecompositionAlgorithm alg_impl_;
  std::set<polynomial_system_t> psi_;
};

template<typename DecompositionAlgorithm>
class TriangularizeWithStl : public Triangularize<DecompositionAlgorithm>
{
public:
  using polynomial_set_t = CFSet;
  using polynomial_system_t = std::tuple<polynomial_set_t, polynomial_set_t>;

  TriangularizeWithStl() = default;

  TriangularizeWithStl(const polynomial_set_t& ps, const polynomial_set_t& qs)
  {
    bool on_rational = isOn(SW_RATIONAL);

    polynomial_set_t ts;
    On(SW_RATIONAL);
    for (const auto& p : ps) {
      ts.insert(bCommonDen(p) * p);
    }

    for (const auto& ts_branch : decom(ts)) {
      phi_.insert({ { simps(ts_branch), qs }, level(get_max_var(ts_branch)) });
    }

    if (on_rational)
      On(SW_RATIONAL);
    else
      Off(SW_RATIONAL);
  }

  bool Elimination()
  {
    while (!phi_.empty()) {
      // std::cout << phi_.size() << std::endl;

      // T2.1
      auto [poly_sys, level] = *phi_.begin();
      phi_.erase(phi_.begin());

      this->get_alg_impl().set_ps(poly_sys);
      this->get_alg_impl().set_level(level);

      // T2.2
      if (auto&& tri_poly_sys = this->get_alg_impl().Eliminate(phi_)) {
        // T2.3
        this->get_psi().insert(*tri_poly_sys);
      }
    }
    return true;
  }

private:
  std::set<std::tuple<polynomial_system_t, int>> phi_;
};

template<typename DecompositionAlgorithm>
class TriangularizeWithTree : public Triangularize<DecompositionAlgorithm>
{
public:
  using polynomial_set_t = CFSet;
  using polynomial_system_t = std::tuple<polynomial_set_t, polynomial_set_t>;

  TriangularizeWithTree() = delete;

  TriangularizeWithTree(const polynomial_set_t& ps, const polynomial_set_t& qs)
    : phi_({ -1, level(get_max_var(ps)), std::make_tuple(ps, qs) })
  {
    if (auto temp = decom(ps); temp.size() > 1) {
      phi_.erase(phi_.begin());
      for (auto& ts : temp) {
        phi_.insert({ { simps(ts), qs }, level(get_max_var(ts)) });
      }
    }
  }

#ifdef WITH_MAPLE
  // 放到maple类中
  ALGEB SetOrdToMaple(bool to_v)
  {
    ALGEB f, dag, ord_maple;
    ord_maple = maple_handle_.AssignMapleStatement("ord", ord_);
    MKernelVector kv = maple_handle_.GetMKernelVector();
    f = to_v ? ToMapleName(kv, "get_ord_v", TRUE)
             : ToMapleName(kv, "get_ord", TRUE);
    dag = EvalMapleProc(kv, f, 1, ord_maple);
    MapleAssign(kv, ord_maple, dag);
    return ord_maple;
  }
#endif

  bool Elimination()
  {
    int num_phi = 0;

#ifdef WITH_MAPLE
    int num_phi_haszero = 0;

    ALGEB has_zeros_maple, ord_v_maple, poly_sys_maple;

    MKernelVector kv = maple_handle_.GetMKernelVector();

    has_zeros_maple = ToMapleName(kv, "HasZeros", TRUE);
    ord_v_maple = SetOrdToMaple(true);
#endif

    while (!phi_.empty()) {
      num_phi++;
      // T2.1
      // auto [poly_sys, level] = *phi_.begin();
      // phi_.erase(phi_.begin());

      auto current_vetex = phi_.begin();
      phi_.set_current_vetex(phi_.begin());
      const auto& current_vertex_property = phi_.get_current_vertex_property();
      auto poly_sys = current_vertex_property.ps;
      auto level = current_vertex_property.level;
      phi_.erase(current_vetex); // 没有真正删除多项式资源

      this->get_alg_impl().set_ps(poly_sys);
      this->get_alg_impl().set_level(level);

#ifdef WITH_MAPLE
      std::cout << poly_sys << std::endl;

      std::ostringstream str;
      str << poly_sys;
      poly_sys_maple =
        maple_handle_.AssignMapleStatement("poly_sys", str.str());

      if (IsMapleProcedure(kv, has_zeros_maple)) {
        ALGEB flag;
        flag =
          EvalMapleProc(kv, has_zeros_maple, 2, ord_v_maple, poly_sys_maple);
        int i = MapleToInteger32(kv, flag); // 1 代表有解
        num_phi_haszero += i;
        std::cout << "flag: " << i << std::endl;
      }
#endif

      // T2.2
      if (auto&& tri_poly_sys = this->get_alg_impl().Eliminate(phi_)) {
        // T2.3
        phi_.get_current_vertex_property().consistent = 1;
        this->get_psi().insert(*tri_poly_sys);
      } else {
        phi_.get_current_vertex_property().consistent = 0;
      }
    }
    
#ifdef WITH_MAPLE
    std::cout << "num_phi_haszero: " << num_phi_haszero << std::endl;
#endif
    std::cout << "num_phi: " << num_phi << std::endl;

    std::ofstream os("./graph.dot");
    phi_.write_graphviz(os);
    return true;
  }

private:
  DecompositionTree phi_;
#ifdef WITH_MAPLE
  std::string ord_;
  Maple& maple_handle_ = Maple::GetMaple();
#endif
};

} // namespace epsilon_cpp17

} // namespace epsilon

#endif