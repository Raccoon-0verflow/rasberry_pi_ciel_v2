# Semaine 2 — Interface graphique et Signaux/Slots

> Projet pédagogique | BTS CIEL | Qt / C++ / Raspberry Pi

---

## Objectif

Deuxième semaine de formation Qt. L'objectif est de créer une **interface graphique complète** avec Qt Designer et de maîtriser le mécanisme de communication entre objets Qt : les **signaux et les slots**.

---

## Ce que fait l'application

L'utilisateur saisit son prénom, choisit une couleur dans une liste déroulante et clique sur **Valider** pour afficher un message personnalisé. Un bouton **Effacer** remet tous les champs à zéro.

```
┌─────────────────────────────────────────┐
│       Semaine 2 - Signaux / Slots       │
│                                         │
│  Prénom :  [ Jean...            ]       │
│  Couleur : [ Rouge ▼            ]       │
│                                         │
│       [ Valider ]  [ Effacer ]          │
│                                         │
│   Bonjour Jean ! Couleur : Rouge        │
│                                         │
└─────────────────────────────────────────┘
```

---

## Structure du projet

```
appS2/
├── CMakeLists.txt       # Configuration du projet Qt
├── main.cpp             # Point d'entrée de l'application
├── cihmapps2.h          # Déclaration de la classe IHM
├── cihmapps2.cpp        # Implémentation de la classe IHM
└── cihmapps2.ui         # Interface graphique (Qt Designer)
```

---

## Notions abordées

| Notion | Description |
|--------|-------------|
| Qt Designer | Création visuelle de l'IHM via le fichier `.ui` |
| QLineEdit | Champ de saisie de texte |
| QComboBox | Liste déroulante avec valeurs prédéfinies |
| QPushButton | Bouton déclencheur d'action |
| QLabel | Affichage dynamique d'un résultat |
| Signaux / Slots | Connexions explicites avec la syntaxe Qt5 moderne |
| setStyleSheet | Mise en forme dynamique des widgets |

---

## Widgets utilisés

| Widget | Nom (objectName) | Rôle |
|--------|-----------------|------|
| `QLabel` | `laTitre` | Titre de la fenêtre |
| `QLineEdit` | `lePrenom` | Saisie du prénom |
| `QComboBox` | `cbCouleur` | Choix de la couleur |
| `QPushButton` | `pbValider` | Valide et affiche le message |
| `QPushButton` | `pbEffacer` | Efface tous les champs |
| `QLabel` | `laResultat` | Affiche le message personnalisé |

---

## Connexions signaux/slots

```cpp
// Connexions explicites Qt5 — bonne pratique
connect(ui->pbValider, &QPushButton::clicked,
        this, &CIhmAppS2::slotValider);

connect(ui->pbEffacer, &QPushButton::clicked,
        this, &CIhmAppS2::slotEffacer);
```

---

## Gestion des cas particuliers

```cpp
// Vérification que le prénom n'est pas vide
if (prenom.isEmpty()) {
    ui->laResultat->setText("Veuillez entrer votre prenom !");
    ui->laResultat->setStyleSheet("color: red;");
    return;
}
```

---

## Compilation et exécution

### Sur Raspberry Pi (Linux)
```bash
git clone https://github.com/viklib/rasberry_pi_ciel.git
cd rasberry_pi_ciel
git checkout Semaine_2
cmake .
make
./appS2
```

### Sous Windows (Qt Creator)
Ouvrir `CMakeLists.txt` dans Qt Creator puis `Ctrl+R`.

---

## Progression du projet

| Semaine | Contenu | Statut |
|---------|---------|--------|
| S1 | Découverte Qt Creator, première fenêtre | ✅ Terminé |
| S2 | IHM complète, signaux/slots | ✅ Terminé |
| S3 | QTimer, acquisition périodique | ⏳ A venir |
| S4 | GPIO, commande LED | ⏳ A venir |
| S5 | Capteur I2C SHT20 | ⏳ A venir |
| S6 | Communication TCP / Série | ⏳ A venir |
| S7 | Structuration logicielle | ⏳ A venir |
| S8 | Finalisation + Soutenance | ⏳ A venir |

---

*BTS CIEL — 2025/2026*
