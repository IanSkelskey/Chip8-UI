#include "quirksdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QDialogButtonBox>

QuirksDialog::QuirksDialog(std::shared_ptr<Chip8Emu::API::EmulatorAPI> emulator,
                           QWidget *parent)
    : QDialog(parent)
    , emulator(emulator)
{
    setWindowTitle(tr("CHIP-8 Quirks Configuration"));
    setModal(true);

    // Get current quirks
    currentQuirks = emulator->getQuirks();

    setupUI();
    updateUIFromQuirks();
}

void QuirksDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);

    // Profile selection
    auto* profileLayout = new QHBoxLayout();
    profileLayout->addWidget(new QLabel(tr("Profile:")));

    profileCombo = new QComboBox();
    profileCombo->addItem(tr("Custom"));
    auto profiles = emulator->getAvailableQuirksProfiles();
    for (const auto& profile : profiles) {
        profileCombo->addItem(QString::fromStdString(profile));
    }
    connect(profileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &QuirksDialog::onProfileChanged);

    profileLayout->addWidget(profileCombo);
    profileLayout->addStretch();
    mainLayout->addLayout(profileLayout);

    // Quirks group
    auto* quirksGroup = new QGroupBox(tr("Individual Quirks"));
    auto* quirksLayout = new QVBoxLayout(quirksGroup);

    // Add checkboxes for each quirk with improved descriptions
    createQuirkCheckbox(quirksLayout,
                       tr("VF Reset"),
                       tr("Logical operations (AND, OR, XOR) reset VF to 0 (original behavior)"),
                       Chip8Emu::Quirk::VF_RESET);

    createQuirkCheckbox(quirksLayout,
                       tr("Memory Increment"),
                       tr("Load/Store opcodes increment I register (original behavior)"),
                       Chip8Emu::Quirk::MEMORY_INCREMENT);

    createQuirkCheckbox(quirksLayout,
                       tr("Display Wait"),
                       tr("Drawing operations wait for vertical blank (limits to 60Hz)"),
                       Chip8Emu::Quirk::DISPLAY_WAIT);

    createQuirkCheckbox(quirksLayout,
                       tr("Sprite Clipping"),
                       tr("Sprites clip at screen edges instead of wrapping around"),
                       Chip8Emu::Quirk::SPRITE_CLIPPING);

    createQuirkCheckbox(quirksLayout,
                       tr("Shift VX Only"),
                       tr("Shift opcodes only use VX (modern) vs. setting VX to VY before shift (original)"),
                       Chip8Emu::Quirk::SHIFT_VX);

    createQuirkCheckbox(quirksLayout,
                       tr("Jump with VX"),
                       tr("BNNN uses VX where X is high nibble of NNN (SUPER-CHIP) vs. always V0 (original)"),
                       Chip8Emu::Quirk::JUMP_VX);

    mainLayout->addWidget(quirksGroup);

    // Add more detailed information
    auto* infoLabel = new QLabel(tr(
        "These quirks control compatibility with different CHIP-8 implementations.\n\n"
        "• Modern profile is best for most recent games\n"
        "• COSMAC VIP profile matches the original 1970s implementation\n"
        "• CHIP-48 and SUPER-CHIP profiles support later variants\n\n"
        "Some games may require specific quirks to run correctly."));
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("QLabel { color: #444; padding: 10px; }");
    mainLayout->addWidget(infoLabel);

    // Buttons
    auto* buttonLayout = new QHBoxLayout();

    auto* resetButton = new QPushButton(tr("Reset to Defaults"));
    connect(resetButton, &QPushButton::clicked, this, &QuirksDialog::onResetClicked);
    buttonLayout->addWidget(resetButton);

    buttonLayout->addStretch();

    auto* applyButton = new QPushButton(tr("Apply"));
    applyButton->setDefault(true);
    connect(applyButton, &QPushButton::clicked, this, &QuirksDialog::onApplyClicked);
    buttonLayout->addWidget(applyButton);

    auto* cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    resize(400, 500);
}

void QuirksDialog::createQuirkCheckbox(QVBoxLayout* layout,
                                      const QString& label,
                                      const QString& description,
                                      Chip8Emu::Quirk quirk)
{
    auto* checkbox = new QCheckBox(label);
    checkbox->setToolTip(description);
    connect(checkbox, &QCheckBox::toggled, this, &QuirksDialog::onQuirkToggled);

    quirkCheckboxes[quirk] = checkbox;

    auto* hLayout = new QHBoxLayout();
    hLayout->addWidget(checkbox);

    auto* descLabel = new QLabel(QString("(%1)").arg(description));
    descLabel->setStyleSheet("QLabel { color: #888; font-size: 9pt; }");
    descLabel->setWordWrap(true);
    hLayout->addWidget(descLabel, 1);

    layout->addLayout(hLayout);
}

void QuirksDialog::updateUIFromQuirks()
{
    isUpdatingUI = true;

    for (auto& [quirk, checkbox] : quirkCheckboxes) {
        checkbox->setChecked(currentQuirks.getQuirk(quirk));
    }

    // Set profile combo to Custom
    profileCombo->setCurrentIndex(0);

    isUpdatingUI = false;
}

void QuirksDialog::updateQuirksFromUI()
{
    for (const auto& [quirk, checkbox] : quirkCheckboxes) {
        currentQuirks.setQuirk(quirk, checkbox->isChecked());
    }
}

void QuirksDialog::onProfileChanged(int index)
{
    if (isUpdatingUI) return;

    if (index == 0) {
        // Custom profile - do nothing
        return;
    }

    // Load the selected profile
    auto profiles = emulator->getAvailableQuirksProfiles();
    if (index > 0 && index <= static_cast<int>(profiles.size())) {
        currentQuirks.loadProfile(profiles[index - 1]);
        updateUIFromQuirks();
    }
}

void QuirksDialog::onQuirkToggled()
{
    if (isUpdatingUI) return;

    // When any quirk is toggled, set profile to Custom
    profileCombo->setCurrentIndex(0);
}

void QuirksDialog::onApplyClicked()
{
    updateQuirksFromUI();
    emulator->setQuirks(currentQuirks);
    accept();
}

void QuirksDialog::onResetClicked()
{
    currentQuirks.reset();
    updateUIFromQuirks();
}
