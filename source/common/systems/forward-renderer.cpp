#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"

namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be drawn after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};

            // Only pixels that are equal to or closer to the sky than the current fragment will be rendered
            skyPipelineState.depthTesting.enabled = true;
            skyPipelineState.depthTesting.function = GL_LEQUAL;

            // We should cull the front faces as we're drawing the sphere from the inside, so the back faces are the ones we'll be seeing
            skyPipelineState.faceCulling.enabled = true;
            skyPipelineState.faceCulling.culledFace = GL_FRONT;
            // ?? no idea
            // skyPipelineState.faceCulling.frontFace = GL_CCW;

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer
            // Set Doom, Normal, and Charge paths
            doomPath = config.value<std::string>("doomed", "");
            shakePath = config.value<std::string>("shaken", "");
            normalPath = config.value<std::string>("postprocess", "");
            // Generating 1(no of buffers) frameBuffer
            // This will be useful for post-processing effects, such as applying filters or bloom
            glGenFramebuffers(1, &postprocessFrameBuffer);
            // This code to check the status of the FBO and ensures it is complete before proceeding to use it for rendering
            auto bufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            
            glBindFramebuffer(GL_FRAMEBUFFER, postprocessFrameBuffer);

            // TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).
            // Creating empty texture with RGBA as an interal format, 
            // which indicates a texture that stores red, green, blue, and alpha color components
            colorTarget = texture_utils::empty(GL_RGBA, windowSize);
            // Creating empty texture with DEPTH_COMPONENT as an interal format,
            // which indicates a texture that stores depth information only
            depthTarget = texture_utils::empty(GL_DEPTH_COMPONENT, windowSize);

            /*
                GL_FRAMEBUFFER: target enum, buffer for which framebuffer is bound
                GL_COLOR_ATTACHMENT0: the texture is being attached to the first color attachment point of the framebuffer.
                GL_TEXTURE_2D: specifies what type of texture is expected in the texture parameter
                colorTarget->getOpenGLName(): name of the texture object to attach
                0: mipmap level
            */
            // This means that any rendering operations performed while the FBO is bound will write their color information to the colorTarget texture
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTarget->getOpenGLName(), 0);
            // This means that any depth testing performed while the FBO is bound will use the depth information stored in the depthTarget texture
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTarget->getOpenGLName(), 0);

            // TODO: (Req 11) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_FRAMEBUFFER,0);

            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
            doomed = false;
            shaken = false;
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        lights.clear();
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();

            if (auto light = entity->getComponent<LightComponent>(); light)
            {
                LightObject* lightObject = new LightObject(light); // calling constructor to set the values except position and direction
                lightObject->position = glm::vec3(entity->getLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); 
                lightObject->direction = glm::vec3(entity->getLocalToWorldMatrix() * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));

                lights.push_back(*lightObject);

            }
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        //  calculates the forward direction vector of the camera,
        //  which is used to determine the direction in which the camera is looking.
        glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
        std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
                      // TODO: (Req 9) Finish this function
                      //  HINT: the following return should return true "first" should be drawn before "second".

                      // Dot product the center with cameraForward gives us the length respevtively to the camera forward.
                      // As if we are applying dot product between the camera forward and a vector from the origin to the point.
                      return (dot(cameraForward, first.center) > dot(cameraForward, second.center)); });

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP
        // Transforms from the world space to camera space (to be respective to the camera)
        // Then, transforming to the NDC space
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();

        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize
        // sets the viewport, which is the rectangular region of the window where the rendered image will be displayed.
        // x: The x-coordinate of the lower-left corner of the viewport in pixels.
        // y: The y-coordinate of the lower-left corner of the viewport in pixels.
        // width: The width of the viewport in pixels.
        // height: The height of the viewport in pixels.
        glViewport(0, 0, windowSize.x, windowSize.y);

        // TODO: (Req 9) Set the clear color to black and the clear depth to 1
        // Sets the color that will be used to fill the color buffer when the framebuffer is cleared
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // Sets the depth that will be used to fill the depth buffer when the framebuffer is cleared
        glClearDepth(1.0f);

        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)

        glColorMask(true, true, true, true);
        glDepthMask(true);

        // If there is a postprocess material, bind the framebuffer
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER,postprocessFrameBuffer);
        }

        // TODO: (Req 9) Clear the color and depth buffers
        // Functions to control writing into the color buffer(r,g,b,a) or the depth buffer (depth)
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command

        for (auto command : opaqueCommands) {
            command.material->setup();

            command.material->shader->set("M", command.localToWorld);
            command.material->shader->set("M_I_T", glm::transpose(glm::inverse(command.localToWorld)));
            glm::vec4 eye = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);    // (0,0,0) position of camera in its local space, 1 for point
            command.material->shader->set("camera_position", glm::vec3(eye));
            command.material->shader->set("VP", VP );
            LightObject::setup(command, lights);
            // This matrix is used by the shader to transform the object's vertices from world space to clip space
            command.material->shader->set("transform", VP * command.localToWorld);
            command.mesh->draw();
        }

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 10) setup the sky material
            this->skyMaterial->setup();

            // TODO: (Req 10) Get the camera position
            // getting the camera position in the world space
            // it is typically rendered using a special shader that takes into account the camera position 
            // to create a realistic illusion of a seamless sky
            glm::vec3 cameraPosition = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            
            // TODO: (Req 10) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            
            // just translate the sky by camera position
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), cameraPosition);

            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1)
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?

            // We can achieve this by scaling the z by 0, then translating by 1, which basically means that we force z to be 1
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f);
            // TODO: (Req 10) set the "transform" uniform

            // We need to multiply by modelMatrix first, then by VP, then at last by the matrix that corrects depth
            skyMaterial->shader->set("transform", alwaysBehindTransform * VP * modelMatrix);

            // TODO: (Req 10) draw the sky sphere
            skySphere->draw();
        }
        // TODO: (Req 9) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command

        for (auto command : transparentCommands)
        {
            command.material->setup();
            // Send local to world matrix, its inverse transform of the rendered object to the light.vert
            // camera position, VP matrix
            // To calculate view vector, normal to the vertex, and its position
            command.material->shader->set("M", command.localToWorld);
            command.material->shader->set("M_I_T", glm::transpose(glm::inverse(command.localToWorld)));
            glm::vec4 eye = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);    // (0,0,0) position of camera in its local space, 1 for point
            command.material->shader->set("camera_position", glm::vec3(eye));
            command.material->shader->set("VP", VP );

            LightObject::setup(command, lights);
            // This matrix is used by the shader to transform the object's vertices from world space to clip space
            command.material->shader->set("transform", VP * command.localToWorld);
            command.mesh->draw();
        }

        // If there is a postprocess material, apply postprocessing
        if (postprocessMaterial) {
            ShaderProgram* postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            if(doomed) // Doom effect
                postprocessShader->attach(doomPath, GL_FRAGMENT_SHADER);
            else if(shaken)
                postprocessShader->attach(shakePath, GL_FRAGMENT_SHADER);
            else
                postprocessShader->attach(normalPath, GL_FRAGMENT_SHADER);

            postprocessShader->link();
            postprocessShader->setTime("time");
            postprocessMaterial->shader = postprocessShader;

            // TODO: (Req 11) Return to the default framebuffer
            // This is necessary because the rendering has been targeting the offscreen textures during the main rendering pass. 
            // Switching back to the default framebuffer ensures that the final output is displayed on the window
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle
            postprocessMaterial->setup();
            glBindVertexArray(postProcessVertexArray);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
            
        }
    }
}