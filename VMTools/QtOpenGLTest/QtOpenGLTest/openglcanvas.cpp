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
    targetTest.initialize(this->width(),this->height());
    targetTest.renderCurrentPosition(0,0,0,0);
    glid_Texture = targetTest.getTextureGLID();
}

void OpenGLCanvas::paintGL(){

    ////// SETUP
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

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

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);

    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(glid_Program, "myTextureSampler");  // THIS is the reference the the shader program parametner. The other is a reference to the actual shader data in memory.
    //qDebug() << "Texture sampler ID" << TextureID;

    ////// END SETUP

    /// ACTUALLY DRAWING
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glUseProgram(glid_Program);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glid_Texture);

    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(TextureID, 0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                3,                  // size
                GL_FLOAT,           // type
                GL_FALSE,           // normalized?
                0,                  // stride
                (void*)0            // array buffer offset
                );

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
        2,                                // size : U+V => 2
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );


    // Draw the square !
    glDrawArrays(GL_QUADS, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

}

void OpenGLCanvas::loadShaders(){
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code
    std::string VertexShaderCode = VERTEX_SHADER.toStdString();

    // Read the Fragment Shader code
    std::string FragmentShaderCode = FRAGMENT_SHADER.toStdString();

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> VertexShaderErrorMessage(static_cast<unsigned long>(InfoLogLength+1));
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        qDebug() << "ERROR compiling vertex shader: " <<  VertexShaderErrorMessage;
    }

    // Compile Fragment Shader
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> FragmentShaderErrorMessage (static_cast<unsigned long>(InfoLogLength+1));
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        qDebug() << "ERROR compiling fragment shader: " <<  FragmentShaderErrorMessage;
    }

    // Link the program
    glid_Program = glCreateProgram();
    glAttachShader(glid_Program, VertexShaderID);
    glAttachShader(glid_Program, FragmentShaderID);
    glLinkProgram(glid_Program);

    // Check the program
    glGetProgramiv(glid_Program, GL_LINK_STATUS, &Result);
    glGetProgramiv(glid_Program, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if ( InfoLogLength > 0 ){
        std::vector<char> ProgramErrorMessage(static_cast<unsigned long>(InfoLogLength+1));
        glGetProgramInfoLog(glid_Program, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        qDebug()  << "ERROR linking program " << ProgramErrorMessage;
    }

    glDetachShader(glid_Program, VertexShaderID);
    glDetachShader(glid_Program, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

}

GLuint OpenGLCanvas::loadTexture(QString path){
    QImage image(path);
    ogltexture = new QOpenGLTexture(image);
    ogltexture->setMinificationFilter(QOpenGLTexture::Nearest);
    ogltexture->setMagnificationFilter(QOpenGLTexture::Nearest);
    return ogltexture->textureId();
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
