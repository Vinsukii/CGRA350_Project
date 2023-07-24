#define GLM_ENABLE_EXPERIMENTAL
// std
#include <iostream>
#include <string>
#include <chrono>

// glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

// project
#include "application.hpp"
#include "cgra/cgra_geometry.hpp"
#include "cgra/cgra_gui.hpp"
#include "cgra/cgra_image.hpp"
#include "cgra/cgra_shader.hpp"
#include "WeberPennTree.h"
#include "TreeLibrary.h"

#define PI 3.1415926
#define E 2.7182818

using namespace std;
using namespace cgra;
using namespace glm;
using namespace chrono;
using namespace particleSim;


int startSection = 0; // start the combustion at this section

float T_AMB = 15;  // Ambient temp
float T_air = 15;  // Air temp
float T_tree = 25; // Tree temp
float T0 = 150;   // T_min
float T1 = 750;   // T_max

// Controllable variables during combustion
float massLossRate = 0.00005f;       // m
float airHeatTransferCoeff = 0.005f; // t_m
float tempDiffussionCoeff = 0.005f;  // a
float charContractionCoeff = 0.8f;   // k
float charInsulationCoeff = 0.5f;    // c_min

// Fixed values
float dryCoeff = 1.0f;         // b_dry
float waterContent = 0.0f;     // W
float heatTransferCoeff = 0.f; // b


void basic_model::draw(const glm::mat4 &view, const glm::mat4 proj, TreeStruct &tree) {
    mat4 modelview = view * modelTransform;

    glUseProgram(shader); // load shader and variables
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
    glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));


    for (int i = 0; i < tree.sections.size(); i++) {
        // update scalar for texture blending
        glUniform3fv(glGetUniformLocation(shader, "uScalar"), 1, value_ptr(tree.sections[i].scalar));
        meshes[i].draw(); // draw mesh
    }
}

void basic_ps::draw(const glm::mat4 &view, const glm::mat4 proj, const glm::vec2 winSize) {
    mat4 modelview = view * modelTransform;
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    glUseProgram(shader); // load shader and variables
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
    glUniform1i(glGetUniformLocation(shader, "uTextureFire"), 3);
    glUniform2fv(glGetUniformLocation(shader, "uWindowSize"), 1, value_ptr(winSize));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    mesh.drawInstanced(ps.particle_count); // draw

    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}


Application::Application(GLFWwindow *window) : m_window(window) {
    WeberPennTree WPTree;
    m_treeParams = TreeLibrary::preset(0);
    m_treeStruct = WPTree.generateTree(m_treeParams);
    vector<gl_mesh> treeMeshes = WPTree.buildMesh(m_treeStruct);

    // shaders (w/ normal map)
    shader_builder sb;
    sb.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//texture_vert.glsl"));
    sb.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//texture_frag.glsl"));
    GLuint shader = sb.build();

    // textures
    GLuint tex_0 = 0;
    GLuint tex_1 = 0;
    GLuint tex_2 = 0;
    GLuint tex_3 = 0;

    rgba_image img_tex_untouch(CGRA_SRCDIR + std::string("//res//textures//wood_a2_tex.jpg"));
    rgba_image img_tex_burning(CGRA_SRCDIR + std::string("//res//textures//wood_burning_tex.jpg"));
    rgba_image img_tex_charred(CGRA_SRCDIR + std::string("//res//textures//wood_char2_tex.jpg"));
    rgba_image  img_normal_map(CGRA_SRCDIR + std::string("//res//textures//wood_normal_map2.jpg"));

    img_tex_untouch.uploadTexture(0, GL_RGBA8, tex_0);
    img_tex_burning.uploadTexture(1, GL_RGBA8, tex_1);
    img_tex_charred.uploadTexture(2, GL_RGBA8, tex_2);
    img_normal_map.uploadTexture( 4, GL_RGBA8, tex_3);

    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "tex_0"), 0);
    glUniform1i(glGetUniformLocation(shader, "tex_1"), 1);
    glUniform1i(glGetUniformLocation(shader, "tex_2"), 2);
    glUniform1i(glGetUniformLocation(shader, "tex_3"), 4);

    m_model.shader = shader;
    m_model.meshes = treeMeshes;
    m_cam_pos = vec2( 0, -3 );


    // build fire
    shader_builder sb_fire;

    cgra::rgba_image smoke_sprite(CGRA_SRCDIR + std::string("//res//textures//smokeTex.png"));
    GLuint texture_fire = 0;
    m_psModel.texture = smoke_sprite.uploadTexture(3, GL_RGBA8, 0);

    m_psModel.ps = particle_system(0);

    sb_fire.set_shader(GL_VERTEX_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_vert_fire.glsl"));
    sb_fire.set_shader(GL_FRAGMENT_SHADER, CGRA_SRCDIR + std::string("//res//shaders//color_frag_fire.glsl"));
    GLuint shader_fire = sb_fire.build();

    m_psModel.ps = particle_system();
    m_psModel.shader = shader_fire;
    m_psModel.mesh = m_psModel.ps.build();
    m_psModel.color = vec3(1, 0, 0);
}


