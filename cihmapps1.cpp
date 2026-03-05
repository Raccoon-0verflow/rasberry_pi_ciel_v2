#include "cihmapps1.h"
#include "ui_cihmapps1.h"

CIhmAppS1::CIhmAppS1(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::CIhmAppS1)
{
    ui->setupUi(this);

    // Connexion explicite du bouton
    connect(ui->pbMessage, &QPushButton::clicked,
            this, &CIhmAppS1::slotAfficherMessage);
}

CIhmAppS1::~CIhmAppS1()
{
    delete ui;
}

/*!
 * \brief Slot : affiche un message a l'utilisateur
 */
void CIhmAppS1::slotAfficherMessage()
{
    QMessageBox::information(this, "Message", "Bonjour depuis Qt !");
}
