# Semaine 5 — Capteur I2C SHT20 et seuils de température

> Projet pédagogique | BTS CIEL | Qt / C++ / Raspberry Pi

---

## Objectif

Cinquième semaine de formation Qt. L'objectif est de lire la **température** et l'**humidité** depuis le capteur **SHT20** via le protocole **I2C**, d'afficher les valeurs en temps réel dans l'IHM, et de commander automatiquement une **LED** selon deux seuils de température réglables en direct.

---

## Ce que fait l'application

```
┌─────────────────────────────────────────────┐
│         Semaine 5 - Capteur SHT20           │
│                                             │
│  Temperature :      24.5 °C                 │
│  Humidite    :      58.2 %                  │
│                                             │
│  Seuil Min (°C) :  [ 20  ]                  │
│  Seuil Max (°C) :  [ 30  ]                  │
│  Broche LED :      [ 22 ▼ ]  [ Activer ]    │
│                                             │
│       [ Demarrer acquisition ]              │
│                                             │
│       LED ETEINTE - Temperature normale     │
│                                             │
└─────────────────────────────────────────────┘
```

**Fonctionnement :**
1. Cliquer sur **Activer** pour initialiser la broche GPIO de la LED
2. Régler les seuils **Min** et **Max** de température (modifiables à tout moment, sans relancer)
3. Cliquer sur **Demarrer acquisition** pour lancer la lecture du capteur toutes les 2 secondes
4. La LED se comporte selon 3 états automatiques :

| Température | État LED | Couleur IHM |
|-------------|----------|-------------|
| En dessous du seuil Min | LED éteinte | 🟢 Vert — *Temperature normale* |
| Entre Min et Max | LED allumée | 🟠 Orange — *Zone intermediaire* |
| Au dessus du seuil Max | LED allumée | 🔴 Rouge — *Seuil MAX depasse !* |

---

## Historique des versions

### v1.0 — Version initiale (création du code de base)

> *Code écrit en classe, fonctionnel sur Raspberry Pi*

**Objectif :** mettre en place la structure minimale du projet pour lire le capteur SHT20 et afficher les données dans l'IHM.

**Ce qui a été développé :**
- Classe `CSht20` : ouverture du bus I2C (`/dev/i2c-1`), adressage du capteur à `0x40`, lecture de la température et de l'humidité avec les formules de conversion SHT20
- Classe `CGpio` : réutilisée telle quelle depuis la Semaine 4, commande la LED via sysfs
- Classe `CIhmAppS5` : affichage des valeurs, un seul seuil de température (`leSeuil`), bouton Démarrer/Arrêter, combobox GPIO
- `QTimer` toutes les 2 secondes pour l'acquisition périodique
- Portabilité Windows avec `#ifdef Q_OS_LINUX` pour simuler les valeurs sans matériel

**Limitations identifiées :**
- Un seul seuil → seulement 2 états possibles pour la LED (allumée / éteinte)
- Pas de retour visuel différencié selon le niveau d'alerte
- L'état de la LED affiché dans `laLed` n'utilise pas d'énumération (magic values)

---

### v2.0 — Améliorations avec l'aide de Claude (IA)

> *Améliorations proposées par l'IA Claude (Anthropic), adaptées et intégrées par l'élève*

**Objectif :** enrichir la logique de seuil et améliorer la qualité du code C++.

#### Modification 1 — Double seuil Min / Max

Le seuil unique `leSeuil` a été remplacé par deux seuils distincts `leSeuilMin` et `leSeuilMax`, permettant de définir une **zone intermédiaire** entre les deux valeurs.

```
Avant (v1)          Après (v2)
──────────          ──────────
[  leSeuil  ]       [ leSeuilMin ]  [ leSeuilMax ]
  → 2 états           → 3 états
```

Fichiers modifiés : `cihmapps5.ui`, `cihmapps5.cpp`

#### Modification 2 — Énumération `EtatLed`

Les états de la LED sont maintenant représentés par une énumération C++ propre, ajoutée dans `cihmapps5.h` :

```cpp
enum EtatLed {
    ETAT_ETEINT,         // Température en dessous du seuil Min
    ETAT_INTERMEDIAIRE,  // Température entre seuil Min et Max
    ETAT_MAX             // Température au dessus du seuil Max
};
```

