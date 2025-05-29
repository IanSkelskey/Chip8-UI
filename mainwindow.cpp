#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QStandardPaths>
#include <QFile>
#include <QDebug>
#include <QTranslator>
#include <QStyleFactory>
#include "keyboardtestdialog.h"
#include "quirksdialog.h"
#include "displaydialog.h"
#include "languagedialog.h"  // Add this include

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Create display widget
    display = std::make_shared<Chip8Display>();
    ui->displayFrame->layout()->addWidget(display.get());
    
    // Create audio subsystem
    audio = std::make_shared<Chip8Audio>();
    
    // Create input subsystem
    input = std::make_shared<Chip8Input>();
    
    // Create emulator instance
    emulator = std::make_shared<Chip8Emu::API::EmulatorAPI>();
    
    // Connect emulator to display, audio, and input
    emulator->setDisplay(display);
    emulator->setAudio(audio);
    emulator->setInput(input);
    
    // Setup timers for emulation
    cycleTimer = new QTimer(this);
    connect(cycleTimer, &QTimer::timeout, this, &MainWindow::runEmulationCycle);
    
    timerUpdateTimer = new QTimer(this);
    connect(timerUpdateTimer, &QTimer::timeout, this, &MainWindow::updateEmulationTimers);
    timerUpdateTimer->start(1000 / 60); // 60 Hz for timers
    
    // Initialize emulator in paused state
    emulator->pause();
    
    // Set default speed
    setEmulationSpeed(700); // Default to 700 instructions per second
    
    // Set focus policy to receive keyboard events
    setFocusPolicy(Qt::StrongFocus);
    
    // Initialize theme
    loadThemePreference();
    
    qDebug() << "MainWindow initialized, emulator created and connected";
}

MainWindow::~MainWindow()
{
    // Stop timers before cleaning up
    cycleTimer->stop();
    timerUpdateTimer->stop();
    
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Pass key events to input handler
    if (!event->isAutoRepeat()) {
        qDebug() << "MainWindow received key press:" << event->key() << "(" << QKeySequence(event->key()).toString() << ")";
        input->handleKeyPress(event->key());
        // Don't call base implementation to avoid conflicts
        // with default key handling (like Space for buttons)
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Pass key events to input handler
    if (!event->isAutoRepeat()) {
        qDebug() << "MainWindow received key release:" << event->key() << "(" << QKeySequence(event->key()).toString() << ")";
        input->handleKeyRelease(event->key());
        // Don't call base implementation for the same reason
        return;
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Clean up before closing
    cycleTimer->stop();
    timerUpdateTimer->stop();
    event->accept();
}

void MainWindow::runEmulationCycle()
{
    if (emulator) {
        emulator->runCycle();
        cyclesRun++;
        
        // Log every 100 cycles for debugging
        if (cyclesRun % 100 == 0) {
            qDebug() << "Emulation cycles run:" << cyclesRun 
                     << "- Paused:" << emulator->isPaused();
        }
    }
}

void MainWindow::updateEmulationTimers()
{
    if (emulator) {
        emulator->updateTimers();
    }
}

// Method to properly control emulation speed
void MainWindow::setEmulationSpeed(int instructionsPerSecond)
{
    if (instructionsPerSecond <= 0) {
        cycleTimer->stop();
        return;
    }

    // Update emulator's internal speed
    emulator->setSpeed(instructionsPerSecond);
    
    // Calculate cycle interval in milliseconds
    int intervalMs = 1000 / instructionsPerSecond;
    
    // Make sure we have at least 1ms interval
    intervalMs = std::max(1, intervalMs);
    
    // Only start the timer if we're not paused
    if (!emulator->isPaused()) {
        cycleTimer->start(intervalMs);
    } else {
        // Just set the interval without starting
        cycleTimer->setInterval(intervalMs);
    }
    
    qDebug() << "Emulation speed set to" << instructionsPerSecond 
             << "instructions per second (timer interval:" << intervalMs << "ms)";
}

// Method to properly start emulation
void MainWindow::startEmulation()
{
    if (emulator) {
        emulator->resume();
        // Get the current speed and properly start the timer
        int currentSpeed = emulator->getConfig().cpuSpeed;
        int intervalMs = 1000 / std::max(1, static_cast<int>(currentSpeed));
        cycleTimer->start(intervalMs);
        qDebug() << "Emulation started - Timer active:" << cycleTimer->isActive();
    }
}

// Method to properly stop emulation
void MainWindow::stopEmulation()
{
    if (emulator) {
        emulator->pause();
        // Stop the cycle timer when pausing
        cycleTimer->stop();
        qDebug() << "Emulation stopped - Timer active:" << cycleTimer->isActive();
    }
}

// Update UI elements based on emulator state
void MainWindow::updateUIState()
{
    bool isPaused = emulator->isPaused();
    
    // Update buttons
    ui->pauseButton->setChecked(isPaused);
    ui->pauseButton->setText(isPaused ? tr("Resume") : tr("Pause"));
    
    // Update menu actions
    ui->actionPause->setChecked(isPaused);
    ui->actionPause->setText(isPaused ? tr("Resume") : tr("Pause"));
    
    // Update status bar
    statusBar()->showMessage(isPaused ? tr("Emulation paused") : tr("Emulation running"));
}

bool MainWindow::loadROM(const QString &filename)
{
    if (filename.isEmpty()) {
        return false;
    }
    
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), 
                             tr("Could not open ROM file: %1").arg(file.errorString()));
        return false;
    }
    
    QByteArray romData = file.readAll();
    file.close();
    
    if (romData.isEmpty()) {
        QMessageBox::critical(this, tr("Error"), tr("ROM file is empty."));
        return false;
    }
    
    // Reset emulator before loading new ROM
    emulator->reset();
    
    // Load ROM data into emulator
    bool success = emulator->loadROM(
        reinterpret_cast<const uint8_t*>(romData.constData()), 
        static_cast<size_t>(romData.size())
    );
    
    if (success) {
        lastLoadedRomPath = filename;
        statusBar()->showMessage(tr("ROM loaded: %1").arg(QFileInfo(filename).fileName()));
        
        // Start emulation
        startEmulation();
        updateUIState();
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to load ROM."));
    }
    
    return success;
}

