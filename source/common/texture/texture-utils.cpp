#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

our::Texture2D* our::texture_utils::empty(GLenum format, glm::ivec2 size){
    our::Texture2D* texture = new our::Texture2D();
    //TODO: (Req 11) Finish this function to create an empty texture with the given size and format
    texture->bind(); // bind texture to GL_TEXTURE_2D texture target
    //creating an empty texture with specific format and size
    // second argument indicates mipmap level
    //third argument represents the internal format of the texture
    //fourth and fifth argument represents size of the texture
    //sixth argument represents border of the texture (it is always 0 )
    //seventh argument represents pixel format of the pixel data
    //eight argument represents data type of pixel data
    //ninth argument represents a pointer to image data (in this case it is null or 0 which means the texture is empty)
    glTexImage2D(GL_TEXTURE_2D,0,format,size.x,size.y,0,format,GL_UNSIGNED_BYTE,0); // creating an empty texture 
    texture->unbind(); //unbind the texture to prevent accidentally modifying the wrong texture (this is optional)
    return texture;
}

our::Texture2D* our::texture_utils::loadImage(const std::string& filename, bool generate_mipmap) {
    glm::ivec2 size;
    int channels;
    //Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    //We need to till stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    //Load image data and retrieve width, height and number of channels in the image
    //The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    //Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char* pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if(pixels == nullptr){
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D* texture = new our::Texture2D();
    //Bind the texture such that we upload the image data to its storage
    //TODO: (Req 5) Finish this function to fill the texture with the data found in "pixels"
    texture->bind(); //binding texture
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,size.x,size.y,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels); //generating texture using image 
    // the texture generated has now only base-level mipmap (level 0)
    if(generate_mipmap) // if generate mipmap is passed to the function as true then it will generate all different mipmap levels for the texture
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(pixels); //Free image data after uploading to GPU
    texture->unbind(); // unbind texture (optional)
    return texture;
}