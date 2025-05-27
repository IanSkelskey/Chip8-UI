#include "chip8display.h"
#include <QPainter>
#include <QResizeEvent>
#include <QMutexLocker>

Chip8Display::Chip8Display(QWidget *parent)
    : QWidget(parent)
    , pixelColor(Qt::green)
    , backgroundColor(Qt::black)
    , displayImage(WIDTH, HEIGHT, QImage::Format_RGB32)
{
    // Initialize display buffer
    currentFrameBuffer.fill(false);
    displayImage.fill(backgroundColor);

    // Set focus policy to receive keyboard events
    setFocusPolicy(Qt::StrongFocus);
}

Chip8Display::~Chip8Display() = default;

void Chip8Display::onFrameUpdate(const std::array<bool, WIDTH * HEIGHT>& frameBuffer)
{
    QMutexLocker locker(&displayMutex);
    currentFrameBuffer = frameBuffer;
    updateDisplayImage();
    update(); // Schedule a repaint
}

void Chip8Display::setPixelColor(const QColor &color)
{
    QMutexLocker locker(&displayMutex);
    pixelColor = color;
    updateDisplayImage();
    update();
}

void Chip8Display::setBackgroundColor(const QColor &color)
{
    QMutexLocker locker(&displayMutex);
    backgroundColor = color;
    updateDisplayImage();
    update();
}

void Chip8Display::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false); // No smoothing for pixel-perfect rendering
    
    // Draw the display image scaled to the widget size
    QMutexLocker locker(&displayMutex);
    painter.drawImage(rect(), displayImage);
}

void Chip8Display::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    // Nothing specific needed here, scaling is handled in paintEvent
}

void Chip8Display::updateDisplayImage()
{
    displayImage.fill(backgroundColor);
    
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int index = y * WIDTH + x;
            if (currentFrameBuffer[index]) {
                displayImage.setPixelColor(x, y, pixelColor);
            }
        }
    }
}
