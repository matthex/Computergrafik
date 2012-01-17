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
    m_focus = 1;
    m_cam = Camera();
    //Initialize the cuboids and spheres
    //initializeCuboids();
    m_sphereCount = 7;
    m_spheres.resize(m_sphereCount);
    m_spheres[0] = new sphere(Color(1,1,0), Vec4d(0,0,0,1), 0.15);
        m_spheres[1] = new sphere(Color(0,1,0), Vec4d(0.5,0,0,1), 0.1);
            m_spheres[2] = new sphere(Color(0,0.8,0), Vec4d(0.7,0,0,1), 0.05);
        m_spheres[3] = new sphere(Color(0,1,1), Vec4d(-0.2,-0.2,-0.2,1), 0.1);
            m_spheres[4] = new sphere(Color(0,0.8,0.8), Vec4d(-0.4,-0.2,-0.2,1), 0.05);
            m_spheres[5] = new sphere(Color(0,0.7,0.7), Vec4d(-0.3,-0.6,-0.3,1), 0.05);
                m_spheres[6] = new sphere(Color(0,0.5,0.5), Vec4d(-0.2,-0.5,-0.2,1), 0.01);

    angle2 = 0.1;
    sphereRotAxis = Vec4d(0,-1,1,1);
    sphereRotAxis = sphereRotAxis.normH();
    sphereRotAxis2 = Vec4d(0,-1,-0.5,1);
    sphereRotAxis2 = sphereRotAxis2.normH();
    sphereRotAxis3 = Vec4d(1,-1,2,1);
    sphereRotAxis3 = sphereRotAxis3.normH();
    sphereRotAxis4 = Vec4d(0,-0.5,0.1,1);
    sphereRotAxis4 = sphereRotAxis4.normH();
    sphereRotAxis5 = Vec4d(-0.6,0.1,0.2,1);
    sphereRotAxis5 = sphereRotAxis5.normH();
    sphereRotAxis6 = Vec4d(0.1,-0.2,0.1,1);
    sphereRotAxis6 = sphereRotAxis5.normH();

    m_matrices.resize(m_sphereCount);
}


GLBox::~GLBox()
{
    for(int i=0; i<m_spheres.size(); i++)
    {
        delete m_spheres[i];
    }
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
        //qDebug() << "Illegal point coordinates (" << p.x << "," << p.y << ")";
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

//    raycast();
    updateGL();
}

