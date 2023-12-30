#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/mesh-renderer.hpp"
#include "../components/light.hpp"
#include "../asset-loader.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>
#include <iostream>

namespace our
{

    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand
    {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh *mesh;
        Material *material;
    };

    // The light object stores the data of a light component in a way that is easier to use
    struct LightObject
    {
        LightType type;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;
        glm::vec3 attenuation;
        glm::vec2 cone;
        glm::vec3 position;
        glm::vec3 direction;

        LightObject(LightComponent *light)
        {
            type = light->type;
            diffuse = light->diffuse;
            specular = light->specular;
            ambient = light->ambient;
            attenuation = light->attenuation;
            cone = light->cone;
        }

        void static setup(RenderCommand &command, std::vector<LightObject> &lights)
        {
            int index = 0;
            for (auto &light : lights)
            {
                // make a uniform name for each light in the shader, e.g. "lights[index].type"
                std::string name = "lights[" + std::to_string(index++) + "].";
                command.material->shader->set(name + "type", (int)light.type);
                command.material->shader->set(name + "diffuse", light.diffuse);
                command.material->shader->set(name + "specular", light.specular);
                command.material->shader->set(name + "ambient", light.ambient);
                command.material->shader->set(name + "position", light.position);
                command.material->shader->set(name + "direction", light.direction);
                command.material->shader->set(name + "attenuation", light.attenuation);
                command.material->shader->set(name + "cone", light.cone);
            }
            // set the light count uniform in the shader
            command.material->shader->set("light_count", (int)lights.size());
        }
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer
    {
        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;

        std::vector<LightObject> lights;
        // Objects used for rendering a skybox
        Mesh *skySphere;
        TexturedMaterial *skyMaterial;

        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial *postprocessMaterial;

        // Booleans to check on for postprocessing effects
        bool doomed = false;    // if the player hit a asteroid
        bool shaken = false;    // if the bullet hit the asteroid
        std::string  normalPath, doomPath, shakePath; 

    public:
        // Initialize the renderer including the sky and the Postprocessing objects.
        // windowSize is the width & height of the window (in pixels).
        void initialize(glm::ivec2 windowSize, const nlohmann::json &config);
        // Clean up the renderer
        void destroy();
        // This function should be called every frame to draw the given world
        void render(World *world);

        // Set Doom effect
        void setDoomed(bool doom) { this->doomed = doom; }
        void setShaken(bool shake) { this->shaken = shake; }
        bool getShaked() {return shaken;}
    };

}