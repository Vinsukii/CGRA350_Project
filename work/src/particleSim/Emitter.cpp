//
// Created by Clara on 8/10/2022.
//

#include "Emitter.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

namespace particleSim {

    void Emitter::update() {
        particleTree.clearTree();
        particleTree.updateParticles(particles);
        particleTree.constructTree();

        KDNode* r = particleTree.root;

        updateRecursive(r);

        particles = particleTree.getAllParticles();
    }

    Emitter::Emitter(glm::vec3 _pos) {
        pos = _pos;
        addParticles(max_particles/2.f);
        particleTree = KDTree(particles);
    }

    Emitter::Emitter(glm::vec3 _pos, float _radius, int id) {
        pos = _pos;
        radius = _radius;
        treeID = id;
        max_particles = int(sqrt(radius * 1000000));
        addParticles(max_particles/2.f);
        particleTree = KDTree(particles);
    }

    void Emitter::addParticles(int num) {
        for (int i = 0; i < num; i++) {
            vec3 Ppos = sphericalRand(radius) + vec3(linearRand(0.f, speadScalar)) * spreadDir;
            Ppos += pos;
            Particle p = Particle(Ppos);
            p.velocity = vec3(linearRand(-0.25, 0.25), linearRand(0.01, 0.5), linearRand(-0.25, 0.25));
            p.turbulence = linearRand(0.f, 5.f);
            p.lifespan = linearRand(0.05f, 0.2f);

            particles.emplace_back(p);
        }
    }

    void Emitter::respawnParticle(Particle &p) {
        vec3 Ppos = sphericalRand(radius) + vec3(linearRand(0.f, speadScalar)) * spreadDir;
        Ppos += pos;
        p = Particle(Ppos);
        p.velocity = vec3(linearRand(-0.25, 0.25), linearRand(0.01, 0.5), linearRand(-0.25, 0.25));
        p.turbulence = linearRand(0.f, 5.f);
        p.lifespan = linearRand(0.05f, 0.2f);
        p.dt = 0;
    }

    void Emitter::updateRange(glm::vec3 dir, float s) {
        spreadDir = dir;
        speadScalar = s;
        max_particles = int(sqrt(glm::max(radius, speadScalar) * 1000000));
    }

    void Emitter::updateRecursive(KDNode *n) {
        cur_particles = particles.size();
        if(n->left != nullptr && n->right != nullptr) {
            updateRecursive(n->left);
            updateRecursive(n->right);
        } else {
            if (!n->points.empty()) {
                vec3 velocitySum(0.f);
                float turbulenceSum = 0;
                for (Particle p: n->points) {
                    velocitySum += normalize(p.velocity);
                    turbulenceSum += p.turbulence;
                }
                vec3 uMean = velocitySum / vec3(n->points.size());
                float turMean = turbulenceSum / float(n->points.size());

                for (unsigned int i = 0; i < n->points.size(); i++){
                    Particle &p = n->points.at(i);
                    p.update(uMean, turbulentCon, turMean);
                    if(!p.isAlive() && cur_particles > max_particles) {
                        n->points.erase(n->points.begin()+i);
                        i--;
                    } else if(!p.isAlive()) {
                        respawnParticle(p);
                    }
                }

                if(cur_particles < max_particles) {
                    addParticles(glm::min(10, max_particles - cur_particles));
                }

            }
        }
    }
}