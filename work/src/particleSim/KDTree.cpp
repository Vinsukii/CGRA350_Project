//
// Created by Clara on 8/10/2022.
//

#include "KDTree.hpp"

#include <utility>

using namespace std;
using namespace glm;

namespace particleSim {

    void KDNode::splitNode() {
        // construct bouding box
        generateBoundingBox();
        int xPos = 0;
        int yPos = 0;
        int zPos = 0;
        float xDiff = 50, yDiff = 50, zDiff = 50;
        // split the box into two from the median medianPoint, try to split evenly

        // try split by x
        sort(points.begin(), points.end(), compareVecX());
        compareAxis(xPos, xDiff, 0);
        if (xPos >= int(points.size() * 0.4)) {
            splitAxis(xPos);
            return;
        }

        // try split by y
        sort(points.begin(), points.end(), compareVecY());
        compareAxis(yPos, yDiff, 1);
        if (yPos >= int(points.size() * 0.4)) {
            splitAxis(yPos);
            return;
        }

        // try split by z
        sort(points.begin(), points.end(), compareVecZ());
        compareAxis(zPos, zDiff, 2);
        if (zPos >= int(points.size() * 0.4)) {
            splitAxis(zPos);
            return;
        }

        int clampIndex = glm::clamp(zPos, 1, int(points.size())-10);
        splitAxis(clampIndex);

    }

    void KDNode::generateBoundingBox() {
        vec3 maxPosition = points.at(0).attrib.pos;
        vec3 minPosition = points.at(0).attrib.pos;

        for (const Particle &p: points) {
            vec3 v = p.attrib.pos;
            maxPosition.x = glm::max(maxPosition.x, v.x);
            maxPosition.y = glm::max(maxPosition.y, v.y);
            maxPosition.z = glm::max(maxPosition.z, v.z);
            minPosition.x = glm::min(minPosition.x, v.x);
            minPosition.y = glm::min(minPosition.y, v.y);
            minPosition.z = glm::min(minPosition.z, v.z);
        }

        size = maxPosition - minPosition;
        startPoint = minPosition;
        medianPoint = startPoint + size / vec3(2.0);
    }

    void KDNode::compareAxis(int &pos, float &diff, int index) {
        float currDiff = points.at(0).attrib.pos[index] - medianPoint[index];
        for (unsigned int i = 0; i < points.size(); i++) {
            float tmp = points.at(i).attrib.pos[index] - medianPoint[index];
            if (glm::abs(tmp) <= glm::abs(currDiff) && (tmp * currDiff >= 0)) {
                currDiff = tmp;
                pos = int(i);
            }
            if (tmp * currDiff < 0 && i != 0) {
                break;
            }
        }
    }


    void KDNode::splitAxis(int index) {
        left = new KDNode();
        left->points = vector<Particle>(points.begin(), points.begin() + index);

        right = new KDNode();
        right->points = vector<Particle>(points.begin() + index, points.end());

        points.clear();
    }

    void KDTree::constructTree() {
        constructTreeRecursive(root);
    }

    void KDTree::constructTreeRecursive(KDNode *n) {
        if (n->points.size() > maxCount) {
            n->splitNode();
            if (n->left != nullptr) { constructTreeRecursive(n->left); }
            if (n->right != nullptr) { constructTreeRecursive(n->right); }
        }
    }


    KDTree::KDTree(std::vector<Particle> points_) {
        root = new KDNode(std::move(points_));
    }

    void KDTree::updateParticles(std::vector<Particle> points_) const {

        root->points = std::move(points_);

        root->left = nullptr;
        root->right = nullptr;
    }

    std::vector<Particle> KDTree::getAllParticles() {
        vector<Particle> results;
        addToParticlesRecursive(root, results);

        return results;
    }

    void KDTree::addToParticlesRecursive(KDNode* n, vector<Particle> &p) {
        if(n->left != nullptr && n->right != nullptr) {
            addToParticlesRecursive(n->left, p);
            addToParticlesRecursive(n->right, p);
        } else {
            p.insert(p.end(), n->points.begin(), n->points.end());
        }
    }

    void KDTree::clearTree() {
        clearTreeRecursive(root);
    }

    void KDTree::clearTreeRecursive(KDNode *n) {
        if(n->left != nullptr && n->right != nullptr){
            clearTreeRecursive(n->left);
            clearTreeRecursive(n->right);
            delete n->left;
            delete n->right;
            n->left = nullptr;
            n->right = nullptr;
        } else {
            n->points.clear();
        }
    }
}