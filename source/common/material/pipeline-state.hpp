#pragma once
#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our {
    // There are some options in the render pipeline that we cannot control via shaders
    // such as blending, depth testing and so on
    // Since each material could require different options (e.g. transparent materials usually use blending),
    // we will encapsulate all these options into a single structure that will also be responsible for configuring OpenGL's pipeline
    struct PipelineState {
        // This set of pipeline options specifies whether face culling will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum culledFace = GL_BACK;
            GLenum frontFace = GL_CCW;
        } faceCulling;

        // This set of pipeline options specifies whether depth testing will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum function = GL_LEQUAL;
        } depthTesting;

        // This set of pipeline options specifies whether blending will be used or not and how it will be configured
        struct {
            bool enabled = false;
            GLenum equation = GL_FUNC_ADD;
            GLenum sourceFactor = GL_SRC_ALPHA;
            GLenum destinationFactor = GL_ONE_MINUS_SRC_ALPHA;
            glm::vec4 constantColor = {0, 0, 0, 0};
        } blending;

        // These options specify the color and depth mask which can be used to
        // prevent the rendering/clearing from modifying certain channels of certain targets in the framebuffer
        glm::bvec4 colorMask = {true, true, true, true}; // To know how to use it, check glColorMask
        bool depthMask = true; // To know how to use it, check glDepthMask


        // This function should set the OpenGL options to the values specified by this structure
        // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
        void setup() const {
            //TODO: (Req 4) Write this function
            if(faceCulling.enabled)  //check if face culling is enabled
            {
                glEnable(GL_CULL_FACE); //enable face culling
                glCullFace(faceCulling.culledFace); //determine face to be culled (front or back)
                glFrontFace(faceCulling.frontFace); // determine direction of front face (clockwise or counter clockwise)

            }
            else
            {
                glDisable(GL_CULL_FACE); // disable face culling
            }

            if(depthTesting.enabled) //check if depth testing is enabled
            {
                glEnable(GL_DEPTH_TEST); //enable depth testing
                glDepthFunc(depthTesting.function); // determines depth function that specifies what to draw based on depth by specifing comparison operators for depth test (like less than , less than or equal ,etc ..) 
                                                    //this allows to control when OpenGL passes or discard fragments and when to update depth buffer
            }
            else{
                glDisable(GL_DEPTH_TEST); // disable depth testing
            }

            if(blending.enabled) //check if blending is enabled
            {
                glEnable(GL_BLEND); //enable blending
                glBlendColor(blending.constantColor[0],blending.constantColor[1],blending.constantColor[2],blending.constantColor[3]); // determines blend color of source and destination 
                glBlendEquation(blending.equation); //determine equation of blending (by changing operator between source and destination before combining them )
                glBlendFunc(blending.sourceFactor,blending.destinationFactor); //setting options for source and destination factors
                
            }
            else
            {
                glDisable(GL_BLEND); 

            }
            glColorMask(colorMask.r,colorMask.g,colorMask.b,colorMask.a); //enable and disable writing of frame buffer color components
            glDepthMask(depthMask); //enable or disable writing into depth buffer
        }

        // Given a json object, this function deserializes a PipelineState structure
        void deserialize(const nlohmann::json& data);
    };

}