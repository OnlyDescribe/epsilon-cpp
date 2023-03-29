
#include <chrono>

#include "canonicalform.h"
#include "cf_algorithm.h"

#define TIMING
#include "factory/timing.h"

#include "Tracy.hpp"

#include "convert.h"
#include "epsilon.h"
#include "subresultant.h"
#include "test.h"

// #include "templates/ftmpl_array.cc"
// #include "templates/ftmpl_list.cc"

TIMING_DEFINE(PROC)
// TIMING_DEFINE(SubResltantChain)
#define PRINT(X) std::cout << #X << ": " << (X) << std::endl;

/* ----------------------------------------------------------------- */
namespace epsilon {
static CanonicalForm
operator^(const Variable& v, int exp)
{
  return power(v, exp);
}
}

int
epsilon::SubResltantChain_Test()
{
#if 1
  Variable x('x'), y('y'), z('z');
  // CanonicalForm f1((z - 1) * ((y ^ 2) + 6) * (x - 1) - 5 * y * ((x ^ 7) +
  // 1)); CanonicalForm f2((z + 1) * ((x ^ 7) + 6) * (y - 1) - 7 * x * ((y ^ 3)
  // + 1));

  // Variable x('x');
  // CanonicalForm f1(2 * power(x, 400) + power(x, 3) - 3 * power(x, 2) - x +
  // 1);
  // CanonicalForm f2(power(x, 2) + 4 * power(x, 1) + 3);

  CanonicalForm f1 = *convStringPFactoryP(
    "(z - 1)*(y^2 + 6)*(x - 1) - 5*y*(x^5 + 1)", { "x", "y", "z" });
  CanonicalForm f2 = *convStringPFactoryP(
    "(z + 1)*(x^3 + 6)*(y - 1) - 7*x*(y^3 + 1)", { "x", "y", "z" });


  printf("/* ----------------------------------------------------------- */\n");
  PRINT(f1)
  PRINT(f2)

  // TIMING_START(PROC)
  // CFArray Chain_epsilon = epsilon::subResChain(f1, f2, x);
  CFArray Chain_sisys = epsilon::SRS(f1, f2, x);
  // CFArray Chain = ::subResChain(f1, f2, x);
  // TIMING_END_AND_PRINT(PROC, "time : ")

  // auto start = std::chrono::steady_clock::now();
  // // CFArray Chain_epsilon = epsilon::subResChain(f1, f2, x);
  // CFArray RegularChain = epsilon::SubResChainFlint(f1, f2, 1);
  // auto end = std::chrono::steady_clock::now();
  // std::chrono::duration<double> elapsed_seconds =
  //   std::chrono::duration<double>(end - start);
  // std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

  // for (int j = Chain_sisys.min(); j <= Chain_sisys.max(); j++) {
  //   // if (!Chain_sisys[j].isZero())
  //     std::cout << "       Chain" << j << ": " << Chain_sisys[j] << std::endl;
  // }
  // std::cout << std::endl;

  // // CFArray RegularChain = epsilon::SRS(f1, f2, x);

  // for (int j = RegularChain.min(); j <= RegularChain.max(); j++) {
  //   std::cout << "RegularChain" << j << ": " << RegularChain[j] << std::endl;
  // }

  // auto test = [](const CFArray& Chain1, const CFArray& Chain2) -> bool {
  //   if (Chain1.size() != Chain2.size()) {
  //     std::cout << "subResChain!=SubResChain" << std::endl;
  //     return false;
  //   }

  //   for (int i = Chain1.min(); i <= Chain1.max(); i++) {
  //     if (!Chain2[i].isZero() && !Chain1[i].isZero()) {
  //       if (!fdivides(Chain2[i], Chain1[i])) {
  //         std::cout << "subResChain!=SubResChain" << std::endl;
  //         return false;
  //       }
  //     }
  //   }
  //   std::cout << "subResChain==SubResChain" << std::endl;
  //   return true;
  // };

  // test(Chain_epsilon, Chain_sisys);
  // test(Chain_epsilon, Chain);
  // for (int j = Chain_epsilon.min(); j <= Chain_epsilon.max(); j++) {
  //   std::cout << "Chain" << j << ": " << Chain_epsilon[j] << std::endl;
  // }

  printf("/* ----------------------------------------------------------- */\n");
#endif

#if 0
  Variable t('t'), u('u'), v('v'), w('w'), x('x'), y('y'), z('z');
  CanonicalForm P1(2 * t * z + 2 * w * y - 1);

  CanonicalForm P2(x + y + z - 1);
  TIMING_START(SubResltantChain)
  printf("/* ----------------------------------------------------------- */\n");
  PRINT(P1)
  PRINT(P2)
  CFArray Chain = epsilon::SubResChain(P1, P2, z);
  CFArray RegularChain = epsilon::SRS(P1, P2, z);
  for (int j = Chain.min(); j <= Chain.max(); j++) {
    std::cout << "Chain" << j << ": " << Chain[j] << std::endl;
  }
  for (int j = RegularChain.min(); j <= RegularChain.max(); j++) {
    std::cout << "RegularChain" << j << ": " << RegularChain[j] << std::endl;
  }
  printf("/* ----------------------------------------------------------- */\n");

#endif

#if 0
  Variable z('z'), x('x'), y('y');
  // Variable z('z'), y('y'), x('x');

  // x\^([0-9])
  // power(x,$1)

  TIMING_START(SubResltantChain)
  printf("/* ----------------------------------------------------------- */\n");
  CanonicalForm F(729 * power(y, 6) - 1458 * power(x, 3) * power(y, 4) +
                  729 * power(x, 2) * power(y, 4) - 4158 * x * power(y, 4) -
                  1685 * power(y, 4) + 729 * power(x, 6) * power(y, 2) -
                  1458 * power(x, 5) * power(y, 2) -
                  2619 * power(x, 4) * power(y, 2) -
                  4892 * power(x, 3) * power(y, 2) -
                  297 * power(x, 2) * power(y, 2) + 5814 * x * power(y, 2) +
                  427 * power(y, 2) + 729 * power(x, 8) + 216 * power(x, 7) -
                  2900 * power(x, 6) - 2376 * power(x, 5) + 3870 * power(x, 4) +
                  4072 * power(x, 3) - 1188 * power(x, 2) - 1656 * x + 529);

  CanonicalForm G(2187 * power(y, 4) - 4374 * power(x, 3) * power(y, 2) -
                  972 * power(x, 2) * power(y, 2) - 12474 * x * power(y, 2) -
                  2868 * power(y, 2) + 2187 * power(x, 6) - 1944 * power(x, 5) -
                  10125 * power(x, 4) - 4800 * power(x, 3) +
                  2501 * power(x, 2) + 4968 * x - 1587);
  PRINT(F)
  PRINT(G)
  CFArray Chain = epsilon::SubResChain(F, G, y);
  CFArray RegularChain = epsilon::SRS(F, G, y);
  for (int j = Chain.min(); j <= Chain.max(); j++) {
    std::cout << "Chain" << j << ": " << Chain[j] << std::endl;
  }
  for (int j = RegularChain.min(); j <= RegularChain.max(); j++) {
    std::cout << "RegularChain" << j << ": " << RegularChain[j] << std::endl;
  }
  printf("/* ----------------------------------------------------------- */\n");
  CanonicalForm f1(2 * power(x, 7) + 2 * power(x, 6) + 2 * power(x, 5) +
                   2 * power(x, 4) + 2 * power(x, 3) + 2 * power(x, 2) + 3 * x +
                   1);
  CanonicalForm g1(2 * power(x, 7) + 2 * power(x, 5) + 2 * power(x, 4) +
                   2 * power(x, 3) + 2 * power(x, 2) + 2 * x + 2);

  PRINT(f1)
  PRINT(g1)
  Chain = epsilon::SubResChain(f1, g1, x);
  RegularChain = epsilon::SRS(f1, g1, x);
  for (int j = Chain.min(); j <= Chain.max(); j++) {
    std::cout << "Chain" << j << ": " << Chain[j] << std::endl;
  }
  for (int j = RegularChain.min(); j <= RegularChain.max(); j++) {
    std::cout << "RegularChain" << j << ": " << RegularChain[j] << std::endl;
  }
  // PRINT(epsilon::subResChain(f1, g1, x))
  printf("/* ----------------------------------------------------------- */\n");
  CanonicalForm f2(-power(x, 2) * power(z, 3) + power(x, 4) - power(z, 4) +
                   power(x, 2) + 2 * power(z, 2) - 1);
  CanonicalForm g2(34 * power(x, 3) - power(x, 2) * power(y, 2) + power(z, 4) +
                   x * power(z, 2) - 2 * power(z, 2) + 1);

  PRINT(f2)
  PRINT(g2)
  Chain = epsilon::SubResChain(f2, g2, x);
  for (int j = 0; j <= Chain.max(); j++) {
    std::cout << "Chain" << j << ": " << Chain[j] << std::endl;
  }
  // PRINT(epsilon::subResChain(f2, g2, x))
  printf("/* ----------------------------------------------------------- */\n");
  CanonicalForm f3(3 * power(y, 6) + x * power(y, 5) + power(x, 3) * y - x * y +
                   power(x, 4) - power(x, 2));
  CanonicalForm g3(5 * x * power(y, 6) - 2 * power(y, 5) +
                   power(x, 2) * power(y, 4) - 2 * x * power(y, 4) +
                   x * power(y, 2) + power(x, 2) * y);

  PRINT(f3)
  PRINT(g3)
  Chain = epsilon::SubResChain(f3, g3, y);
  for (int j = 0; j <= Chain.max(); j++) {
    std::cout << "Chain" << j << ": " << Chain[j] << std::endl;
  }
  // PRINT(epsilon::subResChain(f3, g3, y))
  printf("/* ----------------------------------------------------------- */\n");

  TIMING_END_AND_PRINT(SubResltantChain, "time for experiment_00 : ")
#endif

  return 0;
}
/* ----------------------------------------------------------------- */
int
epsilon::NoPower_Test()
{
#if 0
  Variable t('t'), z('z');
  CanonicalForm P2(
    -39135393 * (t ^ 715) * (z ^ 5) + 213465780 * (t ^ 712) * (z ^ 5) -
    465743520 * (t ^ 709) * (z ^ 5) + 508083840 * (t ^ 706) * (z ^ 5) -
    277136640 * (t ^ 703) * (z ^ 5) + 60466176 * (t ^ 700) * (z ^ 5) +
    37949472 * (t ^ 719) - 207104931 * (t ^ 716) + 452100528 * (t ^ 713) -
    493457184 * (t ^ 710) + 269298432 * (t ^ 707) - 58786560 * (t ^ 704));
  CanonicalForm P1(-1 * t * (z ^ 545) + 2 * (t ^ 16) * (z ^ 530) -
                   (t ^ 31) * (z ^ 515));
  printf("/* ----------------------------------------------------------- */\n");
  PRINT(P2)
  PRINT(P1)
  // CFArray Chain = epsilon::subResChain(P1, P2, z);
  // CFArray Chain = epsilon::SubResChain(P1, P2, z);
  PRINT(::factorize(P2))
  PRINT(epsilon::noPowerList(P2))
  PRINT(epsilon::noPowerPoly(P2))

  {
    ZoneScopedN("psr");
    PRINT(psr(epsilon::noPowerPoly(P1), epsilon::noPowerPoly(P2), z))
  }
#endif

#if 1
  Variable t('t'), z('z');
  CanonicalForm P2(
    -39135393 * (t ^ 715) * (z ^ 5) + 213465780 * (t ^ 712) * (z ^ 5) -
    465743520 * (t ^ 709) * (z ^ 5) + 508083840 * (t ^ 706) * (z ^ 5) -
    277136640 * (t ^ 703) * (z ^ 5) + 60466176 * (t ^ 700) * (z ^ 5) +
    37949472 * (t ^ 719) - 207104931 * (t ^ 716) + 452100528 * (t ^ 713) -
    493457184 * (t ^ 710) + 269298432 * (t ^ 707) - 58786560 * (t ^ 704));
  CanonicalForm P1(-1 * t * (z ^ 545) + 2 * (t ^ 16) * (z ^ 530) -
                   (t ^ 31) * (z ^ 515));
  printf("/* ----------------------------------------------------------- */\n");
  PRINT(P2)
  PRINT(P1)
  // CFArray Chain = epsilon::subResChain(P1, P2, z);
  // CFArray Chain = epsilon::SubResChain(P1, P2, z);
  PRINT(::factorize(P2))
  PRINT(::factorize(P1))
  // PRINT(epsilon::noPowerList(P2))
  // PRINT(epsilon::noPowerPoly(P2))

  // {
  //   ZoneScopedN("psr");
  //   PRINT(psr(epsilon::noPowerPoly(P1), epsilon::noPowerPoly(P2), z))
  // }
#endif

  return 0;
}
/* ----------------------------------------------------------------- */

