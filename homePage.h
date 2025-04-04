#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QPushButton>

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);

signals:
    void openRegisterPage();
    void openLoginPage();

public slots:
    void showHomePage();

private:
    QPushButton *registerButton;
    QPushButton *loginButton;
};

#endif // HOMEPAGE_H
