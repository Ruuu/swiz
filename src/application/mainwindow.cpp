#include "mainwindow.h"
#include "ui_mainwindow.h"

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
    QWidget * w1 = new QWidget();
    topLayout->addWidget(w1);
    topLayout->addWidget(new QLabel(tr("Obraz pobrany z kamery:")));
    QWidget * w2 = new QWidget();
    topLayout->addWidget(w2);
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
    CONSOLE(tr("Uruchamiam odczytywanie obrazów z kamery..."));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("O programie"),
            tr("Program realizowany jest w ramach projektu z przedmiotu <b>SWIZ</b>."));
}
