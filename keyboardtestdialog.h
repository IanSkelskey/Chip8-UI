#ifndef KEYBOARDTESTDIALOG_H
#define KEYBOARDTESTDIALOG_H

#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMap>
#include <QIcon>

class Chip8Input;

class KeyboardTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KeyboardTestDialog(Chip8Input* input, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Chip8Input* input;
    QMap<uint8_t, QLabel*> keyLabels;
    
    void setupUI();
    void updateKeyDisplay(uint8_t key, bool pressed);
};

#endif // KEYBOARDTESTDIALOG_H
