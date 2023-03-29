#ifndef EPSILON_H
#define EPSILON_H

#include "cache.h"

#include <array>
#include <set>
#include <vector>

#include "canonicalform.h"

namespace epsilon {
class TemporarySwitch
{
private:
  const int switches;
  bool state;
  bool original_state;

public:
  TemporarySwitch(int s, bool o)
    : switches{ s }
    , state{ o }
  {
    original_state = cf_glob_switches.isOn(switches);
    if (state != original_state)
      state ? On(switches) : Off(switches);
  }

  TemporarySwitch(int s, bool o, bool original_state)
    : switches{ s }
    , state{ o }
    , original_state{ original_state }
  {
    if (state != original_state)
      state ? On(switches) : Off(switches);
  }

  ~TemporarySwitch()
  {
    if (state != original_state)
      state ? Off(switches) : On(switches);
  }

  TemporarySwitch(TemporarySwitch const&) = delete;
  TemporarySwitch& operator=(TemporarySwitch const&) = delete;
};

CFFList
sqrFreeFlint(const CanonicalForm& p);

bool
fdividesFlint(const CanonicalForm& f,
              const CanonicalForm& g,
              CanonicalForm& quot);

CanonicalForm
moveFactor(const CanonicalForm& f, const CanonicalForm& g);

// CanonicalForm
// noPowerPoly(const CanonicalForm& p);
CanonicalForm
noPowerPolyOrigin(const CanonicalForm& p);
// inline auto
// noPowerPoly(const CanonicalForm& p)
// {
//   return noPowerPolyOrigin(p);
// }
inline auto noPowerPoly = epsilon::make_memoized(noPowerPolyOrigin, 200000);

CanonicalForm
qFactor(const CanonicalForm& p);

CFFList
noPowerList(const CanonicalForm& p);

namespace trisys {
void
Elim(const CFList& PS,
     const CFList& QS,
     const int i,
     CanonicalForm& T,
     CFList& FS,
     CFList& GS,
     List<Array<CFList>>& DELTA);

void
Elim_2(const CFList& PS,
       const CFList& QS,
       const int i,
       CanonicalForm& T,
       CFList& FS,
       CFList& GS,
       List<Array<CFList>>& DELTA);

void
PriTriSys(const CFList& PS,
          const CFList& QS,
          CFList& TS,
          CFList& US,
          List<Array<CFList>>& OMEGA);

List<Array<CFList>>
TriSer(const CFList& PS, const CFList& QS);

}

namespace sisys {

// CanonicalForm
// moveFactor(const CanonicalForm& f, const CanonicalForm& g);

CanonicalForm
simplify(const CanonicalForm& p, const CFList& US);
CFList
simplify(const CFList& US1, const CFList& US2);

CFList
uCompress(const CanonicalForm& p, const CFList& US2);
CFList
uCompress(const CFList& US1, const CFList& US2);

List<Array<CFList>>
TriSerS(const CFList& PS, const CFList& QS);

} // namespace sisys

} // namespace epsilon

template<typename Container, typename Compare>
OSTREAM&
operator<<(OSTREAM& os, const std::set<Container, Compare>& l)
{
  os << "[ ";
  for (auto iter = l.begin(); iter != l.end();) {
    // iter->print(os);
    os << *iter;
    if (++iter != l.end())
      os << ", ";
  }
  os << " ]";
  return os;
}

template<typename Container>
OSTREAM&
operator<<(OSTREAM& os, const std::tuple<Container, Container>& l)
{
  os << "[ ";
  os << std::get<0>(l);
  os << ", ";
  os << std::get<1>(l);
  os << " ]";
  return os;
}

template<typename Compare>
OSTREAM&
operator<<(OSTREAM& os, const std::set<CanonicalForm, Compare>& l)
{
  os << "{ ";
  for (auto iter = l.begin(); iter != l.end();) {
    iter->print(os);
    if (++iter != l.end())
      os << ", ";
  }
  os << " }";
  return os;
}

template<typename Container>
OSTREAM&
operator<<(OSTREAM& os, const std::vector<Container>& l)
{
  os << "[ ";
  for (auto iter = l.begin(); iter != l.end();) {
    // iter->print(os);
    os << *iter;
    if (++iter != l.end())
      os << ", ";
  }
  os << " ]";
  return os;
}

template<typename Container, std::size_t Nm>
OSTREAM&
operator<<(OSTREAM& os, const std::array<Container, Nm>& l)
{
  os << "[ ";
  for (auto iter = l.begin(); iter != l.end();) {
    // iter->print(os);
    os << *iter;
    if (++iter != l.end())
      os << ", ";
  }
  os << " ]";
  return os;
}

#endif