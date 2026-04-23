#include "cihmapps6.h"
#include "ui_cihmapps6.h"

// ═══════════════════════════════════════════════════════════════
//  CONSTRUCTEUR / DESTRUCTEUR
// ═══════════════════════════════════════════════════════════════

/*!
 * \brief Constructeur : initialise tous les objets, remplit les
 *        listes deroulantes, connecte tous les signaux/slots,
 *        cree le fichier CSV et prepare l'IHM.
 */
CIhmAppS6::CIhmAppS6(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::CIhmAppS6),
    m_capteur(nullptr),
    m_gpio(nullptr),
    m_timer(nullptr),
    m_socket(nullptr),
    m_serie(nullptr),
    m_fichierCsv(nullptr),
    m_nbTramesTcp(0),
    m_nbTramesSerie(0),
    m_nbLignesCsv(0)
{
    ui->setupUi(this);

    // ── Capteur SHT20 et timer d'acquisition ──────────────────
    m_capteur = new CSht20(this);
    m_timer   = new QTimer(this);
    m_timer->setInterval(2000); // Acquisition toutes les 2 secondes

    // ── Socket TCP ────────────────────────────────────────────
    m_socket = new QTcpSocket(this);

    // ── Port serie RS232 ──────────────────────────────────────
    m_serie = new QSerialPort(this);

    // ── Remplissage combobox GPIO ─────────────────────────────
    QList<int> broches = {4, 5, 6, 12, 13, 16, 17, 18,
                          19, 20, 21, 22, 23, 24, 25, 26, 27};
    for (int b : broches)
        ui->cbGpio->addItem(QString::number(b));
    ui->cbGpio->setCurrentText("22");

    // ── Remplissage des ports series disponibles ──────────────
    // QSerialPortInfo scanne automatiquement les ports presents
    // sur la machine (COM1, COM3 sous Windows ; /dev/ttyAMA0
    // /dev/ttyUSB0 etc. sous Linux/Raspberry)
    slotScanPorts();

    // ── Remplissage des vitesses (baudrates) disponibles ──────
    remplirBaudrates();

    // ── Connexions signaux/slots — capteur, GPIO, timer ───────
    connect(ui->pbDemarrer, &QPushButton::clicked,
            this, &CIhmAppS6::slotDemarrerArreter);
    connect(ui->pbActiver,  &QPushButton::clicked,
            this, &CIhmAppS6::slotActiverGpio);
    connect(m_timer,        &QTimer::timeout,
            this, &CIhmAppS6::slotAcquisition);
    connect(m_capteur,      &CSht20::erreur,
            this, &CIhmAppS6::slotErreurCapteur);

    // ── Connexions signaux/slots — TCP ────────────────────────
    connect(ui->pbConnecter, &QPushButton::clicked,
            this, &CIhmAppS6::slotConnecter);
    connect(m_socket, &QTcpSocket::connected,
            this, &CIhmAppS6::slotTcpConnecte);
    connect(m_socket, &QTcpSocket::disconnected,
            this, &CIhmAppS6::slotTcpDeconnecte);
    // QOverload est necessaire car Qt6 a plusieurs surcharges
    // du signal errorOccurred — on precise laquelle on veut
    connect(m_socket,
            QOverload<QAbstractSocket::SocketError>::of(
                &QAbstractSocket::errorOccurred),
            this, &CIhmAppS6::slotTcpErreur);

    // ── Connexions signaux/slots — RS232 ──────────────────────
    connect(ui->pbOuvrirSerie, &QPushButton::clicked,
            this, &CIhmAppS6::slotOuvrirFermerSerie);
    connect(ui->pbScanPorts, &QPushButton::clicked,
            this, &CIhmAppS6::slotScanPorts);
    // Signal d'erreur du port serie
    connect(m_serie,
            QOverload<QSerialPort::SerialPortError>::of(
                &QSerialPort::errorOccurred),
            this, &CIhmAppS6::slotErreurSerie);

    // ── Etat initial IHM ──────────────────────────────────────
    ui->laTemperature->setText("-- °C");
    ui->laHumidite->setText("-- %");
    ui->laTrame->setText("--");
    majAffichageLed(ETAT_ETEINT);
    majStatutTcp(false);
    majStatutSerie(false);

    // ── Creation du fichier CSV ───────────────────────────────
    initialiserCsv();
}

