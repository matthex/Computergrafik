//
// GLBox
//
// Description: widget class to display a scene of geometric objects, using OpenGL.
//
// Author: Peter Salz, TU Kaiserslautern, salz@rhrk.uni-kl.de
// Based on the work by Burkhard Lehner <lehner@informatik.uni-kl.de> and Gerd Reis.
//

#ifndef _GLBOX_H_
#define _GLBOX_H_

#include <QtOpenGL>

#include <QMouseEvent>
#include <QWheelEvent>
#include <vector>
#include <QString>
#include <QTimer>
#include "vector.h"
#include "matrix.h"
#include "Color.h"
#include "Point2D.h"

// Texture resolution
#define TEX_RES_X 60
#define TEX_RES_Y 60

// Number of pixels in the texture
#define TEX_RES TEX_RES_X*TEX_RES_Y

// Length of axes (axes run from -TEX_HALF to +TEX_HALF).
#define TEX_HALF_X TEX_RES_X/2
#define TEX_HALF_Y TEX_RES_Y/2

// Converts x,y coordinates to the position in a linear array.
#define TO_LINEAR(x, y) (((x)) + TEX_RES_X*((y)))

class GLBox : public QGLWidget
{
    Q_OBJECT

public:
    // constructor, creates an OpenGL widget as a subwidget of "parent"
    GLBox( QWidget* parent, const QGLWidget* shareWidget = 0 );

    // destructor, deletes all objects of the scene
    ~GLBox();

    // Draws a line from point p1 to point p2 with the given color using Bresenham's algorithm.
    void bresenhamLine(Point2D p1, Point2D p2, Color color = Color(0.0, 0.0, 0.0));

    // Draws a circle of the given radius around the center with the given color using Bresenham's algorithm.
    void bresenhamCircle(Point2D center, int radius, Color color = Color(0.0, 0.0, 0.0));

public slots:
    // Perform all computations necessary to animate the scene. Invoked by the timer.
    void animate();

protected:
    // Initialize the OpenGL setting.
    void initializeGL();

    // Draw the scene on the texture and map the texture to a quad that fills the whole viewport.
    void paintGL();

    // Resize the scene.
    void resizeGL(int w, int h);

    // Deal with the texture settings.
    void manageTexture();


    // Clear the texture to display only white pixels.
    void clearImage(Color c = Color());

    // Draw a point with the given color into the texture.
    // Note that the coordinate range for the point is [-TEX_HALF, TEXHALF].
    void setPoint(Point2D p, Color c = Color(0.0, 0.0, 0.0));

    // methods to deal with events from the mouse and the mouse wheel

    // Invoked when the mouse is moved.
    void mouseMoveEvent (QMouseEvent *);

    // Invoked when a mouse button is pressed.
    void mousePressEvent (QMouseEvent *);

    // Invoked when the mouse is released.
    void mouseReleaseEvent (QMouseEvent *);

    // Invoked when the mouse wheel is moved.
    void wheelEvent (QWheelEvent *);

    // Invoked when a key is pressed.
    void keyPressEvent(QKeyEvent *);

private:
    double scale;   // zoom factor

    // value of the last mouse cursor position, needed for rotating, translating, etc.
    double x0, y0;

    unsigned char *m_buffer; // Global texture memory.
    int m_winWidth; // Window width
    int m_winHeight; // Window height
    GLuint m_texID; // Texture ID for OpenGL

    /*
    unsigned int m_timeout; // Timeout of the animation timer in milliseconds
    int m_elapsed;  // Elapsed time during animation.
    QTimer *m_timer; // Timer object
    */
};

#endif // _GLBOX_H_
