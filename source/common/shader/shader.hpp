#ifndef SHADER_HPP
#define SHADER_HPP
#include <string>
#include <map>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace our
{

    class ShaderProgram
    {

    private:
        std::map<std::string, GLuint> uniform_location_cache;

    public:
        // Shader Program Handle (OpenGL object name)
        GLuint program;
        ShaderProgram()
        {
            // TODO: (Req 1) Create A shader program
            program = glCreateProgram();
        }
        ~ShaderProgram()
        {
            // TODO: (Req 1) Delete a shader program

            // Delete program
            if (program)
                glDeleteProgram(program);

            // Set the program back to 0
            program = 0;
        }

        bool attach(const std::string &filename, GLenum type) const;

        bool link() const;

        void use()
        {
            glUseProgram(program);
        }

        GLuint getUniformLocation(const std::string &name)
        {
            // TODO: (Req 1) Return the location of the uniform with the given name

            // If uniform location wasn't previously cahced, get it and cache it
            if (uniform_location_cache.find(name) != uniform_location_cache.end())
            {
                // If uniform location was previously cahced, return it
                return uniform_location_cache[name];
            }
            /* Call glGetUniformLocation, convert string name to a null terminated c_str to match the function arguments
            both GLchar* & char * are null terminated character arrays
            */
            return uniform_location_cache[name] = glGetUniformLocation(program, name.c_str());
        }

        void set(const std::string &uniform, GLfloat value)
        {
            // TODO: (Req 1) Send the given float value to the given uniform

            // 1f because its type is only 1 float
            glUniform1f(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, GLuint value)
        {
            // TODO: (Req 1) Send the given unsigned integer value to the given uniform

            // 1ui because its type is only 1 unsigned integer
            glUniform1ui(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, GLint value)
        {
            // TODO: (Req 1) Send the given integer value to the given uniform

            // 1ui because its type is only 1 integer
            glUniform1i(getUniformLocation(uniform), value);
        }

        void set(const std::string &uniform, glm::vec2 value)
        {
            // TODO: (Req 1) Send the given 2D vector value to the given uniform

            // 2f because its type is vec2, aka a vector of 2 floats (x,y)
            glUniform2f(getUniformLocation(uniform), value.x, value.y);
        }

        void set(const std::string &uniform, glm::vec3 value)
        {
            // TODO: (Req 1) Send the given 3D vector value to the given uniform

            // 3f because its type is vec3, aka a vector of 3 floats (x,y,z)
            glUniform3f(getUniformLocation(uniform), value.x, value.y, value.z);
        }

        void set(const std::string &uniform, glm::vec4 value)
        {
            // TODO: (Req 1) Send the given 4D vector value to the given uniform

            // 4f because its type is vec4, aka a vector of 4 floats (x,y,z,w)
            glUniform4f(getUniformLocation(uniform), value.x, value.y, value.z, value.w);
        }

        void set(const std::string &uniform, glm::mat4 matrix)
        {
            // TODO: (Req 1) Send the given matrix 4x4 value to the given uniform

            // Matrix4fv because its type is vec4, aka a vector of 4 floats (x,y,z,w)
            // 1: only one matrix, false: dont transpose before u send to shader
            // glm::value_ptr(matrix): obtains a pointer to the internal data of the "matrix" object
            glUniformMatrix4fv(getUniformLocation(uniform), 1, false, glm::value_ptr(matrix));
        }

        // TODO: (Req 1) Delete the copy constructor and assignment operator.
        // Question: Why do we delete the copy constructor and assignment operator?

        /*
        Following Resource Acquisition Is Initialization (RAII), deleting the copy constructor and assignment operator ensures
        that there is a single owner of the resources and that resources are properly released when the objects are destroyed.
        */

        ShaderProgram(ShaderProgram const &) = delete;
        ShaderProgram &operator=(ShaderProgram const &) = delete;
    };

}

#endif