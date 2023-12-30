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
        ll curr_time_boss;
        ll delay;
        ll bossDelay = 50;
        float lowSpeed, highSpeed;
        int bossPositionDelta = 1;

        glm::vec3 position0 = glm::vec3(-1,-1,-3);
        glm::vec3 position1 = glm::vec3(1,-1,-3);
        glm::vec3 position2 = glm::vec3(0,-1,-1);

        std::vector<glm::vec3> positions = {position0, position1, position2};

        float generateRandomFloat(float min = -30, float max = 30)
        {
            return min + rand() * (max - min) / RAND_MAX;
        }

        float generateRandomSpeed()
        {
            float min = lowSpeed;
            float max = highSpeed;
            return min + rand() * (max - min) / RAND_MAX;
        }

    public:
        nlohmann::json asteroid;

        AsteroidsGenerator(const nlohmann::json &asteroid, float lowSpeed = 10, float highSpeed = 30, ll delay = 500) : curr_time(0)
        {
            srand(time(0));
            this->delay = delay;
            this->asteroid = asteroid;
            this->lowSpeed = lowSpeed;
            this->highSpeed = highSpeed;
        }

        void update(World *world, float deltaTime)
        {
            ll now = glfwGetTime()*1000;

            checkBossPos(world);

            if (now - curr_time < delay)
                return;

            const unordered_set<Entity *> entities = world->getEntities();
            
            curr_time = glfwGetTime()*1000;

            vector<Entity*> RGBlights;


            glm::vec3 position;
            glm::vec3 speed;
            Entity *entity = nullptr;

            entity = world->addEntityAndDeserialize(asteroid);
            position = glm::vec3(generateRandomFloat(), generateRandomFloat(), -100);
            speed = glm::vec3(0, 0, generateRandomSpeed());

            if (!entity)
                return;
            entity->localTransform.position = position;
            entity->getComponent<MovementComponent>()->linearVelocity = speed;

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
                    if (position.z < -60)
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

        void checkBossPos(World* world) {
            ll now = glfwGetTime()*1000;

            if (now - curr_time_boss < bossDelay)
                return;
            
            curr_time_boss = glfwGetTime()*1000;

            Entity* boss = nullptr;
            const unordered_set<Entity *> entities = world->getEntities();
            for (auto entity : entities)
            {
                if(entity->name == "boss")  {
                    boss = entity;
                    break;
                }
            }

            if(boss) {
                boss->localTransform.position[0] += bossPositionDelta*4;
                if(boss->localTransform.position[0] > 40) bossPositionDelta = -1;
                else if(boss->localTransform.position[0] < -40) bossPositionDelta = 1;
            }
        }
    };

}