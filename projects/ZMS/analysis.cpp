#include <zeno/zeno.h>
#include <zeno/PrimitiveObject.h>
#include <zeno/NumericObject.h>
#include "Forcefield.h"

using namespace zen;

struct PotentialEnergy: zen::INode {
  virtual void apply() override {
    auto prim = get_input("prim")->as<PrimitiveObject>();
    auto &pos = prim->attr<zen::vec3f>("pos");
    auto boxlength = get_input("boxlength")->as<NumericObject>()->get<float>();
    auto forcefield = get_input("forcefield")->as<ForceFieldObject>();
    
    auto energy = zen::IObject::make<NumericObject>();

    // We strictly use conservative forces (no velocity argument)
    // Mass of particles is contained in the force field;
    // TODO: add primitive type attribute
    energy->set<float>(forcefield->energy(pos, boxlength));
    set_output("potential energy", energy);
  }
};

static int defPotentialEnergy = zen::defNodeClass<PotentialEnergy>("PotentialEnergy",
    { /* inputs: */ {
    "prim",
    "forcefield",
    "boxlength",
    }, /* outputs: */ {
    "potential energy",
    }, /* params: */ {
    }, /* category: */ {
    "Molecular",
    }});

struct KineticEnergy: zen::INode {
  virtual void apply() override {
    auto prim = get_input("prim")->as<PrimitiveObject>();
    auto &vel = prim->attr<zen::vec3f>("vel");
    
    auto energy = zen::IObject::make<NumericObject>();

    float ek = 0.0f;
    #pragma omp parallel for
    for (int i = 0; i < vel.size(); i++) {
        ek += zen::lengthsq(vel[i]);
    }
    energy->set<float>(ek * 0.5f);
    set_output("kinetic energy", energy);
  }
};

static int defKineticEnergy = zen::defNodeClass<KineticEnergy>("KineticEnergy",
    { /* inputs: */ {
    "prim",
    }, /* outputs: */ {
    "kinetic energy",
    }, /* params: */ {
    }, /* category: */ {
    "Molecular",
    }});