Cela remplace l'utilisation d'un simple `bool` et rend le code plus lisible et maintenable.

Fichiers modifiés : `cihmapps5.h`, `cihmapps5.cpp`

#### Modification 3 — Seuils modifiables en temps réel

Les seuils sont lus **directement depuis l'IHM à chaque acquisition** (toutes les 2 secondes), sans bouton "valider" et sans arrêter l'acquisition :

```cpp
// slotAcquisition() — lecture en direct à chaque tick du timer
float seuilMin = ui->leSeuilMin->text().toFloat();
float seuilMax = ui->leSeuilMax->text().toFloat();
```

Il suffit de modifier les valeurs dans les champs pendant que l'acquisition tourne — le changement est pris en compte automatiquement au prochain cycle.

Fichiers modifiés : `cihmapps5.cpp`

#### Modification 4 — Retour visuel coloré dans l'IHM

Le label `laLed` affiche désormais **3 messages distincts avec 3 couleurs** selon l'état :

| État | Message affiché | Couleur |
|------|----------------|---------|
| `ETAT_ETEINT` | LED ETEINTE — Temperature normale | 🟢 Vert |
| `ETAT_INTERMEDIAIRE` | LED ALLUMEE — Zone intermediaire | 🟠 Orange |
| `ETAT_MAX` | LED ALLUMEE — Seuil MAX depasse ! | 🔴 Rouge |

Fichiers modifiés : `cihmapps5.cpp`

---

### v3.0 — Débogage et stabilisation

> *Corrections appliquées après tests sur Raspberry Pi*

**Objectif :** corriger les comportements inattendus observés lors des tests réels.

#### Correction 1 — Ordre de connexion du signal `erreur` de `CGpio`

En v1/v2, le `connect` vers `slotErreurGpio` était effectué **avant** la création effective de l'objet `m_gpio`. En cas d'erreur lors de l'export GPIO (broche déjà utilisée par exemple), le signal était émis sans être connecté et l'erreur passait silencieusement.

**Correction :** le `connect` est maintenant effectué **immédiatement après** l'instanciation de `m_gpio` dans `slotActiverGpio()`.

#### Correction 2 — Protection contre un `leSeuilMax` inférieur à `leSeuilMin`

Si l'utilisateur saisit un Max inférieur au Min, la logique de comparaison produisait un comportement incohérent. Une vérification a été ajoutée :

```cpp
// Securisation des seuils dans slotAcquisition()
if (seuilMax < seuilMin) seuilMax = seuilMin;
```

#### Correction 3 — Arrêt propre du timer à la fermeture

Le destructeur `~CIhmAppS5()` vérifie que le timer est actif avant de l'arrêter, ce qui évite tout crash à la fermeture de l'application.

#### Bilan des tests

| Scénario testé | Résultat |
|----------------|----------|
| Lecture capteur SHT20 sur Raspberry Pi réelle | ✅ OK |
| Simulation Windows (valeurs aléatoires) | ✅ OK |
| Modification des seuils pendant acquisition | ✅ OK |
| Seuil Max < Seuil Min | ✅ Corrigé (v3) |
| Broche GPIO déjà utilisée | ✅ Corrigé (v3) |
| Fermeture de l'application pendant acquisition | ✅ OK |

---

## Structure du projet

```
appS5/
├── CMakeLists.txt          # Configuration du projet Qt
├── main.cpp                # Point d'entrée de l'application
├── csht20.h                # Déclaration de la classe capteur SHT20
├── csht20.cpp              # Implémentation de la classe capteur SHT20
├── cgpio.h                 # Déclaration de la classe GPIO (réutilisée depuis S4)
├── cgpio.cpp               # Implémentation de la classe GPIO
├── cihmapps5.h             # Déclaration de la classe IHM
├── cihmapps5.cpp           # Implémentation de la classe IHM
└── cihmapps5.ui            # Interface graphique (Qt Designer)
```

---

## Architecture POO

