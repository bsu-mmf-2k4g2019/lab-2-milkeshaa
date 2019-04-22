#include "glwindow.h"

#include <QImage>
#include <QTime>
#include <QMouseEvent>
#include <iostream>

static const char *vertexShaderSource_container =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aCol;\n"
    "layout (location = 2) in vec2 aTex;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "    ourColor = aCol;\n"
    "    TexCoord = aTex;\n"
    "}\n\0";

static const char *vertexShaderSource_triangle =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n\0";

static const char *fragmentShaderSource_container =
    "#version 330 core\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D ourBg;\n"
    "uniform sampler2D ourFace;\n"
    "void main()\n"
    "{\n"
    "    FragColor = mix(texture(ourBg, TexCoord), texture(ourFace, TexCoord), 0.5) * vec4(ourColor, 1.0);\n"
    "}\n\0";

static const char *fragmentShaderSource_triangle =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "uniform vec4 color;\n"
    "void main()\n"
    "{\n"
    "    FragColor = color;\n"
    "}\n\0";

static QVector3D cubePosition = QVector3D(0.0f, 0.0f, 0.0f);

static QVector3D piramidPosition = QVector3D(3.0f, 0.0f, -5.0f);

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , camera_up(0.0f, 1.0f, 0.0f)
    , camera_front(0.0f, 0.0f, -1.0f)
{
}

GLWidget::~GLWidget()
{
}

