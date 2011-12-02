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
    // Set the timeout to 50 milliseconds, corresponding to 20 FPS.
    m_timeout = 50; // 50 msecs
    m_timer = new QTimer(this);
    // Connect the timeout signal of the timer to the animate slot.
    connect(m_timer, SIGNAL(timeout()), this, SLOT(animate()));
    // Start the timer.
    m_timer->start(m_timeout);
    //Set the clock
    m_clock = Clock(TEX_HALF_X, TEX_HALF_Y, Vec3d(50,50,1), 50, Vec3d(-0.5,-0.9,1));
    m_elapsed = 0;
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
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

void GLBox::bresenhamLine(Vec3d v1, Vec3d v2, Color color)
{
    int x1, y1, x2, y2, d;
    if(v2(0) > v1(0)) {   //switch points if p2 is left of p1
        x1 = (int)round(v1(0));
        y1 = (int)round(v1(1));
        x2 = (int)round(v2(0));
        y2 = (int)round(v2(1));
    } else {
        x2 = (int)round(v1(0));
        y2 = (int)round(v1(1));
        x1 = (int)round(v2(0));
        y1 = (int)round(v2(1));
    }
    int x = x1;
    int y = y1;
    int deltaX = x2-x1;
    int deltaY = y2-y1;
    //int deltaNE = 2*(deltaY-deltaX);
    //int deltaE = 2*deltaY;
    setPoint(Point2D(x,y), color);

    //differentiation of gradient
    if(deltaY >= 0){    //m >= 0
        if(deltaY <= deltaX){   //m <= 1
            //Case 1: Octant 1 (5)
            d = 2*deltaY-deltaX;
            while(x < x2){
                if(d >= 0){
                    d += 2*(deltaY-deltaX);
                    x++;
                    y++;
                } else {
                    d += 2*deltaY;
                    x++;
                }
                setPoint(Point2D(x,y), color);
            }
        } else {    //m > 1
            //Case 2: Octant 2 (6)          [swap x,y]
            d = 2*deltaX-deltaY;
            while(y < y2){
                if(d >= 0){
                    d += 2*(deltaX-deltaY);
                    y++;
                    x++;
                } else {
                    d += 2*deltaX;
                    y++;
                }
                setPoint(Point2D(x,y), color);
            }
        }
    } else {    //m < 0
        if(-deltaY <= deltaX){   //m >= -1
            //Case 3: Octant 3 (7)          [swap sign]
            d = -2*deltaY-deltaX;
            while(x < x2){
                if(d >= 0){
                    d += 2*(-deltaY-deltaX);
                    x++;
                    y--;
                } else {
                    d -= 2*deltaY;
                    x++;
                }
                setPoint(Point2D(x,y), color);
            }
        } else {    //m < -1
            //Case 4: Octant 4 (8)          [swap x,y]
            d = 2*deltaX+deltaY;
            while(-y < -y2){
                if(d >= 0){
                    d += 2*(deltaX+deltaY);
                    y--;
                    x++;
                } else {
                    d += 2*deltaX;
                    y--;
                }
                setPoint(Point2D(x,y), color);
            }
        }
    }
}

void GLBox::bresenhamCircle(Vec3d center, int radius, Color color)
{
    int x1 = (int)round(center(0));
    int y1 = (int)round(center(1));
    int x = 0;
    int y = radius;
    int d = 5-4*radius;
    int deltaSE;
    int deltaE;
    setPoint(Point2D(x1,y1+radius), color); //y
    setPoint(Point2D(x1,y1-radius), color); //-y
    setPoint(Point2D(x1+radius,y1), color); //x
    setPoint(Point2D(x1-radius,y1), color); //-x
    while(y > x){
        if(d >= 0) {    //SE
            deltaSE = 4*(2*(x-y)+5);
            d += deltaSE;
            x++;
            y--;
        } else {    //E
            deltaE = 4*(2*x+3);
            d += deltaE;
            x++;
        }
        setPoint(Point2D(x1+y,y1+x), color); //1. O.
        setPoint(Point2D(x1+x,y1+y), color); //2. O.
        setPoint(Point2D(x1-x,y1+y), color); //3. O.
        setPoint(Point2D(x1-y,y1+x), color); //4. O.
        setPoint(Point2D(x1-y,y1-x), color); //5. O.
        setPoint(Point2D(x1-x,y1-y), color); //6. O.
        setPoint(Point2D(x1+x,y1-y), color); //7. O.
        setPoint(Point2D(x1+y,y1-x), color); //8. O.
    }
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

    Color black(0.0, 0.0, 0.0);
    Color grey(0.3, 0.3, 0.3);

    //Static clock
    bresenhamCircle(m_clock.getCenter(), m_clock.getRadius(), black);
    bresenhamLine(m_clock.getCenter(), m_clock.getLonghand(), black);
    bresenhamLine(m_clock.getCenter(), m_clock.getShorthand(), grey);


    //-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\\

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
    // At each timeout, increase the elapsed time until it reaches 100. Then it is set to zero and the hands of the clock are moved.
    m_elapsed = (m_elapsed + qobject_cast<QTimer*>(sender())->interval()) % 100;
    m_clock.update(m_elapsed);
    updateGL();
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
