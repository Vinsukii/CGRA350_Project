//
// Created by Clara on 26/09/2022.
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
#include "particleSim/Emitter.hpp"
#include "cgra/cgra_mesh.hpp"
#include "cgra/cgra_geometry.hpp"

namespace particleSim {

    class particle_system {
    public:
        int size = 1;
        int particle_count = 0;
        std::vector<Emitter> emitters;
        cgra::mesh_builder mb;
        GLuint posVBO = 0;
        glm::mat4 Rotation{1};
        bool startSimulation = false;

        // constructors
        particle_system() = default;
        explicit particle_system(std::vector<Emitter> _emitters) {
            emitters = std::move(_emitters);
            size = emitters.size();
        }
        explicit particle_system(int _size);


        // build the gl_mesh for drawing
        cgra::gl_mesh build();

        // update each particle and update the buffer data
        void update();

        // get all the particles in the particle system
        std::vector<particle_data> getAllParticlesData();

        // add an emitter with radius, and tree id
        void addEmitter(glm::vec3 point, float radius, int id);

        // kill the emitter using the tree id
        void killEmitter(int id);

        // spread the fire in one emitter
        void updateEmitterRange(glm::vec3 dir, float scale, int id);

    };



}
