#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GeneratePaperWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnAddQuestion_clicked();
    void on_btnViewQuestionBank_clicked();
    void on_btnGeneratePaper_clicked();
    void on_btnExit_clicked();

private:
    Ui::MainWindow *ui;
    GeneratePaperWindow* win = nullptr;  // Persistent window pointer
};

#endif // MAINWINDOW_H