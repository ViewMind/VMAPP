#include "openvrcontrol.h"

const QString OpenVRControl::VERTEX_SHADER = "#version 330 core\n"
                                             "layout(location = 0) in vec3 vertexPosition_modelspace;\n"
                                             "layout(location = 1) in vec2 vertexUV;\n"
                                             "out vec2 UV;\n"
                                             "uniform mat4 MVP;"
                                             "void main() {\n"
                                             "   gl_Position = MVP * vec4(vertexPosition_modelspace,1);\n"
                                             "   //gl_Position.xyz = vertexPosition_modelspace;\n"
                                             "   //gl_Position.w = 1.0;\n"
                                             "   UV = vertexUV;"
                                             "}\n";
const QString OpenVRControl::FRAGMENT_SHADER = "#version 330 core\n"
                                               "in vec2 UV;\n"
                                               "out vec3 color;\n"
                                               "uniform sampler2D myTextureSampler;\n"
                                               "void main(){\n"
                                               "   color = texture(myTextureSampler,UV).rgb;\n"
                                               "   //color = vec3(UV.x,UV.y,0);\n"
                                               "}\n";

OpenVRControl::OpenVRControl(QWidget *parent):QOpenGLWidget(parent)
{
    vrSystem = nullptr;
    qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
    connect(&timer,&QTimer::timeout,this,&OpenVRControl::updateView);
    connect(&poller,SIGNAL(newEyeData(EyeTrackerData)),this,SLOT(newPositionData(EyeTrackerData)));
    systemInitialized = false;
    enableRefresh = false;
}

/////////////////////////////// INTIALIZATION FUNCTIONS
bool OpenVRControl::initializeVR(){
    vr::HmdError peError;
    vrSystem = vr::VR_Init( &peError, vr::VRApplication_Scene );
    if ( peError != vr::VRInitError_None ){
        qDebug() << "Error intializing VR";
        return false;
    }

    if (!vr::VRCompositor()){
        qDebug() << "Error initialzing compositor";
        return false;
    }
    return true;
}

bool OpenVRControl::initializeOpenGL(){
    initializeOpenGLFunctions();
    shaderProgram = new QOpenGLShaderProgram(this);
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, VERTEX_SHADER)){
        qDebug() << "ERROR compiling vertex shader";
        qDebug() << shaderProgram->log();
        return false;
    }
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, FRAGMENT_SHADER)){
        qDebug() << "ERROR compiling fragment shader";
        qDebug() << shaderProgram->log();
        return false;
    }
    shaderProgram->link();
    glid_Program = shaderProgram->programId();

    // Initialzing texture contents
    if (!vrSystem) return false;
    quint32 renderWidth, renderHeight;
    vrSystem->GetRecommendedRenderTargetSize( &renderWidth, &renderHeight );
    qint32 rWidth = static_cast<qint32>(renderWidth);    // Done simply to avoid warnings.
    qint32 rHeight = static_cast<qint32>(renderHeight);
    targetTest.initialize(rWidth,rHeight);
    targetTest.renderCurrentPosition(0,0,0,0);
    glid_Texture = targetTest.getTextureGLID();

    this->setGeometry(0,0,rWidth,rHeight);

    // Initializing the FB for each eye
    leftFBDesc = initFrameBufferDesc(rWidth,rHeight);
    rightFBDesc = initFrameBufferDesc(rWidth,rHeight);

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
    glid_PerpectiveMatrix          = glGetUniformLocation(glid_Program, "MVP");                         // Reference to the perspective matrix.

    glGenVertexArrays(1, &glid_VertexArrayID);
    glBindVertexArray(glid_VertexArrayID);

    return true;

}

OpenVRControl::FramebufferDesc OpenVRControl::initFrameBufferDesc(int nWidth, int nHeight){

    FramebufferDesc framebufferDesc;

    // Initializing VR Framebuffers. =================================================================================================
    glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId );
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

    // The depth buffer is unnecessary
    glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
    glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight );
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,	framebufferDesc.m_nDepthBufferId ); // this binds it to the current frame buffer.
    // The depth buffer is unnecessary

    glGenTextures(1, &framebufferDesc.m_nRenderTextureId );
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId );
    glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, nWidth, nHeight);  // TODO: Test RBG8.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

    glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId );
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

    glGenTextures(1, &framebufferDesc.m_nResolveTextureId );
    glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);
    //==================================================================================================================================

    return framebufferDesc;
}

////////////////////////////////////////// RENDER FUNCTIONS
void OpenVRControl::updateView(){
    this->update();
}

