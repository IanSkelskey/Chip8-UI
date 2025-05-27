#include "chip8display.h"
#include <QPainter>
#include <QResizeEvent>
#include <QMutexLocker>
#include <QDebug>

Chip8Display::Chip8Display(QWidget *parent)
    : QWidget(parent)
    , pixelColor(Qt::green)
    , backgroundColor(Qt::black)
    , displayImage(WIDTH, HEIGHT, QImage::Format_RGB32)
    , frameUpdateCount(0)
{
    // Initialize display buffer
    currentFrameBuffer.fill(false);
    displayImage.fill(backgroundColor);

    // Set focus policy to receive keyboard events
    setFocusPolicy(Qt::StrongFocus);
    
    // Set a minimum size for the widget
    setMinimumSize(WIDTH * 4, HEIGHT * 4);
    
    qDebug() << "Chip8Display initialized with size" << width() << "x" << height();
}

Chip8Display::~Chip8Display() = default;

void Chip8Display::onFrameUpdate(const std::array<bool, WIDTH * HEIGHT>& frameBuffer)
{
    QMutexLocker locker(&displayMutex);
    currentFrameBuffer = frameBuffer;
    updateDisplayImage();
    
    // Trigger a repaint using update() to ensure the UI thread processes it
    update();
    
    // Log every 60 frame updates (approximately once per second at 60Hz)
    frameUpdateCount++;
    if (frameUpdateCount % 60 == 0) {
        qDebug() << "Display frame updated" << frameUpdateCount << "times";
    }
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
    qDebug() << "Display resized to" << width() << "x" << height();
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