void GLBox::paintGL()
{
    // this method draws the scene into the OpenGL widget
    // usually you do not call this method directly, instead call updateGL(), which in turn calls paintGL()

//    clearImage(Color(1.0, 1.0, 1.0));

//    Color black(0.0, 0.0, 0.0);
//    Color grey(0.3, 0.3, 0.3);

//    //Clock
////    bresenhamCircle(m_clock.getCenter(), m_clock.getRadius(), black);
////    bresenhamLine(m_clock.getCenter(), m_clock.getLonghand(), black);
////    bresenhamLine(m_clock.getCenter(), m_clock.getShorthand(), grey);

//    //Cuboids
//    makeCuboid(m_cub1);
//    makeCuboid(m_cub2);
//    makeCuboid(m_cub3);

//    //Spheres
//    makeSphere(m_sphere1);
//    makeSphere(m_sphere2);

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
    //m_clock.update(m_elapsed);

    //Animate cuboids
//    transVec = Vec4d(-0.003,0.002,0.002,1);
//    rotAxis = Vec4d(1,1,1,1);
//    angle1 = M_PI/12;
//    for(int i=0; i<8; i++)
//    {
//        m_cub3[i] = cubTransMat.makeTransMat(transVec)*m_cub3[i];
//        //m_cub2[i] = cubRotMat.makeRotMat(angle1,rotAxis)*m_cub2[i]; //nur normale oder Punktrotation!
//        m_cub1[i] = cubRotMat.makeRotMatPoint(angle1,rotAxis,m_cub1[1])*m_cub1[i];
//    }

    //Animate spheres
    m_matrices[1] = sphereTransMat.makeRotMatPoint(angle2, sphereRotAxis, m_spheres[0]->getCenter());
    m_matrices[3] = sphereTransMat.makeRotMatPoint(angle2, sphereRotAxis3, m_spheres[0]->getCenter());

    m_spheres[1]->setCenter(m_matrices[1] * m_spheres[1]->getCenter());

    m_spheres[2]->setCenter(m_matrices[1] * m_spheres[2]->getCenter());
    m_matrices[2] = sphereTransMat.makeRotMatPoint(angle2, sphereRotAxis2, m_spheres[1]->getCenter());
    m_spheres[2]->setCenter(m_matrices[2] * m_spheres[2]->getCenter());

    m_spheres[3]->setCenter(m_matrices[3] * m_spheres[3]->getCenter());

    m_spheres[4]->setCenter(m_matrices[3] * m_spheres[4]->getCenter());
    m_matrices[4] = sphereTransMat.makeRotMatPoint(angle2, sphereRotAxis4, m_spheres[3]->getCenter());
    m_spheres[4]->setCenter(m_matrices[4] * m_spheres[4]->getCenter());

    m_spheres[5]->setCenter(m_matrices[3] * m_spheres[5]->getCenter());
    m_matrices[5] = sphereTransMat.makeRotMatPoint(angle2, sphereRotAxis5, m_spheres[3]->getCenter());
    m_spheres[5]->setCenter(m_matrices[5] * m_spheres[5]->getCenter());

    m_spheres[6]->setCenter(m_matrices[3] * m_spheres[6]->getCenter());
    m_spheres[6]->setCenter(m_matrices[5] * m_spheres[6]->getCenter());
    m_matrices[6] = sphereTransMat.makeRotMatPoint(angle2, sphereRotAxis6, m_spheres[5]->getCenter());
    m_spheres[6]->setCenter(m_matrices[6] * m_spheres[6]->getCenter());

    raycast();
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

        Mat4d camRot;
        m_cam.setEyePoint(camRot.makeRotMatPoint(dist,axis,Vec4d(0,0,0,1))*m_cam.getEyePoint());
        m_cam.setViewVec(Vec4d(-m_cam.getEyePoint()(0),-m_cam.getEyePoint()(1),-m_cam.getEyePoint()(2),1));
    }

    // check for right mouse button => translation
    if ((state & Qt::RightButton) != 0)
    {
        Vec4d trans(-dx*1.5*scale, dy*1.5*scale, 0.0, 0.0);

        Mat4d camTrans;
        m_cam.setEyePoint(camTrans.makeTransMat(trans)*m_cam.getEyePoint());
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
    scale *= exp (dist * log (1.05));

    Vec4d tempVec2 = tempVec;
    tempVec(0) = scale*m_cam.getViewVec()(0)-m_cam.getViewVec()(0);
    tempVec(1) = scale*m_cam.getViewVec()(1)-m_cam.getViewVec()(1);
    tempVec(2) = scale*m_cam.getViewVec()(2)-m_cam.getViewVec()(2);
    tempVec(3) = 1;
    m_cam.setEyePoint(m_cam.getEyePoint()+tempVec-tempVec2);

    updateGL();
}

void GLBox::keyPressEvent(QKeyEvent *e)
{
    int key = e->key();
    qDebug() << "keyPressEvent()";

    e->accept();
    updateGL();
}

