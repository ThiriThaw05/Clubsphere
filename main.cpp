#include "mainwindow.h"
#include "database.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Initialize database
    if (!Database::initialize()) {
        QMessageBox::critical(nullptr, "Error", "Failed to initialize database!");
        return -1;
    }

    // Initialize the student names list
    if (!Database::initializeStudentNames()) {
        qDebug() << "Warning: Failed to initialize student names list!";
        // Continue anyway as this is not critical
    }

    // Create the main window instead of individual pages
    MainWindow mainWindow;

    int result = a.exec();

    return result;
}