/*!
 * \brief Destructeur : arrete proprement le timer, ferme le socket
 *        TCP, ferme le port serie et ferme le fichier CSV.
 */
CIhmAppS6::~CIhmAppS6()
{
    // Arreter l'acquisition
    if (m_timer->isActive())
        m_timer->stop();

    // Fermer la connexion TCP proprement
    if (m_socket->state() == QAbstractSocket::ConnectedState)
        m_socket->disconnectFromHost();

    // Fermer le port serie proprement
    if (m_serie->isOpen())
        m_serie->close();

    // Fermer le fichier CSV et s'assurer que tout est ecrit sur le disque
    if (m_fichierCsv && m_fichierCsv->isOpen())
        m_fichierCsv->close();

    delete ui;
}

// ═══════════════════════════════════════════════════════════════
//  SECTION 1 : CAPTEUR / GPIO  (repris et adaptes de S5)
// ═══════════════════════════════════════════════════════════════

/*!
 * \brief Slot : demarre ou arrete l'acquisition periodique.
 *        Appele par le bouton "Demarrer / Arreter acquisition".
 *        Le texte du bouton change selon l'etat du timer.
 */
void CIhmAppS6::slotDemarrerArreter()
{
    if (m_timer->isActive()) {
        // Timer actif → on l'arrete
        m_timer->stop();
        ui->pbDemarrer->setText("Demarrer acquisition");
        qDebug() << "[IHM] Acquisition arretee.";
    } else {
        // Timer inactif → on le demarre
        m_timer->start();
        ui->pbDemarrer->setText("Arreter acquisition");
        qDebug() << "[IHM] Acquisition demarree.";
    }
}

/*!
 * \brief Slot : initialise la broche GPIO choisie pour la LED.
 *        Appele par le bouton "Activer GPIO".
 *        Si une broche etait deja initialisee, on la libere d'abord.
 */
void CIhmAppS6::slotActiverGpio()
{
    // Liberer la broche precedente si elle existe
    if (m_gpio != nullptr) {
        delete m_gpio;
        m_gpio = nullptr;
    }

    int broche = ui->cbGpio->currentText().toInt();
    m_gpio = new CGpio(this, broche, GPIO_OUT);

    // Connexion apres instanciation pour capturer les erreurs d'init
    connect(m_gpio, &CGpio::erreur,
            this, &CIhmAppS6::slotErreurGpio);

    ui->pbActiver->setEnabled(false);
    qDebug() << "[IHM] GPIO" << broche << "activee.";
}

/*!
 * \brief Slot principal d'acquisition : appele toutes les 2 secondes
 *        par le QTimer. Lit le capteur, determine l'etat de la LED,
 *        commande la LED physique, met a jour l'IHM, puis envoie
 *        les donnees sur TCP, RS232 et dans le fichier CSV.
 */
void CIhmAppS6::slotAcquisition()
{
    // ── Lecture capteur SHT20 via I2C ─────────────────────────
    float temperature = m_capteur->lireTemperature();
    float humidite    = m_capteur->lireHumidite();

    // ── Mise a jour affichage des valeurs dans l'IHM ──────────
    ui->laTemperature->setText(QString::number(temperature, 'f', 1) + " °C");
    ui->laHumidite->setText(QString::number(humidite, 'f', 1) + " %");

    // ── Lecture des seuils en direct depuis l'IHM ─────────────
    // Les seuils sont relus a chaque tick : pas besoin de relancer
    // l'acquisition pour modifier les seuils, c'est immediat.
    float seuilMin = ui->leSeuilMin->text().toFloat();
    float seuilMax = ui->leSeuilMax->text().toFloat();

    // Protection : si Max < Min on evite un comportement incoherent
    if (seuilMax < seuilMin) seuilMax = seuilMin;

    // ── Determination de l'etat LED selon les seuils ──────────
    EtatLed etat;
    if (temperature >= seuilMax) {
        etat = ETAT_MAX;          // Au dessus du max → alerte rouge
    } else if (temperature >= seuilMin) {
        etat = ETAT_INTERMEDIAIRE; // Entre min et max → alerte orange
    } else {
        etat = ETAT_ETEINT;       // En dessous du min → tout va bien
    }

    // ── Commande LED physique via GPIO ────────────────────────
    // LED allumee si temperature depasse le seuil Min
    if (m_gpio != nullptr)
        m_gpio->ecrire(etat == ETAT_ETEINT ? 0 : 1);

    // ── Mise a jour affichage LED dans l'IHM ──────────────────
    majAffichageLed(etat);

    // ── Envoi donnees sur TCP + RS232 + ecriture CSV ──────────
    envoyerDonnees(temperature, humidite, etat);
}

