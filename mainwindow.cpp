#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QSettings>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // Create display widget
    display = std::make_shared<Chip8Display>();
    ui->displayFrame->layout()->addWidget(display.get());
    
    // Create audio and input handlers
    audio = std::make_shared<Chip8Audio>();
    input = std::make_shared<Chip8Input>();
    
    // Initialize the emulator
    initializeEmulator();
    
    // Set up timer connections
    connect(&cpuTimer, &QTimer::timeout, this, &MainWindow::runEmulationCycle);
    connect(&timerTimer, &QTimer::timeout, this, &MainWindow::updateEmulationTimers);
    
    // Start timers
    timerTimer.start(1000 / 60); // 60Hz for delay and sound timers
    
    // Set default ROM directory
    lastRomPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    
    // Update UI state
    updateUIState();
    
    // Set window title
    setWindowTitle("Chip8 Emulator");
}

MainWindow::~MainWindow()
{
    // Stop timers before cleaning up
    cpuTimer.stop();
    timerTimer.stop();
    
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // Pass key events to input handler
    if (!event->isAutoRepeat()) {
        input->handleKeyPress(event->key());
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    // Pass key events to input handler
    if (!event->isAutoRepeat()) {
        input->handleKeyRelease(event->key());
    }
    QMainWindow::keyReleaseEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Clean up before closing
    cpuTimer.stop();
    timerTimer.stop();
    event->accept();
}

void MainWindow::runEmulationCycle()
{
    // Run a single CPU cycle
    emulator->runCycle();
}

void MainWindow::updateEmulationTimers()
{
    // Update the 60Hz timers
    emulator->updateTimers();
}

void MainWindow::on_actionLoad_ROM_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open ROM File"),
        lastRomPath,
        tr("Chip8 ROMs (*.ch8 *.c8 *.rom);;All Files (*)")
    );
    
    if (!filePath.isEmpty()) {
        if (loadRom(filePath)) {
            lastRomPath = QFileInfo(filePath).path();
            statusBar()->showMessage(tr("ROM loaded: %1").arg(QFileInfo(filePath).fileName()), 3000);
        }
    }
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionPause_triggered(bool checked)
{
    if (checked) {
        emulator->pause();
        cpuTimer.stop();
    } else {
        emulator->resume();
        cpuTimer.start();
    }
    
    // Update UI
    ui->pauseButton->setChecked(checked);
    updateUIState();
}

void MainWindow::on_actionReset_triggered()
{
    emulator->reset();
    statusBar()->showMessage(tr("Emulator reset"), 2000);
}

void MainWindow::on_actionSettings_triggered()
{
    // To be implemented: Settings dialog for emulator configuration
    QMessageBox::information(this, tr("Settings"), tr("Settings dialog not implemented yet."));
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(
        this,
        tr("About Chip8 Emulator"),
        tr("<h2>Chip8 Emulator</h2>"
           "<p>A simple Chip8 emulator built with Qt6.</p>"
           "<p>Version 0.1</p>")
    );
}

void MainWindow::on_actionKeyboard_Mapping_triggered()
{
    // To be implemented: Keyboard mapping dialog
    QMessageBox::information(this, tr("Keyboard Mapping"), 
        tr("Default mapping:\n"
           "1 2 3 4  ->  1 2 3 C\n"
           "Q W E R  ->  4 5 6 D\n"
           "A S D F  ->  7 8 9 E\n"
           "Z X C V  ->  A 0 B F"));
}

void MainWindow::on_resetButton_clicked()
{
    on_actionReset_triggered();
}

void MainWindow::on_pauseButton_clicked(bool checked)
{
    ui->actionPause->setChecked(checked);
    on_actionPause_triggered(checked);
}

void MainWindow::on_speedSlider_valueChanged(int value)
{
    // Update speed value label
    ui->speedValueLabel->setText(QString("%1 Hz").arg(value));
    
    // Set emulation speed
    setEmulationSpeed(value);
}

void MainWindow::initializeEmulator()
{
    // Create the emulator
    emulator = std::make_unique<Chip8Emu::API::EmulatorAPI>();
    
    // Connect components
    emulator->setDisplay(display);
    emulator->setAudio(audio);
    emulator->setInput(input);
    
    // Initialize with a default speed
    setEmulationSpeed(ui->speedSlider->value());
    
    // Start with emulator paused
    emulator->pause();
}

void MainWindow::setEmulationSpeed(int cyclesPerSecond)
{
    // Update emulator config
    Chip8Emu::Chip8::Config config = emulator->getConfig();
    config.cpuSpeed = cyclesPerSecond;
    emulator->setConfig(config);
    
    // Update CPU timer interval
    int interval = 1000 / cyclesPerSecond;
    cpuTimer.setInterval(interval > 0 ? interval : 1);
    
    // Restart timer if running
    if (!emulator->isPaused() && !cpuTimer.isActive()) {
        cpuTimer.start();
    }
}

bool MainWindow::loadRom(const QString &filename)
{
    // Stop emulation
    bool wasRunning = !emulator->isPaused();
    emulator->pause();
    cpuTimer.stop();
    
    // Load the ROM
    bool success = emulator->loadROM(filename.toStdString());
    
    if (success) {
        // Always resume after loading a ROM (changed from only resuming if wasRunning)
        emulator->resume();
        cpuTimer.start();
        
        // Force a display update
        forceDisplayUpdate();
        
        // Update window title
        setWindowTitle(QString("Chip8 Emulator - %1").arg(QFileInfo(filename).fileName()));
    } else {
        QMessageBox::critical(
            this,
            tr("Error"),
            tr("Failed to load ROM file: %1").arg(filename)
        );
    }
    
    updateUIState();
    return success;
}

// Add this new method to force a display update
void MainWindow::forceDisplayUpdate()
{
    // Run a single cycle to update display, even if paused
    if (emulator) {
        // Temporarily save paused state
        bool wasPaused = emulator->isPaused();
        
        // Unpause, run one cycle, and restore state
        if (wasPaused) {
            emulator->resume();
        }
        
        // Run a cycle to update display
        emulator->runCycle();
        
        // Restore paused state if needed
        if (wasPaused) {
            emulator->pause();
        }
    }
}

void MainWindow::updateUIState()
{
    bool isPaused = emulator->isPaused();
    
    // Update UI elements
    ui->actionPause->setChecked(isPaused);
    ui->pauseButton->setChecked(isPaused);
    ui->pauseButton->setText(isPaused ? tr("Resume") : tr("Pause"));
    
    // Enable/disable controls based on emulator state
    // (Could add more sophisticated state handling here)
}
