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

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/free-camera-controller.hpp"
    class Collision {
        Application* app; // The application in which the state runs
        bool mouse_locked = false; // Is the mouse locked

    public:
        int score = 0;
        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application* app){
            this->app = app;
            score = 0;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent 
        void update(World* world, float deltaTime) {
            
            std::vector<Entity*> bullets;
            std::vector<Entity*> asteroids;
            Entity* player = nullptr;
 
            for(auto entity : world->getEntities()) {
                if(entity->name == "bullet") bullets.push_back(entity);
                else if(entity->name == "asteroid") asteroids.push_back(entity);
                else if(entity->name == "player") player = entity;
            }

            if(!player) return;

            for(auto bullet : bullets) {
                glm::vec3 bulletCenter = bullet->localTransform.position;
                //std::cout<<"-------------"<<bulletCenter[0]<<" "<<bulletCenter[1]<<" "<<bulletCenter[2];
                for (auto it = asteroids.begin(); it != asteroids.end(); it++) 
                {
                    Entity* asteroid = *it;
                    glm::vec3 asteroidCenter = asteroid->localTransform.position;
                    float distance = glm::distance(bulletCenter, asteroidCenter);
                    if (distance < 7) {
                        world->markForRemoval(asteroid);    
                        score++;
                    }
                }
            }

            world->deleteMarkedEntities(); 
            return;
        }

    };

}
