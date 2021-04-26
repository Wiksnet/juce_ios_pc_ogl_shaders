//
// Created by Vitaly Tarasuk on 25.04.2021.
//

#include "simple_gl_component.h"

SimpleGLComponent::SimpleGLComponent() {
    // Indicates that no part of this Component is transparent.
    setOpaque(true);

    // Tell the context to repaint on a loop.
    // openGLContext.setContinuousRepainting(true);
    openGLContext.setContinuousRepainting(false);
    openGLContext.setComponentPaintingEnabled(true);

    // Finally - we attach the context to this Component.
    openGLContext.attachTo(*this);

    // Disabled
    //openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::OpenGLVersion::openGL3_2);
}

SimpleGLComponent::~SimpleGLComponent() {

    // Tell the context to stop using this Component.
    openGLContext.detach();
}

void SimpleGLComponent::initialise() {

}

void SimpleGLComponent::shutdown() {

}

void SimpleGLComponent::render() {

}

void SimpleGLComponent::newOpenGLContextCreated() {

    // Generate 1 buffer, using our vbo variable to store its ID.
    openGLContext.extensions.glGenBuffers(1, &vbo);

    // Generate 1 more buffer, this time using our IBO variable.
    openGLContext.extensions.glGenBuffers(1, &ibo);

    // Create 4 vertices each with a different colour.
    vertexBuffer = {
            // Vertex 0
            {
                    {-1.0f, 1.0f},        // (-0.5, 0.5)
                    {1.f, 0.f,  0.f, 1.f}  // Red
            },
            // Vertex 1
            {
                    {1.0f,  1.0f},         // (0.5, 0.5)
                    {1.f, 0.5f, 0.f, 1.f} // Orange
            },
            // Vertex 2
            {
                    {1.0f,  -1.0f},        // (0.5, -0.5)
                    {1.f, 1.f,  0.f, 1.f}  // Yellow
            },
            // Vertex 3
            {
                    {-1.0f, -1.0f},       // (-0.5, -0.5)
                    {1.f, 0.f,  1.f, 1.f}  // Purple
            }
    };

    // We need 6 indices, 1 for each corner of the two triangles.
    indexBuffer = {
            0, 1, 2,
            0, 2, 3
    };

    // Bind the VBO.
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Send the vertices data.
    openGLContext.extensions.glBufferData(
            GL_ARRAY_BUFFER,                        // The type of data we're sending.
            sizeof(Vertex) * vertexBuffer.size(),   // The size (in bytes) of the data.
            vertexBuffer.data(),                    // A pointer to the actual data.
            GL_STATIC_DRAW                          // How we want the buffer to be drawn.
    );

    // Bind the IBO.
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Send the indices data.
    openGLContext.extensions.glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(unsigned int) * indexBuffer.size(),
            indexBuffer.data(),
            GL_STATIC_DRAW
    );


#if JUCE_OPENGL_ES
    // IOS
    vertexShader =
            R"(
        attribute vec4 a_Position;
        attribute vec4 a_Color;

        varying lowp vec4 frag_Color;

        void main(){
            frag_Color = a_Color;
            gl_Position = a_Position;
        }
    )";

    fragmentShader =
            R"(
        varying lowp vec4 frag_Color;

        void main()
        {
            gl_FragColor = frag_Color;
        }
    )";
#else
    // PC/Mac os
    vertexShader =
            R"(
        attribute vec4 a_Position;
        attribute vec4 a_Color;

        varying vec4 frag_Color;

        void main(){
            frag_Color = a_Color;
            gl_Position = a_Position;
        }
    )";

    fragmentShader =
            R"(
        varying vec4 frag_Color;

        void main()
        {
            gl_FragColor = frag_Color;
        }
    )";
#endif

    // Create an instance of OpenGLShaderProgram
    shaderProgram.reset(new OpenGLShaderProgram(openGLContext));

    // Compile and link the shader.
    // Each of these methods will return false if something goes wrong so we'll
    // wrap them in an if statement
    if (shaderProgram->addVertexShader(vertexShader)
        && shaderProgram->addFragmentShader(fragmentShader)
        && shaderProgram->link()) {
        // No compilation errors - set the shader program to be active
        shaderProgram->use();
    } else {
        // Oops - something went wrong with our shaders!
        // Check the output window of your IDE to see what the error might be.
        printf("!!!!!ERROR SHADER !!!!!!");
        jassertfalse;
    }

}

void SimpleGLComponent::renderOpenGL() {

    // Clear the screen by filling it with black.
    OpenGLHelpers::clear(Colour::fromFloatRGBA(0, 0, 0, 1));

    // Tell the renderer to use this shader program
    shaderProgram->use();

    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    // Enable the position attribute.
    openGLContext.extensions.glVertexAttribPointer(
            0,              // The attribute's index (AKA location).
            2,              // How many values this attribute contains.
            GL_FLOAT,       // The attribute's type (float).
            GL_FALSE,       // Tells OpenGL NOT to normalise the values.
            sizeof(Vertex), // How many bytes to move to find the attribute with
            // the same index in the next vertex.
            nullptr         // How many bytes to move from the start of this vertex
            // to find this attribute (the default is 0 so we just
            // pass nullptr here).
    );
    openGLContext.extensions.glEnableVertexAttribArray(0);

    // Enable to colour attribute.
    openGLContext.extensions.glVertexAttribPointer(
            1,                              // This attribute has an index of 1
            4,                              // This time we have four values for the
            // attribute (r, g, b, a)
            GL_FLOAT,
            GL_FALSE,
            sizeof(Vertex),
            (GLvoid *) (sizeof(float) * 2)    // This attribute comes after the
            // position attribute in the Vertex
            // struct, so we need to skip over the
            // size of the position array to find
            // the start of this attribute.
    );
    openGLContext.extensions.glEnableVertexAttribArray(1);


    glDrawElements(
            GL_TRIANGLES,       // Tell OpenGL to render triangles.
            indexBuffer.size(), // How many indices we have.
            GL_UNSIGNED_INT,    // What type our indices are.
            nullptr             // We already gave OpenGL our indices so we don't
            // need to pass that again here, so pass nullptr.
    );


    openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);
}

void SimpleGLComponent::openGLContextClosing() {

}

void SimpleGLComponent::paint(Graphics &g) {
    g.fillAll(Colour::fromFloatRGBA(0, 0, 0, 0));
}

void SimpleGLComponent::parentHierarchyChanged() {


}

