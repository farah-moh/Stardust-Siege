#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/collision.hpp>
#include <systems/asteroids-generator.hpp>
#include <systems/movement.hpp>
#include <asset-loader.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {
    float doomTime = 0.0;
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
        }
        collision.enter(getApp());
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);
        if (asteroidGenerator)
            asteroidGenerator->update(&world, (float)deltaTime);
        collision.update(&world, (float)deltaTime);

        if(collision.bulletCollide) {

        }

        if(collision.spaceshipCollide && !getApp()->getTimer()) {
            renderer.setDoomed(true);   // Set the renderer to Doom mode
            getApp()->setTimer(true);   // Set the timer (to start the countdown, and to make sure no doom mode is set again)
            doomTime = glfwGetTime();          // The start time of the doom mode
            getApp()->setCountdownTime(doomTime);  // Set the countdown time (to be used in application class)   
        }
        // This condition is used to make sure that the doom mode is set for 5 seconds (Pauses excluded)
        if ((glfwGetTime()  > (0.5f + doomTime)) && getApp()->getTimer()) {
            
            getApp()->setTimer(false);
            // getApp()->setCountdown(2);
            renderer.setDoomed(false);
        }
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);
        getApp()->setScore(collision.score);
        if(getApp()->getScore() < 0) {
            getApp()->changeState("lose");
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
    }
};