chrono::time_point start_time = chrono::high_resolution_clock::now();

void Application::render() {
    // retrieve the window height
    int width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    m_windowsize = vec2(width, height); // update window size
    glViewport(0, 0, width, height); // set the viewport to draw to the entire window

    // clear the back-buffer
    glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // enable flags for normal/forward rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // projection matrix
    mat4 proj = perspective(1.f, float(width) / height, 0.1f, 1000.f);

    // view matrix
    mat4 view = translate(mat4(1), vec3( m_cam_pos.x, m_cam_pos.y, -m_distance))
                * rotate(mat4(1), m_pitch, vec3(1, 0, 0))
                * rotate(mat4(1), m_yaw,   vec3(0, 1, 0));

    // helpful draw options
    if (m_show_grid) drawGrid(view, proj);
    if (m_show_axis) drawAxis(view, proj);
    glPolygonMode(GL_FRONT_AND_BACK, (m_showWireframe) ? GL_LINE : GL_FILL);

    WeberPennTree WPTree;

    // MAIN COMBUSTION LOOP //
    if (startCombustion) {
        decomposeMaterial(m_treeStruct); // main fx

        for (auto &mesh: m_model.meshes) mesh.destroy(); // destroy
        m_model.meshes = WPTree.buildMesh(m_treeStruct); // recompute meshes
    }
    m_model.draw(view, proj, m_treeStruct);

    // END //


    // MAIN FIRE LOOP //
    if (m_psModel.ps.startSimulation) {
        auto current_time = chrono::high_resolution_clock::now();
        if (chrono::duration_cast<chrono::milliseconds>(current_time - start_time).count() >= 33) {
            m_psModel.ps.update();
            start_time = chrono::high_resolution_clock::now();
        }
        m_psModel.draw(view, proj, vec2(width, height));
    }

    // END //
}

