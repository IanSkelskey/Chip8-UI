#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDir>
#include <QDebug>
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Set application icon
    QIcon appIcon(":/icons/logo.png");
    a.setWindowIcon(appIcon);

    // Application information for settings, etc.
    QCoreApplication::setOrganizationName("Chip8Emulator");
    QCoreApplication::setApplicationName("Chip8-UI");
    
    // Enable high DPI scaling
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    
    // Load Qt translations
    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(), "qt", "_", 
                          QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        a.installTranslator(&qtTranslator);
        qDebug() << "Loaded Qt translations for" << QLocale::system().name();
    }
    
    // Load application translations
    QTranslator appTranslator;
    
    // Get the preferred language from settings (default to system locale)
    QSettings settings;
    QString preferredLanguage = settings.value("language", QLocale::system().name()).toString();
    
    qDebug() << "Preferred language from settings:" << preferredLanguage;
    
    // First try to load from resources
    QString translationPath = ":/translations/";
    QString baseName = "Chip8-UI_" + preferredLanguage;
    
    bool loaded = false;
    
    if (appTranslator.load(translationPath + baseName)) {
        qDebug() << "Loaded translation from resources:" << baseName;
        a.installTranslator(&appTranslator);
        loaded = true;
    } 
    // Then try to load from file system
    else if (appTranslator.load(baseName, "translations")) {
        qDebug() << "Loaded translation from file system:" << baseName;
        a.installTranslator(&appTranslator);
        loaded = true;
    }
    
    // Try just the language code if full locale failed
    if (!loaded && preferredLanguage.contains("_")) {
        baseName = "Chip8-UI_" + preferredLanguage.split("_").first();
        if (appTranslator.load(translationPath + baseName)) {
            qDebug() << "Loaded translation from resources (language only):" << baseName;
            a.installTranslator(&appTranslator);
        } else if (appTranslator.load(baseName, "translations")) {
            qDebug() << "Loaded translation from file system (language only):" << baseName;
            a.installTranslator(&appTranslator);
        } else {
            qDebug() << "No translation found for" << preferredLanguage;
        }
    }
    
    MainWindow w;
    w.show();
    return a.exec();
}
