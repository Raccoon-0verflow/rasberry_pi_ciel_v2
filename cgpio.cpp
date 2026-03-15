#include "cgpio.h"

/*!
 * \brief Constructeur : exporte et configure la broche GPIO
 */
CGpio::CGpio(QObject *parent, int addr, int dir)
    : QObject(parent), m_addr(addr)
{
    m_cheminGpio = QString("/sys/class/gpio/gpio%1/").arg(m_addr);

    gpioExport();
    gpioInit();
    gpioDirection(dir);

    qDebug() << "[CGpio] Broche GPIO" << m_addr << "initialisée.";
}

/*!
 * \brief Destructeur : libère la broche GPIO
 */
CGpio::~CGpio()
{
    gpioUnexport();
    qDebug() << "[CGpio] Broche GPIO" << m_addr << "libérée.";
}

/*!
 * \brief Exporte la broche GPIO
 */
int CGpio::gpioExport()
{
#ifdef Q_OS_LINUX
    QFile fExport("/sys/class/gpio/export");

    if (!fExport.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString msg = QString("[CGpio] Erreur export GPIO%1").arg(m_addr);
        qDebug() << msg;
        emit erreur(msg);
        return -1;
    }

    fExport.write(QString::number(m_addr).toStdString().c_str());
    fExport.close();
#else
    qDebug() << "[CGpio] Export GPIO simulé (Windows)";
#endif
    return 0;
}

/*!
 * \brief Libère la broche GPIO
 */
int CGpio::gpioUnexport()
{
#ifdef Q_OS_LINUX
    QFile fUnexport("/sys/class/gpio/unexport");

    if (!fUnexport.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString msg = QString("[CGpio] Erreur unexport GPIO%1").arg(m_addr);
        qDebug() << msg;
        emit erreur(msg);
        return -1;
    }

    fUnexport.write(QString::number(m_addr).toStdString().c_str());
    fUnexport.close();
#else
    qDebug() << "[CGpio] Unexport GPIO simulé (Windows)";
#endif
    return 0;
}

/*!
 * \brief Configure les droits d'accès sysfs
 */
int CGpio::gpioInit()
{
#ifdef Q_OS_LINUX
    QString commande = "sudo chmod -R 777 " + m_cheminGpio;
    QProcess *proc = new QProcess(this);
    proc->start(commande);
    sleep(1);
#else
    qDebug() << "[CGpio] Init GPIO simulée (Windows)";
    QThread::msleep(100);
#endif
    return 0;
}

/*!
 * \brief Configure la direction de la broche
 */
int CGpio::gpioDirection(int dir)
{
#ifdef Q_OS_LINUX
    QFile fDirection(m_cheminGpio + "direction");

    if (!fDirection.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString msg = QString("[CGpio] Erreur direction GPIO%1").arg(m_addr);
        qDebug() << msg;
        emit erreur(msg);
        return -1;
    }

    fDirection.write(dir == GPIO_IN ? "in" : "out");
    fDirection.close();
#else
    qDebug() << "[CGpio] Direction GPIO simulée (Windows) :" << (dir == GPIO_IN ? "IN" : "OUT");
#endif
    return 0;
}

/*!
 * \brief Lit la valeur de la broche
 */
int CGpio::lire()
{
#ifdef Q_OS_LINUX
    QFile fValue(m_cheminGpio + "value");

    if (!fValue.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString msg = QString("[CGpio] Erreur lecture GPIO%1").arg(m_addr);
        qDebug() << msg;
        emit erreur(msg);
        return -1;
    }

    QByteArray val = fValue.read(1);
    fValue.close();
    return val.toInt();
#else
    qDebug() << "[CGpio] Lecture GPIO simulée (Windows)";
    return 0;
#endif
}

/*!
 * \brief Ecrit une valeur sur la broche
 */
int CGpio::ecrire(int value)
{
#ifdef Q_OS_LINUX
    QFile fValue(m_cheminGpio + "value");

    if (!fValue.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString msg = QString("[CGpio] Erreur ecriture GPIO%1").arg(m_addr);
        qDebug() << msg;
        emit erreur(msg);
        return -1;
    }

    fValue.write(value == 0 ? "0" : "1");
    fValue.close();
#else
    qDebug() << "[CGpio] Ecriture GPIO simulée (Windows) : valeur =" << value;
#endif
    return 0;
}
