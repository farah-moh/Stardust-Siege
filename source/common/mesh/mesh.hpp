#pragma once

#include <glad/gl.h>
#include "vertex.hpp"

namespace our {

    #define ATTRIB_LOC_POSITION 0
    #define ATTRIB_LOC_COLOR    1
    #define ATTRIB_LOC_TEXCOORD 2
    #define ATTRIB_LOC_NORMAL   3

    class Mesh {
        // Here, we store the object names of the 3 main components of a mesh:
        // A vertex array object, A vertex buffer and an element buffer
        unsigned int VBO, EBO;
        unsigned int VAO;
        // We need to remember the number of elements that will be draw by glDrawElements 
        GLsizei elementCount;
    public:

        // The constructor takes two vectors:
        // - vertices which contain the vertex data.
        // - elements which contain the indices of the vertices out of which each rectangle will be constructed.
        // The mesh class does not keep a these data on the RAM. Instead, it should create
        // a vertex buffer to store the vertex data on the VRAM,
        // an element buffer to store the element data on the VRAM,
        // a vertex array object to define how to read the vertex & element buffer during rendering 
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& elements)
        {
            //TODO: (Req 2) Write this function
            // remember to store the number of elements in "elementCount" since you will need it for drawing
            // For the attribute locations, use the constants defined above: ATTRIB_LOC_POSITION, ATTRIB_LOC_COLOR, etc

            // update element count
            elementCount = (int)elements.size();

            // create vertex array object
            // 1: no. of vertex array objects
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // create vertex buffer object
            // 1: no. of vertex buffer objects
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            // set vertex buffer data
            // GL_ARRAY_BUFFER: type of buffer
            // 2: size of data in bytes
            // vertices.data(): pointer to vertices data
            // GL_STATIC_DRAW: usage of data
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);


            // enable vertex attribute arrays
            // ATTRIB_LOC_POSITION: attribute location
            glEnableVertexAttribArray(ATTRIB_LOC_POSITION);
            // set vertex attribute pointers to make him understand how to read pixels from the buffer.
            // ATTRIB_LOC_POSITION: attribute location
            // 3: no. of components per attribute
            // GL_FLOAT: type of data
            // GL_FALSE: whether data should be normalized
            // sizeof(Vertex): stride size
            // (void*)offsetof(Vertex, position): pointer to offset --> could be 0 here because position is the first element in vertix.
            glVertexAttribPointer(ATTRIB_LOC_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

            // here the type is not FL_FLOAT, it is GL_UNSIGNED_BYTE because color is an unsigned int i.e. 1 byte 0->255
            // Normalize value is GL_TRUE because to normalize values of 0 -> 255 to 0 -> 1 so we can represent all colors with 1 byte u_int
            glEnableVertexAttribArray(ATTRIB_LOC_COLOR);
            glVertexAttribPointer(ATTRIB_LOC_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, color));

            glEnableVertexAttribArray(ATTRIB_LOC_TEXCOORD);
            glVertexAttribPointer(ATTRIB_LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

            glEnableVertexAttribArray(ATTRIB_LOC_NORMAL);
            glVertexAttribPointer(ATTRIB_LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));


            // create element buffer object
            glGenBuffers(1, &EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int), elements.data(), GL_STATIC_DRAW);

            // unbind vertex array object, vertex buffer object, and element buffer object
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        // this function should render the mesh
        void draw() 
        {
            //TODO: (Req 2) Write this function
            // bind vertex array object
            glBindVertexArray(VAO);
            // draw elements
            // GL_TRIANGLES: type of primitive
            // elementCount: no. of elements
            // GL_UNSIGNED_INT: type of indices
            // 0: pointer to indices
            glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, 0);

            // unbind vertex array object
            glBindVertexArray(0);
        }


        // this function should delete the vertex & element buffers and the vertex array object
        ~Mesh(){
            //TODO: (Req 2) Write this function
            // delete vertex array object
            glDeleteVertexArrays(1, &VAO);
            // delete vertex buffer object
            glDeleteBuffers(1, &VBO);
            // delete element buffer object
            glDeleteBuffers(1, &EBO);
        }

        Mesh(Mesh const &) = delete;
        Mesh &operator=(Mesh const &) = delete;
    };

}