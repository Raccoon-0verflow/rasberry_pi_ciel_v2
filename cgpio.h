#ifndef CGPIO_H
#define CGPIO_H

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QThread>

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#define GPIO_IN  0
#define GPIO_OUT 1

/*!
 * \brief Classe CGpio
 *        Gère une broche GPIO de la Raspberry Pi
 *        via le système de fichiers sysfs Linux.
 */
class CGpio : public QObject
{
    Q_OBJECT

public:
    explicit CGpio(QObject *parent = nullptr, int addr = 4, int dir = GPIO_OUT);
    ~CGpio();

    int lire();
    int ecrire(int value);

signals:
    void erreur(QString msg);

private:
    int     m_addr;
    QString m_cheminGpio;

    int gpioExport();
    int gpioUnexport();
    int gpioDirection(int dir);
    int gpioInit();
};

#endif // CGPIO_H
