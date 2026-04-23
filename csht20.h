#ifndef CSHT20_H
#define CSHT20_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QThread>

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#endif

#define SHT20_ADRESSE    0x40
#define SHT20_CMD_TEMP   0xF3
#define SHT20_CMD_HUMI   0xF5

/*!
 * \brief Classe CSht20
 *        Lit la temperature et l'humidite
 *        depuis le capteur SHT20 via le bus I2C.
 */
class CSht20 : public QObject
{
    Q_OBJECT

public:
    explicit CSht20(QObject *parent = nullptr);
    ~CSht20();

    float lireTemperature();
    float lireHumidite();

signals:
    void erreur(QString msg);

private:
    int m_fd; // Descripteur du bus I2C

    int ouvrirI2C();
    void fermerI2C();
};

#endif // CSHT20_H
