
#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "opengl.hpp"
#include "cgra/cgra_mesh.hpp"
//#include "stem.h"
#include "TreeParameters.h"

#include <random>

using namespace std;
using namespace glm;

struct Node {
    vec3 position;
    vec3 zAxis;
    float radius;
};

struct Section {
    int startNode;
    int endNode;
    int parentSection = -1;
    vec3 xAxis;
    vector<int> childSections;
    int meshIndex;
    int level;

    // SURFACE INFO (for combustion) //

    // Static
    vec3 midwayPt;
    float density = 0.8f;  // p
    float height_0 = 0.f;  // h
    float surArea_0 = 0.f; // A0
    float depth_0 = 0.f;   // H0
    float mass_0 = 0.f;    // M0 = A0 / 2H0*p

    // Dynamic
    vec3 scalar;               // texture blending
    float mass = 0.f;          // M - modelled as a fx of time
    float surArea = 0.f;       // A = A0 * H/H0
    float depth = 0.f;         // H = sqrt(2*M/p*H0/A0)
    float surfaceTemp = 25.0f; // T = a*avg(T) + b(Ta-T)
    float charThickness = 0.f; // Hc = k(H0-H)
    float insulEffect = 0.f;   // c = c_min + (1-c_min)e^(c*Hc)
    float massLossRate = 0.00002f; // m

    bool combusting = false;
    bool finished = false;

    // END //
};

struct TreeStruct {
    vector<Node> nodes;
    vector<Section> sections;
};


class WeberPennTree {
private:
    vector<float> m_errors{0, 0, 0, 0};
    mt19937 m_gen;
    /**
     * Gets a seeded random number in range from positive to negative V.
     * @param V
     * @return
     */
    float getVariation(float V);

    /**
     * Returns a gl_Mesh using the sections dimensions with a number of sides equal to resolution - 1
     * @param treeStruct
     * @param section
     * @param resolution
     * @return
     */
    cgra::gl_mesh buildSection(TreeStruct treeStruct, Section section, const int resolution);

    /**
     * Returns a ring of mesh vertices for a nodes location and direction
     * @param node
     * @param xAxis
     * @param resolution
     * @return
     */
    std::vector<cgra::mesh_vertex> vertexesFromNode(const Node node, const vec3 xAxis, const int resolution);

    /**
     * Recursives generates the tree structure
     * @param params
     * @param treeStruct
     * @param treeScale
     * @param sectionIndex
     * @param recursiveLevel
     * @param stemLevel
     * @param parentLength
     * @param baseR
     * @param lastChildRotation
     */
    void recursiveGenerateTree(const TreeParameters params, TreeStruct* treeStruct, const float treeScale, const int sectionIndex,
                               int recursiveLevel, int stemLevel, float parentLength, float baseR, float lastChildRotation);

    /**
     * Mathematical shape function
     * @param shape
     * @param ratio
     * @return
     */
    float shapeRatio(int shape, float ratio);


    // COMBUSTION MODEL //
    /**
     * Method for computing the initial surface information of a branch for the combustion
     * @param s       (the current section)
     * @param sPos    (start position of the section)
     * @param ePos    (end position of the section)
     * @param rRadius (radius of the start node of the section)
     * @param eRadius (radius of the end node of the section)
     */
    void computeSurfaceInformation(Section &s, vec3 sPos, vec3 ePos, float sRadius, float eRadius);
    // END //

public:
    WeberPennTree() {

    }
    TreeStruct generateTree(const TreeParameters params);
    vector<cgra::gl_mesh> buildMesh(TreeStruct& treeStruct);
};