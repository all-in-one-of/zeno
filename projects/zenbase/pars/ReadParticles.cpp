#include <zeno/zeno.h>
#include <zeno/ParticlesObject.h>
#include <zeno/StringObject.h>
#include <cstring>

namespace zen {

static void readpars(
    const char *path,
    std::vector<glm::vec3> &positions,
    std::vector<glm::vec3> &velocities)
{
  FILE *fp = fopen(path, "r");
  if (!fp) {
    perror(path);
    abort();
  }
  int pos_n = 0, vel_n = 0;

  char hdr[128];
  while (EOF != fscanf(fp, "%s", hdr)) {
    if (!strcmp(hdr, "v")) {
      glm::vec3 position;
      fscanf(fp, "%f %f %f\n", &position.x, &position.y, &position.z);
      positions.push_back(position);
      pos_n++;

    } else if (!strcmp(hdr, "#v_vel")) {
      glm::vec3 velocity;
      fscanf(fp, "%f %f %f\n", &velocity.x, &velocity.y, &velocity.z);
      velocities.push_back(velocity);
      vel_n++;
    }
  }

  for (int i = 0; i < pos_n - vel_n; i++) {
    glm::vec3 velocity(0);
    velocities.push_back(velocity);
  }
  fclose(fp);
}


struct ReadParticles : zen::INode {
  virtual void apply() override {
    auto path = std::get<std::string>(get_param("path"));
    auto pars = zen::IObject::make<ParticlesObject>();
    readpars(path.c_str(), pars->pos, pars->vel);
    set_output("pars", pars);
  }
};

static int defReadParticles = zen::defNodeClass<ReadParticles>("ReadParticles",
    { /* inputs: */ {
    }, /* outputs: */ {
    "pars",
    }, /* params: */ {
    {"string", "path", ""},
    }, /* category: */ {
    "particles",
    }});


struct ImportParticles : zen::INode {
  virtual void apply() override {
    auto path = get_input("path")->as<StringObject>();
    auto pars = zen::IObject::make<ParticlesObject>();
    readpars(path->get().c_str(), pars->pos, pars->vel);
    set_output("pars", pars);
  }
};

static int defImportParticles = zen::defNodeClass<ImportParticles>("ImportParticles",
    { /* inputs: */ {
    "path",
    }, /* outputs: */ {
    "pars",
    }, /* params: */ {
    }, /* category: */ {
    "particles",
    }});

}
