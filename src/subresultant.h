#ifndef EPSILON_RESULTANT_H
#define EPSILON_RESULTANT_H

#include <array>
#include <set>
#include <vector>

#include "flint/fmpq_mpoly.h"
#include "flint/fmpz_mpoly.h"
#include "flint/mpoly.h"

#include "canonicalform.h"

namespace epsilon {
CFArray
subResChain(const CanonicalForm& f, const CanonicalForm& g, const Variable& x);
CFArray
SubResChain(const CanonicalForm& f, const CanonicalForm& g, const Variable& x);

CFArray
SRS(const CanonicalForm& f, const CanonicalForm& g, const Variable& x);

int
SubResChainFlint_(std::vector<CanonicalForm>& srs,
                  const fmpq_mpoly_t f,
                  const fmpq_mpoly_t g,
                  slong var,
                  const fmpq_mpoly_ctx_t ctx);
int
SubResChainFlint_(std::vector<CanonicalForm>& srs,
                  const fmpz_mpoly_t f,
                  const fmpz_mpoly_t g,
                  slong var,
                  const fmpz_mpoly_ctx_t ctx);
CFArray
SubResChainFlint(const CanonicalForm& f, const CanonicalForm& g, const int var);

}

#endif