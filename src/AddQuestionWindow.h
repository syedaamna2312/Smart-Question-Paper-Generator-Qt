#pragma once
#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AddQuestionWindow; }
QT_END_NAMESPACE

class QStringListModel;
class QCompleter;

class AddQuestionWindow : public QDialog
{
    Q_OBJECT

public:
    AddQuestionWindow(QWidget *parent = nullptr);
    ~AddQuestionWindow();

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    Ui::AddQuestionWindow *ui;
    QStringListModel *completerModel = nullptr;
    QCompleter *completer = nullptr;
};