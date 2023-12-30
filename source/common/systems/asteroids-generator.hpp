#pragma once

#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "../ecs/entity.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>

using namespace std;
#include <ctime>
#include "GLFW/glfw3.h"

typedef unsigned long long int ll;

#define MAX_RANGE 10f

namespace our
{

    class AsteroidsGenerator
    {
    private:
        ll curr_time;
        ll delay;

        glm::vec3 position0 = glm::vec3(-1,-1,-3);
        glm::vec3 position1 = glm::vec3(1,-1,-3);
        glm::vec3 position2 = glm::vec3(0,-1,-1);

        std::vector<glm::vec3> positions = {position0, position1, position2};

        float generateRandomFloat(float min = -40, float max = 40)
        {
            return min + rand() * (max - min) / RAND_MAX;
        }

        float generateRandomSpeed(float min = 10, float max = 30)
        {
            return min + rand() * (max - min) / RAND_MAX;
        }

    public:
        nlohmann::json asteroid;

        AsteroidsGenerator(const nlohmann::json &asteroid) : curr_time(0), delay(500)
        {
            srand(time(0));
            this->asteroid = asteroid;
        }

        void update(World *world, float deltaTime)
        {
            ll now = glfwGetTime()*1000;
            if (now - curr_time < delay)
                return;

            curr_time = glfwGetTime()*1000;

            vector<Entity*> RGBlights;


            glm::vec3 position;
            glm::vec3 speed;
            Entity *entity = nullptr;

            entity = world->addEntityAndDeserialize(asteroid);
            position = glm::vec3(generateRandomFloat(), generateRandomFloat(), -60);
            speed = glm::vec3(0, 0, generateRandomSpeed());

            if (!entity)
                return;
            entity->localTransform.position = position;
            entity->getComponent<MovementComponent>()->linearVelocity = speed;

            const unordered_set<Entity *> entities = world->getEntities();
            // remove asteroids not in the screen
            for (auto entity : entities)
            {
                if (!entity)
                    continue;
                if (entity->name == "asteroid")
                {
                    // get the position of the entity
                    glm::vec3 &position = entity->localTransform.position;
                    // if the position of the entity is greater than 60
                    if (position.z > 20)
                    {
                        // delete the entity
                        world->markForRemoval(entity);
                    }
                }
                else if (entity->name == "bullet")
                {
                    // get the position of the entity
                    glm::vec3 &position = entity->localTransform.position;
                    // if the position of the entity is greater than 60
                    if (position.z < -20)
                    {
                        // delete the entity
                        world->markForRemoval(entity);
                    }
                }
                else if(entity->name == "redLight") RGBlights.push_back(entity);
                else if(entity->name == "blueLight") RGBlights.push_back(entity);
                else if(entity->name == "greenLight") RGBlights.push_back(entity);
            }

            RGBlights[0]->localTransform.position = positions[curr_time%3];
            RGBlights[1]->localTransform.position = positions[(curr_time%3+1)%3];
            RGBlights[2]->localTransform.position = positions[(curr_time%3+2)%3];

            world->deleteMarkedEntities();
        }
    };

}