/*!
 * \brief Met a jour le label LED avec la couleur et le texte
 *        correspondant a l'etat passe en parametre.
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
 * \brief Slot : affiche un message d'erreur capteur SHT20
 */
void CIhmAppS6::slotErreurCapteur(QString msg)
{
    qDebug() << "[IHM] Erreur capteur :" << msg;
    QMessageBox::critical(this, "Erreur capteur", msg);
}

/*!
 * \brief Slot : affiche un message d'erreur GPIO
 */
void CIhmAppS6::slotErreurGpio(QString msg)
{
    qDebug() << "[IHM] Erreur GPIO :" << msg;
    QMessageBox::critical(this, "Erreur GPIO", msg);
}

// ═══════════════════════════════════════════════════════════════
//  SECTION 2 : COMMUNICATION TCP
// ═══════════════════════════════════════════════════════════════

/*!
 * \brief Slot : connecte ou deconnecte le socket TCP.
 *        Si deja connecte → deconnecte.
 *        Si non connecte → tente une connexion avec l'IP et le port
 *        saisis dans l'IHM (modifiables sans redemarrer).
 */
void CIhmAppS6::slotConnecter()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        // Deja connecte → on deconnecte
        m_socket->disconnectFromHost();
        return;
    }

    QString  ip   = ui->leIp->text().trimmed();
    quint16  port = static_cast<quint16>(ui->lePort->text().toUInt());

    if (ip.isEmpty() || port == 0) {
        QMessageBox::warning(this, "TCP", "Adresse IP ou port invalide.");
        return;
    }

    qDebug() << "[TCP] Connexion vers" << ip << "port" << port;
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
 * \brief Slot : connexion TCP perdue ou fermee par le serveur
 */
void CIhmAppS6::slotTcpDeconnecte()
{
    qDebug() << "[TCP] Deconnexion.";
    majStatutTcp(false);
}

/*!
 * \brief Slot : erreur TCP (hote injoignable, timeout, refus...)
 *        Le message d'erreur exact est fourni par Qt via errorString()
 */
void CIhmAppS6::slotTcpErreur(QAbstractSocket::SocketError erreur)
{
    Q_UNUSED(erreur) // On utilise errorString() qui est plus lisible
    qDebug() << "[TCP] Erreur :" << m_socket->errorString();
    majStatutTcp(false);
    QMessageBox::warning(this, "Erreur TCP", m_socket->errorString());
}

/*!
 * \brief Met a jour le label statut TCP et le texte du bouton
 * \param connecte true = connexion active, false = deconnecte
 */
void CIhmAppS6::majStatutTcp(bool connecte)
{
    if (connecte) {
        ui->laStatutTcp->setText("Connecte — "
                                 + ui->leIp->text()
                                 + ":" + ui->lePort->text());
        ui->laStatutTcp->setStyleSheet("color: green; font-weight: bold;");
        ui->pbConnecter->setText("Deconnecter TCP");
    } else {
        ui->laStatutTcp->setText("Non connecte");
        ui->laStatutTcp->setStyleSheet("color: red; font-weight: bold;");
        ui->pbConnecter->setText("Connecter TCP");
    }
}

// ═══════════════════════════════════════════════════════════════
//  SECTION 3 : COMMUNICATION RS232 (SERIE)
// ═══════════════════════════════════════════════════════════════