void Application::decomposeMaterial(TreeStruct &tree){
    for (unsigned int i = 0; i < tree.sections.size(); i++) {
        Section s = tree.sections[i];
        int sN = s.startNode;
        int eN = s.endNode;
        T_tree += s.surfaceTemp;

        if (s.combusting & !s.finished) {
            s.surfaceTemp += 0.2;

            // get reaction rate based on surface temperature
            s.massLossRate = computeReactionRate(s.surfaceTemp);

            // update section mass & surface temperature
            s.mass -= s.massLossRate*s.surArea;
            tree.sections[i].surfaceTemp = s.surfaceTemp;

            if (s.massLossRate > 0) {
                s.depth = sqrt(2*(s.mass/s.density)*(s.depth_0/s.surArea_0));

                // update surface information
                tree.sections[i].mass = s.mass;
                tree.sections[i].depth = s.depth;

                if (s.depth < s.depth_0) {
                    // surface area
                    tree.sections[i].surArea = s.surArea_0 * s.depth / s.depth_0;
                    // char thickness
                    tree.sections[i].charThickness = charContractionCoeff * ((s.depth_0 - s.depth)*60);
                    // insulation effect
                    tree.sections[i].insulEffect = charInsulationCoeff + ((1 - charInsulationCoeff) * pow(E, -s.insulEffect*tree.sections[i].charThickness));

                    tree.nodes[eN].radius = s.depth; // deform the branch
                }

                // update scalar value for texture blending (between untouched & burning tex)
                tree.sections[i].scalar = vec3(s.mass/s.mass_0, 1-(s.mass/s.mass_0),0);

            }

            // stop the section combustion when the
            // current mass is less than 20% of its original mass
            if (s.mass < s.mass_0*0.20) {
                if (!s.finished) {
                    tree.sections[i].finished = s.finished = true;
                    m_psModel.ps.killEmitter(i);
                }
            }

            // heat transfer to neighbour and the air
            heatTransfer(tree, s, i);
        }

        // finished combusting
        if (s.finished) {
            // cool down effect
            if (T_air > T_AMB) T_air -= 0.001;
            if (s.surfaceTemp > T_AMB) tree.sections[i].surfaceTemp -= 1;

            // update scalar for texture blending (between burning & charred tex)
            if (s.scalar.x > 0.15)
                tree.sections[i].scalar.x -= 0.0002;
            if (s.scalar.y > 0.05)
                tree.sections[i].scalar.y -= 0.0002;
            if (s.scalar.z <= 0.70)
                tree.sections[i].scalar.z += 0.0006;
        }
    }
    T_tree /= tree.sections.size();
}


float Application::computeReactionRate(float T) {
    float lossRate = massLossRate;
    if      (T < T0) lossRate *= 0;
    else if (T > T1) lossRate *= 1;
    else             lossRate *= S((T-T0)/(T1-T0));

    return lossRate;
}

void Application::heatTransfer(TreeStruct &tree, Section &s, int id) {
    if (!s.finished) {
        // Getting neighbours temperature
        float pT = T_AMB; // parent
        float cT = T_AMB; // children

        int pID = s.parentSection;
        if (pID > -1) pT = tree.sections[pID].surfaceTemp;

        int cSize = s.childSections.size();
        if (cSize > 0) {
            float avg = 0;
            for (int cID: s.childSections)
                avg += tree.sections[cID].surfaceTemp;
            cT = avg / cSize; // average across children
        }

        // compute the heat transfer coefficient based on the water content
        heatTransferCoeff = 1-((1-waterContent)*dryCoeff+waterContent*(0.001*dryCoeff));

        // Calculate the SURFACE heat transfer
        float heatT = (tempDiffussionCoeff*((s.surfaceTemp + pT + cT + T_AMB)/4) + heatTransferCoeff*(T_air - s.surfaceTemp)*0.01);
        heatT = glm::clamp(heatT, -5.0f, 30.0f); // limit the value so it doesn't blow up

        // Calculate the AIR heat transfer
        float airT = (s.mass/s.mass_0)*1.5;
        T_air += airHeatTransferCoeff * airT;

        // getting the delta change
        float delta = s.insulEffect * (heatT+airT)/2;


        // HT to parent
        if (pID > -1 && pT+delta > T_AMB && pT < T1*2)
            tree.sections[pID].surfaceTemp += delta;

        // checking for ignition conditions
        if (pID > -1 && ignitionConditionsMet(tree.sections[pID], pID)) {
            int pSN = tree.sections[pID].startNode;
            tree.sections[pID].combusting = true;
            m_psModel.ps.addEmitter(tree.nodes[pSN].position, tree.nodes[pSN].radius, pID);
        }

        // HT to children
        if (cSize > 0) {
            for (int cID : s.childSections) {
                if (cT+delta > T_AMB && cT < T1*8)
                    tree.sections[cID].surfaceTemp += delta;

                // checking for ignition conditions
                if (ignitionConditionsMet(tree.sections[cID], id)) {
                    int cSN = tree.sections[cID].startNode;
                    if (cSize == 1) {
                        tree.sections[cID].combusting = true;
                        m_psModel.ps.addEmitter(tree.nodes[cSN].position, tree.nodes[cSN].radius, cID);
                    }
                    else if (rand() % 100 < 10) {
                        tree.sections[cID].combusting = true;
                        m_psModel.ps.addEmitter(tree.nodes[cSN].position, tree.nodes[cSN].radius, cID);
                    }
                }
            }
        }

        // Update emitter range (fire)
        vec3 ePos = tree.nodes[s.endNode].position;
        vec3 sPos = tree.nodes[s.startNode].position;
        vec3 dir = normalize(ePos - sPos);
        float scale = 0.8 * s.height_0;
        m_psModel.ps.updateEmitterRange(dir, scale, id);
    }
}

