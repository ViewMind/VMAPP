///  OLD CODE: For intializing OpenGLContext
//    QSurfaceFormat format;
//    format.setMajorVersion( 4 );
//    format.setMinorVersion( 1 );
//    format.setProfile( QSurfaceFormat::CompatibilityProfile );

//    openGLContext = new QOpenGLContext();
//    openGLContext->setFormat( format );
//    if( !openGLContext->create() ){
//        qDebug() << "Open GL Context initialization failed";
//        return false;
//    }

//    // The offscreen surface is necessary so that the OpenGL context has "somewhere" to draw.
//    offscreenSurface = new QOffscreenSurface();
//    offscreenSurface->create();
//    openGLContext->makeCurrent( offscreenSurface );

/// OLD CODE. TRANSFORM DEVICE TO QSTRING FOR PRINTING. Just in case.
/// QString getTrackedDeviceString(vr::TrackedDeviceClass tdc); // Device to string function
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


