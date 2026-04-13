#include "GeneratePaperWindow.h"
#include "ui_GeneratePaperWindow.h"
#include "questionbank.h"
#include "papergenerator.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QListWidgetItem>
#include <QCloseEvent>
#include <QShowEvent>
#include <QSpinBox>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDialog>
#include <algorithm>

//////////////////////////////
// Helper Dialogs for Ratios //
//////////////////////////////
class CLOSettingsDialog : public QDialog {
public:
    QSpinBox *clo1, *clo2, *clo3;
    CLOSettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        clo1 = new QSpinBox(); clo1->setRange(0,100); clo1->setValue(30);
        clo2 = new QSpinBox(); clo2->setRange(0,100); clo2->setValue(30);
        clo3 = new QSpinBox(); clo3->setRange(0,100); clo3->setValue(40);

        QFormLayout *layout = new QFormLayout();
        layout->addRow("CLO1 %:", clo1);
        layout->addRow("CLO2 %:", clo2);
        layout->addRow("CLO3 %:", clo3);

        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout->addWidget(buttons);
        setLayout(layout);

        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }
};

class DifficultySettingsDialog : public QDialog {
public:
    QSpinBox *easy, *medium, *hard;
    DifficultySettingsDialog(QWidget *parent = nullptr) : QDialog(parent) {
        easy = new QSpinBox(); easy->setRange(0,100); easy->setValue(40);
        medium = new QSpinBox(); medium->setRange(0,100); medium->setValue(40);
        hard = new QSpinBox(); hard->setRange(0,100); hard->setValue(20);

        QFormLayout *layout = new QFormLayout();
        layout->addRow("Easy %:", easy);
        layout->addRow("Medium %:", medium);
        layout->addRow("Hard %:", hard);

        QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        layout->addWidget(buttons);
        setLayout(layout);

        connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }
};

/////////////////////////
// GeneratePaperWindow //
/////////////////////////
GeneratePaperWindow::GeneratePaperWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::GeneratePaperWindow)
{
    ui->setupUi(this);
    populateTopicList();

    connect(ui->btnExportPDF, &QPushButton::clicked, this, &GeneratePaperWindow::on_btnExportPDF_clicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &GeneratePaperWindow::on_btnCancel_clicked);
}

GeneratePaperWindow::~GeneratePaperWindow() { delete ui; }

/////////////////////////////
// Show/Close Event Overrides
/////////////////////////////
void GeneratePaperWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    populateTopicList();
}

void GeneratePaperWindow::closeEvent(QCloseEvent *event) {
    this->hide();
    event->ignore();
}

/////////////////////////////
// Populate Topics in UI
/////////////////////////////
void GeneratePaperWindow::populateTopicList() {
    if (!ui->listTopics) return;
    ui->listTopics->clear();

    auto allQuestions = QuestionBank::instance().getAllQuestionsFlat();
    std::set<std::string> topicsSet;
    for(const auto &q : allQuestions) topicsSet.insert(q.topic);

    for(const auto &t : topicsSet){
        QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(t));
        item->setCheckState(Qt::Unchecked);
        ui->listTopics->addItem(item);
    }
}

/////////////////////////////
// Generate Paper Button
/////////////////////////////
void GeneratePaperWindow::on_btnGenerate_clicked() {
    // --- Get CLO ratios ---
    CLOSettingsDialog cloDlg(this);
    if(cloDlg.exec() != QDialog::Accepted) return;
    std::vector<int> cloRatios = {cloDlg.clo1->value(), cloDlg.clo2->value(), cloDlg.clo3->value()};

    // --- Get difficulty ratios ---
    DifficultySettingsDialog diffDlg(this);
    if(diffDlg.exec() != QDialog::Accepted) return;
    std::vector<int> difficultyRatios = {diffDlg.easy->value(), diffDlg.medium->value(), diffDlg.hard->value()};

    // --- Get course and total marks ---
    QString course = ui->txtCourse->text().trimmed();
    int totalMarks = ui->txtTotalMarks->text().toInt();
    if(course.isEmpty() || totalMarks <= 0){
        QMessageBox::warning(this,"Input Error","Please enter valid Course and Total Marks.");
        return;
    }

    // --- Generate paper using PaperGenerator ---
    std::vector<Question> selectedQuestions = selectQuestions(course.toStdString(), totalMarks, cloRatios, difficultyRatios);
    if(selectedQuestions.empty()){
        QMessageBox::warning(this,"No Questions","No questions available for this selection.");
        return;
    }

    // --- Generate HTML preview ---
    QString html;
    html += "<h1 style='text-align:center; font-weight:bold;'>" + ui->txtUniversity->text() + "</h1>";
    html += "<h3 style='text-align:center;'>" + ui->txtDepartment->text() + " | " + ui->txtCourse->text() + "</h3>";
    html += "<p><b>Total Marks:</b> " + QString::number(totalMarks) + "</p>";
    html += "<ol>";
    for(const auto &q: selectedQuestions){
        html += "<li>(" + QString::number(q.marks) + " marks) "
                + QString::fromStdString(q.text)
                + "<br><small><b>CLO:</b> " + QString::fromStdString(q.CLO) + "</small></li>";
    }
    html += "</ol>";

    lastGeneratedHTML = html;
    if(ui->textPreview) ui->textPreview->setHtml(lastGeneratedHTML);
    QMessageBox::information(this,"Paper Generated","Question paper generated successfully! Preview below.");
}

/////////////////////////////
// Export PDF Button
/////////////////////////////
void GeneratePaperWindow::on_btnExportPDF_clicked() {
    if(lastGeneratedHTML.isEmpty()){
        QMessageBox::warning(this,"No Paper","Please generate the paper first!");
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this,"Save PDF","","*.pdf");
    if(fileName.isEmpty()) return;

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QTextDocument doc;
    doc.setHtml(lastGeneratedHTML);
    doc.print(&printer);

    QMessageBox::information(this,"PDF Exported","PDF saved at "+fileName);
}

/////////////////////////////
// Cancel Button
/////////////////////////////
void GeneratePaperWindow::on_btnCancel_clicked() { this->close(); }

/////////////////////////////
// Wrapper to call PaperGenerator
/////////////////////////////
std::vector<Question> GeneratePaperWindow::selectQuestions(
    const std::string &course,
    int totalMarks,
    const std::vector<int> &cloRatios,
    const std::vector<int> &difficultyRatios
) {
    RatioSpec ratios;
    ratios.easyPerc = difficultyRatios[0];
    ratios.mediumPerc = difficultyRatios[1];
    ratios.hardPerc = difficultyRatios[2];
    ratios.clo1Perc = cloRatios[0];
    ratios.clo2Perc = cloRatios[1];
    ratios.clo3Perc = cloRatios[2];

    PaperGenerator pg(&QuestionBank::instance());

    // Collect selected topics from UI
    std::vector<std::string> selectedTopics;
    for(int i=0;i<ui->listTopics->count();i++){
        QListWidgetItem *item = ui->listTopics->item(i);
        if(item->checkState() == Qt::Checked)
            selectedTopics.push_back(item->text().toStdString());
    }

    PaperResult result = pg.generatePaper(course, selectedTopics, totalMarks, ratios);
    return result.selected;
}
