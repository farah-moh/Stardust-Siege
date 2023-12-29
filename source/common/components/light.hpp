#pragma once

#include "../ecs/component.hpp"
#include "../mesh/mesh.hpp"
// #include "../material/material.hpp"
#include "../asset-loader.hpp"


#include <glm/mat4x4.hpp>

namespace our {

    enum class LightType {
        Directional,
        Point,
        Spot
    };

    class LightComponent : public Component {
    public:
        LightType type = LightType::Directional;

        glm::vec3 diffuse = glm::vec3(0, 0, 0);
        glm::vec3 specular = glm::vec3(0, 0, 0);
        glm::vec3 ambient = glm::vec3(0, 0, 0);

        glm::vec3 attenuation = glm::vec3(0.1, 0, 0); // vec3(quadratic, linear, constant)
        glm::vec2 cone = glm::vec2(40, 20); // vec2(outer_angle, inner_angle)

        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}