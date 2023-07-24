//
// Created by Clara on 8/10/2022.
//

#include "Particle.hpp"

using namespace std;
using namespace glm;

namespace particleSim {

    bool Particle::isAlive() const {
        return dt < lifespan;
    }

    void Particle::update(vec3 UMean, float turCon, float turMean) {
        dt += 0.01;

        // update position
        if (dt > 0.01) {
            dW = vec3(dW.x + sqrt(dt) * linearRand(-0.5f, 0.5f),
                      dW.y + sqrt(dt) * linearRand(0.f, 1.f),
                      dW.z + sqrt(dt) * linearRand(-0.5f, 0.5f));

            dw += glm::sqrt(dt) * glm::linearRand(-.5f, .5f);
        }
        velocity += 3.f / 4.f * turCon * turMean * (velocity - UMean) * dt + vec3(glm::sqrt(turCon * 1.2 * turMean)) * dW;
        velocity = normalize(velocity);
        attrib.pos += velocity * dt;
        turbulence += (turMean - turbulence) * 0.01 * turMean * dw;


        // update color
        float ratio = dt/float(lifespan);
        if (ratio >= 0.7 && dt > 0.2) {
            attrib.col = vec3(linearRand(0.45f, 0.55f));
        } else {
            attrib.col = vec3(1, glm::clamp(0.8f-ratio, 0.f, 0.8f), 0);
        }

        // update alpha
        attrib.alpha = glm::clamp(1-ratio, 0.f, 1.f);


    }
}