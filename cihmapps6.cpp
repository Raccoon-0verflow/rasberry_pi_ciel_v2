#include "cihmapps6.h"
#include "ui_cihmapps6.h"

/*!
 * \brief Constructeur : initialise tous les composants,
 *        connecte les signaux/slots, cree le fichier CSV
 */
CIhmAppS6::CIhmAppS6(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::CIhmAppS6),
    m_capteur(nullptr),
    m_gpio(nullptr),
    m_timer(nullptr),
    m_socket(nullptr),
    m_fichierCsv(nullptr),
    m_nbTramesTcp(0),
    m_nbLignesCsv(0)
{
    ui->setupUi(this);

    // ── Capteur SHT20 et timer d'acquisition ──
    m_capteur = new CSht20(this);
    m_timer   = new QTimer(this);
    m_timer->setInterval(2000); // Acquisition toutes les 2 secondes

    // ── Socket TCP ──
    m_socket = new QTcpSocket(this);

    // ── Remplissage combobox GPIO ──
    QList<int> broches = {4, 5, 6, 12, 13, 16, 17, 18,
                          19, 20, 21, 22, 23, 24, 25, 26, 27};
    for (int b : broches)
        ui->cbGpio->addItem(QString::number(b));
    ui->cbGpio->setCurrentText("22");

    // ── Connexions signaux/slots — capteur, GPIO, timer ──
    connect(ui->pbDemarrer, &QPushButton::clicked,
            this, &CIhmAppS6::slotDemarrerArreter);
    connect(ui->pbActiver,  &QPushButton::clicked,
            this, &CIhmAppS6::slotActiverGpio);
    connect(m_timer,        &QTimer::timeout,
            this, &CIhmAppS6::slotAcquisition);
    connect(m_capteur,      &CSht20::erreur,
            this, &CIhmAppS6::slotErreurCapteur);

    // ── Connexions signaux/slots — TCP ──
    connect(ui->pbConnecter, &QPushButton::clicked,
            this, &CIhmAppS6::slotConnecter);
    connect(m_socket, &QTcpSocket::connected,
            this, &CIhmAppS6::slotTcpConnecte);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &CIhmAppS6::slotTcpDeconnecte);
    connect(m_socket,
            QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &CIhmAppS6::slotTcpErreur);

    // ── Etat initial IHM ──
    ui->laTemperature->setText("-- °C");
    ui->laHumidite->setText("-- %");
    ui->laTrame->setText("--");
    majAffichageLed(ETAT_ETEINT);
    majStatutTcp(false);

    // ── Creation du fichier CSV ──
    initialiserCsv();
}

/*!
 * \brief Destructeur : arrete proprement timer, socket et fichier CSV
 */
CIhmAppS6::~CIhmAppS6()
{
    if (m_timer->isActive())
        m_timer->stop();

    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();

    if (m_fichierCsv && m_fichierCsv->isOpen())
        m_fichierCsv->close();

    delete ui;
}

// ═══════════════════════════════════════════════════════
//  SECTION 1 : CAPTEUR / GPIO  (repris et adaptes de S5)
// ═══════════════════════════════════════════════════════

/*!
 * \brief Slot : demarre ou arrete l'acquisition periodique
 */
void CIhmAppS6::slotDemarrerArreter()
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
 * \brief Slot : initialise la broche GPIO choisie pour la LED
 */
void CIhmAppS6::slotActiverGpio()
{
    if (m_gpio != nullptr) {
        delete m_gpio;
        m_gpio = nullptr;
    }

    int broche = ui->cbGpio->currentText().toInt();
    m_gpio = new CGpio(this, broche, GPIO_OUT);

    // Connexion apres instanciation pour ne pas rater les erreurs d'init
    connect(m_gpio, &CGpio::erreur,
            this, &CIhmAppS6::slotErreurGpio);

    ui->pbActiver->setEnabled(false);
    qDebug() << "[IHM] GPIO" << broche << "activee.";
}

/*!
 * \brief Slot principal : lecture capteur, commande LED,
 *        puis envoi TCP + ecriture CSV
 *        Appele toutes les 2 secondes par le QTimer
 */
