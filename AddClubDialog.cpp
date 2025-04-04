#include "AddClubDialog.h"
#include "database.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QDebug>

AddClubDialog::AddClubDialog(QWidget *parent)
    : QDialog(parent), photoSelected(false)
{
    setupUI();
    setWindowTitle("Create New Club");
    setFixedSize(400, 450);
}

void AddClubDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Form layout for inputs
    QFormLayout *formLayout = new QFormLayout();

    // Club name
    clubNameEdit = new QLineEdit(this);
    formLayout->addRow("Club Name:", clubNameEdit);

    // Leader ID
    leaderIdEdit = new QLineEdit(this);
    leaderIdEdit->setPlaceholderText("Enter user ID of the leader");
    formLayout->addRow("Leader ID:", leaderIdEdit);

    mainLayout->addLayout(formLayout);

    // Photo selection
    QVBoxLayout *photoLayout = new QVBoxLayout();
    QLabel *photoLabel = new QLabel("Club Photo:", this);
    photoPreview = new QLabel(this);
    photoPreview->setFixedSize(200, 200);
    photoPreview->setStyleSheet("border: 1px solid #CCCCCC; background-color: #F5F5F5;");
    photoPreview->setAlignment(Qt::AlignCenter);
    photoPreview->setText("No photo selected");

    browseButton = new QPushButton("Browse...", this);

    photoLayout->addWidget(photoLabel);
    photoLayout->addWidget(photoPreview, 0, Qt::AlignCenter);
    photoLayout->addWidget(browseButton, 0, Qt::AlignCenter);

    mainLayout->addLayout(photoLayout);

    // Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    createButton = new QPushButton("Create Club", this);
    cancelButton = new QPushButton("Cancel", this);

    createButton->setEnabled(false); // Disabled until valid input

    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(createButton);

    mainLayout->addLayout(buttonsLayout);

    // Connect signals and slots
    connect(browseButton, &QPushButton::clicked, this, &AddClubDialog::onBrowseClicked);
    connect(createButton, &QPushButton::clicked, this, &AddClubDialog::onCreateClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Add validation
    connect(clubNameEdit, &QLineEdit::textChanged, this, &AddClubDialog::validateForm);
    connect(leaderIdEdit, &QLineEdit::textChanged, this, &AddClubDialog::validateForm);
}

void AddClubDialog::onBrowseClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Club Photo"),
                                                    "", tr("Image Files (*.png *.jpg *.jpeg)"));
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (!image.isNull()) {
            // Scale image if too large
            if (image.width() > 400 || image.height() > 400) {
                image = image.scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            // Show preview
            QPixmap pixmap = QPixmap::fromImage(image);
            photoPreview->setPixmap(pixmap.scaled(photoPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

            // Convert to byte array
            QByteArray byteArray;
            QBuffer buffer(&byteArray);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "PNG");

            clubPhotoData = byteArray;
            photoSelected = true;
            validateForm();
        }
    }
}

void AddClubDialog::onCreateClicked()
{
    bool ok;
    int leaderId = leaderIdEdit->text().toInt(&ok);

    if (!ok) {
        QMessageBox::critical(this, "Error", "Leader ID must be a valid number.");
        return;
    }

    // Check if user is already a leader of another club
    QString existingClubName;
    if (Database::isUserAlreadyLeader(leaderId, existingClubName)) {
        QMessageBox::critical(this, "Error",
                              QString("Leader ID %1 is already assigned to %2").arg(leaderId).arg(existingClubName));
        return;
    }

    // Generate unique club ID
    int clubId = Database::generateUniqueClubId();

    // Create club object
    newClub = Club(clubId, clubNameEdit->text(), leaderId,
                   photoSelected ? clubPhotoData : QByteArray());

    // Save to database
    if (newClub.saveToDatabase()) {
        accept(); // Close dialog with accept result
    } else {
        QMessageBox::critical(this, "Error", "Failed to create club. Please try again.");
    }
}

void AddClubDialog::validateForm()
{
    // Enable create button only if we have valid input
    bool valid = !clubNameEdit->text().isEmpty() &&
                 !leaderIdEdit->text().isEmpty();

    createButton->setEnabled(valid);
}
