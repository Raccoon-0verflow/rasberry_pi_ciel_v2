#include "cihmappgpio.h"
#include "ui_cihmappgpio.h"

/*!
 * \brief Constructeur : initialise l'IHM
 */
CIhmAppGpio::CIhmAppGpio(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::CIhmAppGpio),
    m_gpio(nullptr)
{
    ui->setupUi(this);

    // Connexions manuelles et explicites
    connect(ui->pbActiver, &QPushButton::clicked, this, &CIhmAppGpio::slotActiverGpio);
    connect(ui->pbSS,      &QPushButton::clicked, this, &CIhmAppGpio::slotBasculerLed);

    // Remplissage de la combobox
    QList<int> broches = {4, 5, 6, 12, 13, 16, 17, 18,
                          19, 20, 21, 22, 23, 24, 25, 26, 27};
    for (int broche : broches) {
        ui->cbGpio->addItem(QString::number(broche));
    }
    ui->cbGpio->setCurrentText("22");

    // pbSS désactivé tant que GPIO non initialisée
    ui->pbSS->setEnabled(false);

    // Affichage initial
    majAffichageLed(false);
}

/*!
 * \brief Destructeur
 */
CIhmAppGpio::~CIhmAppGpio()
{
    delete m_gpio;
    delete ui;
}

/*!
 * \brief Slot : initialisation de la GPIO sélectionnée
 */
void CIhmAppGpio::slotActiverGpio()
{
    // Suppression propre si déjà existant
    if (m_gpio != nullptr) {
        delete m_gpio;
        m_gpio = nullptr;
    }

    // Création de l'objet GPIO
    int broche = ui->cbGpio->currentText().toInt();
    m_gpio = new CGpio(this, broche, GPIO_OUT);

    // Connexion signal erreur
    connect(m_gpio, &CGpio::erreur, this, &CIhmAppGpio::slotErreurGpio);

    // Désactivation du bouton Activer
    ui->pbActiver->setEnabled(false);

    // Activation du bouton SS
    ui->pbSS->setEnabled(true);

    qDebug() << "[IHM] GPIO" << broche << "activée.";
}

/*!
 * \brief Slot : bascule l'état de la LED
 */
void CIhmAppGpio::slotBasculerLed()
{
    if (m_gpio == nullptr) return;

    if (ui->pbSS->text() == "Allumer") {
        m_gpio->ecrire(1);
        majAffichageLed(true);
        ui->pbSS->setText("Eteindre");
    } else {
        m_gpio->ecrire(0);
        majAffichageLed(false);
        ui->pbSS->setText("Allumer");
    }
}

/*!
 * \brief Met à jour l'affichage de la LED
 */
void CIhmAppGpio::majAffichageLed(bool allumee)
{
    if (allumee) {
        ui->laImage->setText("LED ALLUMEE");
        ui->laImage->setStyleSheet(
            "color: orange; font-weight: bold; font-size: 16px;");
    } else {
        ui->laImage->setText("LED ETEINTE");
        ui->laImage->setStyleSheet(
            "color: gray; font-weight: bold; font-size: 16px;");
    }
}

/*!
 * \brief Slot : affiche une erreur GPIO
 */
void CIhmAppGpio::slotErreurGpio(QString msg)
{
    qDebug() << "[IHM] Erreur GPIO :" << msg;
    QMessageBox::critical(this, "Erreur GPIO", msg);
}
