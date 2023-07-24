//
// Created by Rhysa on 27/09/2022.
//

#include "TreeLibrary.h"

TreeParameters TreeLibrary::preset(int presetIndex) {
    switch (presetIndex) {
        default:
        case 0:
            return TreeParameters{0,                                            // shape
                                  0.15,                                          // baseSize
                                  10,                                            // scale
                                  0,                                           // scaleV
                                  1,                                           // ZScale
                                  0,                                          // ZScaleV
                                  3,
                                  0.015,
                                  1.15f,
                                  0,
                                  std::vector<float>{1.0f, 0.4f, 0.3f, 0.5},
                                  std::vector<float>{0, 0.05, 0, 0},
                                  std::vector<float>{0.96, 0.96, 0.96, 0.96},
                                  std::vector<float>{0., 0., 0, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{12, 4, 2, 1},
                                  std::vector<float>{30, 30, 10, 0},
                                  std::vector<float>{10, 5, 30, 0},
                                  std::vector<float>{0, 40, 40, 0},
                                  std::vector<float>{0, 10, 10, 0},
                                  std::vector<float>{0, 110, 100, 0},
                                  std::vector<float>{0, 30, 30, 0},
                                  std::vector<float>{0, 20, 10, 0},
            };
        case 1:
            return TreeParameters{0,                                            // shape
                                  0.2,                                          // baseSize
                                  10,                                            // scale
                                  0,                                           // scaleV
                                  1,                                           // ZScale
                                  0,                                          // ZScaleV
                                  3,
                                  0.017,
                                  1.10f,
                                  0,
                                  std::vector<float>{1.0f, 0.3f, 0.5f, 0.4},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0.96, 0.96, 0.96, 0.96},
                                  std::vector<float>{0., 0., 0, 0},
                                  std::vector<float>{20, 20, 0, 0},
                                  std::vector<float>{5, 5, 0, 0},
                                  std::vector<float>{10, 4, 3, 1},
                                  std::vector<float>{0, 5, 20, 0},
                                  std::vector<float>{15, 30, 5, 0},
                                  std::vector<float>{0, 60, 30, 10},
                                  std::vector<float>{0, -90, 20, 0},
                                  std::vector<float>{0, 140, 110, },
                                  std::vector<float>{0, 10, 40, 0},
                                  std::vector<float>{0, 16, 6, 0},
            };
        case 2:
            return TreeParameters{2,                                            // shape
                                  0.1,                                          // baseSize
                                  7,                                            // scale
                                  0,                                           // scaleV
                                  1,                                           // ZScale
                                  0,                                          // ZScaleV
                                  3,
                                  0.015f,
                                  1.0f,
                                  1,
                                  std::vector<float>{1.0f, 0.8f, 0.1f, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{1, 1, 1, 1},
                                  std::vector<float>{0.4, 0.1, 0.2, 0},
                                  std::vector<float>{30, 30, 35, 0},
                                  std::vector<float>{10, 10, 20, 0},
                                  std::vector<float>{5, 5, 3, 1},
                                  std::vector<float>{0, 40, 0, 0},
                                  std::vector<float>{40, 150, 50, 0},
                                  std::vector<float>{0, 30, 30, 0},
                                  std::vector<float>{0, -30, -20, 0},
                                  std::vector<float>{0, 110, 100, 0},
                                  std::vector<float>{0, 30, 20, 0},
                                  std::vector<float>{0, 0, 0, 0},
            };

        case 3:
            return TreeParameters{2,                                            // shape
                                  0.1,                                          // baseSize
                                  7,                                            // scale
                                  0,                                           // scaleV
                                  1,                                           // ZScale
                                  0,                                          // ZScaleV
                                  3,
                                  0.015f,
                                  1.0f,
                                  1,
                                  std::vector<float>{1.0f, 0.8f, 0.3f, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{1, 1, 1, 1},
                                  std::vector<float>{0.4, 0.1, 0.2, 0},
                                  std::vector<float>{30, 30, 35, 0},
                                  std::vector<float>{10, 10, 20, 0},
                                  std::vector<float>{5, 5, 3, 1},
                                  std::vector<float>{0, 40, 0, 0},
                                  std::vector<float>{40, 150, 50, 0},
                                  std::vector<float>{0, 30, 30, 0},
                                  std::vector<float>{0, -30, -20, 0},
                                  std::vector<float>{0, 110, 100, 0},
                                  std::vector<float>{0, 30, 20, 0},
                                  std::vector<float>{0, 10, 2, 0},
            };

        case 4:
            return TreeParameters{0,                                            // shape
                                  0.4,                                          // baseSize
                                  13,                                            // scale
                                  0,                                           // scaleV
                                  1,                                           // ZScale
                                  0,                                          // ZScaleV
                                  3,
                                  0.015,
                                  1.15f,
                                  0,
                                  std::vector<float>{1.0f, 0.5f, 0.6f, 0.4},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0.96, 0.96, 0.96, 0.96},
                                  std::vector<float>{0., 0., 0, 0},
                                  std::vector<float>{20, 20, 0, 0},
                                  std::vector<float>{5, 5, 0, 0},
                                  std::vector<float>{3, 5, 3, 1},
                                  std::vector<float>{0, 5, 20, 0},
                                  std::vector<float>{15, 30, 5, 0},
                                  std::vector<float>{0, 60, 30, 10},
                                  std::vector<float>{0, -90, 20, 0},
                                  std::vector<float>{0, 140, 110, },
                                  std::vector<float>{0, 10, 40, 0},
                                  std::vector<float>{0, 30, 10, 0},
            };

        case 5:
            return TreeParameters{4,                                            // shape
                                  0.2,                                          // baseSize
                                  15,                                            // scale
                                  5,                                           // scaleV
                                  1,                                           // ZScale
                                  0,                                          // ZScaleV
                                  3,
                                  0.015,
                                  1.3,
                                  0,
                                  std::vector<float>{1, 0.3, 0.6, 0.4},
                                  std::vector<float>{0, 0.05, 0.1, 0},
                                  std::vector<float>{0.96, 0.96, 0.96, 0.96},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{10, 10, 10, 1},
                                  std::vector<float>{0, 0, 10, 0},
                                  std::vector<float>{40, 90, 150, 0},
                                  std::vector<float>{0, 60, 30, 45},
                                  std::vector<float>{0, -40, 10, 10},
                                  std::vector<float>{0, 140, 140, 140},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0, 30, 20, 6},
            };
        case 6:
            return TreeParameters{7,                                            // shape
                                  0.4,                                          // baseSize
                                  13,                                            // scale
                                  3,                                           // scaleV
                                  1,                                           // ZScale
                                  0,                                          // ZScaleV
                                  3,
                                  0.015,
                                  1.2,
                                  0,
                                  std::vector<float>{1, 0.3, 0.4, 1},
                                  std::vector<float>{0, 0, 0.1, 0},
                                  std::vector<float>{0.96, 0.96, 0.96, 0.96},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{6, 5, 3, 1},
                                  std::vector<float>{0, 20, 20, 0},
                                  std::vector<float>{20, 50, 50, 0},
                                  std::vector<float>{0, 60, 45, 0},
                                  std::vector<float>{0, -50, 10, 0},
                                  std::vector<float>{0, 140, 140, 0},
                                  std::vector<float>{0, 0, 0, 0},
                                  std::vector<float>{0, 60, 20, 0},
            };
    }
}


