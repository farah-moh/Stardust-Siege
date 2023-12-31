#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/collision.hpp>
#include <systems/asteroids-generator.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>
#include <irrKlang.h>
using namespace irrklang;

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {
    float doomTime = 0.0;
    float shakeTime = 0.0;

    ISoundEngine *DJAmro7a7a = nullptr;

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::AsteroidsGenerator* asteroidGenerator;
    our::Collision collision;
    our::MovementSystem movementSystem;

    std::string getName() override {
        return "play";
    }

    void onInitialize() override {

        // Initialize the sound engine
        DJAmro7a7a = createIrrKlangDevice();
        DJAmro7a7a->setSoundVolume(0.3f); 
        DJAmro7a7a->play2D("assets/sounds/background.mp3", false, false, true);
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }
        if (config.contains("runtimeEntity"))
        {
            // if (!config["runtimeEntity"].is_object())
            //     return;
            asteroidGenerator = new our::AsteroidsGenerator(config["runtimeEntity"][1]);
            cameraController.enter(getApp(), config["runtimeEntity"][2]);
        }
        collision.enter(getApp());
        // We initialize the camera controller system since it needs a pointer to the app
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);

        if (asteroidGenerator)
            asteroidGenerator->update(&world, (float)deltaTime);
        collision.update(&world, cameraController.shielded);

        cameraController.update(&world, (float)deltaTime, collision.score);

        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

        // ################# Postprocessing Effects #################
        // shake effect on collision spaceship with asteroid
        if(collision.bulletCollide && !getApp()->getTimer()) {
            DJAmro7a7a->play2D("assets/sounds/explosion.mp3", false, false, true);
            renderer.setShaken(true); 
            getApp()->setTimer(true);   
            shakeTime = glfwGetTime();          
            getApp()->setCountdownTime(shakeTime);    
        }

        // Doom effect on collision spaceship with asteroid
        if(collision.spaceshipCollide) {
            DJAmro7a7a->play2D("assets/sounds/gethit.mp3", false, false, true);
            if(!getApp()->getTimer()) {
                renderer.setDoomed(true);
                getApp()->setTimer(true);   
                doomTime = glfwGetTime();
                getApp()->setCountdownTime(doomTime);
            }
            if(cameraController.shielded) {
                cameraController.shielded = false;
                cameraController.removeShield(&world);
            }
        }

        // Check if the timer of shaken effect lasted 0.5 seconds
        if ((glfwGetTime()  > (0.5f + shakeTime)) && renderer.getShaked()) {
            getApp()->setTimer(false);
            renderer.setShaken(false);
        }
        // Check if the timer of doom effect lasted 0.5 seconds
        if ((glfwGetTime()  > (0.5f + doomTime)) && getApp()->getTimer()) {
            getApp()->setTimer(false);
            renderer.setDoomed(false);
        }

                
        getApp()->setScore(collision.score);
        if(getApp()->getScore() < 0) {
            DJAmro7a7a->play2D("assets/sounds/lose.wav", false, false, true);
            getApp()->changeState("lose");
        }

        if(getApp()->getScore() >= 10) {
            DJAmro7a7a->play2D("assets/sounds/win.wav", false, false, true);
            getApp()->changeState("mid");
        }

        // Get a reference to the keyboard object
        auto& keyboard = getApp()->getKeyboard();

        if(keyboard.justPressed(GLFW_KEY_ESCAPE)){
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }

    }

    void onDestroy() override {
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
        if (DJAmro7a7a)
        {
            DJAmro7a7a->stopAllSounds();
            DJAmro7a7a->drop();
        }
    }
};