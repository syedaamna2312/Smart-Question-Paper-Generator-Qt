#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "AddQuestionWindow.h"
#include "GeneratePaperWindow.h"
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , win(nullptr) // Initialize pointer to nullptr
{
    ui->setupUi(this);

    // Connect buttons to slots

}

MainWindow::~MainWindow()
{
    delete ui;
    if(win) delete win; // Clean up the window if it exists
}

void MainWindow::on_btnAddQuestion_clicked()
{
    AddQuestionWindow dlg(this);  // Modal dialog
    dlg.exec();                    // Wait until closed
}

void MainWindow::on_btnViewQuestionBank_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Question Bank");
    dialog.resize(800, 400);

    QVBoxLayout layout(&dialog);

    QTableWidget *table = new QTableWidget(&dialog);

    // Get all questions from your backend
    auto all = QuestionBank::instance().getAllQuestionsFlat();

    table->setColumnCount(7);
    table->setHorizontalHeaderLabels({
        "ID", "Course", "Topic", "Question", "Difficulty", "CLO", "Marks"
    });
    table->setRowCount(all.size());

    for (int i = 0; i < all.size(); i++)
    {
        const auto &q = all[i];

        table->setItem(i, 0, new QTableWidgetItem(QString::number(q.id)));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(q.course)));
        table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(q.topic)));
        table->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(q.text)));
        table->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(q.difficulty)));
        table->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(q.CLO)));
        table->setItem(i, 6, new QTableWidgetItem(QString::number(q.marks)));
    }

    table->resizeColumnsToContents();
    layout.addWidget(table);

    dialog.exec();
}


void MainWindow::on_btnGeneratePaper_clicked()
{
    // Create the window only if it doesn't exist
    if (!win) {
        win = new GeneratePaperWindow(this);

        // Optional: override closeEvent in GeneratePaperWindow to hide instead of delete
        // or keep as is if you want to delete and recreate each time
    }

    win->show();           // Show the window
    win->raise();          // Bring it to the front
    win->activateWindow(); // Give it focus
}

void MainWindow::on_btnExit_clicked()
{
    this->close();
}