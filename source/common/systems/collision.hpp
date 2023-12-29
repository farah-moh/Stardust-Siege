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

            for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); bulletIt++)  {
                Entity* bullet = *bulletIt;
                glm::vec3 bulletCenter = bullet->localTransform.position;
                //std::cout<<"-------------"<<bulletCenter[0]<<" "<<bulletCenter[1]<<" "<<bulletCenter[2];
                for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end(); asteroidIt++) 
                {
                    Entity* asteroid = *asteroidIt;
                    auto meshRenderer = asteroid->getComponent<MeshRendererComponent>();
                    if(!meshRenderer) continue;
                    Mesh* mesh = meshRenderer->mesh;
                    std::vector<float> boundingBox = mesh->getBoundingBox();
                    if(detectCollision(asteroid, bulletCenter)) {
                        world->markForRemoval(bullet);
                        world->markForRemoval(asteroid);    
                        score++;
                    }
                    // glm::vec3 asteroidCenter = asteroid->localTransform.position;
                    // float distance = glm::distance(bulletCenter, asteroidCenter);
                    // if (distance < 15) {
                    //     world->markForRemoval(bullet);
                    //     world->markForRemoval(asteroid);    
                    //     score++;
                    // }
                }
            }

            glm::vec3& playerCenter = player->parent->localTransform.position;
            for(auto asteroid : asteroids) {
                glm::vec3 asteroidCenter = asteroid->localTransform.position;
                float distance = glm::distance(playerCenter, asteroidCenter);
                if (distance < 7) {
                    // return to initial position
                    playerCenter = glm::vec3(0, 0, 10);
                    score--;
                    world->markForRemoval(asteroid);
                }
            }

            world->deleteMarkedEntities(); 
            return;
        }

        // bool detectCollision(std::vector<float>boundingBox, glm::vec3 center) {
        //     float x = center[0], y = center[1], z = center[2];
        //     for(auto i : boundingBox) std::cout<<i<<std::endl;
        //     float minX = boundingBox[0], maxX = boundingBox[1];
        //     float minY = boundingBox[2], maxY = boundingBox[3];
        //     float minZ = boundingBox[4], maxZ = boundingBox[5];
        //     return  (x >= minX && x <= maxX) &&
        //             (y >= minY && y <= maxY) &&
        //             (z >= minZ && z <= maxZ);
        // }
        bool detectCollision(Entity* entity, glm::vec3 center) {
            float x = center[0], y = center[1], z = center[2];
            float scaleX = entity->localTransform.scale[0]*5;
            float scaleY = entity->localTransform.scale[1]*5;
            float scaleZ = entity->localTransform.scale[2]*5;

            float Ex = entity->localTransform.position[0];
            float Ey = entity->localTransform.position[1];
            float Ez = entity->localTransform.position[2];

            float minX = Ex - scaleX, maxX = Ex + scaleX;
            float minY = Ey - scaleY, maxY = Ey + scaleY;
            float minZ = Ez - scaleZ, maxZ = Ez + scaleZ;
            return  (x >= minX && x <= maxX) &&
                    (y >= minY && y <= maxY) &&
                    (z >= minZ && z <= maxZ);
        }

    };

}
