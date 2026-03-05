#ifndef CIHMAPPS1_H
#define CIHMAPPS1_H

#include <QMainWindow>
#include <QMessageBox>

namespace Ui {
class CIhmAppS1;
}

/*!
 * \brief Classe CIhmAppS1
 *        Semaine 1 : fenetre principale avec un bouton
 *        qui affiche un message a l'utilisateur.
 */
class CIhmAppS1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhmAppS1(QWidget *parent = nullptr);
    ~CIhmAppS1();

private slots:
    void slotAfficherMessage();

private:
    Ui::CIhmAppS1 *ui;
};

#endif // CIHMAPPS1_H
