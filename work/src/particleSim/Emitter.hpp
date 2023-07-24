//
// Created by Clara on 8/10/2022.
//

#pragma once

//std
#include <utility>
#include <vector>
// glm
#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
//project
#include "Particle.hpp"
#include "KDTree.hpp"
#include "cgra/cgra_mesh.hpp"
#include "cgra/cgra_geometry.hpp"

namespace particleSim {

    struct Emitter {
        int treeID;
        glm::vec3 pos{};
        float speadScalar;
        int max_particles = 500;
        int cur_particles = 0;
        float turbulentCon = 2.1;
        glm::vec3 direction{0, 1, 0};
        glm::vec3 spreadDir{0};
        std::vector<Particle> particles;
        KDTree particleTree;
        float radius = 0.25;

        // constructor
        explicit Emitter(glm::vec3 _pos);
        explicit Emitter(glm::vec3 _pos, float radius, int id);

        // methods
        void update();
        void updateRecursive(KDNode* n);

        // add a number of randomized particles into the emitter
        void addParticles(int num);

        // respawn the dead particle in the emitter
        void respawnParticle(Particle &p);

        // update the particle spawning position range
        void updateRange(glm::vec3 dir, float s);
    };

}

