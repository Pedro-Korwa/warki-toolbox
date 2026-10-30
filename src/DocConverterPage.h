#ifndef DOCCONVERTERPAGE_H
#define DOCCONVERTERPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QComboBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QTabWidget>
#include <QProgressBar>
#include <QProcess>
#include <QTimer>

class DocConverterPage : public QWidget {
    Q_OBJECT

public:
    explicit DocConverterPage(QWidget *parent = nullptr);
    ~DocConverterPage();

private:
    QTabWidget *tabWidget;
    QProcess *currentProcess;
    QTimer *animTimer;
    int animFrame;
    bool cancelRequested;

    // --- TAB 1: PDF CONVERTER ---
    QTableWidget *tableConvertFiles;
    QPushButton *btnAddPdfConvert;
    QPushButton *btnClearPdfConvert;
    QPushButton *btnStartConvert;
    QPushButton *btnCancelConvert;
    QComboBox *comboTargetFormat;
    QProgressBar *progressConvert;
    QLabel *statusConvertLabel;

    // --- TAB 2: MERGE PDF ---
    QTableWidget *tableMergeFiles;
    QPushButton *btnAddPdfMerge;
    QPushButton *btnMoveUp;
    QPushButton *btnMoveDown;
    QPushButton *btnRemoveMerge;
    QPushButton *btnStartMerge;
    QPushButton *btnCancelMerge;
    QProgressBar *progressMerge;
    QLabel *statusMergeLabel;

    void setupUI();
    QWidget* createConvertTab();
    QWidget* createMergeTab();

    void addPdfToConvertTable();
    void addPdfToMergeTable();
    void processPdfConversion();
    void processPdfMerge();
    void cancelOperation();
    void moveSelectedItem(int direction);
    void updateLoadingAnimation();
};

#endif // DOCCONVERTERPAGE_H