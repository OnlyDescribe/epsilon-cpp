#ifndef EPSILON_POLYNOMIAL_H
#define EPSILON_POLYNOMIAL_H
#include <memory>
#include <tuple>

#include <flint/flint_exception.h>
#include <flint/fmpq_mpoly.h>

namespace epsilon {
std::tuple<int> i = 1;

struct fmpq_mpoly_data
{
  using fmpq_mpoly_ctx_sptr = std::shared_ptr<fmpq_mpoly_ctx_struct>;
  //   typedef fmpq_mpoly_t& data_ref_t;
  //   typedef const fmpq_mpoly_t& data_srcref_t;

  fmpq_mpoly_t inner_;
  fmpq_mpoly_ctx_sptr ctx_; //   fmpq_mpoly_ctx_t ctx;

  fmpq_mpoly_data(slong nvars)
  {
    fmpq_mpoly_ctx_init(ctx_.get(), nvars, ORD_LEX);
    fmpq_mpoly_init(inner_, ctx_.get());
  }
  fmpq_mpoly_data(const fmpq_mpoly_ctx_sptr& ctx)
  {
    ctx_ = ctx;
    fmpq_mpoly_init(inner_, ctx_.get());
  }
  fmpq_mpoly_data(slong alloc, const fmpq_mpoly_ctx_sptr& ctx)
  {
    ctx_ = ctx;
    fmpq_mpoly_init2(inner_, alloc, ctx_.get());
  }

  fmpq_mpoly_data(const char* str,
                  const char** x,
                  const fmpq_mpoly_ctx_sptr& ctx)
  {
    fmpq_mpoly_init(inner_, ctx.get());
    flint::execution_check(
      !fmpq_mpoly_set_str_pretty(inner_, str, x, ctx.get()),
      "construct from string",
      "fmpq_polyxx");
  }

  fmpq_mpoly_data(fmpq_mpoly_t mpoly, const fmpq_mpoly_ctx_sptr& ctx)
  {
    *inner_ = *mpoly;
    ctx_ = ctx;
  }

  ~fmpq_mpoly_data() { fmpq_mpoly_clear(inner_, ctx_.get()); }

  fmpq_mpoly_data(const fmpq_mpoly_data& o)
  {
    ctx_ = o.ctx_;
    fmpq_mpoly_init(inner_, ctx_.get());
    fmpq_mpoly_set(inner_, o.inner_, o.ctx_.get());
  }
};

template<class Derived, class Operation, class Data>
class expression
{
private:
  Data poly_;
};

template<class Operation, class Data>
class fmpq_mpolyxx_expression
  : public expression<fmpq_mpolyxx_expression<Operation, Data>, Operation, Data>
{

public:
  fmpq_mpoly_t& poly_() { return this->_data().inner; }
  const fmpq_mpoly_t& poly_() const { return this->_data().inner; }

public:
  static fmpq_mpolyxx_expression randtest(fmpq_mpolyxx_expression e1,
                                          fmpq_mpolyxx_expression e2)
  {
    fmpq_mpolyxx_expression res;
    // fmpq_poly_resultant(to._fmpq(), e1._poly(), e2._poly();
    return res;
  }

private:
  int i;
};

}
#endif