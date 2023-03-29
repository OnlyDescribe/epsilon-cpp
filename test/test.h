#ifndef TEST_H
#define TEST_H

#include "canonicalform.h"
#include "nlohmann/json.hpp"

/* --------------------------------------------------------------- */
// TODO: 
namespace epsilon {
std::pair<nlohmann::basic_json<>::value_type,
          nlohmann::basic_json<>::value_type>
get_bpas_poly_system(const char* poly_name);
std::pair<nlohmann::basic_json<>::value_type,
          nlohmann::basic_json<>::value_type>
get_biology_poly_system(const char* poly_name);

// TODO 命名格式统一
int
SubResltantChain_Test();
int
NoPower_Test();
int
TriSer_Test(int argc, char** argv);
int
TriSer_Test_BPAS(int argc, char** argv, const char* poly_name);
int
TriSer_Test_Biology(int argc, char** argv, const char* poly_name);
int
RegSer_Test(int argc, char** argv);
int
Parse_Test();
int
Prem_Test();
int
Decom_Test();
int
Maple_Test(int argc, char** argv);
int
Tree_Test();
int
DecomTestWithMaple(int argc, char** argv);
int
DecomSpeedTest(int argc, char** argv);
int
Triangularize_Test(int argc, char** argv);
} // namespace epsilon
/* --------------------------------------------------------------- */
#if 0
namespace my_resultant {
int
Experiment_01();
int
Experiment_02();
} // namespace my_resultant
/* --------------------------------------------------------------- */
namespace my_isolate {
int
Experiment();
void
Experiment_Test();
} // namespace my_isolate
/* --------------------------------------------------------------- */
#endif

#endif