void CIhmAppS6::slotAcquisition()
{
    // ── Lecture capteur ──
    float temperature = m_capteur->lireTemperature();
    float humidite    = m_capteur->lireHumidite();

    // ── Mise a jour affichage valeurs ──
    ui->laTemperature->setText(QString::number(temperature, 'f', 1) + " °C");
    ui->laHumidite->setText(QString::number(humidite, 'f', 1) + " %");

    // ── Lecture seuils en direct (modifiables sans arreter l'acquisition) ──
    float seuilMin = ui->leSeuilMin->text().toFloat();
    float seuilMax = ui->leSeuilMax->text().toFloat();
    if (seuilMax < seuilMin) seuilMax = seuilMin; // Protection incoherence

    // ── Determination de l'etat LED ──
    EtatLed etat;
    if (temperature >= seuilMax) {
        etat = ETAT_MAX;
    } else if (temperature >= seuilMin) {
        etat = ETAT_INTERMEDIAIRE;
    } else {
        etat = ETAT_ETEINT;
    }

    // ── Commande LED physique ──
    if (m_gpio != nullptr)
        m_gpio->ecrire(etat == ETAT_ETEINT ? 0 : 1);

    // ── Mise a jour affichage LED ──
    majAffichageLed(etat);

    // ── Envoi TCP + ecriture CSV ──
    envoyerDonnees(temperature, humidite, etat);
}

/*!
 * \brief Met a jour le label LED avec couleur selon l'etat
 */
void CIhmAppS6::majAffichageLed(EtatLed etat)
{
    switch (etat) {
    case ETAT_MAX:
        ui->laLed->setText("LED ALLUMEE — Seuil MAX depasse !");
        ui->laLed->setStyleSheet("color: red; font-weight: bold; font-size: 13px;");
        break;
    case ETAT_INTERMEDIAIRE:
        ui->laLed->setText("LED ALLUMEE — Zone intermediaire");
        ui->laLed->setStyleSheet("color: orange; font-weight: bold; font-size: 13px;");
        break;
    case ETAT_ETEINT:
    default:
        ui->laLed->setText("LED ETEINTE — Temperature normale");
        ui->laLed->setStyleSheet("color: green; font-weight: bold; font-size: 13px;");
        break;
    }
}

/*!
 * \brief Slot : erreur capteur SHT20
 */
void CIhmAppS6::slotErreurCapteur(QString msg)
{
    qDebug() << "[IHM] Erreur capteur :" << msg;
    QMessageBox::critical(this, "Erreur capteur", msg);
}

/*!
 * \brief Slot : erreur GPIO
 */
void CIhmAppS6::slotErreurGpio(QString msg)
{
    qDebug() << "[IHM] Erreur GPIO :" << msg;
    QMessageBox::critical(this, "Erreur GPIO", msg);
}

// ═══════════════════════════════════════════════════════
//  SECTION 2 : COMMUNICATION TCP
// ═══════════════════════════════════════════════════════

/*!
 * \brief Slot : connecte ou deconnecte le socket TCP
 *        selon l'IP et le port saisis dans l'IHM
 */
void CIhmAppS6::slotConnecter()
{
    // Si deja connecte : on deconnecte
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        return;
    }

    QString ip   = ui->leIp->text().trimmed();
    quint16 port = static_cast<quint16>(ui->lePort->text().toUInt());

    if (ip.isEmpty() || port == 0) {
        QMessageBox::warning(this, "TCP", "Adresse IP ou port invalide.");
        return;
    }

    qDebug() << "[TCP] Tentative de connexion vers" << ip << "port" << port;
    m_socket->connectToHost(ip, port);
}

/*!
 * \brief Slot : connexion TCP etablie avec succes
 */
void CIhmAppS6::slotTcpConnecte()
{
    qDebug() << "[TCP] Connexion etablie.";
    majStatutTcp(true);
}

/*!
 * \brief Slot : connexion TCP perdue ou fermee
 */
void CIhmAppS6::slotTcpDeconnecte()
{
    qDebug() << "[TCP] Deconnexion.";
    majStatutTcp(false);
}

/*!
 * \brief Slot : erreur TCP (hote injoignable, timeout, etc.)
 */
void CIhmAppS6::slotTcpErreur(QAbstractSocket::SocketError erreur)
{
    Q_UNUSED(erreur)
    qDebug() << "[TCP] Erreur :" << m_socket->errorString();
    majStatutTcp(false);
    QMessageBox::warning(this, "Erreur TCP", m_socket->errorString());
}

/*!
 * \brief Met a jour le label statut et le texte du bouton TCP
 */
void CIhmAppS6::majStatutTcp(bool connecte)
{
    if (connecte) {
        ui->laStatutTcp->setText("Connecte — " + ui->leIp->text()
                                                                 + ":" + ui->lePort->text());
        ui->laStatutTcp->setStyleSheet("color: green; font-weight: bold;");
        ui->pbConnecter->setText("Deconnecter");
    } else {
        ui->laStatutTcp->setText("Non connecte");
        ui->laStatutTcp->setStyleSheet("color: red; font-weight: bold;");
        ui->pbConnecter->setText("Connecter");
    }
}

