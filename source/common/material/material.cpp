#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: (Req 7) Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: (Req 7) Write this function

        // Calling the parent (Material) setup function
        Material::setup();
        // Setting the tint uniform
        shader->set("tint",tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: (Req 7) Write this function

        // Calling the parent (TintedMaterial) setup function
        TintedMaterial::setup();

        // Setting the alpha threshold
        shader->set("alphaThreshold",alphaThreshold);

        /* Setting the active texture unit to unit 0, even tho it's the default value
         it's better to explicitly set it */
        glActiveTexture(GL_TEXTURE0);

        /* Here we set the active texture unit to 0 then bind the texture to it if it's not null
        otherwise, we unbind it
        */
        if(texture) texture->bind();
        else Texture2D::unbind();

        // Then we bind the sampler to unit 0 if it's not null, otherwise, we unbind it
        if(sampler) sampler->bind(0);
        else Sampler::unbind(0);

        // Then we send 0 (the index of the texture unit we used above) to the "tex" uniform
        shader->set("tex",0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

}