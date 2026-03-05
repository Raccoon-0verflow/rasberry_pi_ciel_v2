#include "cihmapps2.h"
#include "ui_cihmapps2.h"

CIhmAppS2::CIhmAppS2(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::CIhmAppS2)
{
    ui->setupUi(this);

    // Remplissage de la combobox
    ui->cbCouleur->addItem("Rouge");
    ui->cbCouleur->addItem("Vert");
    ui->cbCouleur->addItem("Bleu");
    ui->cbCouleur->addItem("Jaune");
    ui->cbCouleur->addItem("Noir");

    // Connexions explicites
    connect(ui->pbValider, &QPushButton::clicked,
            this, &CIhmAppS2::slotValider);
    connect(ui->pbEffacer, &QPushButton::clicked,
            this, &CIhmAppS2::slotEffacer);

    // Label resultat vide au démarrage
    ui->laResultat->setText("");
}

CIhmAppS2::~CIhmAppS2()
{
    delete ui;
}

/*!
 * \brief Slot : affiche un message personnalise
 *        avec le prenom et la couleur choisis
 */
void CIhmAppS2::slotValider()
{
    QString prenom  = ui->lePrenom->text();
    QString couleur = ui->cbCouleur->currentText();

    // Verification que le prenom n'est pas vide
    if (prenom.isEmpty()) {
        ui->laResultat->setText("Veuillez entrer votre prenom !");
        ui->laResultat->setStyleSheet("color: red;");
        return;
    }

    // Affichage du message personnalise
    ui->laResultat->setText("Bonjour " + prenom + " ! Couleur preferee : " + couleur);
    ui->laResultat->setStyleSheet("color: green; font-weight: bold;");
}

/*!
 * \brief Slot : efface tous les champs
 */
void CIhmAppS2::slotEffacer()
{
    ui->lePrenom->clear();
    ui->cbCouleur->setCurrentIndex(0);
    ui->laResultat->setText("");
    ui->laResultat->setStyleSheet("");
}
