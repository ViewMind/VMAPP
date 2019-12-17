#ifndef OPENGLCANVAS_H
#define OPENGLCANVAS_H

#include <QOffscreenSurface>
#include <QTimer>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>
#include <QDebug>

#include "openvr.h"
#include "targettest.h"

class OpenVRControlObject : public QObject , protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit OpenVRControlObject(QObject *parent = nullptr);
    ~OpenVRControlObject() override;

    void initializeGL();
    void paintGL();

    Q_INVOKABLE void startTest();
    Q_INVOKABLE void stopTest();

private slots:
    void updateView();

private:

     // Pixel and Fragment Shader
    static const QString VERTEX_SHADER;
    static const QString FRAGMENT_SHADER;

    // VR Related variables and functions
    vr::IVRSystem * vrSystem;    
    QMatrix4x4 eyeMath(vr::EVREye eye);
    QTimer timer;
    bool initializeVR();

    // Open GL Related variables.
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLTexture *ogltexture;
    QOpenGLContext *openGLContext;
    QOffscreenSurface *offscreenSurface;
    GLuint glid_Program;
    GLuint glid_Texture;
    GLint  glid_ShaderTextureParameterID;
    GLint  glid_PerpectiveMatrix;
    GLuint glid_VertexBuffer;
    GLuint glid_UVBuffer;
    GLuint glid_VertexArrayID;
    struct FramebufferDesc
    {
        GLuint m_nDepthBufferId;
        GLuint m_nRenderTextureId;
        GLuint m_nRenderFramebufferId;
        GLuint m_nResolveTextureId;
        GLuint m_nResolveFramebufferId;
    };

    FramebufferDesc leftFBDesc;
    FramebufferDesc rightFBDesc;
    FramebufferDesc initFrameBufferDesc(int nWidth, int nHeight);
    bool initializeOpenGL();
    void renderToEye(qint32 whichEye);

    // The QGraphics Framework class that actually draws.
    TargetTest targetTest;

    // Other variables and functions.
    bool systemInitialized;
    bool enableRefresh;

};

#endif // OPENGLCANVAS_H
