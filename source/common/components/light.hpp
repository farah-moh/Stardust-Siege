#pragma once

#include "../ecs/component.hpp"
#include "../mesh/mesh.hpp"
// #include "../material/material.hpp"
#include "../asset-loader.hpp"


#include <glm/mat4x4.hpp>
//light component represents properties of light like type , color , attenutation (if point light or spot light) and cone (if spot light )
namespace our {

//we have three types of light : directional , point and spot light
    enum class LightType {
        Directional, // we assume light rays are parallel in same direction 
        Point, // we assume light source is single point that emits light in all directions
        Spot // we assume light source is single point that emits light in direction of cone
    };

    class LightComponent : public Component {
    public:
        LightType type = LightType::Directional; // directional light is default

// defining colors of light for phong model , we have diffuse , specular and ambient colors
//initially they are all 0
        glm::vec3 diffuse = glm::vec3(0, 0, 0);
        glm::vec3 specular = glm::vec3(0, 0, 0);
        glm::vec3 ambient = glm::vec3(0, 0, 0);
// attenuation represents diminishing of light with respect to distance
// there the default value is quadratic attenuation but with 10% only 
        glm::vec3 attenuation = glm::vec3(0.1, 0, 0); // vec3(quadratic, linear, constant)
        //for spot light defining outer angle and inner angle
        glm::vec2 cone = glm::vec2(40, 20); // vec2(outer_angle, inner_angle)

        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}