/*!
 * \brief Slot : scanne et liste tous les ports series disponibles
 *        sur la machine et remplit la combobox.
 *
 *        Sous Windows : COM1, COM3, COM4...
 *        Sous Raspberry Pi : /dev/ttyAMA0 (UART natif via MAX232),
 *                            /dev/ttyUSB0 (adaptateur USB-Serie)
 *
 *        QSerialPortInfo::availablePorts() fait le scan
 *        automatiquement sur toutes les plateformes.
 */
void CIhmAppS6::slotScanPorts()
{
    ui->cbPortSerie->clear();

    // Recuperation de la liste des ports series disponibles
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    if (ports.isEmpty()) {
        // Aucun port detecte → on propose quand meme les valeurs
        // par defaut pour chaque OS afin de permettre la saisie manuelle
#ifdef Q_OS_LINUX
        ui->cbPortSerie->addItem("/dev/ttyAMA0"); // UART Pi via MAX232
        ui->cbPortSerie->addItem("/dev/ttyUSB0"); // Adaptateur USB-Serie
        ui->cbPortSerie->addItem("/dev/ttyS0");   // UART alternatif
#else
        ui->cbPortSerie->addItem("COM3");         // Windows simulation
        ui->cbPortSerie->addItem("COM4");
#endif
        qDebug() << "[RS232] Aucun port detecte, valeurs par defaut ajoutees.";
    } else {
        // Ports detectes → on les ajoute avec leur description
        for (const QSerialPortInfo &info : ports) {
            // Affiche : nom du port + description (ex: "USB Serial Device")
            QString label = info.portName();
            if (!info.description().isEmpty())
                label += " — " + info.description();
            ui->cbPortSerie->addItem(label, info.portName());
        }
        qDebug() << "[RS232]" << ports.count() << "port(s) detecte(s).";
    }
}

/*!
 * \brief Remplit la combobox des vitesses (baudrates) disponibles.
 *        Les baudrates les plus courants sont proposes.
 *        9600 est selectionne par defaut (vitesse standard RS232).
 */
void CIhmAppS6::remplirBaudrates()
{
    ui->cbBaudrate->addItem("1200",   QSerialPort::Baud1200);
    ui->cbBaudrate->addItem("2400",   QSerialPort::Baud2400);
    ui->cbBaudrate->addItem("4800",   QSerialPort::Baud4800);
    ui->cbBaudrate->addItem("9600",   QSerialPort::Baud9600);   // Defaut
    ui->cbBaudrate->addItem("19200",  QSerialPort::Baud19200);
    ui->cbBaudrate->addItem("38400",  QSerialPort::Baud38400);
    ui->cbBaudrate->addItem("57600",  QSerialPort::Baud57600);
    ui->cbBaudrate->addItem("115200", QSerialPort::Baud115200);

    // Selectionner 9600 par defaut (indice 3 dans la liste)
    ui->cbBaudrate->setCurrentIndex(3);
}

/*!
 * \brief Slot : ouvre ou ferme le port serie selon son etat actuel.
 *        Si ouvert → ferme.
 *        Si ferme → tente d'ouvrir avec le port et le baudrate
 *        selectionnes dans les combobox.
 *
 *        Configuration fixe : 8 bits de donnees, pas de parite,
 *        1 bit de stop (8N1) — configuration RS232 standard.
 */