// ═══════════════════════════════════════════════════════
//  SECTION 3 : CSV + ENVOI DONNEES
// ═══════════════════════════════════════════════════════

/*!
 * \brief Cree le fichier CSV avec un nom horodate unique
 *        et ecrit la ligne d'en-tete des colonnes.
 *
 *        Nom     : data_YYYYMMDD_HHMMSS.csv
 *        En-tete : datetime;temperature_c;humidite_pct;etat_led
 *
 *        Le nom horodate evite tout ecrasement entre deux sessions.
 */
void CIhmAppS6::initialiserCsv()
{
    QString nomFichier = QString("data_%1.csv")
    .arg(QDateTime::currentDateTime()
             .toString("yyyyMMdd_HHmmss"));

    m_fichierCsv = new QFile(nomFichier, this);

    if (!m_fichierCsv->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "[CSV] Impossible de creer le fichier :" << nomFichier;
        return;
    }

    // En-tete avec nommage explicite des colonnes
    // date et heure separes pour compatibilite Excel
    QTextStream out(m_fichierCsv);
    out << "date;heure;temperature_c;humidite_pct;etat_led\n";
    m_fichierCsv->flush();

    qDebug() << "[CSV] Fichier cree :" << nomFichier;
}

/*!
 * \brief Construit la trame CSV a partir des mesures actuelles
 *
 *        Format : datetime;temperature_c;humidite_pct;etat_led
 *        Exemple : 2024-01-15 14:32:05;24.5;58.2;NORMAL
 *
 * \param temperature  Valeur lue en degres Celsius
 * \param humidite     Valeur lue en pourcentage
 * \param etat         Etat courant de la LED
 * \return             Trame formatee (sans '\n')
 */
QString CIhmAppS6::construireTrame(float temperature, float humidite, EtatLed etat)
{
    QDateTime now = QDateTime::currentDateTime();

    // Date et heure dans deux colonnes separees pour compatibilite Excel
    QString date      = now.toString("dd/MM/yyyy"); // Format reconnu par Excel FR
    QString heure     = now.toString("HH:mm:ss");

    // Virgule decimale pour compatibilite Excel FR (qui utilise , et non .)
    QString temp      = QString::number(temperature, 'f', 1).replace('.', ',');
    QString humi      = QString::number(humidite,    'f', 1).replace('.', ',');

    QString etatTexte = etatLedVersTexte(etat);

    return QString("%1;%2;%3;%4;%5").arg(date, heure, temp, humi, etatTexte);
}

/*!
 * \brief Convertit un EtatLed en chaine de texte pour le CSV
 */
QString CIhmAppS6::etatLedVersTexte(EtatLed etat)
{
    switch (etat) {
    case ETAT_MAX:           return "SEUIL_MAX";
    case ETAT_INTERMEDIAIRE: return "INTERMEDIAIRE";
    case ETAT_ETEINT:
    default:                 return "NORMAL";
    }
}

/*!
 * \brief Envoie les donnees via TCP et les ecrit dans le CSV
 *        Appele a chaque tick du timer (toutes les 2 secondes)
 *
 *        - Si le socket est connecte : envoi de la trame via TCP
 *        - Toujours : ecriture dans le fichier CSV local
 *        - Mise a jour des labels IHM (derniere trame + compteurs)
 *
 * \param temperature  Valeur lue sur le capteur
 * \param humidite     Valeur lue sur le capteur
 * \param etat         Etat courant de la LED
 */
void CIhmAppS6::envoyerDonnees(float temperature, float humidite, EtatLed etat)
{
    QString trame = construireTrame(temperature, humidite, etat);

    // ── Envoi TCP (seulement si connecte) ──
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write((trame + "\n").toUtf8());
        m_socket->flush();
        m_nbTramesTcp++;
        qDebug() << "[TCP] Envoye :" << trame;
    }

    // ── Ecriture CSV (toujours, connexion TCP ou non) ──
    if (m_fichierCsv && m_fichierCsv->isOpen()) {
        QTextStream out(m_fichierCsv);
        out << trame << "\n";
        m_fichierCsv->flush(); // Flush immediat = pas de perte en cas de crash
        m_nbLignesCsv++;
    }

    // ── Mise a jour IHM ──
    ui->laTrame->setText(trame);
    ui->laCompteur->setText(
        QString("Trames envoyees : %1  |  Enregistrees CSV : %2")
            .arg(m_nbTramesTcp)
            .arg(m_nbLignesCsv)
        );
}