#if 0

TIMING_DEFINE(PROC_01)
TIMING_DEFINE(PROC_02)

/* ----------------------------------------------------------------- */
// 实验一：结果验证 1
int
my_resultant::Experiment_01()
{
  std::cout << "Experiment 01: " << std::endl;
  std::cout << std::endl;

  // 设置域的特征
  setCharacteristic(0);

  // 变量
  Variable t(1, 't'), x(2, 'x'), y(3, 'y');

  // 多项式
  CanonicalForm f((power(t, 2) + 1) * x - t), g((power(t, 2) + 1) * y - 2);

  std::cout << "f = " << f << std::endl << "g = " << g << std::endl;
  std::cout << std::endl;

  TIMING_START(PROC_01)
  // 计算结式
  std::cout << "res(f, g, t) = " << Res(f, g, t) << std::endl;
  std::cout << std::endl;
  TIMING_END_AND_PRINT(PROC_01, "time for experiment_01 : ")
  return 0;
}
/* ----------------------------------------------------------------- */
// 实验二：结果验证 2
int
my_resultant::Experiment_02()
{
  std::cout << "Experiment 02: " << std::endl;
  std::cout << std::endl;

  // 设置域的特征
  setCharacteristic(0);
  // 有理数域
  On(SW_RATIONAL);

  // 变量
  Variable t(1, 't'), s(2, 's'), x(3, 'x'), y(4, 'y'), z(5, 'z');

  // 多项式
  CanonicalForm f(2 * x - power(t, 3)),
    g((power(s, 2) + 1) * y - (power(s, 2) - 1) * power(t, 2)),
    h((power(s, 2) + 1) * z - 2 * s * power(t, 2));

  std::cout << "f = " << f << std::endl
            << "g = " << g << std::endl
            << "h = " << h << std::endl;
  std::cout << std::endl;

  // 计算多项式之间的结式
  std::cout << "Calculate using the res function implemented by the algorithm "
               "in this assignment: "
            << std::endl;
  TIMING_START(PROC_02)
  std::cout << "P1 = res(res(f, g, t), res(f, h, t), s) = "
            << Res(Res(f, g, t), Res(f, h, t), s) << std::endl;
  TIMING_END_AND_PRINT(PROC_02, "time : ")

  std::cout << std::endl;
  TIMING_START(PROC_02)
  std::cout << "P2 = res(res(f, g, t), res(g, h, t), s) = "
            << Res(Res(f, g, t), Res(g, h, t), s) << std::endl;
  TIMING_END_AND_PRINT(PROC_02, "time : ")

  std::cout
    << std::endl
    << "Calculate using the res function implemented by Singular/factory: "
    << std::endl;
  TIMING_START(PROC_02)
  std::cout << "P3 = res(res(f, h, t), res(g, h, t), s) = "
            << resultant(resultant(f, h, t), resultant(g, h, t), s)
            << std::endl;
  // std::cout << "P3 = res(res(f, h, t), res(g, h, t), s) = " << res(res(f, h,
  // t), res(g, h, t), s) << std::endl;
  TIMING_END_AND_PRINT(PROC_02, "time : ")

  std::cout << std::endl
            << "P1 = 0 or P2 = 0 or P3 = 0 is the implicit equation that "
               "satisfies the condition."
            << std::endl;
  return 0;
}
/* ----------------------------------------------------------------- */
#endif