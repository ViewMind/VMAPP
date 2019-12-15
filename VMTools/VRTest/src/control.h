#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QtMath>

#include "openvr.h"
#include "targettest.h"


class Control : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    ~Control();

    Q_INVOKABLE void startTest();
    Q_INVOKABLE void stopTest();

signals:

public slots:

    void newPositionData(int rx, int ry, int lx, int ly);

private slots:
    void updateView();

private:

    // VR Related variables and functions
    vr::IVRSystem * vrSystem;    
    QMatrix4x4 eyeMath(vr::EVREye eye); // Eye Perspective / Proyection Matrix.
    QTimer timer; // To Refresh.
    // Pixel and Fragment Shader
    static const char *vertexShaderSource;
    static const char *fragmentShaderSource;
    bool initializeVR();

    // Open GL Related variables.
    QOpenGLContext *openGLContext;
    QOffscreenSurface *offscreenSurface;
    QOpenGLShaderProgram *shaderProgram;
    GLuint glid_PosAttr;
    GLuint glid_PerpectiveMatrix;
    bool initializeOpenGL();
    void renderEye(vr::EVREye eye);

    // The QGraphics Framework class that actually draws.
    TargetTest targetTest;
    bool initializeDrawSystem();

    // Other variables and functions.
    bool systemInitialized;
    bool doIntialization();

};

#endif // CONTROL_H
