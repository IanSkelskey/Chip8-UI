#ifndef LANGUAGEDIALOG_H
#define LANGUAGEDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QMap>

class LanguageDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LanguageDialog(QWidget *parent = nullptr);
    QString getSelectedLanguageCode() const;

private slots:
    void onLanguageSelected(QListWidgetItem* item);
    void onApplyClicked();

private:
    void setupUI();
    void populateLanguageList();

    QListWidget* languageList;
    QPushButton* applyButton;
    QString selectedLanguageCode;
    QMap<QString, QString> availableLanguages; // Display name -> language code
};

#endif // LANGUAGEDIALOG_H