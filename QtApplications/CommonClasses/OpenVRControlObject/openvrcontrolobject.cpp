#include "openvrcontrolobject.h"

const QString OpenVRControlObject::VERTEX_SHADER = "#version 330 core\n"
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
const QString OpenVRControlObject::FRAGMENT_SHADER = "#version 330 core\n"
                                                     "in vec2 UV;\n"
                                                     "out vec3 color;\n"
                                                     "uniform sampler2D myTextureSampler;\n"
                                                     "void main(){\n"
                                                     "   color = texture(myTextureSampler,UV).rgb;\n"
                                                     "   //color = vec3(UV.x,UV.y,0);\n"
                                                     "}\n";


/////////////////////////////// INTIALIZATION FUNCTIONS

OpenVRControlObject::OpenVRControlObject(QObject *parent):QObject(parent)
{
    vrSystem = nullptr;
    isInitialized = false;
    openGLTexture = nullptr;
    logger.setID("OpenVR Control Object");
    connect(&timer,SIGNAL(timeout()),this,SLOT(onRefresh()));
    shouldUpdate = false;
}

bool OpenVRControlObject::initializeVR(){
    vr::HmdError peError;
    vrSystem = vr::VR_Init( &peError, vr::VRApplication_Scene );
    if ( peError != vr::VRInitError_None ){
        logger.appendError("Error intializing VR. Error code: " + QString::number(peError));
        return false;
    }

    if (!vr::VRCompositor()){
        logger.appendError("Error initialzing compositor");
        return false;
    }
    return true;
}

bool OpenVRControlObject::initializeOpenGL(){

    QSurfaceFormat format;
    format.setMajorVersion( 4 );
    format.setMinorVersion( 1 );
    format.setProfile( QSurfaceFormat::CompatibilityProfile );

    openGLContext = new QOpenGLContext();
    openGLContext->setFormat( format );
    if( !openGLContext->create() ){
        logger.appendError("Open GL Context initialization failed");
        return false;
    }

    // The offscreen surface is necessary so that the OpenGL context has "somewhere" to draw.
    offscreenSurface = new QOffscreenSurface();
    offscreenSurface->create();
    openGLContext->makeCurrent( offscreenSurface );

    initializeOpenGLFunctions();
    shaderProgram = new QOpenGLShaderProgram(this);
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, VERTEX_SHADER)){
        logger.appendError("ERROR compiling vertex shader: \n" + shaderProgram->log());
        return false;
    }
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, FRAGMENT_SHADER)){
        logger.appendError("ERROR compiling fragment shader: \n" + shaderProgram->log());
        return false;
    }
    shaderProgram->link();
    glid_Program = shaderProgram->programId();

    // Initialzing texture contents
    quint32 renderWidth, renderHeight;
    vrSystem->GetRecommendedRenderTargetSize( &renderWidth, &renderHeight );
    qint32 rWidth = static_cast<qint32>(renderWidth);    // Done simply to avoid warnings.
    qint32 rHeight = static_cast<qint32>(renderHeight);

    recommendedSize.setWidth(rWidth);
    recommendedSize.setHeight(rHeight);

    // Initializing the FB for each eye
    leftFBDesc = initFrameBufferDesc(rWidth,rHeight);
    rightFBDesc = initFrameBufferDesc(rWidth,rHeight);

    QImage image(rWidth,rHeight,QImage::Format_RGB888);
    image.fill(backgroundColor);
    setImage(&image);

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

OpenVRControlObject::FramebufferDesc OpenVRControlObject::initFrameBufferDesc(int nWidth, int nHeight){

    FramebufferDesc framebufferDesc;
    glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId );
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

    glGenTextures(1, &framebufferDesc.m_nRenderTextureId );
    glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nRenderTextureId );
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, nWidth, nHeight);  // TODO: Test RBG8.
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nRenderTextureId, 0);

    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return framebufferDesc;
}

bool OpenVRControlObject::initialization(){

    if (isInitialized) return true; // Intialize only once.

    if (!initializeVR()) {
        logger.appendError("VR Initializtion failed");
        return false;
    }

    if (!initializeOpenGL()) {
        logger.appendError("OpenGL Initialization Failed");
        return false;
    }

    isInitialized = true;

    return true;
}

////////////////////////////////////////// RENDER FUNCTIONS

bool OpenVRControlObject::startRendering(){

    if (!initialization()){
        logger.appendError("Intialization failed. Not rendering anything");
        return false;
    }

    timer.start(REFRESH_RATE_IN_MS);
    shouldUpdate = true;
    return true;
}

