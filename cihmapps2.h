#ifndef CIHMAPPS2_H
#define CIHMAPPS2_H

#include <QMainWindow>

namespace Ui {
class CIhmAppS2;
}

/*!
 * \brief Classe CIhmAppS2
 *        Semaine 2 : IHM avec QLineEdit, QComboBox,
 *        QPushButton et QLabel connectes via signaux/slots.
 */
class CIhmAppS2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhmAppS2(QWidget *parent = nullptr);
    ~CIhmAppS2();

private slots:
    void slotValider();
    void slotEffacer();

private:
    Ui::CIhmAppS2 *ui;
};

#endif // CIHMAPPS2_H