void OpenVRControl::renderToEye(qint32 whichEye){
    vr::EVREye eye = vr::Eye_Left;
    bool renderToScreen = false;
    FramebufferDesc framebufferDesc;
    if (whichEye == 0) {
        framebufferDesc = leftFBDesc;
        eye = vr::Eye_Left;
    }
    else if (whichEye == 1) {
        framebufferDesc = rightFBDesc;
        eye = vr::Eye_Right;
    }
    else renderToScreen = true;

    /// ACTUALLY DRAWING
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    glUseProgram(glid_Program);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glid_Texture);

    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(glid_ShaderTextureParameterID, 0);

    QMatrix4x4 matrix;
    if (!renderToScreen){

        QMatrix4x4 vrp = eyeMath(eye);
        matrix.translate(0, 0, -2);
        matrix = vrp*matrix;

        // Setting the perspective matrix.
        shaderProgram->setUniformValue(glid_PerpectiveMatrix, matrix);
    }
    else{
        QMatrix4x4 matrix;
        // Setting the perspective matrix.
        shaderProgram->setUniformValue(glid_PerpectiveMatrix, matrix);
    }


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

    if (!renderToScreen){
        glEnable( GL_MULTISAMPLE );
        glBindFramebuffer( GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId );
        glDrawArrays(GL_QUADS, 0, 4);
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );

        glDisable( GL_MULTISAMPLE );

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId );

        glBlitFramebuffer( 0, 0, targetTest.getSize().width(), targetTest.getSize().height(), 0, 0, targetTest.getSize().width(), targetTest.getSize().height(),
                           GL_COLOR_BUFFER_BIT,
                           GL_LINEAR );

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        vr::Texture_t texture = {(void*)(uintptr_t)framebufferDesc.m_nResolveTextureId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        //vr::Texture_t texture = {(void*)(uintptr_t)framebufferDesc.m_nRenderFramebufferId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        vr::VRCompositor()->Submit(eye, &texture );
    }
    else{
        // Draw the square !
        glDrawArrays(GL_QUADS, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

QMatrix4x4 OpenVRControl::eyeMath(vr::EVREye eye){
    vr::HmdMatrix44_t p = vrSystem->GetProjectionMatrix( eye, 1.0f, 30.0f);
    vr::HmdMatrix34_t e = vrSystem->GetEyeToHeadTransform( eye );

    QMatrix4x4 qp(
                p.m[0][0], p.m[0][1], p.m[0][2], p.m[0][3],
            p.m[1][0], p.m[1][1], p.m[1][2], p.m[1][3],
            p.m[2][0], p.m[2][1], p.m[2][2], p.m[2][3],
            p.m[3][0], p.m[3][1], p.m[3][2], p.m[3][3]);
    QMatrix4x4 qe2h(
                e.m[0][0], e.m[0][1], e.m[0][2], e.m[0][3],
            e.m[1][0], e.m[1][1], e.m[1][2], e.m[1][3],
            e.m[2][0], e.m[2][1], e.m[2][2], e.m[2][3],
            0,         0,         0,         1.0f);

    return qp*qe2h.inverted();
}

////////////////////////////////////////// EYE TRACKING FUNCTIONS
void OpenVRControl::newPositionData(EyeTrackerData data){
    //    qDebug() << data.toString();
    //    targetTest.renderCurrentPosition(data.xLeft,data.yLeft,data.xRight,data.yRight);
    //    glid_Texture = targetTest.getTextureGLID();
    //    this->update();
}

bool OpenVRControl::initializeEyeTracking(){
    int error = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE, nullptr);
    if (error == ViveSR::Error::WORK) {
        poller.setMaxWidthAndHeight(targetTest.getSize().width(),targetTest.getSize().height());
        poller.start();
        return true;
    }
    else if (error == ViveSR::Error::RUNTIME_NOT_FOUND) {
        qDebug() << "Eye Tracking Runtime Not Found";
        return false;
    }
    else {
        qDebug() << "Failed to initialize Eye engine. please refer the code " << error << " of ViveSR::Error";
        return false;
    }
}

////////////////////////////////////////// CONTROL FUNCTIONS
void OpenVRControl::start(){
    enableRefresh = true;
    timer.start(4);
}

void OpenVRControl::stop(){
    enableRefresh = false;
    if (poller.isRunning()) {
        poller.stop();
        qDebug() << "Eye tracking stopped";
    }
    timer.stop();
}

////////////////////////////////////////// REIMPLEMENTED FUNCTIONS
void OpenVRControl::initializeGL(){

    systemInitialized = false;

    if (!initializeVR()) return;
    qDebug() << "VR Initialized";

    if (!initializeOpenGL()) return;
    qDebug() << "Open GL Initialized";

#ifdef ENABLE_EYE_TRACKING
    if (!initializeEyeTracking()) return;
    qDebug() << "Eye Tracking initialized";
#endif
    systemInitialized = true;
    timer.start(4);
}

void OpenVRControl::paintGL(){
    if (!enableRefresh) return;
    vr::VRCompositor()->WaitGetPoses(nullptr,0,nullptr,0);

    if (poller.isRunning()){
        EyeTrackerData data = poller.getLastData();
        targetTest.renderCurrentPosition(data.xLeft,data.yLeft,data.xRight,data.yRight);
        glid_Texture = targetTest.getTextureGLID();
    }

    renderToEye(0);
    renderToEye(1);
    renderToEye(2);
}

OpenVRControl::~OpenVRControl(){
    QOpenGLWidget::~QOpenGLWidget();
}
