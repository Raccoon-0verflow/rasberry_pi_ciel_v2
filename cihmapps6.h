#ifndef CIHMAPPS6_H
#define CIHMAPPS6_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QTcpSocket>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "csht20.h"
#include "cgpio.h"

namespace Ui {
class CIhmAppS6;
}

/*!
 * \brief Enumeration des etats possibles de la LED
 *        Reprise de S5, inchangee
 */
enum EtatLed {
    ETAT_ETEINT,         // Temperature en dessous du seuil Min → LED eteinte
    ETAT_INTERMEDIAIRE,  // Temperature entre seuil Min et Max → LED orange
    ETAT_MAX             // Temperature au dessus du seuil Max → LED rouge
};

/*!
 * \brief Classe CIhmAppS6
 *
 *        Semaine 6 : reprise du projet S5 (capteur SHT20, seuils Min/Max,
 *        commande LED) avec ajout de la communication TCP et de la
 *        sauvegarde CSV horodatee.
 *
 *        Fonctionnement :
 *        - Toutes les 2 secondes, le capteur est lu
 *        - Les donnees sont envoyees via TCP si connecte
 *        - Les donnees sont ecrites dans un fichier CSV local
 *        - La LED est commandee selon les seuils Min/Max
 *
 *        Format trame (TCP et CSV) :
 *        datetime;temperature_c;humidite_pct;etat_led
 *        Exemple : 2024-01-15 14:32:05;24.5;58.2;NORMAL
 */
class CIhmAppS6 : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhmAppS6(QWidget *parent = nullptr);
    ~CIhmAppS6();

private slots:
    // ── Capteur / GPIO (repris de S5) ──
    void slotDemarrerArreter();
    void slotActiverGpio();
    void slotAcquisition();
    void slotErreurCapteur(QString msg);
    void slotErreurGpio(QString msg);

    // ── Communication TCP ──
    void slotConnecter();
    void slotTcpConnecte();
    void slotTcpDeconnecte();
    void slotTcpErreur(QAbstractSocket::SocketError erreur);

private:
    Ui::CIhmAppS6 *ui;
    CSht20        *m_capteur;      // Capteur SHT20 via I2C
    CGpio         *m_gpio;         // LED via sysfs GPIO
    QTimer        *m_timer;        // Timer acquisition 2s
    QTcpSocket    *m_socket;       // Socket TCP client
    QFile         *m_fichierCsv;   // Fichier CSV local

    int m_nbTramesTcp;   // Compteur de trames envoyees via TCP
    int m_nbLignesCsv;   // Compteur de lignes ecrites dans le CSV

    // ── Methodes privees ──
    void    majAffichageLed(EtatLed etat);
    void    majStatutTcp(bool connecte);
    void    initialiserCsv();
    void    envoyerDonnees(float temperature, float humidite, EtatLed etat);
    QString construireTrame(float temperature, float humidite, EtatLed etat);
    QString etatLedVersTexte(EtatLed etat);
};

#endif // CIHMAPPS6_H