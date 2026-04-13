#pragma once

#include <QMainWindow>
#include <QString>
#include <vector>
#include "papergenerator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GeneratePaperWindow; }
QT_END_NAMESPACE

class GeneratePaperWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GeneratePaperWindow(QWidget *parent = nullptr);
    ~GeneratePaperWindow();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_btnGenerate_clicked();
    void on_btnExportPDF_clicked();
    void on_btnCancel_clicked();

private:
    Ui::GeneratePaperWindow *ui;
    QString lastGeneratedHTML;

    // Wrapper to call PaperGenerator
    std::vector<Question> selectQuestions(
        const std::string &course,
        int totalMarks,
        const std::vector<int> &cloRatios,
        const std::vector<int> &difficultyRatios
    );

    void populateTopicList();
};