// Menu actions
void MainWindow::on_actionLoad_ROM_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        tr("Open ROM File"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("CHIP-8 ROMs (*.ch8 *.chip8);;All Files (*)"));
    
    if (!filePath.isEmpty()) {
        loadROM(filePath);
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionPause_triggered(bool checked)
{
    if (checked) {
        stopEmulation();
    } else {
        startEmulation();
    }
    updateUIState();
}

void MainWindow::on_actionReset_triggered()
{
    emulator->reset();
    if (!lastLoadedRomPath.isEmpty()) {
        loadROM(lastLoadedRomPath);
    }
    updateUIState();
}

void MainWindow::on_actionSettings_triggered()
{
    // Show the display settings dialog
    DisplayDialog dialog(display, this);
    dialog.exec();
}

// Add this new method for the display settings menu item if it's separate
void MainWindow::on_actionDisplay_Settings_triggered()
{
    DisplayDialog dialog(display, this);
    dialog.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About CHIP-8 Emulator"),
                      tr("CHIP-8 Emulator\n\n"
                         "A simple CHIP-8 emulator written in C++ using Qt."));
}

void MainWindow::on_actionKeyboard_Mapping_triggered()
{
    KeyboardTestDialog dialog(input.get(), this);
    dialog.exec();
}

void MainWindow::on_actionQuirks_triggered()
{
    QuirksDialog dialog(emulator, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Quirks updated - you can add any refresh logic here if needed
    }
}

void MainWindow::on_resetButton_clicked()
{
    on_actionReset_triggered();
}

void MainWindow::on_pauseButton_clicked(bool checked)
{
    on_actionPause_triggered(checked);
}

void MainWindow::on_speedSlider_valueChanged(int value)
{
    setEmulationSpeed(value);
    ui->speedValueLabel->setText(QString("%1 Hz").arg(value));
}

