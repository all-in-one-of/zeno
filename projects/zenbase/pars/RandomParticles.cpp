#include <zeno/zeno.h>
#include <zeno/ParticlesObject.h>
#include <cstring>

#ifdef _MSC_VER
static inline double drand48() {
	return rand() / (double)RAND_MAX;
}
#endif

namespace zen {

struct RandomParticles : zen::INode {
  virtual void apply() override {
    int count = std::get<int>(get_param("count"));
    float Prange = std::get<float>(get_param("Prange"));
    float Vrange = std::get<float>(get_param("Vrange"));
    auto pars = zen::IObject::make<ParticlesObject>();

    for (int i = 0; i < count; i++) {
      glm::vec3 p(drand48() * 2 - 1, drand48() * 2 - 1, drand48() * 2 - 1);
      glm::vec3 v(drand48() * 2 - 1, drand48() * 2 - 1, drand48() * 2 - 1);

      pars->pos.push_back(p*Prange);
      pars->vel.push_back(v*Vrange);
    }

    set_output("pars", pars);
  }
};

static int defRandomParticles = zen::defNodeClass<RandomParticles>("RandomParticles",
    { /* inputs: */ {
    }, /* outputs: */ {
    "pars",
    }, /* params: */ {
    {"int", "count", "1 0"},
    {"float", "Prange", "1 0"},
    {"float", "Vrange", "1 0"},
    }, /* category: */ {
    "particles",
    }});

}