```
CIhmAppS5      (IHM uniquement — affichage et interactions)
      │
      ├── CSht20    (lecture température/humidité via I2C)
      ├── CGpio     (commande LED via sysfs — réutilisée depuis S4)
      └── QTimer    (acquisition périodique toutes les 2 secondes)
```

| Classe | Fichiers | Responsabilité |
|--------|----------|----------------|
| `CIhmAppS5` | `cihmapps5.h/.cpp/.ui` | Gestion de l'interface graphique uniquement |
| `CSht20` | `csht20.h/.cpp` | Lecture du capteur SHT20 via le bus I2C |
| `CGpio` | `cgpio.h/.cpp` | Commande de la LED via sysfs Linux |

---

## Le protocole I2C

Le bus I2C permet de communiquer avec des composants externes en utilisant seulement **2 fils** :

| Fil | Broche Raspberry | Rôle |
|-----|-----------------|------|
| SDA | GPIO2 (broche 3) | Données |
| SCL | GPIO3 (broche 5) | Horloge |

Le capteur SHT20 est accessible à l'adresse I2C **0x40**.

---

## Notions abordées

| Notion | Description |
|--------|-------------|
| Protocole I2C | Communication avec le capteur via `/dev/i2c-1` |
| `ioctl` | Sélection de l'adresse I2C du capteur |
| Formule SHT20 | Conversion des données brutes en température/humidité |
| QTimer | Acquisition périodique toutes les 2 secondes |
| Seuils dynamiques | Deux seuils Min/Max réglables en direct depuis l'IHM, sans arrêter l'acquisition |
| `enum EtatLed` | Enumération C++ pour représenter les 3 états possibles de la LED |
| Réutilisation POO | Classe CGpio réutilisée telle quelle depuis la S4 |

---

## Formules de conversion SHT20

```cpp
// Temperature (°C)
float temperature = -46.85f + 175.72f * (raw / 65536.0f);

// Humidite (%)
float humidite = -6.0f + 125.0f * (raw / 65536.0f);
```

---

## Portabilité Windows / Linux

Le code fonctionne sous Windows (simulation) et sur Raspberry Pi (réel) :

```cpp
#ifdef Q_OS_LINUX
    // Lecture réelle via /dev/i2c-1
    m_fd = open("/dev/i2c-1", O_RDWR);
#else
    // Simulation de valeurs aléatoires sous Windows
    return 20.0f + (rand() % 100) / 10.0f;
#endif
```

---

## Dépendances et installation

### Sur Windows (développement / simulation)

**Aucune dépendance système à installer.** Qt Creator suffit. Le code compile et tourne tel quel — les valeurs du capteur sont simulées automatiquement grâce au `#ifdef Q_OS_LINUX`.

---

### Sur Raspberry Pi (exécution réelle)

Avant de cloner et compiler le projet, il faut s'assurer que les dépendances suivantes sont installées.

#### 1. Mettre à jour le système

```bash
sudo apt update && sudo apt upgrade -y
```

#### 2. Qt5 — bibliothèques de développement

```bash
sudo apt install qtbase5-dev qtbase5-dev-tools qtchooser qt5-qmake
```

> Sur Debian Bullseye ou Kali Linux récent, le paquet `qt5-default` n'existe plus.
> La commande ci-dessus le remplace complètement.

#### 3. CMake

```bash
sudo apt install cmake
```

> Vérifier la version : `cmake --version` — il faut au minimum la **3.16** (requis dans `CMakeLists.txt`).

#### 4. Support I2C — `libi2c-dev`

Le code utilise les appels système Linux I2C (`open`, `ioctl`, `read`) déclarés dans `<linux/i2c-dev.h>`. Ce header fait partie du paquet `libi2c-dev` :

```bash
sudo apt install libi2c-dev i2c-tools
```

> `i2c-tools` n'est pas nécessaire pour compiler, mais il fournit la commande `i2cdetect` qui permet de vérifier que le capteur SHT20 est bien détecté avant de lancer l'application.

#### 5. Git

```bash
sudo apt install git
```

#### 6. Activation du bus I2C

Le bus I2C est désactivé par défaut sur certaines distributions. Pour l'activer :

