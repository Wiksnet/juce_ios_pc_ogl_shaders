//
// Created by Vitaly Tarasuk on 25.04.2021.
// Original author ref: https://medium.com/@Im_Jimmi/using-opengl-for-2d-graphics-in-a-juce-plug-in-24aa82f634ff
//

#pragma once

#include <JuceHeader.h>

using namespace juce;

class SimpleGLComponent : public OpenGLAppComponent {
public:
    SimpleGLComponent();

    ~SimpleGLComponent() override;

    void initialise() override;

    void shutdown() override;

    void render() override;

    void newOpenGLContextCreated() override;

    void renderOpenGL() override;

    void openGLContextClosing() override;

    void paint (Graphics& g) override;

    void parentHierarchyChanged() override;

private:
    struct Vertex
    {
        float position[2];
        float sourceColour[4];
    };

    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> indexBuffer;

    GLuint vbo; // Vertex buffer object.
    GLuint ibo; // Index buffer object.

    String vertexShader;
    String fragmentShader;

    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
};

