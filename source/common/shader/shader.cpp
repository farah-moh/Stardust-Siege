#include "shader.hpp"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

//Forward definition for error checking functions
std::string checkForShaderCompilationErrors(GLuint shader);
std::string checkForLinkingErrors(GLuint program);

bool our::ShaderProgram::attach(const std::string &filename, GLenum type) const {
    // Here, we open the file and read a string from it containing the GLSL code of our shader
    std::ifstream file(filename);
    if(!file){
        std::cerr << "ERROR: Couldn't open shader file: " << filename << std::endl;
        return false;
    }
    std::string sourceString = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    const char* sourceCStr = sourceString.c_str();
    file.close();

    //TODO: Complete this function
    //Note: The function "checkForShaderCompilationErrors" checks if there is
    // an error in the given shader. You should use it to check if there is a
    // compilation error and print it so that you can know what is wrong with
    // the shader. The returned string will be empty if there is no errors.

    // Creating the shader using the type
    GLuint shader = glCreateShader(type);

    /* Adding the shader source code that we read from the file
    shader: shader ID
    1: number of strings in array sourceCStr, here it's only one string
    &sourceCStr: memory location of the source code string, to match the argument of "const GLchar **string"
    nullptr: since the source code string is null terminated, we can just input length as NULL
    */
    glShaderSource(shader, 1, &sourceCStr, nullptr); //Send shader source code


    // Compiling the source code
    glCompileShader(shader);

    std::string error = checkForShaderCompilationErrors(shader);
    if(error.size())
    {
        std::cout << "Error in shader compilation: " << error << std::endl;
        return false;
    }

    // Attaching the shader to the program
    glAttachShader(program, shader); //Attach shader to program

    /*
    Deleting shader since it's already attached to the program so its object is no longer needed,
    it will mark the object for deletion, but it will not be deleted until it is no longer attached to any program object
    */
    glDeleteShader(shader);

    // Return true if the compilation succeeds
    return true;
}



bool our::ShaderProgram::link() const {
    //TODO: Complete this function
    //Note: The function "checkForLinkingErrors" checks if there is
    // an error in the given program. You should use it to check if there is a
    // linking error and print it so that you can know what is wrong with the
    // program. The returned string will be empty if there is no errors.

    // Linking the shader to the program, it verifies it & makes sure it can be executed
    glLinkProgram(program);

    std::string error = checkForLinkingErrors(program);
    if(error.size())
    {
        std::cout << "Error in shader linking: " << error << std::endl;
        return false;
    }

    // Return true if the linking succeeds
    return true;
}

////////////////////////////////////////////////////////////////////
// Function to check for compilation and linking error in shaders //
////////////////////////////////////////////////////////////////////

std::string checkForShaderCompilationErrors(GLuint shader){
     //Check and return any error in the compilation process
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetShaderInfoLog(shader, length, nullptr, logStr);
        std::string errorLog(logStr);
        delete[] logStr;
        return errorLog;
    }
    return std::string();
}

std::string checkForLinkingErrors(GLuint program){
     //Check and return any error in the linking process
    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *logStr = new char[length];
        glGetProgramInfoLog(program, length, nullptr, logStr);
        std::string error(logStr);
        delete[] logStr;
        return error;
    }
    return std::string();
}