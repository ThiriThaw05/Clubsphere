#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

signals:
    void navigateToHome();
    void openadminHome(int userId);

private slots:
    void goBackToHome(); // Slot to handle back button click

public slots:
    void onLoginButtonClicked();
    void showLoginPage();

private:
    QLabel *titleLabel;
    QLabel *subtitleLabel;
    QLabel *logoLabel;
    QLabel *descriptionLabel;

    QLineEdit *userIdField;
    QLineEdit *passwordField;
    QPushButton *loginButton;
    QPushButton *backButton;
    QVBoxLayout *mainLayout;
};

#endif // LOGINPAGE_H
