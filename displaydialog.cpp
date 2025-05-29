#include "displaydialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QDebug>

DisplayDialog::DisplayDialog(std::shared_ptr<Chip8Display> display, QWidget *parent)
    : QDialog(parent)
    , display(display)
{
    setWindowTitle(tr("Display Settings"));
    setModal(true);
    
    // Get current colors from display
    currentPixelColor = display->getPixelColor();
    currentBackgroundColor = display->getBackgroundColor();
    
    setupUI();
}

void DisplayDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    
    // Color configuration group
    auto* colorGroup = new QGroupBox(tr("Display Colors"));
    auto* colorLayout = new QVBoxLayout(colorGroup);
    
    // Pixel (foreground) color
    auto* pixelLayout = new QHBoxLayout();
    pixelLayout->addWidget(new QLabel(tr("Pixel Color:")));
    
    pixelColorButton = new QPushButton(tr("Change"));
    connect(pixelColorButton, &QPushButton::clicked, this, &DisplayDialog::onPixelColorClicked);
    pixelLayout->addWidget(pixelColorButton);
    
    pixelColorPreview = new QLabel();
    pixelColorPreview->setFixedSize(24, 24);
    pixelColorPreview->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    updatePixelColorPreview();
    pixelLayout->addWidget(pixelColorPreview);
    
    colorLayout->addLayout(pixelLayout);
    
    // Background color
    auto* bgLayout = new QHBoxLayout();
    bgLayout->addWidget(new QLabel(tr("Background Color:")));
    
    backgroundColorButton = new QPushButton(tr("Change"));
    connect(backgroundColorButton, &QPushButton::clicked, this, &DisplayDialog::onBackgroundColorClicked);
    bgLayout->addWidget(backgroundColorButton);
    
    backgroundColorPreview = new QLabel();
    backgroundColorPreview->setFixedSize(24, 24);
    backgroundColorPreview->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    updateBackgroundColorPreview();
    bgLayout->addWidget(backgroundColorPreview);
    
    colorLayout->addLayout(bgLayout);
    
    mainLayout->addWidget(colorGroup);
    
    // Buttons
    auto* buttonLayout = new QHBoxLayout();
    
    auto* resetButton = new QPushButton(tr("Reset to Defaults"));
    connect(resetButton, &QPushButton::clicked, this, &DisplayDialog::onResetClicked);
    buttonLayout->addWidget(resetButton);
    
    buttonLayout->addStretch();
    
    auto* applyButton = new QPushButton(tr("Apply"));
    applyButton->setDefault(true);
    connect(applyButton, &QPushButton::clicked, this, &DisplayDialog::onApplyClicked);
    buttonLayout->addWidget(applyButton);
    
    auto* cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    resize(400, 200);
}

void DisplayDialog::updatePixelColorPreview()
{
    pixelColorPreview->setStyleSheet(QString("background-color: %1;").arg(currentPixelColor.name()));
}

void DisplayDialog::updateBackgroundColorPreview()
{
    backgroundColorPreview->setStyleSheet(QString("background-color: %1;").arg(currentBackgroundColor.name()));
}

void DisplayDialog::onPixelColorClicked()
{
    QColor color = QColorDialog::getColor(currentPixelColor, this, tr("Select Pixel Color"));
    if (color.isValid()) {
        currentPixelColor = color;
        updatePixelColorPreview();
    }
}

void DisplayDialog::onBackgroundColorClicked()
{
    QColor color = QColorDialog::getColor(currentBackgroundColor, this, tr("Select Background Color"));
    if (color.isValid()) {
        currentBackgroundColor = color;
        updateBackgroundColorPreview();
    }
}

void DisplayDialog::onApplyClicked()
{
    display->setPixelColor(currentPixelColor);
    display->setBackgroundColor(currentBackgroundColor);
    accept();
}

void DisplayDialog::onResetClicked()
{
    // Reset to default green on black
    currentPixelColor = Qt::green;
    currentBackgroundColor = Qt::black;
    updatePixelColorPreview();
    updateBackgroundColorPreview();
}
