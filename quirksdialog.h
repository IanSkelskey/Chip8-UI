#ifndef QUIRKSDIALOG_H
#define QUIRKSDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <map>
#include <memory>
#include "core/include/api/EmulatorAPI.hpp"

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QGroupBox;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

class QuirksDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuirksDialog(std::shared_ptr<Chip8Emu::API::EmulatorAPI> emulator, 
                         QWidget *parent = nullptr);

private slots:
    void onProfileChanged(int index);
    void onQuirkToggled();
    void onApplyClicked();
    void onResetClicked();

private:
    void setupUI();
    void createQuirkCheckbox(QVBoxLayout* layout, 
                           const QString& label, 
                           const QString& description,
                           Chip8Emu::Quirk quirk);
    void updateUIFromQuirks();
    void updateQuirksFromUI();
    bool isUpdatingUI = false;

    std::shared_ptr<Chip8Emu::API::EmulatorAPI> emulator;
    Chip8Emu::QuirksConfig currentQuirks;
    
    // UI elements
    QComboBox* profileCombo;
    std::map<Chip8Emu::Quirk, QCheckBox*> quirkCheckboxes;
};

#endif // QUIRKSDIALOG_H
