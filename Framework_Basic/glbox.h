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
#include "clock.h"
#include "camera.h"
#include "sphere.h"
#include "light.h"
#include <QImage>

// Texture resolution
#define TEX_RES_X 400
#define TEX_RES_Y 400

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
    void bresenhamLine(Vec3d v1, Vec3d v2, Color color = Color(0.0, 0.0, 0.0));

    // Draws a circle of the given radius around the center with the given color using Bresenham's algorithm.
    void bresenhamCircle(Vec3d center, int radius, Color color = Color(0.0, 0.0, 0.0));

    //Set new focus
    void setFocus(double focus);

    //Get focus
    double getFocus();

    // Change phi rotation
    void setPhiRot(int phi);

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

    //Initialize cuboids
    void initializeCuboids();

    // Draw a cuboid
    void makeCuboid(Vec4d cub[8]);

    // Projection
    Vec4d projectZ(Vec4d &vec, double focus);

    // Draw sphere
    void makeSphere(sphere sph);

    // Ray casting
    void raycast();

    // Sort hit points
    void sortHits(std::vector<Vec3d> &hits, std::vector<int> &indices);

    // Phong shading
    Color phong(Vec3d hit, Vec3d eyePos, Vec3d normal, Light light, Material Material);

    // Shadow sensor
    bool isShadowed(sphere *sphere, Vec3d hit, Light light);

    // Load texture
    void loadTexture(QString filename);

    // Get texture color
    Color getTextureValue (double phi, double theta);

    // Round
    int round(double dnumber);

    // Get phi
    double getPhi(Vec3d point);

    // Get theta
    double getTheta(Vec3d point);


private:
    double scale;   // zoom factor

    // value of the last mouse cursor position, needed for rotating, translating, etc.
    double x0, y0;

    unsigned char *m_buffer; // Global texture memory.
    int m_winWidth; // Window width
    int m_winHeight; // Window height
    GLuint m_texID; // Texture ID for OpenGL

    unsigned int m_timeout; // Timeout of the animation timer in milliseconds
    Clock m_clock;  //Clock
    int m_elapsed;  // Elapsed time during animation.
    QTimer *m_timer; // Timer object

    double m_focus; //focus

    Vec4d m_cub1[8];
    Vec4d m_cub2[8];
    Vec4d m_cub3[8];

    Mat4d cubTransMat;
    Vec4d transVec;
    Mat4d cubRotMat;
    Vec4d rotAxis;
    double angle1;

    Vec3d cub2[8];
    Vec4d projectedVec;

    Camera m_cam;

    int m_sphereCount;
    sphere m_sphere1;
    sphere m_sphere2;
    sphere m_sphere3;
    sphere m_sphere4;
    sphere m_sphere5;
    sphere m_sphere6;

    Mat4d sphereTransMat;

    double angle2;

    Vec4d sphereRotAxis;
    Vec4d sphereRotAxis2;
    Vec4d sphereRotAxis3;
    Vec4d sphereRotAxis4;
    Vec4d sphereRotAxis5;
    Vec4d sphereRotAxis6;

    Vec4d tempVec;

    std::vector<sphere*> m_spheres;
    std::vector<Mat4d> m_matrices;

    Light m_light;

    QImage m_texture;

    int m_phiRot;
};

#endif // _GLBOX_H_
