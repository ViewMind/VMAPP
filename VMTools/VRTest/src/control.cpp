#include "control.h"

const char * Control::vertexShaderSource =
        "attribute highp vec4 posAttr;\n"
        "attribute highp vec4 colAttr;\n"
        "varying highp vec2 texCoord;\n"
        "uniform highp mat4 matrix;\n"
        "void main() {\n"
        "   texCoord = posAttr.xy;\n"
        "   gl_Position = matrix * posAttr;\n"
        "}\n";

const char * Control::fragmentShaderSource =
        "varying highp vec2 texCoord;\n"
        "uniform sampler2D ourTexture;\n"
        "void main() {\n"
        "   gl_FragColor = texture2D(ourTexture, texCoord);\n"
        //        "   gl_FragColor = vec4(1.0,1.0,1.0,1.0);\n"
        //"   gl_FragColor = vec4(texCoord.x,texCoord.y,0.0,1.0);\n"
        "}\n";

Control::Control(QObject *parent) : QObject(parent)
{
    vrSystem = nullptr;
    connect(&timer,&QTimer::timeout,this,&Control::updateView);
    systemInitialized = false;
}


/////////////////////////////// INTIALIZATION FUNCTIONS
bool Control::initializeVR(){
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

bool Control::initializeOpenGL(){

    QSurfaceFormat format;
    format.setMajorVersion( 4 );
    format.setMinorVersion( 1 );
    format.setProfile( QSurfaceFormat::CompatibilityProfile );

    openGLContext = new QOpenGLContext();
    openGLContext->setFormat( format );
    if( !openGLContext->create() ){
        qDebug() << "Open GL Context initialization failed";
        return false;
    }

    // The offscreen surface is necessary so that the OpenGL context has "somewhere" to draw.
    offscreenSurface = new QOffscreenSurface();
    offscreenSurface->create();
    openGLContext->makeCurrent( offscreenSurface );

    initializeOpenGLFunctions();


    shaderProgram = new QOpenGLShaderProgram(this);
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource)){
        qDebug() << "ERROR compiling vertex shader";
        qDebug() << shaderProgram->log();
        return false;
    }
    if (!shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource)){
        qDebug() << "ERROR compiling fragment shader";
        qDebug() << shaderProgram->log();
        return false;
    }
    shaderProgram->link();
    glid_PosAttr = static_cast<GLuint>(shaderProgram->attributeLocation("posAttr"));
    glid_PerpectiveMatrix = static_cast<GLuint>(shaderProgram->uniformLocation("matrix"));

    return true;
}

bool Control::initializeDrawSystem(){
    if (!vrSystem) return false;
    quint32 renderWidth, renderHeight;
    vrSystem->GetRecommendedRenderTargetSize( &renderWidth, &renderHeight );
    qDebug() << "Recommended Render Width - Height" << renderWidth << renderHeight;
    //qDebug() << "Recommende width and height to next power of 2" << qNextPowerOfTwo(renderWidth) << qNextPowerOfTwo(renderHeight);
    targetTest.initialize(renderWidth,renderHeight);
    qDebug() << targetTest.getRecommendedRenderSize();
    return true;
}

bool Control::doIntialization(){
    if (!initializeVR()) return false;
    qDebug() << "VR Initialized";

    if (!initializeOpenGL()) return false;
    qDebug() << "Open GL Initialized";

    if (!initializeDrawSystem()) return false;
    qDebug() << "Qt Draw System Initialized";

    return true;
}


////////////////////////////////////////// CONTROL FUNCTIONS
void Control::startTest(){
    if (!systemInitialized){
        if (!doIntialization()){
            qDebug() << "INITIALIZATION FAILED";
            return;
        }
        systemInitialized = true;
    }
    timer.start(4);
    qDebug() << "Started rendering";
}


void Control::stopTest(){
    timer.stop();
    qDebug() << "Stopped rendering";
}

////////////////////////////////////////// RENDER FUNCTIONS
void Control::updateView(){
    vr::VRCompositor()->WaitGetPoses(nullptr,0,nullptr,0);
    renderEye(vr::Eye_Left);
    renderEye(vr::Eye_Right);
}

void Control::renderEye(vr::EVREye eye){

    QOpenGLFramebufferObject qTextureBufferObject(targetTest.getRecommendedRenderSize(), GL_TEXTURE_2D);
    QOpenGLPaintDevice device( qTextureBufferObject.size() );
    QPainter painter( &device );
    targetTest.renderCurrentPosition(0,0,0,0);


    //const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, targetTest.getRecommendedRenderSize().width(), targetTest.getRecommendedRenderSize().width());

    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture

    shaderProgram->bind();
    //glBindTexture(GL_TEXTURE_2D, qTextureBufferObject.texture());

    QMatrix4x4 matrix;
    QMatrix4x4 vrp = eyeMath(eye);
    matrix.translate(0, 0, -2);
    matrix = vrp*matrix;

    shaderProgram->setUniformValue(static_cast<qint32>(glid_PerpectiveMatrix), matrix);

    GLfloat vertices[] = {
        -1.0f,  1.0f, 0, 1,
        -1.0f, -1.0f, 0, 1,
         1.0f, -1.0f, 0, 1,
         1.0f,  1.0f, 0, 1,
    };

    glVertexAttribPointer(glid_PosAttr, 4, GL_FLOAT, GL_FALSE, 0, vertices);

    glEnableVertexAttribArray(0);

    QOpenGLFramebufferObject qDrawBufferObject(targetTest.getRecommendedRenderSize(),GL_TEXTURE_2D);

    glDrawArrays(GL_QUADS, 0, 4);

    vr::Texture_t texture = {(void*)qDrawBufferObject.texture(), vr::TextureType_OpenGL, vr::ColorSpace_Auto };
    vr::VRCompositor()->Submit(eye,&texture);

    qDrawBufferObject.release();

    glDisableVertexAttribArray(0);

    shaderProgram->release();

}

