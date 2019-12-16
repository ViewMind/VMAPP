#include "openglcanvas.h"

const QString OpenGLCanvas::VERTEX_SHADER = "#version 330 core\n"
                                            "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
                                            "layout(location = 1) in vec2 vertexUV;\n"
                                            "out vec2 UV;\n"
                                            "void main() {\n"
                                            "   gl_Position.xyz = vertexPosition_modelspace;\n"
                                            "   gl_Position.w = 1.0;"
                                            "   UV = vertexUV;"
                                            "}\n";
const QString OpenGLCanvas::FRAGMENT_SHADER = "#version 330 core\n"
                                              "in vec2 UV;\n"
                                              "out vec3 color;\n"
                                              "uniform sampler2D myTextureSampler;\n"
                                              "void main(){\n"
                                              "   color = texture(myTextureSampler,UV).rgb;\n"
                                              "   //color = vec3(UV.x,UV.y,0);\n"
                                              "}\n";

OpenGLCanvas::OpenGLCanvas(QWidget *parent):QOpenGLWidget(parent)
{

}

void OpenGLCanvas::initializeGL(){
    initializeOpenGLFunctions();
    loadShaders();

    // Initialzing texture contents
    targetTest.initialize(this->width(),this->height());
    targetTest.renderCurrentPosition(0,0,0,0);
    glid_Texture = targetTest.getTextureGLID();

    // Intialze buffers for the verstex and UV coordinates.
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };

    static const GLfloat g_uv_buffer_data[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };

    glGenBuffers(1, &glid_VertexBuffer);                                                                // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glBindBuffer(GL_ARRAY_BUFFER, glid_VertexBuffer);                                                   // The following commands will talk about our 'vertexbuffer' buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);  // Give our vertices to OpenGL.

    glGenBuffers(1, &glid_UVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, glid_UVBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    glid_ShaderTextureParameterID  = glGetUniformLocation(glid_Program, "myTextureSampler");            // THIS is the reference the the shader program parametner.

    glGenVertexArrays(1, &glid_VertexArrayID);
    glBindVertexArray(glid_VertexArrayID);


}

void OpenGLCanvas::paintGL(){

    /// ACTUALLY DRAWING
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glUseProgram(glid_Program);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glid_Texture);

    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(glid_ShaderTextureParameterID, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, glid_VertexBuffer);
    glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                nullptr             // array buffer offset
     );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, glid_UVBuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        nullptr                           // array buffer offset
    );


    // Draw the square !
    glDrawArrays(GL_QUADS, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

}

void OpenGLCanvas::loadShaders(){
    shaderProgram = new QOpenGLShaderProgram(this);
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, VERTEX_SHADER)){
        qDebug() << "ERROR compiling vertex shader";
        qDebug() << shaderProgram->log();
    }
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, FRAGMENT_SHADER)){
        qDebug() << "ERROR compiling fragment shader";
        qDebug() << shaderProgram->log();
    }
    shaderProgram->link();
    glid_Program = shaderProgram->programId();
}


void OpenGLCanvas::mouseMoveEvent(QMouseEvent *ev){
    QOpenGLWidget::mouseMoveEvent(ev);
    targetTest.renderCurrentPosition(ev->x(),ev->y(),ev->x(),ev->y());
    glid_Texture = targetTest.getTextureGLID();
    this->update();
}

void OpenGLCanvas::resizeGL(int width, int height)
{

    Q_UNUSED(width) Q_UNUSED(height)

}