void GLWidget::timerEvent(QTimerEvent *event)
{
    update();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_xRot) {
        m_xRot = angle;
        emit xRotationChanged(angle);
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_yRot) {
        m_yRot = angle;
        emit yRotationChanged(angle);
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != m_zRot) {
        m_zRot = angle;
        emit zRotationChanged(angle);
        update();
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    m_lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    float cameraSpeed = 0.30f; // adjust accordingly
    if (event->key() == Qt::Key_Space) {
        camera_pos += cameraSpeed * camera_up;
    }
    if (event->key() == Qt::Key_W)
        camera_pos += cameraSpeed * camera_front;
    if (event->key() == Qt::Key_S)
        camera_pos -= cameraSpeed * camera_front;
    if (event->key() == Qt::Key_A)
        camera_pos -= QVector3D::crossProduct(camera_front, camera_up).normalized() * cameraSpeed;
    if (event->key() == Qt::Key_D)
        camera_pos += QVector3D::crossProduct(camera_front, camera_up).normalized() * cameraSpeed;
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);

    // Create VAO & VBO, bind it to current GL_ARRAY_BUFFER and load vertices into it
    GLfloat cube_vertices_container[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,   // top left

        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   // top left

       -0.5f,  0.5f,-0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 1.0f,   // top right
       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        0.5f,  0.5f,-0.5f,   1.0f, 0.0f, 0.5f,   0.0f, 1.0f,   // top left

       -0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
       -0.5f,  0.5f,-0.5f,   1.0f, 0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
        0.5f,  0.5f,-0.5f,   1.0f, 0.0f, 0.5f,   0.0f, 0.0f,   // bottom left
        0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   // top left

        0.5f,  0.5f,-0.5f,   1.0f, 0.0f, 0.5f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f,-0.5f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
        0.5f, -0.5f, 0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        0.5f,  0.5f, 0.5f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   // top left

       -0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top right
       -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
       -0.5f, -0.5f,-0.5f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f,-0.5f,   1.0f, 0.5f, 0.0f,   0.0f, 1.0f,   // top left
    };

    GLfloat piramid_vertices_container[] = {
        -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top
        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left

        -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top
        0.0f, 0.0f,-0.5f,  0.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
        0.0f, 0.5f, 0.0f,  1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left

        0.0f, 0.0f,-0.5f,  1.0f, 0.5f, 0.0f,   1.0f, 1.0f,   // top
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f,   // bottom right
        0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // bottom left

        -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,   1.0f, 1.0f,   // top
        0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
        0.0f, 0.0f,-0.5f, 1.0f, 0.0f, 0.5f,   0.0f, 0.0f,   // bottom left
    };
    GLfloat vertices_triangle[] = {
        0.9f,  0.9f, 0.0f,
        0.7f,  0.9f, 0.0f,
        0.8f,  0.7f, 0.0f
    };
    GLuint cube_indices_container[] = {  // note that we start from 0!
        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11,

        12, 13, 14,
        12, 14, 15,

        16, 17, 18,
        16, 18, 19,

        20, 21, 22,
        20, 22, 23,
    };
    GLuint piramid_indices_container[] = {  // note that we start from 0!
        0, 1, 2,

        3, 4, 5,

        6, 7, 8,

        9, 10, 11,
    };
    GLuint indices_triangle[] = {
        0, 1, 2
    };

    GLuint VBO;
    GLuint EBO;

    // Create VAO ids
    glGenVertexArrays(1, &m_vao_container_id);
    glGenVertexArrays(1, &m_vao_piramids_id);
    glGenVertexArrays(1, &m_vao_triangle_id);

    // Fill data for the container
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind container VAO to store all buffer settings related to container object
    glBindVertexArray(m_vao_container_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices_container), cube_vertices_container, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices_container), cube_indices_container, GL_STATIC_DRAW);
    // Configure how OpenGL will interpret the VBO data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0); // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind current VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind current EBO

    // Fill data for the piramid
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind container VAO to store all buffer settings related to container object
    glBindVertexArray(m_vao_piramids_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(piramid_vertices_container), piramid_vertices_container, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(piramid_indices_container), piramid_indices_container, GL_STATIC_DRAW);
    // Configure how OpenGL will interpret the VBO data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0); // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind current VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind current EBO

    // Fill data for the triangle
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(m_vao_triangle_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_triangle), vertices_triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_triangle), indices_triangle, GL_STATIC_DRAW);
    // Configure how OpenGL will interpret the VBO data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // Unbind VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind current VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Unbind current EBO

    // Prepare textures
    QImage bg(":/img/container.jpg");
    bg = bg.convertToFormat(QImage::Format_RGB888);

    glGenTextures(1, &m_texture_bg_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_bg_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bg.width(), bg.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, bg.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    QImage face(":/img/awesomeface.jpg");
    face = face.convertToFormat(QImage::Format_RGB888).mirrored(false, true);

    glGenTextures(1, &m_texture_smile_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_smile_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, face.width(), face.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, face.bits());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Prepare shader programms
    m_prog_container.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource_container);
    m_prog_container.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource_container);
    m_prog_container.link();

    m_prog_triangle.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource_triangle);
    m_prog_triangle.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource_triangle);
    m_prog_triangle.link();

    m_prog_piramids.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource_container);
    m_prog_piramids.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource_container);
    m_prog_piramids.link();

    // Obtain uniform ids
    m_triangle_color_id = m_prog_triangle.uniformLocation("color");
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    QTime cur_t = QTime::currentTime();
    int t = cur_t.second() * 1000 + cur_t.msec();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_bg_id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture_smile_id);

    QMatrix4x4 model;
    QMatrix4x4 view;
    QMatrix4x4 projection;

    view.lookAt(camera_pos, camera_pos + camera_front, camera_up);
    projection.perspective(45.0f, width() / height(), 0.1f, 100.0f);

    glBindVertexArray(m_vao_container_id);
    m_prog_container.bind();
    m_prog_container.setUniformValue("ourBg", 0);
    m_prog_container.setUniformValue("ourFace", 1);
    m_prog_container.setUniformValue("view", view);
    m_prog_container.setUniformValue("projection", projection);

        model.setToIdentity();
        model.translate(cubePosition);
        model.rotate(m_xRot / 16.0f, 1, 0, 0);
        model.rotate(m_yRot / 16.0f, 0, 1, 0);
        model.rotate(m_zRot / 16.0f, 0, 0, 1);
        m_prog_container.setUniformValue("model", model);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glBindVertexArray(m_vao_piramids_id);
    m_prog_piramids.bind();
    m_prog_piramids.setUniformValue("ourBg", 0);
    m_prog_piramids.setUniformValue("view", view);
    m_prog_piramids.setUniformValue("projection", projection);

        model.setToIdentity();
        model.translate(piramidPosition);
        model.rotate(-(t / 10) % 360, QVector3D(1.0f, 0.0f, 1.0f));
        m_prog_piramids.setUniformValue("model", model);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    // Calculate color for the second triangle
    GLfloat r = sin(t     * 3.14f / 11000.0f) / 2.0f + 0.5f;
    GLfloat g = sin(t * 3 * 3.14f / 17000.0f) / 2.0f + 0.5f;
    GLfloat b = sin(t * 7 * 3.14f / 13000.0f) / 2.0f + 0.5f;

    glBindVertexArray(m_vao_triangle_id);
    m_prog_triangle.bind();
    m_prog_triangle.setUniformValue(m_triangle_color_id, r, g, b, 1.0f);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
}