QMatrix4x4 Control::eyeMath(vr::EVREye eye){
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
void Control::newPositionData(int rx, int ry, int lx, int ly){
    qDebug() << rx << ry << lx << ly;
}


Control::~Control(){
}


/// OLD CODE. TRANSFORM DEVICE TO QSTRING FOR PRINTING. Just in case.
///QString getTrackedDeviceString(vr::TrackedDeviceClass tdc); // Device to string function
//QString Control::getTrackedDeviceString(vr::TrackedDeviceClass tdc){
//    switch (tdc) {
//    case vr::TrackedDeviceClass_Invalid:
//        return "INVALID";
//    case vr::TrackedDeviceClass_HMD:
//        return "HMD";
//    case vr::TrackedDeviceClass_Controller:
//        return "Controller";
//    case vr::TrackedDeviceClass_GenericTracker:
//        return "Generic Tracker";
//    case vr::TrackedDeviceClass_TrackingReference:
//        return "Tracking reference";
//    case vr::TrackedDeviceClass_DisplayRedirect:
//        return "DisplayRedirect";
//    default:
//        return "Unknown Divce Class";
//    }
//}

/////////////// OLD CODE. WORKING EXAMPLE OF CREATING AN OVERLAY. Just in case.
/// Requires the use of this Global:
/// vr::VROverlayHandle_t vrOverlay;
//void Control::startTestOverlay(){
//    vr::HmdError peError;
//    vrSystem = vr::VR_Init( &peError, vr::VRApplication_Overlay );
//    //    qDebug() << "LISTING VR TRACKED DEVICES";
//    //    for (quint32 i = 0; i < vr::k_unMaxTrackedDeviceCount; i++){
//    //        vr::TrackedDeviceClass tdc = vrSystem->GetTrackedDeviceClass(i);
//    //        qDebug() << "DEVICE" << i << ":" << getTrackedDeviceString(tdc);
//    //    }

//    // Creating an overlay
//    //vr::VROverlayError error = vr::IVROverlay::CreateOverlay( "OverlayKey", "MyOverlay",  &vrOverlay );
//    vr::VROverlayError overlayError;
//    if( vr::VROverlay() )
//    {
//        std::string sKey = "my.personal.key";
//        std::string sFriendlyName = "My First Overlay";
//        overlayError = vr::VROverlay()->CreateOverlay( sKey.c_str(), sFriendlyName.c_str(), &vrOverlay);
//        if (overlayError == vr::VROverlayError_None){
//            qDebug() << "Created empty overlay";
//        }
//        else{
//            qDebug() << "There was an overlay error. Could not create overlay: " << overlayError;
//            return;
//        }
//    }
//    else{
//        qDebug() << "There is no pointer to vr::VROverlay";
//        return;
//    }


//    // At this point the overlay is created. I will setup the color, the alpha and display it.
//    //    overlayError = vr::VROverlay()->SetOverlayColor( vrOverlay, 1.0f, 0.0f, 0.0f );
//    //    if (overlayError != vr::VROverlayError_None){
//    //        qDebug() << "ERROR: Setting the overlay color: " << overlayError;
//    //        return;
//    //    }
//    //    else{
//    //        qDebug() << "OVERLAY Color SET.";
//    //    }

//    //    overlayError = vr::VROverlay()->SetOverlayAlpha(vrOverlay, 1.0f);
//    //    if (overlayError != vr::VROverlayError_None){
//    //        qDebug() << "ERROR: Setting the overlay alpha: " << overlayError;
//    //        return;
//    //    }
//    //    else{
//    //        qDebug() << "OVERLAY Alpha SET.";
//    //    }

//    overlayError = vr::VROverlay()->ShowOverlay(vrOverlay);
//    if (overlayError != vr::VROverlayError_None){
//        qDebug() << "ERROR: Making the OverlayVisible: " << overlayError;
//        return;
//    }
//    else{
//        qDebug() << "OVERLAY Should be visible";
//    }

//    overlayError = vr::VROverlay()->SetOverlayWidthInMeters( vrOverlay, 1.5f );
//    if (overlayError != vr::VROverlayError_None){
//        qDebug() << "ERROR: Making the Setting the overlay Width: " << overlayError;
//        return;
//    }
//    else{
//        qDebug() << "OVERLAY Width was set";
//    }

//    // Creating the frame buffer object
//    targetTest.initialize(1920,1080);
//    qDebug() << "TargetTest was initialized";

//    // Now we render to the screen.
//    targetTest.renderCurrentPosition(0,0,0,0);
//    qDebug() << "Rendered position 0,0,0,0";

//    GLuint unTexture = targetTest.getFBO()->texture();
//    if( unTexture != 0 )
//    {
//        vr::Texture_t texture = {(void*)(uintptr_t)unTexture, vr::TextureType_OpenGL, vr::ColorSpace_Auto };
//        vr::VROverlay()->SetOverlayTexture( vrOverlay, &texture );
//    }
//    qDebug() << "Setted the Overlay Texture";

//}


