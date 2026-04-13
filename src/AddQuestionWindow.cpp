#include "AddQuestionWindow.h"
#include "ui_AddQuestionWindow.h"
#include "questionbank.h"
#include <QMessageBox>
#include <QCompleter>
#include <QStringListModel>

AddQuestionWindow::AddQuestionWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddQuestionWindow)
{
    ui->setupUi(this);

    // Difficulty combo box
    if (ui->comboDifficulty->count() == 0)
        ui->comboDifficulty->addItems({"Easy", "Medium", "Hard"});

    // Topic autocomplete
    QStringList topicList;
    auto suggestions = QuestionBank::instance().suggestTopics("");
    for (const auto &s : suggestions) topicList << QString::fromStdString(s);

    completerModel = new QStringListModel(topicList, this);
    completer = new QCompleter(completerModel, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->txtTopic->setCompleter(completer);

    connect(ui->btnSave, &QPushButton::clicked, this, &AddQuestionWindow::onSaveClicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &AddQuestionWindow::onCancelClicked);
}

AddQuestionWindow::~AddQuestionWindow()
{
    delete ui;
}

void AddQuestionWindow::onSaveClicked()
{
    QString course = ui->txtCourse->text().trimmed();
    QString topic = ui->txtTopic->text().trimmed();
    QString qtext = ui->txtQuestion->toPlainText().trimmed();  // preserve multi-line input
    QString diff = ui->comboDifficulty->currentText();
    QString clo = ui->txtCLO->text().trimmed();
    int marks = ui->spinMarks->value();

    if (course.isEmpty() || topic.isEmpty() || qtext.isEmpty() || clo.isEmpty() || marks <= 0) {
        QMessageBox::warning(this, "Invalid Input", "Please fill all fields correctly.");
        return;
    }

    QuestionBank::instance().addQuestion(course.toStdString(),
                                         topic.toStdString(),
                                         qtext.toStdString(),
                                         diff.toStdString(),
                                         clo.toStdString(),
                                         marks);

    // Update topic autocomplete
    QStringList updatedList;
    auto updatedSuggestions = QuestionBank::instance().suggestTopics("");
    for (const auto &s : updatedSuggestions) updatedList << QString::fromStdString(s);
    completerModel->setStringList(updatedList);

    QMessageBox::information(this, "Saved", "Question added to bank.");

    // Clear inputs for next question
    ui->txtCourse->clear();
    ui->txtTopic->clear();
    ui->txtQuestion->clear();
    ui->txtCLO->clear();
    ui->spinMarks->setValue(1);
    ui->comboDifficulty->setCurrentIndex(0);
}

void AddQuestionWindow::onCancelClicked()
{
    reject();
}
