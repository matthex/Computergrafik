//
// C++ Interface: BasicViewer
//
// Description: main function
//
// Author: Burkhard Lehner <lehner@informatik.uni-kl.de>, (C) 2006

#include <qapplication.h>
#include "MainWindow.h"

int main( int argc, char** argv )
{
    // always the first thing to do in a Qt application: create a QApplication object
    QApplication app( argc, argv );

    // check for OpenGL support
    if ( !QGLFormat::hasOpenGL() )
    {
            qWarning( "This system has no OpenGL support. Exiting." );
            return -1;
    }

    // create the main window
    MainWindow main;
    // set it as the main widget (so closing the window exits the program)
    app.setActiveWindow(&main);

    // show the main window on the screen
    main.show();

    // and start waiting for events
    return app.exec();
}
