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
        bool spaceshipCollide = false;
        bool bulletCollide = false;
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

            bool collided = false;

            for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); bulletIt++)  {
                Entity* bullet = *bulletIt;
                glm::vec3 bulletCenter = bullet->localTransform.position;

                for (auto asteroidIt = asteroids.begin(); asteroidIt != asteroids.end(); asteroidIt++) 
                {
                    Entity* asteroid = *asteroidIt;
                    if(detectCollision(asteroid, bulletCenter)) {
                        for(auto i : bullets) world->markForRemoval(i);
                        world->markForRemoval(asteroid);    
                        score++;
                        collided = true;
                        bulletCollide = true;
                        break;
                    }
                }
                if(collided) break;
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
                    spaceshipCollide = true;
                }
            }

            world->deleteMarkedEntities(); 
            return;
        }

        bool detectCollision(Entity* entity, glm::vec3 center) {
            float x = center[0], y = center[1], z = center[2];
            float scaleX = entity->localTransform.scale[0]*3;
            float scaleY = entity->localTransform.scale[1]*3;
            float scaleZ = entity->localTransform.scale[2]*3;

            float Ex = entity->localTransform.position[0];
            float Ey = entity->localTransform.position[1];
            float Ez = entity->localTransform.position[2];

            // float distance = glm::distance(center, entity->localTransform.position);

            float minX = Ex - scaleX, maxX = Ex + scaleX;
            float minY = Ey - scaleY, maxY = Ey + scaleY;
            float minZ = Ez - scaleZ, maxZ = Ez + scaleZ;
            return  ((x >= minX && x <= maxX) &&
                    (y >= minY && y <= maxY) &&
                    (z >= minZ && z <= maxZ));
        }

    };

}
