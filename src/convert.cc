#include "convert.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

#include "FLINTconvert.h"
#include <flint/fmpz_mpoly.h>

#define USING_CPP17
#ifdef USING_CPP17
#define APPEND(F, Y) F.insert(Y)
#else
#define APPEND(F, Y) F.append(Y)
#endif

// CanonicalForm
// readCF(ISTREAM&);

namespace epsilon {
inline namespace epsilon_cpp17 {

// 当源文件在项目文件夹中, 通过相对路径和项目文件夹名得到数据绝对路径
std::optional<fs::path>
getDataPath(const fs::path& project_name, const fs::path& data_relative_path)
{
  fs::path current_path = fs::current_path();
  fs::path data_path;

  auto res = std::find(current_path.begin(), current_path.end(), project_name);
  if (res != current_path.end()) {
    for (auto iter = current_path.begin(); iter != res; iter++) {
      data_path /= *iter;
    }
    data_path = data_path / project_name / data_relative_path;
  } else {
    // std::cout << "error" << std::endl;
    return std::nullopt;
  }
  return data_path;
}

// 设置Factory库意义下的变元名称, 注意: 这里 vars_ 变元序应由大到小
void
setVarsNames_(const char** vars_, mp_limb_signed_t nvars)
{
  for (int i = 0; i < nvars; ++i) {
    if (strlen(vars_[nvars - 1 - i]) == 1) {
      Variable(i + 1, *vars_[nvars - 1 - i]);
    }
  }
}
void
setVarsNames(const std::vector<const char*>& vars)
{
  auto vars_ = vars;
  std::reverse(vars_.begin(), vars_.end());
  setVarsNames_(const_cast<const char**>(vars_.data()),
                static_cast<mp_limb_signed_t>(vars_.size()));
}

// 通过flint2解析多项式, 再将数据结构转为Factory所支持的多项式
// 注意:
// 1. vars_str_ 变元序应由大到小
// 2. Factory中多项式变元名称长度只支持为1,
// 若变元名称长度超过1使用默认变元名称(v_1,v_2,..)
std::optional<CanonicalForm>
convStringPFactoryP_(const char* poly_str,
                     const char** vars_str_,
                     mp_limb_signed_t nvars)
{
  // 通过flint2解析多项式
  fmpq_mpoly_ctx_t ctx_flint;
  fmpq_mpoly_t mpoly_flint;

  fmpq_mpoly_ctx_init(ctx_flint, nvars, ORD_DEGLEX);
  fmpq_mpoly_init(mpoly_flint, ctx_flint);

  CanonicalForm poly;
  if (!fmpq_mpoly_set_str_pretty(mpoly_flint, poly_str, vars_str_, ctx_flint)) {
    // std::cout << fmpq_mpoly_get_str_pretty(mpoly_flint, vars_str, ctx_flint)
    //           << std::endl;
    // 将flint2多项式转为Factory多项式
    poly = convFlintMPFactoryP(mpoly_flint, ctx_flint, int(nvars));

  } else {
    return std::nullopt;
  }

  fmpq_mpoly_clear(mpoly_flint, ctx_flint);
  fmpq_mpoly_ctx_clear(ctx_flint);

  return poly;
}

std::optional<CanonicalForm>
convStringPFactoryP(const char* poly, const std::vector<const char*>& vars)
{
  auto nvars = static_cast<mp_limb_signed_t>(vars.size());
  auto vars_ = vars;
  std::reverse(vars_.begin(), vars_.end());

  return convStringPFactoryP_(
    poly, const_cast<const char**>(vars_.data()), nvars);
}

std::optional<std::set<CanonicalForm, std::greater<>>>
convStringPSFactoryPS(const std::vector<const char*>& polys,
                      const std::vector<const char*>& vars)
{
  std::set<CanonicalForm, std::greater<>> ps;

  for (const auto& p : polys) {
    ps.insert(convStringPFactoryP(p, vars).value());
  }

  return ps;
}

std::optional<CFSet>
convJsonPSFactoryPS(const nlohmann::basic_json<>::value_type& poly_system_json,
                    const nlohmann::basic_json<>::value_type& vars_json,
                    bool set_var_names)
{
  CFSet PS;

  auto nvars = static_cast<mp_limb_signed_t>(vars_json.size());
  auto vars_str_ = new const char*[vars_json.size()];

  // 注意vars_str中第一个元素为最大变元, 与nvars相反
  for (size_t i = 0; i < nvars; ++i) {
    auto var = new char[vars_json[nvars - 1 - i].size()];
    strcpy(var, vars_json[nvars - 1 - i].get<std::string>().data());
    vars_str_[i] = var;
  }

  std::string poly_str;
  CanonicalForm poly_factory;

  for (const auto& poly_json : poly_system_json) {
    poly_str = poly_json.get<std::string>();

    if (auto poly_factory =
          convStringPFactoryP_(poly_str.data(), vars_str_, nvars)) {
      PS.insert(*poly_factory);
    } else {
      return std::nullopt;
    }
  }

  if (set_var_names) {
    setVarsNames_(vars_str_, nvars);
  }

  for (size_t i = 0; i < nvars; ++i) {
    delete[] vars_str_[i];
  }
  delete[] vars_str_;

  return PS;
}
}
}
