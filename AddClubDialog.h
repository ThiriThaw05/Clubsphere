#ifndef ADDCLUBDIALOG_H
#define ADDCLUBDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QBuffer>
#include "Club.h"

class AddClubDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddClubDialog(QWidget *parent = nullptr);
    Club getClub() const { return newClub; }

private slots:
    void onBrowseClicked();
    void onCreateClicked();
    void validateForm();

private:
    QLineEdit *clubNameEdit;
    QLineEdit *leaderIdEdit;
    QPushButton *browseButton;
    QLabel *photoPreview;
    QPushButton *createButton;
    QPushButton *cancelButton;
    Club newClub;
    QByteArray clubPhotoData;
    bool photoSelected;

    void setupUI();
};
#endif
