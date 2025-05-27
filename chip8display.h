#ifndef CHIP8DISPLAY_H
#define CHIP8DISPLAY_H

#include <QWidget>
#include <QColor>
#include "core/include/api/IDisplay.hpp"
#include <memory>
#include <QMutex>

class Chip8Display : public QWidget, public Chip8Emu::API::IDisplay
{
    Q_OBJECT

public:
    explicit Chip8Display(QWidget *parent = nullptr);
    ~Chip8Display() override;

    // IDisplay interface implementation
    void onFrameUpdate(const std::array<bool, WIDTH * HEIGHT>& frameBuffer) override;

    // Set display colors
    void setPixelColor(const QColor& color);
    void setBackgroundColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QMutex displayMutex;
    std::array<bool, WIDTH * HEIGHT> currentFrameBuffer;
    QImage displayImage;
    QColor pixelColor;
    QColor backgroundColor;
    int frameUpdateCount; // Add this line to track frame updates
    
    void updateDisplayImage();
};

#endif // CHIP8DISPLAY_H
