#ifndef OPENGLCANVAS_H
#define OPENGLCANVAS_H

#include <QOpenGLWidget>
#include "targettest.h"
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLExtraFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QOpenGLShaderProgram>
#include <QDebug>

class OpenGLCanvas : public QOpenGLWidget , protected QOpenGLExtraFunctions
{
    Q_OBJECT
public:
    explicit OpenGLCanvas(QWidget *parent = nullptr);

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

protected:
    void mouseMoveEvent(QMouseEvent *ev) override;

private:

    static const QString VERTEX_SHADER;
    static const QString FRAGMENT_SHADER;

    TargetTest targetTest;

    GLuint glid_Program;
    GLuint glid_Texture;
    GLint glid_ShaderTextureParameterID;
    GLuint glid_VertexBuffer;
    GLuint glid_UVBuffer;
    GLuint glid_VertexArrayID;

    void loadShaders();

    QOpenGLShaderProgram *shaderProgram;
    QOpenGLTexture *ogltexture;

};

#endif // OPENGLCANVAS_H