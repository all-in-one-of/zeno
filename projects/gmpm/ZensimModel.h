#pragma once

#include "zensim/physics/ConstitutiveModel.hpp"
#include <zeno/zeno.h>

namespace zeno {

struct ZenoConstitutiveModel : zeno::IObject {
  auto &get() noexcept { return model; }
  const auto &get() const noexcept { return model; }
  zs::ConstitutiveModelConfig model;
};

} // namespace zeno