void MainWindow::on_actionLanguage_Settings_triggered()
{
    LanguageDialog dialog(this);
    
    if (dialog.exec() == QDialog::Accepted) {
        // User selected a new language, apply it
        QString langCode = dialog.getSelectedLanguageCode();
        switchLanguage(langCode);
    }
}

bool MainWindow::switchLanguage(const QString &language)
{
    // Get the application instance
    QApplication *app = qobject_cast<QApplication*>(QApplication::instance());
    if (!app) return false;
    
    static QTranslator *translator = nullptr;
    
    // Remove current translator if it exists
    if (translator) {
        app->removeTranslator(translator);
        delete translator;
        translator = nullptr;
    }
    
    // If we're switching to English, we don't need a translator
    if (language == "en_US") {
        qDebug() << "Switching to English (US) - no translator needed";
        // Force UI retranslation even when switching to English
        ui->retranslateUi(this);
        return true;
    }
    
    // Create a new translator
    translator = new QTranslator();
    
    // First try to load from resources
    QString translationPath = ":/translations/";
    QString baseName = "Chip8-UI_" + language;
    
    bool loaded = false;
    
    if (translator->load(translationPath + baseName)) {
        qDebug() << "Loaded translation from resources:" << baseName;
        loaded = true;
    }
    // Then try to load from file system
    else if (translator->load(baseName, "translations")) {
        qDebug() << "Loaded translation from file system:" << baseName;
        loaded = true;
    }
    // Try just the language code if full locale failed
    else if (language.contains("_")) {
        baseName = "Chip8-UI_" + language.split("_").first();
        if (translator->load(translationPath + baseName)) {
            qDebug() << "Loaded translation from resources (language only):" << baseName;
            loaded = true;
        } else if (translator->load(baseName, "translations")) {
            qDebug() << "Loaded translation from file system (language only):" << baseName;
            loaded = true;
        }
    }
    
    if (loaded) {
        // Install the translator
        app->installTranslator(translator);
        
        // Force a UI retranslation
        ui->retranslateUi(this);
        
        qDebug() << "Language switched to" << language;
        return true;
    } else {
        qDebug() << "Failed to load translation for" << language;
        return false;
    }
}

void MainWindow::applyTheme(const QString &themeName)
{
    QSettings settings;
    settings.setValue("theme", themeName);
    
    if (themeName == "light") {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette lightPalette;
        lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));
        lightPalette.setColor(QPalette::WindowText, Qt::black);
        lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
        lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
        lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
        lightPalette.setColor(QPalette::ToolTipText, Qt::black);
        lightPalette.setColor(QPalette::Text, Qt::black);
        lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
        lightPalette.setColor(QPalette::ButtonText, Qt::black);
        lightPalette.setColor(QPalette::BrightText, Qt::red);
        lightPalette.setColor(QPalette::Link, QColor(0, 0, 255));
        lightPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        lightPalette.setColor(QPalette::HighlightedText, Qt::white);
        qApp->setPalette(lightPalette);
    } else if (themeName == "dark") {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(35, 35, 35));
        darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        darkPalette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        darkPalette.setColor(QPalette::Active, QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, Qt::darkGray);
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
        qApp->setPalette(darkPalette);
    } else {
        // System theme
        qApp->setStyle(QStyleFactory::create("Fusion"));
        qApp->setPalette(qApp->style()->standardPalette());
    }
    
    updateThemeActions();
    qDebug() << "Applied theme:" << themeName;
}

void MainWindow::updateThemeActions()
{
    QSettings settings;
    QString currentTheme = settings.value("theme", "system").toString();
    
    ui->actionLightTheme->setChecked(currentTheme == "light");
    ui->actionDarkTheme->setChecked(currentTheme == "dark");
    ui->actionSystemTheme->setChecked(currentTheme == "system");
}

void MainWindow::loadThemePreference()
{
    QSettings settings;
    QString themeName = settings.value("theme", "system").toString();
    applyTheme(themeName);
}

// Add theme action handlers
void MainWindow::on_actionLightTheme_triggered()
{
    applyTheme("light");
}

void MainWindow::on_actionDarkTheme_triggered()
{
    applyTheme("dark");
}

void MainWindow::on_actionSystemTheme_triggered()
{
    applyTheme("system");
}