void CIhmAppS6::slotOuvrirFermerSerie()
{
    if (m_serie->isOpen()) {
        // Port ouvert → on le ferme
        m_serie->close();
        majStatutSerie(false);
        qDebug() << "[RS232] Port ferme.";
        return;
    }

    // Recuperer le nom du port selectionne
    // Si la combobox a une userData (scan auto), on l'utilise
    // Sinon on prend le texte brut (avant le " — description")
    QString nomPort = ui->cbPortSerie->currentData().toString();
    if (nomPort.isEmpty()) {
        // Pas de userData → le texte contient peut-etre "— description"
        nomPort = ui->cbPortSerie->currentText().split(" — ").first().trimmed();
    }

    // Recuperer le baudrate selectionne via la userData de la combobox
    int baudrate = ui->cbBaudrate->currentData().toInt();

    // Configuration du port serie
    m_serie->setPortName(nomPort);
    m_serie->setBaudRate(baudrate);
    m_serie->setDataBits(QSerialPort::Data8);       // 8 bits de donnees
    m_serie->setParity(QSerialPort::NoParity);       // Pas de parite
    m_serie->setStopBits(QSerialPort::OneStop);      // 1 bit de stop
    m_serie->setFlowControl(QSerialPort::NoFlowControl); // Pas de controle de flux

    // Tentative d'ouverture en ecriture seule
    // (on envoie des donnees, on ne recoit rien du recepteur)
    if (!m_serie->open(QIODevice::WriteOnly)) {
        QString msg = "Impossible d'ouvrir " + nomPort
                      + " : " + m_serie->errorString();
        qDebug() << "[RS232]" << msg;
        QMessageBox::warning(this, "Erreur RS232", msg);
        majStatutSerie(false);
        return;
    }

    majStatutSerie(true);
    qDebug() << "[RS232] Port" << nomPort << "ouvert a" << baudrate << "bauds.";
}

/*!
 * \brief Slot : erreur sur le port serie.
 *        QSerialPort::NoError est emis au demarrage, on l'ignore.
 */
void CIhmAppS6::slotErreurSerie(QSerialPort::SerialPortError erreur)
{
    // NoError est emis normalement au demarrage → on l'ignore
    if (erreur == QSerialPort::NoError) return;

    QString msg = "[RS232] Erreur : " + m_serie->errorString();
    qDebug() << msg;

    // Si le port etait ouvert il est probablement maintenant inutilisable
    if (m_serie->isOpen()) {
        m_serie->close();
        majStatutSerie(false);
    }
}

/*!
 * \brief Met a jour le label statut RS232 et le texte du bouton
 * \param ouvert true = port ouvert, false = port ferme
 */
void CIhmAppS6::majStatutSerie(bool ouvert)
{
    if (ouvert) {
        ui->laStatutSerie->setText("Port ouvert — "
                                   + m_serie->portName()
                                   + " @ "
                                   + QString::number(m_serie->baudRate())
                                   + " bauds");
        ui->laStatutSerie->setStyleSheet("color: green; font-weight: bold;");
        ui->pbOuvrirSerie->setText("Fermer port serie");
    } else {
        ui->laStatutSerie->setText("Port ferme");
        ui->laStatutSerie->setStyleSheet("color: red; font-weight: bold;");
        ui->pbOuvrirSerie->setText("Ouvrir port serie");
    }
}

// ═══════════════════════════════════════════════════════════════
//  SECTION 4 : CSV + ENVOI DONNEES (TCP + RS232)
// ═══════════════════════════════════════════════════════════════

/*!
 * \brief Cree le fichier CSV avec un nom horodate unique
 *        et ecrit la ligne d'en-tete des colonnes.
 *
 *        Nom     : data_YYYYMMDD_HHMMSS.csv
 *        En-tete : date;heure;temperature_c;humidite_pct;etat_led
 *
 *        Le nom horodate garantit qu'un nouveau fichier est cree
 *        a chaque lancement → aucun risque d'ecrasement.
 */
void CIhmAppS6::initialiserCsv()
{
    QString nomFichier = QString("data_%1.csv")
                         .arg(QDateTime::currentDateTime()
                              .toString("yyyyMMdd_HHmmss"));

    m_fichierCsv = new QFile(nomFichier, this);

    if (!m_fichierCsv->open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "[CSV] Impossible de creer :" << nomFichier;
        return;
    }

    // En-tete avec colonnes nommees explicitement
    QTextStream out(m_fichierCsv);
    out << "date;heure;temperature_c;humidite_pct;etat_led\n";
    m_fichierCsv->flush();

    qDebug() << "[CSV] Fichier cree :" << nomFichier;
}

