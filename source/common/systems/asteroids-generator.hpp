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

        float generateRandomFloat(float min = -100, float max = 100)
        {
            return min + rand() * (max - min) / RAND_MAX;
        }

    public:
        nlohmann::json asteroid;

        AsteroidsGenerator(const nlohmann::json &asteroid) : curr_time(0), delay(1000)
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

            glm::vec3 position;
            Entity *entity = nullptr;

            entity = world->addEntityAndDeserialize(asteroid);
            position = glm::vec3(generateRandomFloat(), generateRandomFloat(), -60);

            if (!entity)
                return;
            entity->localTransform.position = position;

            // remove asteroids not in the screen
            const unordered_set<Entity *> entities = world->getEntities();
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
            }
            world->deleteMarkedEntities();
        }
    };

}