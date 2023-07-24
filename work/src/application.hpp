
#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "opengl.hpp"
#include "cgra/cgra_mesh.hpp"
#include "skeleton_model.hpp"
#include "WeberPennTree.h"
#include "particleSim/KDTree.hpp"
#include "particleSim/particle_system.hpp"



// Basic model that holds the shader, mesh and transform for drawing.
// Can be copied and modified for adding in extra information for drawing
// including textures for texture mapping etc.
struct basic_model {
    GLuint shader = 0;
    cgra::gl_mesh mesh;
    std::vector<cgra::gl_mesh> meshes;
    glm::vec3 color{0.7};
    glm::mat4 modelTransform{1.0};
    GLuint texture;

    void draw(const glm::mat4 &view, const glm::mat4 proj, TreeStruct &tree);
};

struct basic_ps {
    GLuint shader = 0;
    cgra::gl_mesh mesh;
    particleSim::particle_system ps;
    glm::vec3 color{0.7};
    glm::mat4 modelTransform{1.0};
    GLuint texture;

    void draw(const glm::mat4 &view, const glm::mat4 proj, const glm::vec2 winSize);
};


// Main application class
//
class Application {
private:
    // window
    glm::vec2 m_windowsize;
    GLFWwindow *m_window;

    // oribital camera
    float m_pitch = .70;
    float m_yaw = -.50;
    float m_distance = 15;
    glm::vec2 m_cam_pos;

    // last input
    bool m_leftMouseDown = false;
    bool m_rightMouseDown = false;
    bool m_middleMouseDown = false;
    bool startCombustion = false;
    glm::vec2 m_mousePosition;

    // drawing flags
    bool m_show_axis = false;
    bool m_show_grid = false;
    bool m_showWireframe = false;

    // geometry
    basic_model m_model;
    basic_ps m_psModel;

    // tree
    TreeStruct m_treeStruct;
    TreeParameters m_treeParams;

public:
    // setup
    Application(GLFWwindow *);

    // disable copy constructors (for safety)
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // rendering callbacks (every frame)
    void render();
    void renderGUI();

    // input callbacks
    void cursorPosCallback(double xpos, double ypos);
    void mouseButtonCallback(int button, int action, int mods);
    void scrollCallback(double xoffset, double yoffset);
    void keyCallback(int key, int scancode, int action, int mods);
    void charCallback(unsigned int c);


    // COMBUSTION MODEL //

    /**
     * A sigmoid-like function for interpolating smoothly from zero to one for a value between T0 and T1
     * @param x (the surface temperature of the section)
     * return float - interpolated value
     */
    float S(float x) { return (3*pow(x,2) - 2*pow(x,3)); };

    /**
     * Method for computing the reaction rate for a section given it current surface temperature
     * @param T (the surface temperature of the section)
     * return float - reaction rate
     */
    float computeReactionRate(float T);

    /**
     * Method for decomposing the branch structure (pyrolysis) via physical attributes and changes
     * @param tree (the tree structure)
     */
    void decomposeMaterial(TreeStruct &tree);

    /**
     * Method for spreading the heat to neighbouring sections and the air via physical properties
     * @param tree (the tree structure)
     * @param s    (the current section)
     * @param id   (id of the section)
     */
    void heatTransfer(TreeStruct &tree, Section &s, int id);

    /**
     * Method for checking if the ignition conditions are met for a given section
     * @param s   (the section to check)
     * @param pId (parent id)
     * @return boolean - 'true' if conditions are met; else 'false'
     */
    bool ignitionConditionsMet(Section s, int pId);


    // END //
};