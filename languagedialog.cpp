#include "languagedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QDir>
#include <QDebug>

LanguageDialog::LanguageDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Language Settings"));
    setModal(true);
    
    // Get currently selected language from settings
    QSettings settings;
    selectedLanguageCode = settings.value("language", "en_US").toString();
    
    setupUI();
    populateLanguageList();
    
    // Select the current language in the list
    for (int i = 0; i < languageList->count(); ++i) {
        QListWidgetItem* item = languageList->item(i);
        QString langCode = item->data(Qt::UserRole).toString();
        if (langCode == selectedLanguageCode) {
            languageList->setCurrentItem(item);
            break;
        }
    }
}

void LanguageDialog::setupUI()
{
    auto* mainLayout = new QVBoxLayout(this);
    
    auto* label = new QLabel(tr("Select your preferred language:"));
    mainLayout->addWidget(label);
    
    languageList = new QListWidget();
    connect(languageList, &QListWidget::itemClicked, this, &LanguageDialog::onLanguageSelected);
    mainLayout->addWidget(languageList);
    
    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    applyButton = new QPushButton(tr("Apply"));
    applyButton->setEnabled(false);
    connect(applyButton, &QPushButton::clicked, this, &LanguageDialog::onApplyClicked);
    buttonLayout->addWidget(applyButton);
    
    auto* cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    resize(300, 200);
}

void LanguageDialog::populateLanguageList()
{
    // Clear the list
    languageList->clear();
    availableLanguages.clear();
    
    // Add English (US) as default
    availableLanguages["English (US)"] = "en_US";
    
    // Add Spanish
    availableLanguages["Español"] = "es";
    
    // Find available translations in resources
    QDir resourceDir(":/translations");
    QStringList qmFiles = resourceDir.entryList(QStringList("*.qm"), QDir::Files);
    
    // Also check local translations directory
    QDir localDir("translations");
    if (localDir.exists()) {
        qmFiles.append(localDir.entryList(QStringList("*.qm"), QDir::Files));
    }
    
    // Add any additional translations found
    for (const QString& qmFile : qmFiles) {
        // Extract language code from filename (Chip8-UI_xx_YY.qm -> xx_YY)
        QString langCode = qmFile.mid(9, qmFile.length() - 12);
        
        // Skip already added languages
        if (availableLanguages.values().contains(langCode)) {
            continue;
        }
        
        // Get the display name for this language code
        // For simplicity, we'll use the language code as the display name for others
        availableLanguages[langCode] = langCode;
    }
    
    // Populate the list widget
    for (auto it = availableLanguages.begin(); it != availableLanguages.end(); ++it) {
        QListWidgetItem* item = new QListWidgetItem(it.key(), languageList);
        item->setData(Qt::UserRole, it.value());
    }
}

void LanguageDialog::onLanguageSelected(QListWidgetItem* item)
{
    if (!item) return;
    
    QString langCode = item->data(Qt::UserRole).toString();
    bool languageChanged = (langCode != selectedLanguageCode);
    
    // Only enable the Apply button if the language changed
    applyButton->setEnabled(languageChanged);
    
    if (languageChanged) {
        selectedLanguageCode = langCode;
    }
}

void LanguageDialog::onApplyClicked()
{
    // Save the selected language in settings
    QSettings settings;
    settings.setValue("language", selectedLanguageCode);
    
    accept();
}

QString LanguageDialog::getSelectedLanguageCode() const
{
    return selectedLanguageCode;
}