void GLBox::initializeCuboids()
{
    //Creation of cuboid 1
    Vec4d cub1p1;
    cub1p1(0) = -0.4;
    cub1p1(1) = 0.2;
    cub1p1(2) = -0.5;
    cub1p1(3) = 1.0;
    m_cub1[0] = cub1p1;
    Vec4d cub1p2;
    cub1p2(0) = 0.5;
    cub1p2(1) = 0.2;
    cub1p2(2) = -0.5;
    cub1p2(3) = 1.0;
    m_cub1[1] = cub1p2;
    Vec4d cub1p3;
    cub1p3(0) = 0.5;
    cub1p3(1) = 0.6;
    cub1p3(2) = -0.5;
    cub1p3(3) = 1.0;
    m_cub1[2] = cub1p3;
    Vec4d cub1p4;
    cub1p4(0) = -0.4;
    cub1p4(1) = 0.6;
    cub1p4(2) = -0.5;
    cub1p4(3) = 1.0;
    m_cub1[3] = cub1p4;
    Vec4d cub1p5;
    cub1p5(0) = -0.4;
    cub1p5(1) = 0.2;
    cub1p5(2) = -0.3;
    cub1p5(3) = 1.0;
    m_cub1[4] = cub1p5;
    Vec4d cub1p6;
    cub1p6(0) = 0.5;
    cub1p6(1) = 0.2;
    cub1p6(2) = -0.3;
    cub1p6(3) = 1.0;
    m_cub1[5] = cub1p6;
    Vec4d cub1p7;
    cub1p7(0) = 0.5;
    cub1p7(1) = 0.6;
    cub1p7(2) = -0.3;
    cub1p7(3) = 1.0;
    m_cub1[6] = cub1p7;
    Vec4d cub1p8;
    cub1p8(0) = -0.4;
    cub1p8(1) = 0.6;
    cub1p8(2) = -0.3;
    cub1p8(3) = 1.0;
    m_cub1[7] = cub1p8;

    //Creation of cuboid 2
    Vec4d cub2p1;
    cub2p1(0) = 0.7;
    cub2p1(1) = 0.2;
    cub2p1(2) = -0.8;
    cub2p1(3) = 1.0;
    m_cub2[0] = cub2p1;
    Vec4d cub2p2;
    cub2p2(0) = 0.5;
    cub2p2(1) = 0.2;
    cub2p2(2) = -0.8;
    cub2p2(3) = 1.0;
    m_cub2[1] = cub2p2;
    Vec4d cub2p3;
    cub2p3(0) = 0.5;
    cub2p3(1) = -0.4;
    cub2p3(2) = -0.8;
    cub2p3(3) = 1.0;
    m_cub2[2] = cub2p3;
    Vec4d cub2p4;
    cub2p4(0) = 0.7;
    cub2p4(1) = -0.4;
    cub2p4(2) = -0.8;
    cub2p4(3) = 1.0;
    m_cub2[3] = cub2p4;
    Vec4d cub2p5;
    cub2p5(0) = 0.7;
    cub2p5(1) = 0.2;
    cub2p5(2) = -0.3;
    cub2p5(3) = 1.0;
    m_cub2[4] = cub2p5;
    Vec4d cub2p6;
    cub2p6(0) = 0.5;
    cub2p6(1) = 0.2;
    cub2p6(2) = -0.3;
    cub2p6(3) = 1.0;
    m_cub2[5] = cub2p6;
    Vec4d cub2p7;
    cub2p7(0) = 0.5;
    cub2p7(1) = -0.4;
    cub2p7(2) = -0.3;
    cub2p7(3) = 1.0;
    m_cub2[6] = cub2p7;
    Vec4d cub2p8;
    cub2p8(0) = 0.7;
    cub2p8(1) = -0.4;
    cub2p8(2) = -0.3;
    cub2p8(3) = 1.0;
    m_cub2[7] = cub2p8;

    //Creation of cuboid 3
    Vec4d cub3p1;
    cub3p1(0) = -0.2;
    cub3p1(1) = -0.7;
    cub3p1(2) = -0.5;
    cub3p1(3) = 1.0;
    m_cub3[0] = cub3p1;
    Vec4d cub3p2;
    cub3p2(0) = 0.5;
    cub3p2(1) = -0.7;
    cub3p2(2) = -0.5;
    cub3p2(3) = 1.0;
    m_cub3[1] = cub3p2;
    Vec4d cub3p3;
    cub3p3(0) = 0.5;
    cub3p3(1) =  -0.9;
    cub3p3(2) = -0.5;
    cub3p3(3) = 1.0;
    m_cub3[2] = cub3p3;
    Vec4d cub3p4;
    cub3p4(0) = -0.2;
    cub3p4(1) =  -0.9;
    cub3p4(2) = -0.5;
    cub3p4(3) = 1.0;
    m_cub3[3] = cub3p4;
    Vec4d cub3p5;
    cub3p5(0) = -0.2;
    cub3p5(1) = -0.7;
    cub3p5(2) = -0.3;
    cub3p5(3) = 1.0;
    m_cub3[4] = cub3p5;
    Vec4d cub3p6;
    cub3p6(0) = 0.5;
    cub3p6(1) = -0.7;
    cub3p6(2) = -0.3;
    cub3p6(3) = 1.0;
    m_cub3[5] = cub3p6;
    Vec4d cub3p7;
    cub3p7(0) = 0.5;
    cub3p7(1) =  -0.9;
    cub3p7(2) = -0.3;
    cub3p7(3) = 1.0;
    m_cub3[6] = cub3p7;
    Vec4d cub3p8;
    cub3p8(0) = -0.2;
    cub3p8(1) =  -0.9;
    cub3p8(2) = -0.3;
    cub3p8(3) = 1.0;
    m_cub3[7] = cub3p8;
}

void GLBox::makeCuboid(Vec4d cub[8])
{
    Color black(0.0, 0.0, 0.0);

    //Calculate actual points
    for(int i=0; i<8; i++)
    {
        projectedVec = projectZ(cub[i], m_focus);
        cub2[i](0)=projectedVec(0)*double(TEX_HALF_X);
        cub2[i](1)=projectedVec(1)*double(TEX_HALF_Y);
        cub2[i](2)=projectedVec(2);
    }

    //Draw lines
    bresenhamLine(cub2[0], cub2[1], black);
    bresenhamLine(cub2[0], cub2[3], black);
    bresenhamLine(cub2[0], cub2[4], black);
    bresenhamLine(cub2[1], cub2[2], black);
    bresenhamLine(cub2[1], cub2[5], black);
    bresenhamLine(cub2[2], cub2[3], black);
    bresenhamLine(cub2[2], cub2[6], black);
    bresenhamLine(cub2[3], cub2[7], black);
    bresenhamLine(cub2[4], cub2[5], black);
    bresenhamLine(cub2[4], cub2[7], black);
    bresenhamLine(cub2[5], cub2[6], black);
    bresenhamLine(cub2[6], cub2[7], black);
}