void OpenVRControlObject::renderToEye(qint32 whichEye){
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
    glBindFramebuffer( GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId );
    glClearColor(static_cast<float>(backgroundColor.redF()),
                 static_cast<float>(backgroundColor.greenF()),
                 static_cast<float>(backgroundColor.blueF()), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(glid_Program);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glid_Texture);

    glViewport(0,0,recommendedSize.width(),recommendedSize.height());

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
        glDrawArrays(GL_QUADS, 0, 4);
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        vr::Texture_t texture = {(void*)(uintptr_t)framebufferDesc.m_nRenderFramebufferId, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
        vr::VRCompositor()->Submit(eye, &texture );
    }
    else{
        // Draw the square !
        glDrawArrays(GL_QUADS, 0, 4); // Starting from vertex 0; 3 vertices total -> 1 triangle
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

QMatrix4x4 OpenVRControlObject::eyeMath(vr::EVREye eye){
    vr::HmdMatrix44_t p = vrSystem->GetProjectionMatrix( eye, 1.0f, 30.0f);
    vr::HmdMatrix34_t e = vrSystem->GetEyeToHeadTransform( eye );

    QMatrix4x4 qp(
                p.m[0][0], p.m[0][1], p.m[0][2], p.m[0][3],
            p.m[1][0], p.m[1][1], p.m[1][2], p.m[1][3],
            p.m[2][0], p.m[2][1], p.m[2][2], p.m[2][3],
            p.m[3][0], p.m[3][1], p.m[3][2], p.m[3][3]);

    if (eye == vr::Eye_Left){
        leftProjectionMatrix = qp;
    }
    else {
        rightProjectionMatrix = qp;
    }

    QMatrix4x4 qe2h(
                e.m[0][0], e.m[0][1], e.m[0][2], e.m[0][3],
            e.m[1][0], e.m[1][1], e.m[1][2], e.m[1][3],
            e.m[2][0], e.m[2][1], e.m[2][2], e.m[2][3],
            0,         0,         0,         1.0f);

    return qp*qe2h.inverted();
}

void OpenVRControlObject::onRefresh(){
    //QElapsedTimer m;
    //m.start();
    //qDebug() << 0;
    timer.stop();
    vr::VRCompositor()->WaitGetPoses(nullptr,0,nullptr,0);
    //qDebug() << 1 << m.elapsed();
    emit(requestUpdate());
    //qDebug() << 2 << m.elapsed();
    renderToEye(1);
    //qDebug() << 3 << m.elapsed();
    renderToEye(0);
    //qDebug() << 4 << m.elapsed();
    emit(newProjectionMatrixes(rightProjectionMatrix,leftProjectionMatrix));    
    //qDebug() << 5 << m.elapsed();
    if (shouldUpdate) timer.start(REFRESH_RATE_IN_MS); // This will allow me to go as fast as possible when set to 1.
}

////////////////////////////////////////// CONTROL FUNCTIONS

void OpenVRControlObject::setImage(QImage *image){
    if (openGLTexture == nullptr){
        openGLTexture = new QOpenGLTexture(image->mirrored());
        openGLTexture->setMinificationFilter(QOpenGLTexture::Linear);
        openGLTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    }
    else{
//        if (measure_timer.isValid()){
//            qDebug() << "Set image @" << measure_timer.elapsed();
//        }
//        measure_timer.start();
        //QElapsedTimer m;
        //m.start();
        openGLTexture->destroy();
        //qDebug() << "D" << m.elapsed();
        openGLTexture->create();
        //qDebug() << "C" << m.elapsed();
        openGLTexture->setSize(image->size().width()*image->size().height());
        //qDebug() << "SS" << m.elapsed();
        openGLTexture->setData(image->mirrored(),QOpenGLTexture::DontGenerateMipMaps);
        //qDebug() << "Draw" << m.elapsed();
    }
    glid_Texture = openGLTexture->textureId();
}

bool OpenVRControlObject::isRendering(){
    return shouldUpdate;
}

void OpenVRControlObject::stopRendering(){
    shouldUpdate = false;
    timer.stop();
}

QSize OpenVRControlObject::getRecommendedSize(){
    return recommendedSize;
}

void OpenVRControlObject::setScreenColor(QColor color){
    backgroundColor = color;
}

////////////////////////////////////////// EYE TRACKING FUNCTIONS
//void OpenVRControlObject::newPositionData(EyeTrackerData data){
//    targetTest.renderCurrentPosition(data.xLeft,data.yLeft,data.xRight,data.yRight);
//    glid_Texture = targetTest.getTextureGLID();
//    update();
//}

//bool OpenVRControlObject::initializeEyeTracking(){
//    int error = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE, nullptr);
//    if (error == ViveSR::Error::WORK) {
//        poller.setMaxWidthAndHeight(targetTest.getSize().width(),targetTest.getSize().height());
//        poller.start();
//        return true;
//    }
//    else if (error == ViveSR::Error::RUNTIME_NOT_FOUND) {
//        qDebug() << "Eye Tracking Runtime Not Found";
//        return false;
//    }
//    else {
//        qDebug() << "Failed to initialize Eye engine. please refer the code " << error << " of ViveSR::Error";
//        return false;
//    }
//}


OpenVRControlObject::~OpenVRControlObject(){

}
