#pragma once

#include <unordered_set>
#include "entity.hpp"

namespace our {

    // This class holds a set of entities
    class World {
        std::unordered_set<Entity*> entities; // These are the entities held by this world
        std::unordered_set<Entity*> markedForRemoval; // These are the entities that are awaiting to be deleted
                                                      // when deleteMarkedEntities is called
    public:

        World() = default;

        // This will deserialize a json array of entities and add the new entities to the current world
        // If parent pointer is not null, the new entities will be have their parent set to that given pointer
        // If any of the entities has children, this function will be called recursively for these children
        void deserialize(const nlohmann::json& data, Entity* parent = nullptr);

        // This adds an entity to the entities set and returns a pointer to that entity
        // WARNING The entity is owned by this world so don't use "delete" to delete it, instead, call "markForRemoval"
        // to put it in the "markedForRemoval" set. The elements in the "markedForRemoval" set will be removed and
        // deleted when "deleteMarkedEntities" is called.
        Entity* add() {
            //TODO: (Req 8) Create a new entity, set its world member variable to this,
            // and don't forget to insert it in the suitable container.
            Entity* entity = new Entity();
            entity->world = this;
            entities.insert(entity);
            return entity;
        }

        
        Entity* addEntityAndDeserialize(const nlohmann::json& data, Entity* parent = nullptr) {
            if (!data.is_object())
                return nullptr;
            auto newEntity = add();
            newEntity->deserialize(data);
            newEntity->parent = parent;
            if (data.contains("children"))
            {
                deserialize(data["children"], newEntity);
            }
            return newEntity;
        }

        // This returns and immutable reference to the set of all entites in the world.
        const std::unordered_set<Entity*>& getEntities() {
            return entities;
        }

        void markForRemoval(Entity* entity){
            //TODO: (Req 8) If the entity is in this world, add it to the "markedForRemoval" set.
            // STEP 1: Check if the entity is in this world
            if(entities.find(entity)!=entities.end()){
                // STEP 2: Add the entity to the "markedForRemoval" set
                markedForRemoval.insert(entity);
                entities.erase(entity);
            }
        }

        // This removes the elements in "markedForRemoval" from the "entities" set.
        // Then each of these elements are deleted.
        void deleteMarkedEntities(){
            //TODO: (Req 8) Remove and delete all the entities that have been marked for removal
            // STEP 1: Remove the entities in "markedForRemoval" from the "entities" set
            for(auto&it:markedForRemoval){
                // STEP 2: Remove the entity from the "entities" set
                delete it;
            }
            // STEP 3: Clear the "markedForRemoval" set
            markedForRemoval.clear();
        }

        //This deletes all entities in the world
        void clear(){
            //TODO: (Req 8) Delete all the entites and make sure that the containers are empty
            for(auto entity : entities) delete entity; //deleting the actual entities
            // clearing containers
            markedForRemoval.clear();
            entities.clear();
        }

        //Since the world owns all of its entities, they should be deleted alongside it.
        ~World(){
            clear();
        }

        // The world should not be copyable
        World(const World&) = delete;
        World &operator=(World const &) = delete;
    };

}