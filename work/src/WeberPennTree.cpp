//
// Created by Rhysa
//

#include "WeberPennTree.h"

#include <glm/gtc/matrix_transform.hpp>

#define PI 3.1415926f

using namespace std;
using namespace glm;
using namespace cgra;

float WeberPennTree::getVariation(float V) {
    std::uniform_real_distribution<float> unif(-V, V);
    return unif(m_gen);
}

TreeStruct WeberPennTree::generateTree(const TreeParameters params) {

    TreeStruct treeStruct;

    // trunk
    float treeScale = params.scale + getVariation(params.scaleV);
    float trunkLength = (params.nLength[0] + getVariation(params.nLengthV[0])) * treeScale;
    float baseRadius = trunkLength * params.ratio;
    float nextRadius = baseRadius * std::max((1.f - params.nTaper[0] / params.nCurveRes[0]), 0.01f);
    float segLength = trunkLength / params.nCurveRes[0];

    vec3 startPos = vec3(0);
    vec3 nextPos = vec3(0, segLength, 0);

    //section 1
    treeStruct.nodes.push_back({
                                       startPos,
                                       vec3(0, 1, 0),
                                       baseRadius
                               });
    Section section_1;
    section_1.startNode = 0;
    treeStruct.nodes.push_back({
                                       nextPos,
                                       vec3(0, 1, 0),
                                       nextRadius
                               });
    section_1.endNode = 1;
    section_1.xAxis = vec3(1, 0, 0);
    section_1.level = 0;


    // compute initial information
    computeSurfaceInformation(section_1, startPos, nextPos, baseRadius, nextRadius);

    treeStruct.sections.push_back(section_1);

    recursiveGenerateTree(params, &treeStruct, treeScale, 0, 0, 1, trunkLength, baseRadius, 0);

    return treeStruct;
}

