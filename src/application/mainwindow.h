#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QImage>
#include <QLabel>
#include <QWidget>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "imageconversion.h"
#include "utils.h"

#include "StripePattern.h"
#include "StripePatternAnalyzer.h"
#include "DepthAnalyzer.h"
#include "3DScannerUtil.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private slots:
        void startShowing();
        void startShowingOld();
        void about();

    private:
        Ui::MainWindow * ui;

        void createActions();
        void createMenus();
        void createToolBars();
        void createStatusBar();
        void createLayout();

        int getInt(QString title, QString label, int value, int min, int max, int step);
        double getDouble(QString title, QString label, double value, double min, double max, double decimal);
        QString getText(QString title, QString label);

        void CONSOLE(QString text);

        QWidget * topWidget;

        QTextEdit * console;

        QMenu * fileMenu;
        QMenu * helpMenu;

        QAction * startShowingAct;
        QAction * aboutAct;

        QLineEdit * parameter_1;
        QLineEdit * parameter_2;

        QLabel * left_image;
        QLabel * right_image;

        QWidget * secondary;

        DepthAnalyzer * analyzer;
        IplImage * foregroundTextureImage;

        bool smooth;
        bool accurate;
};

#endif // MAINWINDOW_H
