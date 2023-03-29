#include "Tracy.hpp"
#include "test.h"
#include <flint/flint.h>

int
main(int argc, char* argv[])
{
  ZoneScoped;
  // epsilon::TriSer_Test_BPAS(argc, argv, "tryme");
  // epsilon::TriSer_Test_Biology(argc, argv, "BIOMD0000000334");

  epsilon::Triangularize_Test(argc, argv);
  // epsilon::TriSer_Test(argc, argv);

  // epsilon::RegSer_Test(argc, argv);

  // epsilon::Maple_Test(argc, argv);

  // epsilon::Parse_Test();
  // epsilon::Prem_Test();

  // epsilon::Decom_Test();
  // epsilon::DecomTestWithMaple(argc, argv);
  // epsilon::DecomSpeedTest(argc, argv);

  // epsilon::SubResltantChain_Test();
  // epsilon::NoPower_Test();

  // epsilon::Test();

  return 0;
}
