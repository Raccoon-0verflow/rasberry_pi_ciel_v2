# Semaine 4 — Commande GPIO et architecture POO 🦝

> Projet pédagogique | BTS CIEL | Qt / C++ / Raspberry Pi

---

## Objectif

Première semaine de projet concret sur Raspberry Pi. L'objectif est de commander une **LED physique** depuis une interface graphique Qt, en utilisant les broches GPIO de la Raspberry Pi via le système de fichiers **sysfs** de Linux.

Une architecture **POO rigoureuse** est mise en place dès cette semaine pour garantir la réutilisabilité du code dans les semaines suivantes.

---

## Ce que fait l'application

```
┌─────────────────────────────────────────┐
│         Commande LED GPIO               │
│                                         │
│    [ GPIO : [22 ▼] ]  [ Activer ]       │
│                                         │
│            ⚫ LED ETEINTE               │
│                                         │
│              [ Allumer ]                │
│                                         │
└─────────────────────────────────────────┘
```

1. Choisir la broche GPIO dans la liste déroulante
2. Cliquer sur **Activer** pour initialiser la broche
3. Cliquer sur **Allumer / Eteindre** pour basculer l'état de la LED
4. Le label se met à jour en temps réel

---

## Structure du projet

```
appGpio/
├── CMakeLists.txt          # Configuration du projet Qt
├── main.cpp                # Point d'entrée de l'application
├── cgpio.h                 # Déclaration de la classe GPIO
├── cgpio.cpp               # Implémentation de la classe GPIO
├── cihmappgpio.h           # Déclaration de la classe IHM
├── cihmappgpio.cpp         # Implémentation de la classe IHM
└── cihmappgpio.ui          # Interface graphique (Qt Designer)
```

---

## Architecture POO

Le projet sépare clairement l'interface graphique de la logique métier :

```
CIhmAppGpio        (IHM uniquement — affichage et interactions)
      │
      └── CGpio    (logique métier — accès GPIO via sysfs Linux)
```

| Classe | Fichiers | Responsabilité |
|--------|----------|----------------|
| `CIhmAppGpio` | `cihmappgpio.h/.cpp/.ui` | Gestion de l'interface graphique uniquement |
| `CGpio` | `cgpio.h/.cpp` | Accès aux broches GPIO via le système sysfs |

---

## Notions abordées

| Notion | Description |
|--------|-------------|
| GPIO sysfs Linux | Accès aux broches via `/sys/class/gpio/` |
| Architecture POO | Séparation IHM / logique métier |
| Héritage QObject | Classe CGpio hérite de QObject |
| Signal erreur Qt | Remontée d'erreurs de CGpio vers l'IHM |
| Portabilité | Directives `#ifdef Q_OS_LINUX` pour Windows/Linux |
| QComboBox | Liste déroulante pour choisir la broche |
| QMessageBox | Affichage des erreurs GPIO |

---

## Fonctionnement GPIO sysfs

Sur Raspberry Pi, les broches GPIO sont accessibles via des fichiers Linux :

```bash
# Exporter la broche
echo 22 > /sys/class/gpio/export

# Configurer en sortie
echo out > /sys/class/gpio/gpio22/direction

# Allumer la LED
echo 1 > /sys/class/gpio/gpio22/value

# Eteindre la LED
echo 0 > /sys/class/gpio/gpio22/value

# Libérer la broche
echo 22 > /sys/class/gpio/unexport
```

---

## Portabilité Windows / Linux

Le code est développé sous **Windows** avec Qt Creator et déployé sur **Raspberry Pi** sous Linux. Les appels système Linux sont isolés pour garantir la portabilité :

```cpp
#ifdef Q_OS_LINUX
    // Accès réel aux fichiers sysfs GPIO
    QFile fValue(m_cheminGpio + "value");
    fValue.write("1");
#else
    // Simulation sous Windows
    qDebug() << "[CGpio] Ecriture GPIO simulée (Windows)";
#endif
```

---

## Widgets IHM utilisés

| Widget | Nom Qt | Rôle |
|--------|--------|------|
| Liste déroulante | `cbGpio` | Sélection de la broche GPIO |
| Bouton Activer | `pbActiver` | Initialise la GPIO (une seule fois) |
| Label état | `laImage` | Affiche l'état de la LED |
| Bouton bascule | `pbSS` | Allume / Eteint la LED |

---

## Compilation et exécution

### Sur Raspberry Pi (Linux)
```bash
git clone https://github.com/viklib/rasberry_pi_ciel.git
cd rasberry_pi_ciel
git checkout Semaine_4
cmake .
make
./appGpio
```

### Sous Windows (Qt Creator)
Ouvrir `CMakeLists.txt` dans Qt Creator puis `Ctrl+R`.
L'IHM s'affiche mais les appels GPIO sont simulés (pas de matériel).

---

## Matériel requis

- Raspberry Pi 3 (ou compatible)
- LED + résistance 330 ohms
- Broche GPIO disponible (ex : GPIO22 = broche physique 15)
- Shield pédagogique BTS CIEL (optionnel)

---

## Progression du projet

| Semaine | Contenu | Statut |
|---------|---------|--------|
| S1 | Découverte Qt Creator, première fenêtre | ✅ Terminé |
| S2 | IHM complète, signaux/slots | ✅ Terminé |
| S3 | QTimer, acquisition périodique | ✅ Terminé |
| S4 | GPIO, commande LED | ✅ Terminé |
| S5 | Capteur I2C SHT20 | ⏳ A venir |
| S6 | Communication TCP / Série | ⏳ A venir |
| S7 | Structuration logicielle | ⏳ A venir |
| S8 | Finalisation + Soutenance | ⏳ A venir |

---

*BTS CIEL — 2025/2026* 🦝
