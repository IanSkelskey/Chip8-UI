#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <memory>
#include "chip8input.h"
#include "chip8display.h"
#include "chip8audio.h"
#include "core/include/api/EmulatorAPI.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void runEmulationCycle();
    void updateEmulationTimers();
    
    void on_actionLoad_ROM_triggered();
    void on_actionExit_triggered();
    void on_actionPause_triggered(bool checked);
    void on_actionReset_triggered();
    void on_actionSettings_triggered();
    void on_actionAbout_triggered();
    void on_actionKeyboard_Mapping_triggered();
    void on_resetButton_clicked();
    void on_pauseButton_clicked(bool checked);
    void on_speedSlider_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    
    // Components
    std::shared_ptr<Chip8Display> display;
    std::shared_ptr<Chip8Audio> audio;
    std::shared_ptr<Chip8Input> input;
    std::shared_ptr<Chip8Emu::API::EmulatorAPI> emulator;
    
    // Timers
    QTimer *cycleTimer;
    QTimer *timerUpdateTimer;
    
    // State tracking
    int cyclesRun = 0;
    QString lastLoadedRomPath;
    
    // Methods
    void setEmulationSpeed(int instructionsPerSecond);
    void startEmulation();
    void stopEmulation();
    void updateUIState();
    bool loadROM(const QString &filename);
};

#endif // MAINWINDOW_H
