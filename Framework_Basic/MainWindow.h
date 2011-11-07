//
// MainWindow
//
// Description: main window of the program, interface
//
// Author: Peter Salz, TU Kaiserslautern, salz@rhrk.uni-kl.de, 2011
// Based on the original work by Burkhard Lehner <Burkhard.Lehner@googlemail.com>, (C) 2006


#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "ui/ui_MainWindowBase.h"
#include "glbox.h"
#include <QMainWindow>
#include <QSlider>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        // constructor, creates the main window of the program
        MainWindow (QWidget *parent = 0, const char *name = 0);
        // returns the GLBox widget of the main window
        GLBox *getGLBox();

    public slots:
        // this slot processes the selection of the menu entry "Exit"
        void fileExit();

    private:
        GLBox *glbox;
	Ui::MainWindowBase ui;
};

#endif
