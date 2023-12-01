#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const {
        //TODO: (Req 3) Write this function
        glm::mat4 transform_matrix = glm::mat4(1.0f); // defining 4x4 transformation matrix
        transform_matrix=glm::translate(transform_matrix,position); // forming translating matrix
        transform_matrix= transform_matrix* glm::yawPitchRoll(rotation.y, rotation.x, rotation.z); // apply rotation using euler angles
        transform_matrix= glm::scale(transform_matrix,scale); //apply scaling
        return transform_matrix; 
    }

     // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json& data){
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale    = data.value("scale", scale);
    }

}