```bash
# Charger le module immédiatement (sans redémarrer)
sudo modprobe i2c-dev

# Rendre l'activation permanente au démarrage
echo "i2c-dev" | sudo tee -a /etc/modules

# Activer dans la config matérielle
sudo nano /boot/config.txt
# → Ajouter ou décommenter la ligne : dtparam=i2c_arm=on

sudo reboot
```

---

### Récapitulatif des dépendances

| Dépendance | Obligatoire | Rôle | Commande |
|------------|-------------|------|----------|
| `qtbase5-dev` + `qt5-qmake` | ✅ Oui | Compiler le projet Qt | `sudo apt install qtbase5-dev qtbase5-dev-tools qt5-qmake` |
| `cmake` ≥ 3.16 | ✅ Oui | Générer les Makefiles | `sudo apt install cmake` |
| `libi2c-dev` | ✅ Oui | Header I2C (`linux/i2c-dev.h`) | `sudo apt install libi2c-dev` |
| `git` | ✅ Oui | Cloner le dépôt | `sudo apt install git` |
| `i2c-tools` | ⚙️ Recommandé | Vérifier le câblage (`i2cdetect`) | `sudo apt install i2c-tools` |

---

### Vérification matérielle avant de lancer

Une fois le câblage effectué (SDA → broche 3, SCL → broche 5), vérifier que le capteur est bien détecté :

```bash
sudo i2cdetect -y 1
```

Le capteur SHT20 doit apparaître à l'adresse **0x40** :

```
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
...
40: 40 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
...
```

> Si `0x40` n'apparaît pas : vérifier le câblage SDA/SCL et que le bus I2C est bien activé dans `/boot/config.txt`.

---

## Widgets utilisés

| Widget | Nom (objectName) | Rôle |
|--------|-----------------|------|
| `QLabel` | `laTemperature` | Affiche la température en °C |
| `QLabel` | `laHumidite` | Affiche l'humidité en % |
| `QLineEdit` | `leSeuilMin` | Seuil minimum réglable |
| `QLineEdit` | `leSeuilMax` | Seuil maximum réglable |
| `QComboBox` | `cbGpio` | Sélection de la broche GPIO pour la LED |
| `QPushButton` | `pbActiver` | Initialise la broche GPIO |
| `QPushButton` | `pbDemarrer` | Démarre / Arrête l'acquisition |
| `QLabel` | `laLed` | Affiche l'état de la LED avec couleur |

---

## Matériel requis

- Raspberry Pi 3 (ou compatible)
- Capteur SHT20 (adresse I2C 0x40)
- LED + résistance 330 ohms
- Connexion SDA → broche 3 (GPIO2)
- Connexion SCL → broche 5 (GPIO3)
- I2C activé sur la Raspberry (`/dev/i2c-1` disponible)

---

## Démarche de travail avec l'IA Claude

Durant cette semaine, l'IA **Claude** (Anthropic) a été utilisée comme outil d'aide au développement. La démarche a été la suivante :

1. **Analyse du code existant** — Claude a lu et expliqué les fichiers fournis, notamment le rôle du capteur SHT20 et son lien avec le protocole I2C
2. **Proposition d'améliorations** — Claude a suggéré le double seuil Min/Max et l'énumération `EtatLed`
3. **Adaptation des propositions** — Les suggestions ont été relues, comprises et intégrées après validation, pas copiées aveuglément
4. **Génération des fichiers modifiés** — `cihmapps5.h`, `cihmapps5.cpp` et `cihmapps5.ui` produits avec l'aide de Claude

---

## Progression du projet

| Semaine | Contenu | Statut |
|---------|---------|--------|
| S1 | Découverte Qt Creator, première fenêtre | ✅ Terminé |
| S2 | IHM complète, signaux/slots | ✅ Terminé |
| S3 | QTimer, acquisition périodique | ✅ Terminé |
| S4 | GPIO, commande LED | ✅ Terminé |
| S5 | Capteur I2C SHT20, seuils Min/Max | ✅ Terminé |
| S6 | Communication TCP / Série | ⏳ À venir |
| S7 | Structuration logicielle | ⏳ À venir |
| S8 | Finalisation + Soutenance | ⏳ À venir |

---

*BTS CIEL — 2025/2026*