bool Application::ignitionConditionsMet(Section s, int pId) {
    // checking if surface temperature > T0 & parent exists
    if   (s.surfaceTemp >= T0 && !s.combusting && pId > -1) return true;
    else return false;
}


void Application::renderGUI() {

    // setup window
    ImGui::SetNextWindowPos(ImVec2(5, 5), ImGuiSetCond_Once);
    ImGui::SetNextWindowSize(ImVec2(400, 255), ImGuiSetCond_Once);
    ImGui::Begin("Options", 0);

    // display current camera parameters
    if (ImGui::TreeNode("Camera")) {
        ImGui::Text("Application %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::SliderFloat("Pitch", &m_pitch, -pi<float>() / 2, pi<float>() / 2, "%.2f");
        ImGui::SliderFloat("Yaw", &m_yaw, -pi<float>(), pi<float>(), "%.2f");
        ImGui::SliderFloat("Distance", &m_distance, 0, 100, "%.2f", 2.0f);

        // helpful drawing options
        ImGui::Checkbox("Show axis", &m_show_axis);
        ImGui::SameLine();
        ImGui::Checkbox("Show grid", &m_show_grid);
        ImGui::Checkbox("Wireframe", &m_showWireframe);
        ImGui::SameLine();
        if (ImGui::Button("Screenshot")) rgba_image::screenshot(true);

        ImGui::TreePop();
    }

    ImGui::Separator();

    if (ImGui::Button("Preset0")) {
        m_treeParams = TreeLibrary::preset(0);
    }
    ImGui::SameLine();
    if (ImGui::Button("Preset1")) {
        m_treeParams = TreeLibrary::preset(1);
    }

    if (ImGui::Button("Preset2")) {
        m_treeParams = TreeLibrary::preset(2);
    }
    ImGui::SameLine();
    if (ImGui::Button("Preset3")) {
        m_treeParams = TreeLibrary::preset(3);
    }
    ImGui::SameLine();
    if (ImGui::Button("Preset4")) {
        m_treeParams = TreeLibrary::preset(4);
    }
    ImGui::SameLine();
    if (ImGui::Button("Preset5")) {
        m_treeParams = TreeLibrary::preset(5);
    }
    ImGui::SameLine();
    if (ImGui::Button("Preset6")) {
        m_treeParams = TreeLibrary::preset(6);
    }
    ImGui::SliderInt("Shape", &m_treeParams.shape, 0, 7);

    if (ImGui::Button("Generate")) {
        WeberPennTree WPTree;
        m_treeStruct = WPTree.generateTree(m_treeParams);
        for (auto &mesh: m_model.meshes) mesh.destroy(); // destroy
        m_model.meshes = WPTree.buildMesh(m_treeStruct); // recompute meshes
        m_psModel.ps.emitters.clear();
    }

    ImGui::Separator();

    ImGui::SetNextTreeNodeOpen(true);

    if (ImGui::TreeNode("Simulation")) {
        if (ImGui::Button(". START .")) {
            startCombustion = true;
            m_psModel.ps.startSimulation = true;

            m_treeStruct.sections[startSection].combusting = true;
            m_treeStruct.sections[startSection].surfaceTemp = T0 + 300;

            int nID = m_treeStruct.sections[startSection].startNode;
            m_psModel.ps.addEmitter(m_treeStruct.nodes[nID].position, m_treeStruct.nodes[nID].radius, startSection);
        }
        ImGui::SameLine();
        if (ImGui::Button(". STOP .")) {
            for (int i = 0; i < m_treeStruct.sections.size(); i++) m_psModel.ps.killEmitter(i);
            startCombustion = false;

            // reset values
            T_air = T_AMB;
            T_tree = T_AMB;

            massLossRate = 0.00005f;
            airHeatTransferCoeff = 0.005f;
            tempDiffussionCoeff = 0.005f;
            charInsulationCoeff = 0.5f;
            waterContent = 0.f;
        }

        ImGui::Text("Air  Temperature: %.3f", T_air);
        ImGui::Text("Tree Temperature: %.3f", T_tree);

        ImGui::Separator();

        ImGui::Text("Parameter Controls:");
        ImGui::SliderInt("Start Section", &startSection, 0, m_treeStruct.sections.size());
        ImGui::SliderFloat("Mass Loss Rate", &massLossRate, 0.00001f, 0.0005f, "%.6f");
        ImGui::SliderFloat("Air Heat Transfer coeff", &airHeatTransferCoeff, 0.0001, 0.01, "%.4f");
        ImGui::SliderFloat("Surface Diffusion coeff", &tempDiffussionCoeff, 0.0001, 0.01, "%.4f");
        ImGui::SliderFloat("Char Insulation coeff", &charInsulationCoeff, 0, 1, "%.3f");
        ImGui::SliderFloat("Water Content", &waterContent, 0, 1, "%.3f");

        ImGui::TreePop();
    }

    // finish creating window
    ImGui::End();
}


void Application::cursorPosCallback(double xpos, double ypos) {
    vec2 whsize = m_windowsize / 2.0f;

    double y0 = glm::clamp((m_mousePosition.y - whsize.y) / whsize.y, -1.0f, 1.0f);
    double y = glm::clamp((float(ypos) - whsize.y) / whsize.y, -1.0f, 1.0f);
    double dy = -( y - y0 );

    double x0 = glm::clamp((m_mousePosition.x - whsize.x) / whsize.x, -1.0f, 1.0f);
    double x = glm::clamp((float(xpos) - whsize.x) / whsize.x, -1.0f, 1.0f);
    double dx = x - x0;

    if (m_leftMouseDown) {
        // clamp the pitch to [-pi/2, pi/2]
        m_pitch += float( acos(y0) - acos(y) );
        m_pitch = float(glm::clamp(m_pitch, -pi<float>() / 2, pi<float>() / 2));

        // wrap the yaw to [-pi, pi]
        m_yaw += float( acos(x0) - acos(x) );
        if (m_yaw > pi<float>())
            m_yaw -= float(2 * pi<float>());
        else if (m_yaw < -pi<float>())
            m_yaw += float(2 * pi<float>());
    } else if ( m_rightMouseDown ) {
        m_distance += dy * 10;
    } else if ( m_middleMouseDown ) {
        m_cam_pos += vec2( dx, dy ) * 10.f;
    }

    // updated mouse position
    m_mousePosition = vec2(xpos, ypos);
}


void Application::mouseButtonCallback(int button, int action, int mods) {
    (void)mods; // currently un-used

    // capture is left-mouse down
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        m_leftMouseDown = (action == GLFW_PRESS); // only other option is GLFW_RELEASE
    else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        m_rightMouseDown = (action == GLFW_PRESS);
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        m_middleMouseDown = (action == GLFW_PRESS);
}


void Application::scrollCallback(double xoffset, double yoffset) {
    (void)xoffset; // currently un-used
    m_distance *= pow(1.1f, -yoffset);
}


void Application::keyCallback(int key, int scancode, int action, int mods) {
    (void)key, (void)scancode, (void)action, (void)mods; // currently un-used
}


void Application::charCallback(unsigned int c) {
    (void)c; // currently un-used
}
