#include "light.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"
#include <iostream>

namespace our
{

    void LightComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        diffuse = data.value("diffuse", nlohmann::json());
        specular = data.value("specular", nlohmann::json());
        ambient = data.value("ambient", nlohmann::json());

        std::string lightTypeStr = data.value("lightType", "directional");
        
        if (lightTypeStr == "directional")
        {
            type = our::LightType::Directional;
        }
        else if (lightTypeStr == "point")
        {
            type = our::LightType::Point;
        }
        else if (lightTypeStr == "spot")
        {
            type = our::LightType::Spot;
        }

        attenuation = data.value("attenuation", attenuation);

        cone = data.value("cone", cone) * (glm::pi<float>() / 180);
    }
}