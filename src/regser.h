#ifndef REGSER_H
#define REGSER_H

#include "triangularize.h"
#include "triser.h"

namespace epsilon {
inline namespace epsilon_cpp17 {

// TODO
class RegSer : public TriangularizeWithStl<TriSer>
{
public:
  using polynomial_set_t = CFSet;
  using polynomial_system_t = std::tuple<polynomial_set_t, polynomial_set_t>;

  using TriangularizeWithStl::TriangularizeWithStl;
};

} // namespace epsilon_cpp17

} // namespace epsilon

#endif