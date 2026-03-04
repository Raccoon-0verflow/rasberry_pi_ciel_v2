#ifndef CIHMAPPGPIO_H
#define CIHMAPPGPIO_H

#include <QMainWindow>
#include <QMessageBox>
#include "cgpio.h"

namespace Ui {
class CIhmAppGpio;
}

/*!
 * \brief Classe CIhmAppGpio
 *        Gère uniquement l'affichage et les interactions utilisateur.
 *        La logique GPIO est déléguée à la classe CGpio.
 */
class CIhmAppGpio : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhmAppGpio(QWidget *parent = nullptr);
    ~CIhmAppGpio();

private slots:
    void slotActiverGpio();
    void slotBasculerLed();
    void slotErreurGpio(QString msg);

private:
    Ui::CIhmAppGpio *ui;
    CGpio           *m_gpio;

    void majAffichageLed(bool allumee);
};

#endif
