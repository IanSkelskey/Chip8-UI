#include "keyboardtestdialog.h"
#include "chip8input.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QDebug>

KeyboardTestDialog::KeyboardTestDialog(Chip8Input* input, QWidget *parent)
    : QDialog(parent)
    , input(input)
{
    setWindowTitle(tr("Keyboard Mapping Test"));
    setWindowIcon(QIcon(":/icons/keyboard_icon.png"));
    setupUI();
    
    // Make sure we can receive key events
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    
    // Setup a timer to regularly update the key display
    QTimer* updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, [this]() {
        for (uint8_t key = 0; key <= 0xF; key++) {
            updateKeyDisplay(key, this->input->isKeyPressed(key));
        }
    });
    updateTimer->start(50); // 20 updates per second
    
    // Always start the dialog with keyboard focus
    QTimer::singleShot(100, this, [this]() {
        this->activateWindow();
        this->setFocus();
    });
}

void KeyboardTestDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Instructions
    QLabel* instructionsLabel = new QLabel(tr(
        "Press keys to test CHIP-8 keyboard mapping.\n"
        "Default mapping:\n"
        "1 2 3 4  ->  1 2 3 C\n"
        "Q W E R  ->  4 5 6 D\n"
        "A S D F  ->  7 8 9 E\n"
        "Z X C V  ->  A 0 B F"
    ));
    instructionsLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(instructionsLabel);
    
    // Create grid for key display
    QGridLayout* keyGrid = new QGridLayout();
    
    // Create labels for each CHIP-8 key
    const char* keyLabelsText[16] = {
        "0", "1", "2", "3",
        "4", "5", "6", "7",
        "8", "9", "A", "B",
        "C", "D", "E", "F"
    };
    
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            int index = row * 4 + col;
            uint8_t key = 0;
            
            // Map grid position to CHIP-8 key value
            switch (index) {
                case 0: key = 0x1; break;
                case 1: key = 0x2; break;
                case 2: key = 0x3; break;
                case 3: key = 0xC; break;
                case 4: key = 0x4; break;
                case 5: key = 0x5; break;
                case 6: key = 0x6; break;
                case 7: key = 0xD; break;
                case 8: key = 0x7; break;
                case 9: key = 0x8; break;
                case 10: key = 0x9; break;
                case 11: key = 0xE; break;
                case 12: key = 0xA; break;
                case 13: key = 0x0; break;
                case 14: key = 0xB; break;
                case 15: key = 0xF; break;
            }
            
            QLabel* keyLabel = new QLabel(keyLabelsText[key]);
            keyLabel->setAlignment(Qt::AlignCenter);
            keyLabel->setMinimumSize(40, 40);
            keyLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
            keyLabel->setStyleSheet("background-color: #f0f0f0;");
            
            keyGrid->addWidget(keyLabel, row, col);
            keyLabels[key] = keyLabel;
        }
    }
    
    mainLayout->addLayout(keyGrid);
    
    // Close button
    QPushButton* closeButton = new QPushButton(tr("Close"));
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeButton);
    
    setLayout(mainLayout);
    resize(300, 250);
}

void KeyboardTestDialog::updateKeyDisplay(uint8_t key, bool pressed)
{
    if (keyLabels.contains(key)) {
        QLabel* label = keyLabels[key];
        if (pressed) {
            label->setStyleSheet("background-color: #90ee90;"); // Light green
        } else {
            label->setStyleSheet("background-color: #f0f0f0;"); // Light gray
        }
    }
}

void KeyboardTestDialog::keyPressEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        qDebug() << "Test dialog key press:" << event->key();
        input->handleKeyPress(event->key());
    }
    QDialog::keyPressEvent(event);
}

void KeyboardTestDialog::keyReleaseEvent(QKeyEvent *event)
{
    if (!event->isAutoRepeat()) {
        qDebug() << "Test dialog key release:" << event->key();
        input->handleKeyRelease(event->key());
    }
    QDialog::keyReleaseEvent(event);
}
