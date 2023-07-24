
#pragma once

#include <vector>

struct TreeParameters {
    int shape;
    float baseSize;
    float scale;
    float scaleV;
    float zScale;
    float zScaleV;
    int levels;
    float ratio;
    float ratioPower;
    int baseSplits;

    std::vector<float> nLength;
    std::vector<float> nLengthV;
    std::vector<float> nTaper;

    std::vector<float> nSegSplits;
    std::vector<float> nSplitAngle;
    std::vector<float> nSplitAngleV;

    std::vector<float> nCurveRes;
    std::vector<float> nCurve;
    std::vector<float> nCurveV;

    std::vector<float> nDownAngle;
    std::vector<float> nDownAngleV;

    std::vector<float> nRotate;
    std::vector<float> nRotateV;
    std::vector<float> nBranches;
};
