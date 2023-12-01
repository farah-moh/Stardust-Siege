#pragma once

#include <glad/gl.h>

namespace our {

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture2D {
        // The OpenGL object name of this texture 
        GLuint name = 0;
    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name" 
        Texture2D() {
            //TODO: (Req 5) Complete this function
            glGenTextures(1,&name); //first parameter indicates how many textures we want to generate , second parameter takes pointer of array that stores unique identifiers for the textures
                                    // in this case , we generate only one texture so we store it in an array of one element holding one unique identifier for the texture so we can use name to use this texture
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture2D() { 
            //TODO: (Req 5) Complete this function
            glDeleteTextures(1,&name); //first parameter specifies number of textures to delete , second argument specifies pointer to array of texture names to delete
        }

        // Get the internal OpenGL name of the texture which is useful for use with framebuffers
        GLuint getOpenGLName() {
            return name;
        }

        // This method binds this texture to GL_TEXTURE_2D
        void bind() const {
            //TODO: (Req 5) Complete this function
            glBindTexture(GL_TEXTURE_2D,name); // binding texture with identifier name to the 2D texture target
        }

        // This static method ensures that no texture is bound to GL_TEXTURE_2D
        static void unbind(){
            //TODO: (Req 5) Complete this function
            glBindTexture(GL_TEXTURE_2D,0); //0 is the default which means don`t bind any texture to this 2D texture target
        }

        Texture2D(const Texture2D&) = delete;
        Texture2D& operator=(const Texture2D&) = delete;
    };
    
}