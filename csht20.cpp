#include "csht20.h"

CSht20::CSht20(QObject *parent)
    : QObject(parent), m_fd(-1)
{
    ouvrirI2C();
}

CSht20::~CSht20()
{
    fermerI2C();
}

/*!
 * \brief Ouvre le bus I2C et selectionne l'adresse du capteur
 */
int CSht20::ouvrirI2C()
{
#ifdef Q_OS_LINUX
    m_fd = open("/dev/i2c-1", O_RDWR);

    if (m_fd < 0) {
        QString msg = "[CSht20] Erreur ouverture /dev/i2c-1";
        qDebug() << msg;
        emit erreur(msg);
        return -1;
    }

    if (ioctl(m_fd, I2C_SLAVE, SHT20_ADRESSE) < 0) {
        QString msg = "[CSht20] Erreur adressage capteur 0x40";
        qDebug() << msg;
        emit erreur(msg);
        return -1;
    }

    qDebug() << "[CSht20] Bus I2C ouvert, capteur adresse.";
#else
    qDebug() << "[CSht20] Simulation Windows - I2C non disponible";
#endif
    return 0;
}

/*!
 * \brief Ferme le bus I2C
 */
void CSht20::fermerI2C()
{
#ifdef Q_OS_LINUX
    if (m_fd >= 0) {
        close(m_fd);
        qDebug() << "[CSht20] Bus I2C ferme.";
    }
#endif
}

/*!
 * \brief Lit la temperature depuis le capteur SHT20
 * \return Temperature en degres Celsius
 */
float CSht20::lireTemperature()
{
#ifdef Q_OS_LINUX
    if (m_fd < 0) return -1.0f;

    unsigned char cmd = SHT20_CMD_TEMP;
    if (write(m_fd, &cmd, 1) != 1) {
        emit erreur("[CSht20] Erreur envoi commande temperature");
        return -1.0f;
    }

    QThread::msleep(85); // Attente mesure (max 85ms)

    unsigned char buf[3];
    if (read(m_fd, buf, 3) != 3) {
        emit erreur("[CSht20] Erreur lecture temperature");
        return -1.0f;
    }

    // Formule de conversion SHT20
    unsigned int raw = (buf[0] << 8) | (buf[1] & 0xFC);
    float temperature = -46.85f + 175.72f * (raw / 65536.0f);
    return temperature;
#else
    // Simulation sous Windows : valeur aleatoire entre 20 et 30
    qDebug() << "[CSht20] Simulation temperature (Windows)";
    return 20.0f + (rand() % 100) / 10.0f;
#endif
}

/*!
 * \brief Lit l'humidite depuis le capteur SHT20
 * \return Humidite en pourcentage
 */
float CSht20::lireHumidite()
{
#ifdef Q_OS_LINUX
    if (m_fd < 0) return -1.0f;

    unsigned char cmd = SHT20_CMD_HUMI;
    if (write(m_fd, &cmd, 1) != 1) {
        emit erreur("[CSht20] Erreur envoi commande humidite");
        return -1.0f;
    }

    QThread::msleep(29); // Attente mesure (max 29ms)

    unsigned char buf[3];
    if (read(m_fd, buf, 3) != 3) {
        emit erreur("[CSht20] Erreur lecture humidite");
        return -1.0f;
    }

    // Formule de conversion SHT20
    unsigned int raw = (buf[0] << 8) | (buf[1] & 0xFC);
    float humidite = -6.0f + 125.0f * (raw / 65536.0f);
    return humidite;
#else
    // Simulation sous Windows : valeur aleatoire entre 40 et 70
    qDebug() << "[CSht20] Simulation humidite (Windows)";
    return 40.0f + (rand() % 300) / 10.0f;
#endif
}
