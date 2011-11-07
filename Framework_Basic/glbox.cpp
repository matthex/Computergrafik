//
// GLBox
//
// Widget to display an OpenGL scene. The scene is bounded by the [-1,1]^3 volume.
//
// Author: Peter Salz, TU Kaiserslautern, salz@rhrk.uni-kl.de
// Based on the original work by Burkhard Lehner <lehner@informatik.uni-kl.de> and Gerd Reis.

#include <math.h> // for sqrt

#include "glbox.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDebug>
#include "matrix.h"

GLBox::GLBox( QWidget* parent, const QGLWidget* shareWidget )
	: QGLWidget( parent,  shareWidget )
{
    scale = 1.0;
    m_texID = 0;
    m_winWidth = 600;
    m_winHeight = 600;
    // Initialize the texture buffer.
    m_buffer = new unsigned char[3*TEX_RES];
    /*
    // Set the timeout to 50 milliseconds, corresponding to 20 FPS.
    m_timeout = 50; // 50 msecs
    m_timer = new QTimer(this);
    // Connect the timeout signal of the timer to the animate slot.
    connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
    // Start the timer.
    m_timer->start(m_timeout);
    m_elapsed = 0;
    */


    //--- Blatt 1 - Aufgabe 3 ---
    //Text Matrix Mult.
    Matrix<double, 4> mat1;
    mat1(0,0) = -1;
    mat1(0,1) = -1;
    mat1(0,2) = 0;
    mat1(0,3) = 0;
    mat1(1,0) = -2;
    mat1(1,1) = -3;
    mat1(1,2) = 1;
    mat1(1,3) = 1;
    mat1(2,0) = 4;
    mat1(2,1) = -1;
    mat1(2,2) = -2;
    mat1(2,3) = 2;
    mat1(3,0) = 0;
    mat1(3,1) = 0;
    mat1(3,2) = 2;
    mat1(3,3) = 1;

    qDebug() << "Matrix 1"; qDebug() << mat1(0,0) << " " << mat1(0,1) << " " << mat1(0,2) << " " << mat1(0,3);
    qDebug() << mat1(1,0) << " " << mat1(1,1) << " " << mat1(1,2) << " " << mat1(1,3);
    qDebug() << mat1(2,0) << " " << mat1(2,1) << " " << mat1(2,2) << " " << mat1(2,3);
    qDebug() << mat1(3,0) << " " << mat1(3,1) << " " << mat1(3,2) << " " << mat1(3,3);

    Matrix<double, 4> mat2;
    bool singular;
    mat2 = mat1.inverse(singular);

    qDebug() << "Matrix 1 Invers"; qDebug() << mat2(0,0) << " " << mat2(0,1) << " " << mat2(0,2) << " " << mat2(0,3);
    qDebug() << mat2(1,0) << " " << mat2(1,1) << " " << mat2(1,2) << " " << mat2(1,3);
    qDebug() << mat2(2,0) << " " << mat2(2,1) << " " << mat2(2,2) << " " << mat2(2,3);
    qDebug() << mat2(3,0) << " " << mat2(3,1) << " " << mat2(3,2) << " " << mat2(3,3);

    Matrix<double, 4> mat3;
    mat3 = mat1.operator *(mat2);

    qDebug() << "Matrix 1 * Matrix 1 Invers"; qDebug() << mat3(0,0) << " " << mat3(0,1) << " " << mat3(0,2) << " " << mat3(0,3);
    qDebug() << mat3(1,0) << " " << mat3(1,1) << " " << mat3(1,2) << " " << mat3(1,3);
    qDebug() << mat3(2,0) << " " << mat3(2,1) << " " << mat3(2,2) << " " << mat3(2,3);
    qDebug() << mat3(3,0) << " " << mat3(3,1) << " " << mat3(3,2) << " " << mat3(3,3);

    //--- End Blatt 1 Aufgabe 3 Matrix Mult. Test---
}


GLBox::~GLBox()
{
    delete [] m_buffer;
    m_buffer = NULL;
}

void GLBox::manageTexture()
{
    glEnable(GL_TEXTURE_2D);

    if (m_texID == 0)
        glGenTextures(1, &m_texID);

    glBindTexture(GL_TEXTURE_2D, m_texID);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_RES_X, TEX_RES_Y, 0, GL_RGB, GL_UNSIGNED_BYTE, m_buffer);

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);
}

void GLBox::clearImage(Color c)
{
    for (int i = 0; i < TEX_RES; i++)
    {
        m_buffer[3*i  ] = (unsigned char)(255.0*c.r);
        m_buffer[3*i+1] = (unsigned char)(255.0*c.g);
        m_buffer[3*i+2] = (unsigned char)(255.0*c.b);
    }
}

