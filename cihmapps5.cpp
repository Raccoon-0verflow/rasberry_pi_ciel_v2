#include "cihmapps5.h"
#include "ui_cihmapps5.h"

CIhmAppS5::CIhmAppS5(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::CIhmAppS5),
    m_capteur(nullptr),
    m_gpio(nullptr),
    m_timer(nullptr)
{
    ui->setupUi(this);

    // Initialisation du capteur et du timer
    m_capteur = new CSht20(this);
    m_timer   = new QTimer(this);
    m_timer->setInterval(2000); // Acquisition toutes les 2 secondes

    // Remplissage combobox GPIO
    QList<int> broches = {4, 5, 6, 12, 13, 16, 17, 18,
                          19, 20, 21, 22, 23, 24, 25, 26, 27};
    for (int b : broches)
        ui->cbGpio->addItem(QString::number(b));
    ui->cbGpio->setCurrentText("22");

    // Connexions explicites
    connect(ui->pbDemarrer,  &QPushButton::clicked,
            this, &CIhmAppS5::slotDemarrerArreter);
    connect(ui->pbActiver,   &QPushButton::clicked,
            this, &CIhmAppS5::slotActiverGpio);
    connect(m_timer,         &QTimer::timeout,
            this, &CIhmAppS5::slotAcquisition);
    connect(m_capteur,       &CSht20::erreur,
            this, &CIhmAppS5::slotErreurCapteur);

    // Etat initial
    ui->pbActiver->setEnabled(true);
    ui->laTemperature->setText("-- °C");
    ui->laHumidite->setText("-- %");
    majAffichageLed(false);
}

CIhmAppS5::~CIhmAppS5()
{
    if (m_timer->isActive()) m_timer->stop();
    delete ui;
}

/*!
 * \brief Slot : demarre ou arrete l'acquisition
 */
void CIhmAppS5::slotDemarrerArreter()
{
    if (m_timer->isActive()) {
        m_timer->stop();
        ui->pbDemarrer->setText("Demarrer acquisition");
        qDebug() << "[IHM] Acquisition arretee.";
    } else {
        m_timer->start();
        ui->pbDemarrer->setText("Arreter acquisition");
        qDebug() << "[IHM] Acquisition demarree.";
    }
}

/*!
 * \brief Slot : initialise la broche GPIO pour la LED
 */
void CIhmAppS5::slotActiverGpio()
{
    if (m_gpio != nullptr) {
        delete m_gpio;
        m_gpio = nullptr;
    }

    int broche = ui->cbGpio->currentText().toInt();
    m_gpio = new CGpio(this, broche, GPIO_OUT);

    connect(m_gpio, &CGpio::erreur,
            this, &CIhmAppS5::slotErreurGpio);

    ui->pbActiver->setEnabled(false);
    qDebug() << "[IHM] GPIO" << broche << "activee.";
}

/*!
 * \brief Slot : lit le capteur et met a jour l'IHM
 *        Allume la LED si temperature > seuil
 */
void CIhmAppS5::slotAcquisition()
{
    float temperature = m_capteur->lireTemperature();
    float humidite    = m_capteur->lireHumidite();

    // Mise a jour affichage
    ui->laTemperature->setText(QString::number(temperature, 'f', 1) + " °C");
    ui->laHumidite->setText(QString::number(humidite, 'f', 1) + " %");

    // Gestion LED selon seuil
    float seuil = ui->leSeuil->text().toFloat();

    if (m_gpio != nullptr) {
        if (temperature >= seuil) {
            m_gpio->ecrire(1);
            majAffichageLed(true);
        } else {
            m_gpio->ecrire(0);
            majAffichageLed(false);
        }
    }
}

/*!
 * \brief Met a jour l'affichage de la LED
 */
void CIhmAppS5::majAffichageLed(bool allumee)
{
    if (allumee) {
        ui->laLed->setText("LED ALLUMEE - Seuil depasse !");
        ui->laLed->setStyleSheet("color: orange; font-weight: bold; font-size: 14px;");
    } else {
        ui->laLed->setText("LED ETEINTE");
        ui->laLed->setStyleSheet("color: gray; font-weight: bold; font-size: 14px;");
    }
}

/*!
 * \brief Slot : affiche une erreur capteur
 */
void CIhmAppS5::slotErreurCapteur(QString msg)
{
    qDebug() << "[IHM] Erreur capteur :" << msg;
    QMessageBox::critical(this, "Erreur capteur", msg);
}

/*!
 * \brief Slot : affiche une erreur GPIO
 */
void CIhmAppS5::slotErreurGpio(QString msg)
{
    qDebug() << "[IHM] Erreur GPIO :" << msg;
    QMessageBox::critical(this, "Erreur GPIO", msg);
}
