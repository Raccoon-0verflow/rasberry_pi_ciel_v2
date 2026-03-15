# Semaine 5 — Capteur I2C SHT20 et seuil de température

> Projet pédagogique | BTS CIEL | Qt / C++ / Raspberry Pi

---

## Objectif

Cinquième semaine de formation Qt. L'objectif est de lire la **température** et l'**humidité** depuis le capteur **SHT20** via le protocole **I2C**, d'afficher les valeurs en temps réel dans l'IHM, et de commander automatiquement une **LED** lorsque la température dépasse un seuil réglable.

---

## Ce que fait l'application

```
┌─────────────────────────────────────────────┐
│         Semaine 5 - Capteur SHT20           │
│                                             │
│  Temperature :      24.5 °C                 │
│  Humidite    :      58.2 %                  │
│                                             │
│  Seuil (°C) :      [ 25  ]                 │
│  Broche LED :      [ 22 ▼ ]  [ Activer ]   │
│                                             │
│       [ Demarrer acquisition ]              │
│                                             │
│           LED ETEINTE                       │
│                                             │
└─────────────────────────────────────────────┘
```

**Fonctionnement :**
1. Cliquer sur **Activer** pour initialiser la broche GPIO de la LED
2. Régler le seuil de température souhaité
3. Cliquer sur **Demarrer acquisition** pour lancer la lecture du capteur toutes les 2 secondes
4. Si la température dépasse le seuil → la LED s'allume automatiquement
5. Si la température repasse sous le seuil → la LED s'éteint automatiquement

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
| Seuil dynamique | Comparaison température / seuil réglable depuis l'IHM |
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

## Widgets utilisés

| Widget | Nom (objectName) | Rôle |
|--------|-----------------|------|
| `QLabel` | `laTemperature` | Affiche la température en °C |
| `QLabel` | `laHumidite` | Affiche l'humidité en % |
| `QLineEdit` | `leSeuil` | Seuil de température réglable |
| `QComboBox` | `cbGpio` | Sélection de la broche GPIO pour la LED |
| `QPushButton` | `pbActiver` | Initialise la broche GPIO |
| `QPushButton` | `pbDemarrer` | Démarre / Arrête l'acquisition |
| `QLabel` | `laLed` | Affiche l'état de la LED |

---

## Compilation et exécution

### Sur Raspberry Pi (Linux)
```bash
git clone https://github.com/viklib/rasberry_pi_ciel.git
cd rasberry_pi_ciel
git checkout Semaine_5
mkdir -p build && cd build
cmake ..
make
sudo ./appS5
```

### Sous Windows (Qt Creator)
Ouvrir `CMakeLists.txt` dans Qt Creator puis `Ctrl+R`.
Les valeurs affichées sont simulées (pas de capteur réel).

---

## Matériel requis

- Raspberry Pi 3 (ou compatible)
- Capteur SHT20 (adresse I2C 0x40)
- LED + résistance 330 ohms
- Connexion SDA → broche 3 (GPIO2)
- Connexion SCL → broche 5 (GPIO3)
- I2C activé sur la Raspberry (`/dev/i2c-1` disponible)

### Activer le I2C sur Kali Linux
```bash
sudo modprobe i2c-dev
echo "i2c-dev" | sudo tee -a /etc/modules
sudo nano /boot/config.txt  # Ajouter : dtparam=i2c_arm=on
sudo reboot
```

### Vérifier que le capteur est détecté
```bash
sudo i2cdetect -y 1
# Le capteur SHT20 doit apparaître à l'adresse 0x40
```

---

## Améliorations apportées avec l'aide de Claude (IA)

Certaines bonnes pratiques ont été suggérées par l'IA **Claude** (Anthropic) :

- **Portabilité Windows/Linux** — utilisation de `#ifdef Q_OS_LINUX` pour simuler le capteur sous Windows et permettre le développement sans matériel
- **Réutilisation de CGpio** — la classe GPIO développée en S4 est réutilisée sans modification, démontrant l'intérêt de l'architecture POO
- **Gestion des erreurs I2C** — remontée des erreurs d'ouverture du bus et d'adressage du capteur vers l'IHM via le mécanisme signaux/slots Qt

---

## Progression du projet

| Semaine | Contenu | Statut |
|---------|---------|--------|
| S1 | Découverte Qt Creator, première fenêtre | ✅ Terminé |
| S2 | IHM complète, signaux/slots | ✅ Terminé |
| S3 | QTimer, acquisition périodique | ✅ Terminé |
| S4 | GPIO, commande LED | ✅ Terminé |
| S5 | Capteur I2C SHT20 | ✅ Terminé |
| S6 | Communication TCP / Série | ⏳ A venir |
| S7 | Structuration logicielle | ⏳ A venir |
| S8 | Finalisation + Soutenance | ⏳ A venir |

---

*BTS CIEL — 2024/2025*
