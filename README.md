# Semaine 1 — Découverte de Qt Creator 🦝

> Projet pédagogique | BTS CIEL | Qt / C++ / Raspberry Pi

---

## Objectif

Première application Qt en C++. L'objectif de cette semaine était de prendre en main l'environnement **Qt Creator** et de comprendre la structure d'un projet Qt from scratch.

---

## Ce que fait l'application

Une fenêtre simple avec un bouton **"Afficher un message"** qui ouvre une boîte de dialogue Qt au clic.

```
┌─────────────────────────────────────┐
│   Semaine 1 - Découverte de Qt      │
│                                     │
│       [ Afficher un message ]       │
│                                     │
└─────────────────────────────────────┘
```

---

## Structure du projet

```
appS1/
├── CMakeLists.txt       # Configuration du projet Qt
├── main.cpp             # Point d'entrée de l'application
├── cihmapps1.h          # Déclaration de la classe IHM
├── cihmapps1.cpp        # Implémentation de la classe IHM
└── cihmapps1.ui         # Interface graphique (Qt Designer)
```

---

## Notions abordées

| Notion | Description |
|--------|-------------|
| Structure projet Qt | Rôle de chaque fichier `.h` / `.cpp` / `.ui` / `CMakeLists.txt` |
| QMainWindow | Classe de base pour une fenêtre principale Qt |
| QPushButton | Bouton cliquable |
| QMessageBox | Boîte de dialogue d'information |
| Signaux / Slots | Connexion explicite avec la syntaxe moderne Qt5 |
| Qt Designer | Création de l'IHM via le fichier `.ui` |

---

## Compilation et exécution

### Sur Raspberry Pi (Linux)
```bash
git clone https://github.com/viklib/rasberry_pi_ciel.git
cd rasberry_pi_ciel
git checkout Semaine_1
cmake .
make
./appS1
```

### Sous Windows (Qt Creator)
Ouvrir `CMakeLists.txt` dans Qt Creator puis `Ctrl+R`.

---

## Connexion signal/slot utilisée

```cpp
// Connexion explicite Qt5 — bonne pratique
connect(ui->pbMessage, &QPushButton::clicked,
        this, &CIhmAppS1::slotAfficherMessage);
```

---

## Progression du projet

| Semaine | Contenu | Statut |
|---------|---------|--------|
| S1 | Découverte Qt Creator, première fenêtre | ✅ Terminé |
| S2 | IHM complète, signaux/slots | 🔄 En cours |
| S3 | QTimer, acquisition périodique | ⏳ A venir |
| S4 | GPIO, commande LED | ⏳ A venir |
| S5 | Capteur I2C SHT20 | ⏳ A venir |
| S6 | Communication TCP / Série | ⏳ A venir |
| S7 | Structuration logicielle | ⏳ A venir |
| S8 | Finalisation + Soutenance | ⏳ A venir |

---

*BTS CIEL — 2025/2026* 🦝
