#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QImage>
#include <QLabel>
#include <QWidget>

#include <opencv/cv.h>
#include <opencv/highgui.h>

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
        void about();

    private:
        Ui::MainWindow * ui;

        void createActions();
        void createMenus();
        void createToolBars();
        void createStatusBar();
        void createLayout();

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
};

#endif // MAINWINDOW_H
