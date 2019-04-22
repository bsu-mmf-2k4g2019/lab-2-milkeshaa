#ifndef WIDGET_H
#define WIDGET_H

#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget() override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);

signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void timerEvent(QTimerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    // Shader programms
    QOpenGLShaderProgram m_prog_container;
    QOpenGLShaderProgram m_prog_piramids;
    QOpenGLShaderProgram m_prog_triangle;

    // VAOs
    GLuint m_vao_container_id;
    GLuint m_vao_piramids_id;
    GLuint m_vao_triangle_id;

    // Textures
    GLuint m_texture_bg_id;
    GLuint m_texture_smile_id;

    // Uniforms
    GLint  m_triangle_color_id;

    QVector3D camera_pos;
    QVector3D camera_up;
    QVector3D camera_front;

    QTime frame_delta_time;
    QTime last_frame_time;

    int m_xRot;
    int m_yRot;
    int m_zRot;
    QPoint m_lastPos;
};

#endif // WIDGET_H
