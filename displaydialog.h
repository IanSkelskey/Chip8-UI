#ifndef DISPLAYDIALOG_H
#define DISPLAYDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <memory>
#include "chip8display.h"

class DisplayDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayDialog(std::shared_ptr<Chip8Display> display, QWidget *parent = nullptr);

private slots:
    void onPixelColorClicked();
    void onBackgroundColorClicked();
    void onApplyClicked();
    void onResetClicked();

private:
    void setupUI();
    void updatePixelColorPreview();
    void updateBackgroundColorPreview();

    std::shared_ptr<Chip8Display> display;
    QColor currentPixelColor;
    QColor currentBackgroundColor;
    
    // UI elements
    QPushButton* pixelColorButton;
    QPushButton* backgroundColorButton;
    QLabel* pixelColorPreview;
    QLabel* backgroundColorPreview;
};

#endif // DISPLAYDIALOG_H