void GLBox::setPoint(Point2D p, Color c)
{
    // Transform coordinates from [-TEX_HALF,TEX_HALF] to [0, TEX_RES]
    int x = p.x + TEX_HALF_X;
    int y = p.y + TEX_HALF_Y;
    if (x < 0 || y < 0 || x >= TEX_RES_X || y >= TEX_RES_Y)
    {
        qDebug() << "Illegal point coordinates (" << p.x << "," << p.y << ")";
        return;
    }

    m_buffer[3*TO_LINEAR(x,y)  ] = (unsigned char)(255.0*c.r);
    m_buffer[3*TO_LINEAR(x,y)+1] = (unsigned char)(255.0*c.g);
    m_buffer[3*TO_LINEAR(x,y)+2] = (unsigned char)(255.0*c.b);
}

void GLBox::bresenhamLine(Point2D p1, Point2D p2, Color color)
{

}

void GLBox::bresenhamCircle(Point2D center, int radius, Color color)
{

}


void GLBox::initializeGL()
{
    // this method is called exactly once on program start
    clearImage();

    glViewport(0, 0, m_winWidth, m_winHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-m_winWidth/2, m_winWidth/2, -m_winHeight/2, m_winHeight/2, 0, 1);

    glClear (GL_COLOR_BUFFER_BIT);

}


void GLBox::resizeGL(int w, int h)
{
    m_winWidth = w;
    m_winHeight = h;

    glViewport(0,0,w,h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w/2, w/2, -h/2, h/2, 0, 1);

    updateGL();
}

void GLBox::paintGL()
{
    // this method draws the scene into the OpenGL widget
    // usually you do not call this method directly, instead call updateGL(), which in turn calls paintGL()

    clearImage(Color(1.0, 1.0, 1.0));

    Color red(1.0, 0.0, 0.0);
    Color blue(0.0, 0.0, 1.0);

    Point2D p1(0, 0);
    Point2D p2(-10, 10);
    setPoint(p1, red);
    setPoint(p2, red);

    Point2D center(20,20);
    setPoint(center, blue);

    manageTexture();

    glClear( GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, m_texID);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2i(0, 0);
        glVertex2i(-m_winWidth/2, -m_winHeight/2);
        glTexCoord2i(1, 0);
        glVertex2i( m_winWidth/2, -m_winHeight/2);
        glTexCoord2i(1, 1);
        glVertex2i( m_winWidth/2,  m_winHeight/2);
        glTexCoord2i(0, 1);
        glVertex2i(-m_winWidth/2, m_winHeight/2);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    // perform all output operations
    glFlush();
}

void GLBox::animate()
{
    /*
    // At each timeout, increase the elapsed time until it reaches 100. Then it is set to zero and the hands of the clock are moved.
    m_elapsed = (m_elapsed + qobject_cast<QTimer*>(sender())->interval()) % 100;
    m_clock.update(m_elapsed);
    updateGL();
    */
}

void GLBox::mousePressEvent( QMouseEvent *e )
{
    // a mouse button was pressed

    // if we are in moving mode, store the current mouse position (in viewport coordinates)
    // for calculating the cursor movement later (see mouseMoveEvent)
    x0 = e->x();
    y0 = e->y();
}

void GLBox::mouseMoveEvent( QMouseEvent *e )
{
    // mouse was moved while a mouse button was pressed (dragging)

    // so we are in moving mode
    // get the current cursor position (viewport coordinates)
    int x = e->x();
    int y = e->y();

    // calculate the difference to the last cursor position, relative to the dimension of the viewport
    double dx = (x - x0) / height();
    double dy = (y - y0) / height();
    // calculate the difference to the center of the viewport, relative to the dimension of the viewport
    double rx = (x - .5 * width()) / height();
    double ry = (y - .5 * height()) / height();
    // store the cursor position for the next move event
    x0 = x;
    y0 = y;

    int state = e->buttons ();
    // check for left mouse button => rotation
    if ((state & Qt::LeftButton) != 0)
    {
        double dist = sqrt(dx*dx + dy*dy);
        Vec4d axis(dy, dx, 4.0*(ry*dx - rx*dy), 0.0);
    }

    // check for right mouse button => translation
    if ((state & Qt::RightButton) != 0)
    {
        Vec4d trans(-dx*1.5*scale, dy*1.5*scale, 0.0, 0.0);
    }

    // repaint the scene
    updateGL();
}

void GLBox::mouseReleaseEvent( QMouseEvent * )
{
    // nothing to be done here
}

void GLBox::wheelEvent (QWheelEvent *e)
{
    // turning the mouse wheel will change the scale and therefore zoom in and out
    double dist = e->delta() / 120.0;  // one wheel "tick" counts for 120
    //scale *= exp (dist * log (1.05));
    updateGL();
}

void GLBox::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();
    qDebug() << "keyPressEvent()";

    e->accept();
    updateGL();
}