void WeberPennTree::recursiveGenerateTree(const TreeParameters params, TreeStruct *treeStruct, const float treeScale, const int sectionIndex,
                                          int recursiveLevel, int stemLevel, float parentLength, float baseR, float lastChildRotation) {

    Section parentSection = treeStruct->sections[sectionIndex];
    Node parentStartNode = treeStruct->nodes[parentSection.startNode];
    Node parentEndNode = treeStruct->nodes[parentSection.endNode];

    // stem children for parent
    if (recursiveLevel < params.levels) {
        float lengthBase = params.baseSize * parentLength; // size of bare area at base of tree
        float segLength = length(parentEndNode.position - parentStartNode.position);
        int stems = round(params.nBranches[recursiveLevel + 1] * (segLength / parentLength));

        for (int i = 0; i < stems; i++) {
            float offset = segLength / (stems + 1) * (i + 1);
            float parentOffset = segLength * (stemLevel - 1) + segLength / (stems + 1) * (i + 1);
            if (recursiveLevel == 0 && parentOffset < lengthBase) {
                continue;
            }
            float childLength = recursiveLevel == 0 ? parentLength * (params.nLength[recursiveLevel + 1] + getVariation(params.nLengthV[recursiveLevel + 1])) *
                                shapeRatio(params.shape, (parentLength - parentOffset) / parentLength) :
                                (params.nLength[recursiveLevel + 1] + getVariation(params.nLengthV[recursiveLevel + 1])) *
                                (parentLength - 0.6f * parentOffset);
            float childStartRadius = baseR * pow((childLength / parentLength), params.ratioPower);
            if (childStartRadius < 0.005f) {
                continue;
            }
            float parentRadiusAtChild = treeStruct->nodes[parentSection.startNode].radius * (1.f - params.nTaper[recursiveLevel] * parentOffset / parentLength);
            childStartRadius = std::min(childStartRadius, parentRadiusAtChild);
            vec3 childStartPos = parentStartNode.position +
                    normalize(parentEndNode.position - parentStartNode.position) * offset;

            float downAngle = params.nDownAngleV[recursiveLevel + 1] > 0 ?   // allow for angle to depend on child position along parent if V is negative
                              params.nDownAngle[recursiveLevel + 1] + getVariation(params.nDownAngleV[recursiveLevel + 1]) :
                              params.nDownAngle[recursiveLevel + 1] + (getVariation(-params.nDownAngleV[recursiveLevel + 1]) *
                                                      (1.f - 2.f * shapeRatio(params.shape, (parentLength - parentOffset) / (parentLength))));

            vec3 zAxis = vec4(parentStartNode.zAxis, 1) * rotate(mat4(1), radians(downAngle), parentSection.xAxis);

            lastChildRotation += params.nRotate[recursiveLevel + 1] + getVariation(params.nRotateV[recursiveLevel + 1]);
            zAxis = vec4(zAxis, 1) * rotate(mat4(1), radians(lastChildRotation), vec3(0, 1, 0));
            vec3 xAxis = vec4(parentSection.xAxis, 1) * rotate(mat4(1), radians(lastChildRotation), vec3(0, 1, 0));
            vec3 childNextPos = childStartPos + zAxis * childLength / params.nCurveRes[recursiveLevel + 1];
            float childNextRadius = childStartRadius * std::max((1.f - params.nTaper[recursiveLevel + 1] / params.nCurveRes[recursiveLevel + 1]), 0.01f);

            Section childSection;
            childSection.level = recursiveLevel + 1;
            treeStruct->nodes.push_back({
                childStartPos,
                zAxis,
                childStartRadius
            });
            childSection.startNode = treeStruct->nodes.size() - 1;
            treeStruct->nodes.push_back({
                                                childNextPos,
                                                zAxis,
                                                childNextRadius
                                        });
            childSection.endNode = treeStruct->nodes.size() - 1;
            childSection.xAxis = xAxis;
            childSection.parentSection = sectionIndex;


            // compute initial information
            computeSurfaceInformation(childSection, childStartPos, childNextPos, childStartRadius, childNextRadius);

            treeStruct->sections.push_back(childSection);

            int child = treeStruct->sections.size() - 1;
            treeStruct->sections[sectionIndex].childSections.push_back(child);
            if (recursiveLevel < params.levels) {
                recursiveGenerateTree(params, treeStruct, treeScale, child, recursiveLevel + 1,
                                      0, childLength, childStartRadius, 0);
            }
        }
    }


    // Branch Splitting
    if (stemLevel < params.nCurveRes[recursiveLevel]) {
        int numSplits = 0;

        if (recursiveLevel == 0 && stemLevel == 1) {
            numSplits = round(params.baseSplits + m_errors[recursiveLevel]);
            m_errors[recursiveLevel] -= numSplits - params.baseSplits;
        } else {
            numSplits = round(params.nSegSplits[recursiveLevel] + m_errors[recursiveLevel]);
            m_errors[recursiveLevel] -= numSplits - params.nSegSplits[recursiveLevel];
        }
        numSplits += 1;

        for (int i = 0; i < numSplits; i++) {
            Section sect;
            sect.startNode = parentSection.endNode;
            sect.level = recursiveLevel;

            float splitAngle = params.nSplitAngle[recursiveLevel] + getVariation(params.nSplitAngleV[recursiveLevel]);
            if (numSplits == 1) {
                splitAngle = 0;
            }
            vec3 zAxis = vec4(treeStruct->nodes[parentSection.endNode].zAxis, 1) * rotate(mat4(1), radians(splitAngle),
                                                                                          parentSection.xAxis);
            float angle = 360.0f / numSplits;
            zAxis = vec4(zAxis, 1) * rotate(mat4(1), radians(angle * i), treeStruct->nodes[parentSection.endNode].zAxis);
            vec3 xAxis = vec4(parentSection.xAxis, 1) * rotate(mat4(1), radians(angle * i),
                                                               treeStruct->nodes[parentSection.endNode].zAxis);
            sect.xAxis = xAxis;
            float curveAngle = radians((params.nCurve[recursiveLevel] + getVariation(params.nCurveV[recursiveLevel])) / params.nCurveRes[recursiveLevel]);

            // stem curve towards up, don't let it rotate further than up
            if (acos(dot(zAxis, vec3(0, 1, 0))) <= curveAngle) {
                zAxis = vec3(0, 1, 0);
            }
            else {
                zAxis = vec4(zAxis, 1) * rotate(mat4(1),-curveAngle, sect.xAxis);
            }
            vec3 sectNextPos = treeStruct->nodes[parentSection.endNode].position + zAxis * parentLength / params.nCurveRes[recursiveLevel];
            float sectNextRadius = baseR * std::max((1.f - params.nTaper[recursiveLevel] * (stemLevel + 1) / params.nCurveRes[recursiveLevel]), 0.001f);

            treeStruct->nodes.push_back({
                                                sectNextPos,
                                                zAxis,
                                                sectNextRadius
                                        });

            sect.endNode = treeStruct->nodes.size() - 1;
            sect.parentSection = sectionIndex;

            float sectStartRadius = treeStruct->nodes[parentSection.endNode].radius;
            vec3 sectStartPos = treeStruct->nodes[parentSection.endNode].position;


            // compute initial information
            computeSurfaceInformation(sect, sectStartPos, sectNextPos, sectStartRadius, sectNextRadius);

            treeStruct->sections.push_back(sect);

            int child = treeStruct->sections.size() - 1;
            treeStruct->sections[sectionIndex].childSections.push_back(child);

            if (stemLevel < params.nCurveRes[recursiveLevel]) {
                recursiveGenerateTree(params, treeStruct, treeScale, treeStruct->sections.size()-1, recursiveLevel, stemLevel + 1, parentLength, baseR, lastChildRotation);
            }
        }
    }
}

