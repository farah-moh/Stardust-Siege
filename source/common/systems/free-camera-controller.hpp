#pragma once
#include <iostream>
#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include "../../states/play-state.hpp"

#include <irrKlang.h>
using namespace irrklang;

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreeCameraControllerSystem {
        Application* app; // The application in which the state runs
        bool mouse_locked = false; // Is the mouse locked
        nlohmann::json shield;
        Entity* shieldEnt;
        ISoundEngine *DJAmro7a7a = nullptr;

    public:
        // When a state enters, it should call this function and give it the pointer to the application
        bool shielded = false;
        void enter(Application* app, nlohmann::json shield){
            this->app = app;
            this->shield = shield;
            DJAmro7a7a = createIrrKlangDevice();
            DJAmro7a7a->setSoundVolume(0.3f); 
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent 
        void update(World* world, float deltaTime, int& score) {
            // First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
            // As soon as we find one, we break
            CameraComponent* camera = nullptr;
            Entity* player = nullptr;
            FreeCameraControllerComponent *controller = nullptr;
            for(auto entity : world->getEntities()){
                camera = entity->getComponent<CameraComponent>();
                controller = entity->getComponent<FreeCameraControllerComponent>();
                if(camera && controller) break;
            }
            for(auto entity : world->getEntities()){
                if(entity->name == "player") player = entity;
            }
            // If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
            if(!(camera && controller)) return;
            // Get the entity that we found via getOwner of camera (we could use controller->getOwner())
            Entity* entity = camera->getOwner();

            // If the left mouse button is pressed, we lock and hide the mouse. This common in First Person Games.
            if(app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && !mouse_locked){
                app->getMouse().lockMouse(app->getWindow());
                mouse_locked = true;
            // If the left mouse button is released, we unlock and unhide the mouse.
            } else if(!app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && mouse_locked) {
                app->getMouse().unlockMouse(app->getWindow());
                mouse_locked = false;
            }

            // We get a reference to the entity's position and rotation
            glm::vec3& position = entity->localTransform.position;
            glm::vec3& rotation = entity->localTransform.rotation;

            // If the left mouse button is pressed, we get the change in the mouse location
            // and use it to update the camera rotation
            if(app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1)){
                glm::vec2 delta = app->getMouse().getMouseDelta();
                rotation.x -= delta.y * controller->rotationSensitivity; // The y-axis controls the pitch
                rotation.y -= delta.x * controller->rotationSensitivity; // The x-axis controls the yaw
            }

            // We prevent the pitch from exceeding a certain angle from the XZ plane to prevent gimbal locks
            if(rotation.x < -glm::half_pi<float>() * 0.99f) rotation.x = -glm::half_pi<float>() * 0.99f;
            if(rotation.x >  glm::half_pi<float>() * 0.99f) rotation.x  = glm::half_pi<float>() * 0.99f;
            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time. 
            rotation.y = glm::wrapAngle(rotation.y);

            // We update the camera fov based on the mouse wheel scrolling amount
            float fov = camera->fovY + app->getMouse().getScrollOffset().y * controller->fovSensitivity;
            fov = glm::clamp(fov, glm::pi<float>() * 0.01f, glm::pi<float>() * 0.99f); // We keep the fov in the range 0.01*PI to 0.99*PI
            camera->fovY = fov;

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = entity->localTransform.toMat4();

            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)), 
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;
            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if(app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT)) current_sensitivity *= controller->speedupFactor;

            // We change the camera position based on the keys WASD/QE
            // S & W moves the player back and forth
            glm::vec3& playerRotation = player->localTransform.rotation;
            bool isRotating = false;
            if(app->getKeyboard().isPressed(GLFW_KEY_W)) {
                position += front * (deltaTime * current_sensitivity.z);
                // tilting logic
                if (playerRotation.z < 0.1) {
                    isRotating = true;
                    playerRotation.z += 0.01f;
                }
            }
            if(app->getKeyboard().isPressed(GLFW_KEY_S)) position -= front * (deltaTime * current_sensitivity.z);
            // Q & E moves the player up and down
            if(app->getKeyboard().isPressed(GLFW_KEY_Q)) position += up * (deltaTime * current_sensitivity.y);
            if(app->getKeyboard().isPressed(GLFW_KEY_E)) position -= up * (deltaTime * current_sensitivity.y);
            // A & D moves the player left or right 
            if(app->getKeyboard().isPressed(GLFW_KEY_D)) {
                position += right * (deltaTime * current_sensitivity.x);
                // tilting logic
                if (playerRotation.x > -1.0) {
                    isRotating = true;
                    playerRotation.x -= 0.1f;
                }
            }
            if(app->getKeyboard().isPressed(GLFW_KEY_A)) {
                position -= right * (deltaTime * current_sensitivity.x);
                // tilting logic
                if (playerRotation.x < 1.0) {
                    isRotating = true;
                    playerRotation.x += 0.1f;
                }
            }
            // return to initial position
            if(app->getKeyboard().isPressed(GLFW_KEY_R)) position = glm::vec3(0, 0, 10);
            // apply powerup
            if(app->getKeyboard().isPressed(GLFW_KEY_F) && score >= 1 && !shielded) {
                DJAmro7a7a->play2D("assets/sounds/shield.mp3", false, false, true);
                score--;
                shieldEnt = world->addEntityAndDeserialize(shield, entity);
                auto position = player->localTransform.position;
                shieldEnt->localTransform.position = {position[0]+0.03,position[1]+0.12,position[2]+1.2};
                //shieldEnt->localTransform.rotation = rotation;
                shielded = true;
            }

            if(app->getKeyboard().justPressed(GLFW_KEY_SPACE)) {
                DJAmro7a7a->play2D("assets/sounds/bullet2.0.mp3", false, false, true);
                auto config = app->getConfig();
                // get the bullet config
                auto bulletJson = config["scene"]["runtimeEntity"][0];
                //auto position = player->localTransform.position;
                bulletJson["position"] = {position[0]+0.2,position[1]+2,position[2]-5};
                auto rotation = player->localTransform.rotation;
                bulletJson["rotation"] = {270,0,0};

                world->addEntityAndDeserialize(bulletJson);
            }

            // undoing tilting
            if (!isRotating)
            {
                if (playerRotation.x < 0.1 && playerRotation.x > -0.1)
                {
                    playerRotation.x = 0;
                }
                if (playerRotation.x > 0)
                {
                    playerRotation.x -= 0.1f;
                }
                else if (playerRotation.x < 0)
                {
                    playerRotation.x += 0.1f;
                }

                if (playerRotation.z < 0.1 && playerRotation.z > -0.1)
                {
                    playerRotation.z = 0;
                }
                if (playerRotation.z > 0)
                {
                    playerRotation.z -= 0.01f;
                }
                else if (playerRotation.z < 0)
                {
                    playerRotation.z += 0.01f;
                }
            }
        }

        void removeShield(World* world) {
            world->markForRemoval(shieldEnt);
            world->deleteMarkedEntities();
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit(){
            if(mouse_locked) {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }

    };

}
