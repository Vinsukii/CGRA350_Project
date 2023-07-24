//
// Created by Clara on 8/10/2022.
//

#pragma once

#include "Particle.hpp"
#include <algorithm>
#include <utility>

namespace particleSim {

    struct compareVecX {
        inline bool operator()(const Particle &x, const Particle &y) {
            return x.attrib.pos.x < y.attrib.pos.x;
        }
    };

    struct compareVecY {
        inline bool operator()(const Particle &x, const Particle &y) {
            return x.attrib.pos.y < y.attrib.pos.y;
        }
    };

    struct compareVecZ {
        inline bool operator()(const Particle &x, const Particle &y) {
            return x.attrib.pos.z < y.attrib.pos.z;
        }
    };

    struct KDNode {
        std::vector<Particle> points;
        glm::vec3 medianPoint{};
        glm::vec3 startPoint{};
        glm::vec3 size{};
        KDNode *left = nullptr;
        KDNode *right = nullptr;

        KDNode() = default;

        explicit KDNode(std::vector<Particle> points_) : points(std::move(points_)) {}

        // split the node
        void splitNode();

        // helper method to find the bounding box of the particles
        void generateBoundingBox();

        // helper method to compare and find which axis to split the node from
        void compareAxis(int &pos, float &diff, int index);

        // helper method to split the current node at a given axis
        void splitAxis(int index);
    };

    class KDTree {
    public:
        KDNode *root = nullptr;
        int maxCount = 32; // maximum particles in each node

        KDTree() = default;

        explicit KDTree(std::vector<Particle> points_);

        // construct the tree
        void constructTree();
        void constructTreeRecursive(KDNode *n);

        // update all the particles inside the tree
        void updateParticles(std::vector<Particle> points_) const;

        // get all the particles inside the tree as a vector
        std::vector<Particle> getAllParticles();
        void addToParticlesRecursive(KDNode* n, std::vector<Particle> &p);

        // clear the tree
        void clearTree();
        void clearTreeRecursive(KDNode* n);
    };

}