std::vector<cgra::gl_mesh> WeberPennTree::buildMesh(TreeStruct& treeStruct) {

    vector<gl_mesh> meshes;

    for (auto &section : treeStruct.sections) {
        switch (section.level) {
            case 0:
                meshes.push_back(buildSection(treeStruct, section, 12));
                break;
            case 1:
                meshes.push_back(buildSection(treeStruct, section, 7));
                break;
            case 2:
                meshes.push_back(buildSection(treeStruct, section, 5));
                break;
            case 3:
                meshes.push_back(buildSection(treeStruct, section, 4));
                break;
            default:
                meshes.push_back(buildSection(treeStruct, section, 12));
                break;
        }

        section.meshIndex = meshes.size() - 1;
    }

    return meshes;
}

cgra::gl_mesh WeberPennTree::buildSection(TreeStruct treeStruct, Section section, int resolution) {

    std::vector<mesh_vertex> startVertexes = vertexesFromNode(treeStruct.nodes[section.startNode], section.xAxis, resolution);
    std::vector<mesh_vertex> endVertexes = vertexesFromNode(treeStruct.nodes[section.endNode], section.xAxis, resolution);
    for (int i = 0; i < startVertexes.size(); i++) {
        startVertexes[i].uv = vec2(i / float(startVertexes.size() - 1), 0);
        endVertexes[i].uv = vec2(i / float(endVertexes.size() - 1), 1);
    }

    mesh_builder builder;

    for (auto &vert : startVertexes) {
        builder.push_vertex(vert);
    }
    for (auto &vert : endVertexes) {
        builder.push_vertex(vert);
    }

    for (int i = 0; i < startVertexes.size() - 1; i++) {
        builder.push_index(i);
        builder.push_index(i + 1);
        builder.push_index(resolution  + i);

        builder.push_index(i + 1);
        builder.push_index(resolution  + i);
        builder.push_index(resolution  + i + 1);
    }
    return builder.build();
}

std::vector<cgra::mesh_vertex> WeberPennTree::vertexesFromNode(const Node node, const vec3 xAxis, const int resolution) {

    std::vector<mesh_vertex> vertexes;
    float angle_delta = 2 * PI / (resolution - 1);

    vector<vec3> points;
    //cricle
    for (int i = 0; i < resolution; i++) {
        float a = i * angle_delta;
        points.push_back(vec3(sin(a) * node.radius, cos(a) * node.radius, 0));
    }

    // transform points
    mat4 pointTransform = glm::translate(mat4(1), node.position);

    mat4 rot = inverse(lookAt(vec3(0), node.zAxis, cross(node.zAxis, xAxis)));
    pointTransform *= rot;

    for (auto &point : points) {
        mesh_vertex vertex;
        vertex.pos = pointTransform * vec4(point, 1);
        vertex.norm = normalize(vertex.pos -node.position);
        vertexes.push_back(vertex);
    }

    return vertexes;
}


void WeberPennTree::computeSurfaceInformation(Section &s, vec3 sP, vec3 eP, float sR, float eR) {
    float h = distance(sP, eP);

    s.midwayPt.x = (sP.x + eP.x)/2;
    s.midwayPt.y = (sP.y + eP.y)/2;
    s.midwayPt.z = (sP.z + eP.z)/2;
    s.scalar = vec3(1,0,0);

    s.height_0 = h;
    s.surArea_0 = (PI*sR + PI*eR) * sqrt(pow((sR-eR),2) + pow(h,2) + PI*(pow(sR,2) + pow(eR,2)));
    s.depth_0 = eR;
    s.mass_0 = (PI*(pow(sR,2) + (sR*eR) + pow(eR,2))*h)/3;

    s.mass = s.mass_0;
    s.depth = s.depth_0;
    s.surArea = s.surArea_0;
}

float WeberPennTree::shapeRatio(int shape, float ratio) {

    switch(shape) {
        case 0:
            return 0.2f + 0.8f * ratio;
        case 1:
            return 0.2f + 0.8f * sin(PI * ratio);
        case 2:
            return 0.2f + 0.8f * sin(0.5f * PI * ratio);
        case 3:
            return 1.0f;
        case 4:
            return 0.5 + 0.5 * ratio;
        case 5:
            if (ratio <= 0.7f)
                return ratio/0.7f;
            else
                return (1.f - ratio) / 0.3f;
        case 6:
            return 1.f - 0.8f * ratio;
        case 7:
            if (ratio <= 0.7f)
                return 0.5f + 0.5f * ratio / 0.7f;
            else
                return 0.5f + 0.5f * (1.f - ratio) / 0.3f;
        default:
            return 1.0f;
    }
}
