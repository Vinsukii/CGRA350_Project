//
// Created by Clara on 26/09/2022.
//

#include <glm/gtc/matrix_transform.hpp>
#include "particle_system.hpp"

using namespace cgra;
using namespace glm;
using namespace std;

namespace particleSim {

    gl_mesh particle_system::build() {
        vector<particle_data> allParticles = getAllParticlesData();

        vector<mesh_vertex> vertices;
        vec3 point(0, 0, 0);
        vector<int> plane_indices = {0};

        gl_mesh m;
        glGenVertexArrays(1, &m.vao); // VAO stores information about how the buffers are set up
        glGenBuffers(1, &m.vbo); // VBO stores the vertex data
        glGenBuffers(1, &m.ibo); // IBO stores the indices that make up primitives

        glGenBuffers(1, &posVBO); // stores transformation data
        glBindBuffer(GL_ARRAY_BUFFER, posVBO);
        glBufferData(GL_ARRAY_BUFFER, allParticles.size() * sizeof(particle_data), &allParticles[0], GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // VAO
        glBindVertexArray(m.vao);

        // VBO (single buffer, interleaved)
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
        // upload ALL the vertex data in one buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3), &point, GL_STREAM_DRAW);

        // this buffer will use location=0 when we use our VAO
        glEnableVertexAttribArray(0);
        // tell opengl how to treat data in location=0 - the data is treated in lots of 3 (3 floats = vec3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *) 0);

        // position
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, posVBO);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(particle_data), (void *) (offsetof(particle_data, pos)));
        glVertexAttribDivisor(1, 1); // for instancing

        // color
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, posVBO);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(particle_data), (void *) (offsetof(particle_data, col)));
        glVertexAttribDivisor(2, 1); // for instancing

        // alpha
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, posVBO);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(particle_data),
                              (void *) (offsetof(particle_data, alpha)));
        glVertexAttribDivisor(3, 1); // for instancing

        // IBO
        //
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);
        // upload the indices for drawing primitives
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * plane_indices.size(), &plane_indices[0],
                     GL_STREAM_DRAW);


        // set the index count and draw modes
        m.index_count = plane_indices.size();
        m.mode = GL_POINTS;

        // clean up by binding VAO 0 (good practice)
        glBindVertexArray(0);

        return m;
    }

    void particle_system::update() {
        if (startSimulation) {
            vector<particle_data> allParticles = getAllParticlesData();
            for (Emitter &e: emitters) {
                e.update();
            }

            allParticles = getAllParticlesData();

            glBindBuffer(GL_ARRAY_BUFFER, posVBO);
            glBufferData(GL_ARRAY_BUFFER, allParticles.size() * sizeof(particle_data), &allParticles[0], GL_STREAM_DRAW);
        }
    }

    // add an emitter at a random place
    particle_system::particle_system(int _size) {
        size = _size;
        for (int i = 0; i < size; i++) {
            emitters.emplace_back(Emitter(vec3(linearRand(-5.f, 5.f), 5.f, linearRand(-5.f, 5.f))));
        }
    }

    std::vector<particle_data> particle_system::getAllParticlesData() {
        vector<particle_data> particles;
        for (const Emitter &e: emitters) {
            for (const Particle &p: e.particles) {
                particles.push_back(p.attrib);
            }
        }
        particle_count = particles.size();
        return particles;
    }

    void particle_system::addEmitter(vec3 point, float radius, int id) {
        emitters.emplace_back(Emitter(point, radius, id));
    }

    void particle_system::killEmitter(int id) {
        for (Emitter &e: emitters) {
            if (e.treeID == id) {
                e.max_particles = 0;
            }
        }
    }

    void particle_system::updateEmitterRange(vec3 dir, float scale, int id) {
        for (Emitter &e: emitters) {
            if (e.treeID == id) {
                e.updateRange(dir, scale);
            }
        }
    }

}