Vec4d GLBox::projectZ(Vec4d &vec, double focus)
{
    if(focus==0) return Vec4d();

    //Camera
    Mat4d camMat = m_cam.makeTransMat();
    Vec4d camVec;
    camVec = camMat*vec;

    Mat4d projectMat;
    projectMat(0,0) = 1;
    projectMat(1,1) = 1;
    projectMat(3,3) = 1;
    projectMat(3,2) = -1/focus;

    Vec4d projectVec;
    projectVec = projectMat*camVec;

    //Normalize
    for(int i=0; i<4; i++)
    {
        projectVec(i) /= projectVec(3);
    }

    return projectVec;
}

void GLBox::setFocus(double focus)
{
    m_focus = focus;
    raycast();
    updateGL();
}

double GLBox::getFocus()
{
    return m_focus;
}

void GLBox::makeSphere(sphere sphere)
{
    Vec3d tempVec[sphere.points.size()];

    for (int i=0; i<sphere.points.size(); i++)
    {
        projectedVec = projectZ(sphere.points[i], m_focus);
        tempVec[i](0) = projectedVec(0)*double(TEX_HALF_X) + sphere.getCenter()(0);
        tempVec[i](1) = projectedVec(1)*double(TEX_HALF_Y) + sphere.getCenter()(1);
        tempVec[i](2) = projectedVec(2);
    }

    for(int i=0; i<sphere.points.size(); i++)
    {
        setPoint(Point2D(int(tempVec[i](0)), int(tempVec[i](1))), sphere.getColor());
    }
}

void GLBox::sortHits(std::vector<Vec3d> &hits, std::vector<int> &indices)
{
    //Bubble sort
    bool swapped = true;
    int temp;
    Vec3d temp2;

    int j=0;

    while (swapped)
    {
        swapped = false;
        j++;

        for(int i=0; i<hits.size()-j; i++)
        {
            if(hits[i](2) < hits[i+1](2))
            {
                temp = indices[i];
                indices[i] = indices[i+1];
                indices[i+1] = temp;

                temp2 = hits[i];
                hits[i] = hits[i+1];
                hits[i+1] = temp2;

                swapped = true;
            }
        }
    }
}

void GLBox::raycast()
{
    clearImage(Color(1.0, 1.0, 1.0));
    Vec3d eye(0, 0, m_focus);
    Vec3d pixel;
    Vec3d view;
    double x_min = -1;
    double x_max = 1;
    double y_min = -1;
    double y_max = 1;
    for(int x = 0; x < TEX_RES_X; x++)
    {
        for(int y = 0; y < TEX_RES_Y; y++)
        {
            pixel(0) = x_min + x * ((x_max - x_min) / (double) TEX_RES_X);
            pixel(1) = y_min + y * ((y_max - y_min) / (double) TEX_RES_Y);
            pixel(2) = 0;
            view = pixel - eye;
            view = view.norm();

            std::vector<Vec3d> hits(m_sphereCount);
            std::vector<int> indices(m_sphereCount);

            for(int i=0; i<m_sphereCount; i++)
            {
                sphere *curSph = m_spheres[i];
                hits[i] = curSph->intersect(eye, view);
                indices[i] = i;
            }

            sortHits(hits, indices);
            if(hits[0](2)!=-INFINITY)
            {
                sphere *curSph = m_spheres[indices[0]];
                setPoint(Point2D(x - TEX_HALF_X, y - TEX_HALF_Y), curSph->getColor());
            }

//            Vec3d s1 = m_sphere1.intersect(eye, view);
//            Vec3d s2 = m_sphere2.intersect(eye, view);
//            if(s1(2)>s2(2))
//            {
//                setPoint(Point2D(x - TEX_HALF_X, y - TEX_HALF_Y), m_sphere1.getColor());    //Draw Sphere 1
//            }
//            if(s2(2)>s1(2))
//            {
//                setPoint(Point2D(x - TEX_HALF_X, y - TEX_HALF_Y), m_sphere2.getColor());    //Draw Sphere 2
//            }
        }
    }
}