/*!
 * \brief Construit la trame de donnees au format CSV.
 *
 *        Format : date;heure;temperature_c;humidite_pct;etat_led
 *        Exemple : 15/01/2026;14:32:05;24,5;58,2;NORMAL
 *
 *        Notes de format pour compatibilite Excel FR :
 *        - Date en dd/MM/yyyy (format francais reconnu par Excel)
 *        - Separateur decimal virgule (pas point) → 24,5 et non 24.5
 *        - Colonnes separees par ; (separateur liste FR)
 *
 * \param temperature  Valeur lue en degres Celsius
 * \param humidite     Valeur lue en pourcentage
 * \param etat         Etat courant de la LED
 * \return             Trame formatee sans retour a la ligne
 */
QString CIhmAppS6::construireTrame(float temperature, float humidite, EtatLed etat)
{
    QDateTime now = QDateTime::currentDateTime();

    // Horodatage : date et heure dans deux colonnes separees
    QString date  = now.toString("dd/MM/yyyy"); // Format Excel FR
    QString heure = now.toString("HH:mm:ss");

    // Virgule decimale pour compatibilite Excel en francais
    QString temp = QString::number(temperature, 'f', 1).replace('.', ',');
    QString humi = QString::number(humidite,    'f', 1).replace('.', ',');

    QString etatTexte = etatLedVersTexte(etat);

    return QString("%1;%2;%3;%4;%5").arg(date, heure, temp, humi, etatTexte);
}

/*!
 * \brief Convertit un EtatLed en chaine de texte lisible pour le CSV
 * \return "NORMAL", "INTERMEDIAIRE" ou "SEUIL_MAX"
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
 * \brief Envoie les donnees sur tous les canaux actifs et
 *        les ecrit dans le fichier CSV local.
 *        Appele a chaque tick du timer (toutes les 2 secondes).
 *
 *        Canaux d'envoi :
 *        1. TCP   : si le socket est connecte
 *        2. RS232 : si le port serie est ouvert
 *        3. CSV   : toujours, meme si aucun canal n'est connecte
 *
 * \param temperature  Valeur lue sur le capteur
 * \param humidite     Valeur lue sur le capteur
 * \param etat         Etat courant de la LED
 */
void CIhmAppS6::envoyerDonnees(float temperature, float humidite, EtatLed etat)
{
    // Construction de la trame une seule fois pour tous les canaux
    QString trame = construireTrame(temperature, humidite, etat);

    // ── Canal 1 : envoi TCP ────────────────────────────────────
    // On envoie uniquement si le socket est dans l'etat "connecte"
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        // La trame est envoyee avec un \n pour delimiter les messages
        m_socket->write((trame + "\n").toUtf8());
        m_socket->flush(); // Envoi immediat sans attendre le buffer
        m_nbTramesTcp++;
        qDebug() << "[TCP] Envoye :" << trame;
    }

    // ── Canal 2 : envoi RS232 ──────────────────────────────────
    // On envoie uniquement si le port serie est ouvert
    if (m_serie->isOpen()) {
        // Meme format que TCP : trame + retour a la ligne
        // Le recepteur (PuTTY, Hercules, autre appareil) verra
        // les donnees ligne par ligne
        m_serie->write((trame + "\r\n").toUtf8());
        // \r\n (CRLF) est le standard RS232 — certains terminaux
        // n'affichent pas correctement avec \n seul
        m_serie->flush();
        m_nbTramesSerie++;
        qDebug() << "[RS232] Envoye :" << trame;
    }

    // ── Canal 3 : ecriture CSV locale ─────────────────────────
    // Toujours ecrit, meme si TCP et RS232 ne sont pas connectes
    // flush() immediat = pas de perte de donnees en cas de coupure
    if (m_fichierCsv && m_fichierCsv->isOpen()) {
        QTextStream out(m_fichierCsv);
        out << trame << "\n";
        m_fichierCsv->flush();
        m_nbLignesCsv++;
    }

    // ── Mise a jour IHM : derniere trame et compteurs ──────────
    ui->laTrame->setText(trame);
    ui->laCompteur->setText(
        QString("TCP : %1  |  RS232 : %2  |  CSV : %3")
        .arg(m_nbTramesTcp)
        .arg(m_nbTramesSerie)
        .arg(m_nbLignesCsv)
    );
}
