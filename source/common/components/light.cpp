#include "light.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"
#include <iostream>

namespace our
{
//getting light data from json file
    void LightComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        diffuse = data.value("diffuse", nlohmann::json()); //getting diffuse color from json
        specular = data.value("specular", nlohmann::json()); //getting specular color from json
        ambient = data.value("ambient", nlohmann::json()); //getting ambient color from json

        std::string lightTypeStr = data.value("lightType", "directional"); //getting type of light from json
        
        if (lightTypeStr == "directional") //setting type of light according to type in json
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

        attenuation = data.value("attenuation", attenuation); // getting attenuation which is vec3 from json

        cone = data.value("cone", cone) * (glm::pi<float>() / 180);//getting cone angle from json and convert to radian
    }
}