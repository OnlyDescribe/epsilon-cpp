#ifndef CONVERT_H
#define CONVERT_H

#include <filesystem>
#include <optional>
#include <set>

#include "nlohmann/json.hpp"

#include "canonicalform.h"

namespace epsilon {
using json = nlohmann::json;
namespace fs = std::filesystem;

inline namespace epsilon_cpp17 {
using CFSet = std::set<CanonicalForm, std::greater<>>;

std::optional<fs::path>
getDataPath(const fs::path& project_name, const fs::path& data_relative_path);

void
setVarsNames(const std::vector<const char*>& vars);

std::optional<CanonicalForm>
convStringPFactoryP(const char* poly, const std::vector<const char*>& vars);

std::optional<CFSet>
convStringPSFactoryPS(const std::vector<const char*>& polys,
                      const std::vector<const char*>& vars);

std::optional<CFSet>
convJsonPSFactoryPS(const nlohmann::basic_json<>::value_type& poly_system_json,
                    const nlohmann::basic_json<>::value_type& vars_json,
                    bool set_var_names = false);

} // namespace epsilon_cpp17

} // namespace epsilon

#endif