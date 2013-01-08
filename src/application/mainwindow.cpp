#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageconversion.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->createActions();
    this->createMenus();
    this->createToolBars();
    this->createStatusBar();
    this->createLayout();

    CONSOLE(tr("Program został uruchomiony poprawnie."));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    startShowingAct = new QAction(tr("&Uruchom"), this);
    connect(startShowingAct, SIGNAL(triggered()), this, SLOT(startShowing()));

    aboutAct = new QAction(tr("&O programie"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Plik"));
    fileMenu->addAction(startShowingAct);

    helpMenu = menuBar()->addMenu(tr("P&omoc"));
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    parameter_1 = new QLineEdit();
    parameter_2 = new QLineEdit();

    ui->mainToolBar->addWidget(new QLabel(tr("Parametr 1: ")));
    ui->mainToolBar->addWidget(parameter_1);

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addWidget(new QLabel(tr("Parametr 2: ")));
    ui->mainToolBar->addWidget(parameter_2);

    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(startShowingAct);
}

void MainWindow::createStatusBar()
{

}

void MainWindow::createLayout()
{
    QVBoxLayout * mainLayout = new QVBoxLayout;

    topWidget = new QWidget();
    QHBoxLayout * topLayout = new QHBoxLayout;
    topLayout->addWidget(new QLabel(tr("Obraz wyświetlany przez projektor:")));
    left_image = new QLabel();
    topLayout->addWidget(left_image);
    topLayout->addWidget(new QLabel(tr("Obraz pobrany z kamery:")));
    right_image = new QLabel();
    topLayout->addWidget(right_image);
    topWidget->setLayout(topLayout);

    console = new QTextEdit();
    console->setReadOnly(true);

    mainLayout->addWidget(topWidget);
    mainLayout->addWidget(new QLabel(tr("Komunikaty aplikacji:")));
    mainLayout->addWidget(console);

    ui->centralWidget->setLayout(mainLayout);
}

void MainWindow::CONSOLE(QString text)
{
    console->append("> " + text);
}

void MainWindow::startShowing()
{
    CONSOLE(tr("Uruchamiam wyświetlanie obrazów poprzez projektor..."));
    CONSOLE(tr("Pobrane parametry:"));
    CONSOLE(tr("\tParametr 1:\t") + parameter_1->text());
    CONSOLE(tr("\tParametr 2:\t") + parameter_2->text());
    CONSOLE(tr("Uruchamiam odczytywanie obrazów z kamery...\n\n"));

    int height, width, step, channels;
    uchar * data;

    IplImage * img = cvLoadImage("/home/proz/mila.jpg");

    if(!img)
    {
        CONSOLE(tr("Nie moge zaladowac obrazu. \n"));
        return ;
    }

    // a se cos z niego przeczytamy
    height    = img->height;
    width     = img->width;
    step      = img->widthStep;
    channels  = img->nChannels;
    data      = (uchar *)img->imageData;

    CONSOLE(tr("Pracuje nad obrazem o rozmiarze ") + QString::number(height) + tr("x") + QString::number(width) + tr(" z ") + QString::number(channels) + tr(" kanalami\n"));

    CONSOLE(tr("Dokonuje konwersji z IplImage na QImage\n"));

    QImage qimg = ImageConversion::IplImage2QImage(img);

    CONSOLE(tr("Konwersja z IplImage na QImage przeprowadzona poprawnie!\n"));
    CONSOLE(tr("Wyswietlam wczytany i przekonwertowany obraz\n"));

    left_image->setPixmap(QPixmap::fromImage(qimg));
    left_image->show();

    CONSOLE(tr("Obraz zostal wyswietlony poprawnie!\n"));

    CONSOLE(tr("Dokonuje konwersji z QImage na IplImage\n"));

    IplImage * img_conv = ImageConversion::qImage2IplImage(qimg);

    if(!img_conv)
    {
        CONSOLE(tr("Nie moge zaladowac przekonwertowanego obrazu. \n"));
        return ;
    }
    else
    {
        CONSOLE(tr("Konwersja z QImage na IplImage przeprowadzona poprawnie!\n"));
    }

    // a se cos z niego tez przeczytamy
    height    = img_conv->height;
    width     = img_conv->width;
    step      = img_conv->widthStep;
    channels  = img_conv->nChannels;
    data      = (uchar *)img_conv->imageData;

    CONSOLE(tr("Pracuje nad przekonwertowanym obrazem o rozmiarze ") + QString::number(height) + tr("x") + QString::number(width) + tr(" z ") + QString::number(channels) + tr(" kanalami\n"));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("O programie"),
            tr("Program realizowany jest w ramach projektu z przedmiotu <b>SWIZ</b>."));
}
