# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterordner relativ zum Projektordner: .
# Das Target ist eine Anwendung:  

FORMS += ui/MainWindowBase.ui

HEADERS += \
           glbox.h \
           MainWindow.h \  
    Color.h \
    Point2D.h \
    vector.h \
    matrix.h

SOURCES += glbox.cpp \
           main.cpp \
           MainWindow.cpp   

INCLUDEPATH += . ui /usr/include /usr/local/include

QMAKE_CXXFLAGS_RELEASE += -Wno-non-virtual-dtor 
QMAKE_CXXFLAGS_DEBUG += -Wno-non-virtual-dtor 
CONFIG += opengl \
	debug \
        warn_on \
        qt \

QT += opengl	

MOC_DIR = moc
UI_DIR = ui
OBJECTS_DIR = obj


TEMPLATE = app 

#The following line was inserted by qt3to4
QT +=  opengl 
