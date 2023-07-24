//
// Created by Clara on 8/10/2022.
//

#pragma once

//std
#include <utility>
#include <iostream>
#include <vector>
// glm
#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
//project
#include "cgra/cgra_mesh.hpp"
#include "cgra/cgra_geometry.hpp"

namespace particleSim {

    struct particle_data {
        glm::vec3 pos{0};
        glm::vec3 col{1, 1, 0};
        float alpha = 1;
    };

    struct Particle {
        // initialise attributes
        particle_data attrib;
        float lifespan = 120;
        float dt = 0;
        glm::vec3 velocity{0, 1, 0};
        float size = 1;
        float turbulence = 2.1;
        glm::vec3 dW = glm::vec3(glm::sqrt(0.01f) * glm::linearRand(0.f, 1.f));
        float dw = glm::sqrt(0.01f) * glm::linearRand(-.5f, .5f);

        // constructor
        explicit Particle(glm::vec3 _pos) { attrib.pos = _pos; }

        // methods
        bool isAlive() const;

        void update(glm::vec3 UMean, float turCon, float turMean);

    };
}

