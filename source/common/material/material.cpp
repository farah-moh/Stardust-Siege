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

    void LitMaterial::setup() const
    {
        // Calling the parent (Material) setup function
        Material::setup();

        glActiveTexture(GL_TEXTURE0);
        if (albedo)
            albedo->bind();
        else
            Texture2D::unbind();
        if (sampler)
            sampler->bind(0);
        else
            Sampler::unbind(0);
        //Setting uniform tex_material.albedo in shader
        shader->set("tex_material.albedo", 0);

        glActiveTexture(GL_TEXTURE1);
        if (specular)
            specular->bind();
        else
            Texture2D::unbind();
        if (sampler)
            sampler->bind(1);
        else
            Sampler::unbind(1);
        shader->set("tex_material.specular", 1);

        glActiveTexture(GL_TEXTURE2);
        if (roughness)
            roughness->bind();
        else
            Texture2D::unbind();
        if (sampler)
            sampler->bind(2);
        else
            Sampler::unbind(2);
        shader->set("tex_material.roughness", 2);

        glActiveTexture(GL_TEXTURE3);
        if (emissive)
            emissive->bind();
        else
            Texture2D::unbind();
        if (sampler)
            sampler->bind(3);
        else
            Sampler::unbind(3);
        shader->set("tex_material.emissive", 3);

        glActiveTexture(GL_TEXTURE4);
        if (ambient_occlusion)
            ambient_occlusion->bind();
        else
            Texture2D::unbind();
        if (sampler)
            sampler->bind(4);
        else
            Sampler::unbind(4);
        shader->set("tex_material.ambient_occlusion", 4);
    }

    void LitMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;

        albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));
        specular = AssetLoader<Texture2D>::get(data.value("specular", ""));
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));
        emissive = AssetLoader<Texture2D>::get(data.value("emissive", ""));
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));

    }

}