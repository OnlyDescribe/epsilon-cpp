#include "test.h"

#include <fstream>

#include "nlohmann/json.hpp"

#include "convert.h"

namespace epsilon {
std::pair<nlohmann::basic_json<>::value_type,
          nlohmann::basic_json<>::value_type>
get_bpas_poly_system(const char* poly_name)
{
  // 得到 bpas_polys.json 文件位置
  auto json_path = getDataPath("epsilon-cpp", "data/bpas_polys.json");

  // 反序列化
  json data_json = json::parse(std::ifstream{ json_path.value() });

  // 处理json
  nlohmann::basic_json<>::value_type poly_sys_json;
  nlohmann::basic_json<>::value_type polys_json;
  nlohmann::basic_json<>::value_type vars_json;

  poly_sys_json = data_json.at(poly_name);
  polys_json = poly_sys_json.at(std::string("polys"));
  vars_json = poly_sys_json.at(std::string("vars"));

  return { polys_json, vars_json };
}

std::pair<nlohmann::basic_json<>::value_type,
          nlohmann::basic_json<>::value_type>
get_biology_poly_system(const char* poly_name)
{
  // 得到 bpas_polys.json 文件位置
  auto json_path = getDataPath("epsilon-cpp", "data/biology_polys.json");

  // 反序列化
  json data_json = json::parse(std::ifstream{ json_path.value() });

  // 处理json
  nlohmann::basic_json<>::value_type poly_sys_json;
  nlohmann::basic_json<>::value_type polys_json;
  nlohmann::basic_json<>::value_type vars_json;

  poly_sys_json = data_json.at(poly_name);
  polys_json = poly_sys_json.at(std::string("polys"));
  vars_json = poly_sys_json.at(std::string("vars"));

  return { polys_json, vars_json };
}
}