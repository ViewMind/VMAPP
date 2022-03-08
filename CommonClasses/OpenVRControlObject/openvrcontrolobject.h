#ifndef OPENGLCANVAS_H
#define OPENGLCANVAS_H

#include <QOffscreenSurface>
#include <QTimer>
#include <QPainter>
#include <QImage>
#include <QDebug>
#include <QElapsedTimer>

#include <QOpenGLFunctions>
#include <QOpenGLShader>
#include <QOpenGLExtraFunctions>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

#include "../LogInterface/loginterface.h"
#include "openvr.h"


class OpenVRControlObject : public QObject , protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit OpenVRControlObject(QObject *parent = nullptr);
    ~OpenVRControlObject() override;

    bool startRendering();

    void setImage(QImage *image);
    void setScreenColor(QColor color);
    bool isRendering();
    void stopRendering();
    QSize getRecommendedSize();

signals:
    void newProjectionMatrixes(QMatrix4x4 r, QMatrix4x4 l);
    void requestUpdate();

private slots:
    void onRefresh();

private:

     // Pixel and Fragment Shader
    static const QString VERTEX_SHADER;
    static const QString FRAGMENT_SHADER;

    // Update timer constant
    static const qint32 REFRESH_RATE_IN_MS = 1;

    // VR Related variables and functions
    vr::IVRSystem * vrSystem;    
    QMatrix4x4 eyeMath(vr::EVREye eye);
    bool initializeVR();

    // Open GL Related variables.
    QOpenGLShaderProgram *shaderProgram;
    QOpenGLTexture *ogltexture;
    QOpenGLContext *openGLContext;
    QOffscreenSurface *offscreenSurface;
    QOpenGLTexture *openGLTexture;

    GLuint glid_Program;
    GLuint glid_Texture;
    GLint  glid_ShaderTextureParameterID;
    GLint  glid_PerpectiveMatrix;
    GLuint glid_VertexBuffer;
    GLuint glid_UVBuffer;
    GLuint glid_VertexArrayID;

    struct FramebufferDesc
    {
        GLuint m_nRenderTextureId;
        GLuint m_nRenderFramebufferId;
    };
    FramebufferDesc leftFBDesc;
    FramebufferDesc rightFBDesc;
    FramebufferDesc initFrameBufferDesc(int nWidth, int nHeight);
    bool initializeOpenGL();
    void renderToEye(qint32 whichEye);

    // Other variables and functions.
    bool initialization();
    bool isInitialized;
    QSize recommendedSize;
    QColor backgroundColor;
    QMatrix4x4 leftProjectionMatrix;
    QMatrix4x4 rightProjectionMatrix;

    // Logger for errors.
    LogInterface logger;

    // Found no other way of updating the images.
    QTimer timer;
    bool shouldUpdate;


    // For Debugging.
    QElapsedTimer measure_timer;

};

#endif // OPENGLCANVAS_H
