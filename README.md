# Projet Qt / Raspberry Pi — BTS CIEL 🦝

> Projet pédagogique annuel | BTS CIEL | Qt / C++ / Raspberry Pi
> Année scolaire **2025/2026**

---

## Présentation

Ce dépôt contient l'ensemble des projets développés semaine par semaine dans le cadre du cours **Qt / C++ sur Raspberry Pi**. Chaque semaine est sur une branche Git dédiée. Le projet monte en compétences progressivement : de la première fenêtre Qt jusqu'à une application complète avec capteur I2C, commande GPIO, communication TCP, RS232 et sauvegarde CSV.

Le matériel utilisé à partir de la S4 est un **shield pédagogique fait maison**, conçu et soudé en cours, qui se branche directement sur le connecteur GPIO 40 broches de la Raspberry Pi.

---

## Sommaire

- [Progression du projet](#progression-du-projet)
- [Aperçu des semaines](#aperçu-des-semaines)
- [Le shield — carte d'extension pédagogique](#le-shield--carte-dextension-pédagogique)
- [Connexion du shield à la Raspberry Pi](#connexion-du-shield-à-la-raspberry-pi)
- [Prérequis matériel](#prérequis-matériel)
- [Installation sur Raspberry Pi 3](#installation-sur-raspberry-pi-3)
- [Installation sur Raspberry Pi 5](#installation-sur-raspberry-pi-5)
- [Récupérer et compiler le code](#récupérer-et-compiler-le-code)
- [Mode d'emploi par semaine](#mode-demploi-par-semaine)
- [Résolution des problèmes courants](#résolution-des-problèmes-courants)

---

## Progression du projet

| Semaine | Branche | Thème | Statut |
|---------|---------|-------|--------|
| S1 | [`Semaine_1`](../../tree/Semaine_1) | Découverte Qt Creator, première fenêtre | ✅ Terminé |
| S2 | [`Semaine_2`](../../tree/Semaine_2) | IHM Qt, signaux/slots | ✅ Terminé |
| S3 | [`Semaine_3`](../../tree/Semaine_3) | QTimer, acquisition périodique | ✅ Terminé |
| S4 | [`Semaine_4`](../../tree/Semaine_4) | GPIO, commande LED via sysfs | ✅ Terminé |
| S5 | [`Semaine_5`](../../tree/Semaine_5) | Capteur I2C SHT20, seuils Min/Max | ✅ Terminé |
| S6 | [`Semaine_6`](../../tree/Semaine_6) | Communication TCP + RS232 + CSV | ✅ Terminé |
| S7 | [`Semaine_7`](../../tree/Semaine_7) | Structuration logicielle | ✅ Terminé |
| S8 | [`Semaine_8`](../../tree/Semaine_8) | Finalisation + Soutenance | ⏳ À venir |

---

## Aperçu des semaines

### Semaines 1, 2, 3 — Bases Qt

> Branches : [`Semaine_1`](../../tree/Semaine_1) · [`Semaine_2`](../../tree/Semaine_2) · [`Semaine_3`](../../tree/Semaine_3)

Les trois premières semaines couvrent les bases de Qt Creator : création d'une première fenêtre, utilisation de Qt Designer, connexion de boutons via les signaux/slots, et mise en place d'un `QTimer` pour exécuter des actions périodiques. Projets simples, sans matériel externe.

---

### Semaine 4 — GPIO et commande LED

> Branche : [`Semaine_4`](../../tree/Semaine_4)

Première interaction avec le matériel. La LED **D1** du shield est commandée depuis l'IHM Qt via le système de fichiers **sysfs** Linux (`/sys/class/gpio/`). L'utilisateur choisit la broche GPIO dans une liste déroulante, active la broche, puis allume ou éteint la LED avec un bouton.

**Composant utilisé sur le shield :** LED D1 sur GPIO22.

---

### Semaine 5 — Capteur I2C SHT20

> Branche : [`Semaine_5`](../../tree/Semaine_5)

Lecture de la **température** et de l'**humidité** depuis le capteur **SHT20** du shield via le protocole **I2C**. La LED est commandée automatiquement selon deux seuils Min/Max réglables en temps réel. Trois états visuels distincts dans l'IHM (vert / orange / rouge).

**Composants utilisés sur le shield :** SHT20 (I2C `0x40`) + LED D1.

---

### Semaine 6 — Communication TCP + RS232 + CSV

> Branche : [`Semaine_6`](../../tree/Semaine_6)

Ajout de **deux canaux de communication simultanés** : les données du capteur sont envoyées toutes les 2 secondes via **TCP** (réseau) et via **RS232** (port DB9 du shield, géré par le MAX232). En parallèle, chaque mesure est sauvegardée dans un **fichier CSV horodaté**. Tous les paramètres sont modifiables en direct sans redémarrer.

```
SHT20  →  IHM  →  QTcpSocket   →  Serveur TCP (réseau)
                →  QSerialPort  →  Port DB9 / MAX232 (câble série)
                →  QFile        →  data_YYYYMMDD_HHMMSS.csv
```

Format CSV :
```
date;heure;temperature_c;humidite_pct;etat_led
15/01/2026;14:32:05;24,5;58,2;NORMAL
```

**Composants utilisés sur le shield :** SHT20 + LED D1 + MAX232 + port DB9.

---

## Le shield — carte d'extension pédagogique

Le shield est une carte PCB conçue et soudée en cours. Elle se connecte directement sur le connecteur GPIO 40 broches de la Raspberry Pi via une nappe 26 fils.

### Composants du shield

| Référence | Composant | Protocole | Rôle | Utilisé en |
|-----------|-----------|-----------|------|------------|
| U1 (CMS) | **SHT20** | I2C — `0x40` | Capteur température + humidité | S5, S6 |
| U2 (CMS) | **TC72** | SPI | Capteur température secondaire | À venir |
| U3 | **MAX232** | UART | Convertisseur RS232 pour le port DB9 | S6 |
| J4 | **Afficheur LCD** | I2C (grove) | Ecran d'affichage (câble grove 5cm) | — |
| J3 | **Port DB9** | RS232 | Connecteur série vers PC ou équipement | S6 |
| J1 | **Nappe GPIO** | — | Connexion à la Raspberry (26 broches) | S4+ |
| SW1 | **Bouton poussoir** | GPIO | Entrée numérique | — |
| D2 | **LED jaune** | — | Témoin présence 3.3V | S4+ |
| D1 | **LED commandée** | GPIO22 | LED pilotée par l'application Qt | S4+ |
| FU1 | **Fusible** | — | Protection alimentation | — |

### Vérifications après mise sous tension

Avant de lancer la moindre application, vérifier que la carte fonctionne :

```
✅ La LED jaune D2 doit s'allumer → présence du 3.3V confirmée
✅ Broche 2 (VS+) du MAX232 ≈ +9.1V → mesurer au multimètre
✅ Broche 6 (VS-) du MAX232 ≈ -8.7V → mesurer au multimètre
```

Vérifier la présence des composants I2C sur le bus :
```bash
sudo i2cdetect -y 1
```

**5 adresses doivent apparaître :**
- `0x40` → SHT20 (capteur température/humidité)
- 4 autres adresses → afficheur LCD I2C (grove)

Si `0x40` n'apparaît pas, voir la section [résolution des problèmes](#résolution-des-problèmes-courants).

---

## Connexion du shield à la Raspberry Pi

Le shield se connecte via une **nappe 26 fils** sur les broches 1 à 26 du connecteur GPIO.

> ⚠️ **Le sens de la nappe est important !**
> Le **fil rouge** (broche 1) doit être côté **broche 1 de la Raspberry** (3.3V, marquée d'un carré sur le PCB).
> Brancher à l'envers risque d'alimenter des GPIO en 5V → destruction possible.

```
RASPBERRY PI — Connecteur 40 broches         SHIELD (nappe 26 fils)
┌──────────────────────────────────────────────────────────────┐
│                                                              │
│  Broche  1  (3.3V)  ───────────────────────  SHT20 VCC      │
│  Broche  3  (SDA)   ───────────────────────  SHT20 SDA      │  ← I2C
│  Broche  5  (SCL)   ───────────────────────  SHT20 SCL      │  ← I2C
│  Broche  6  (GND)   ───────────────────────  SHT20 GND      │
│                                                              │
│  Broche  8  (TX)    ───────────────────────  MAX232 entrée   │  ← UART→RS232
│  Broche 10  (RX)    ───────────────────────  MAX232 sortie   │  ← UART→RS232
│                                                              │
│  Broche 15  (GPIO22)───────────────────────  LED D1          │  ← GPIO
│                                                              │
│  Broche 19  (MOSI)  ───────────────────────  TC72 SDI        │  ← SPI
│  Broche 21  (MISO)  ───────────────────────  TC72 SDO        │  ← SPI
│  Broche 23  (SCLK)  ───────────────────────  TC72 CLK        │  ← SPI
│  Broche 24  (CE0)   ───────────────────────  TC72 CS         │  ← SPI
│                                                              │
└──────────────────────────────────────────────────────────────┘
        ↑ nappe branchée broches 1 à 26 uniquement
```

---

## Prérequis matériel

| Composant | Requis à partir de |
|-----------|-------------------|
| Raspberry Pi 3 ou 5 | S1 |
| Carte SD + Raspberry Pi OS (Bookworm recommandé) | S1 |
| Connexion internet (WiFi ou Ethernet) | S1 |
| Écran + clavier + souris | S1 |
| Shield pédagogique (carte faite maison) | S4 |
| Nappe 26 fils pour connexion shield ↔ Raspberry | S4 |
| Câble DB9 ou câble console (pour RS232) | S6 |

---

## Installation sur Raspberry Pi 3

### Étape 1 — Mettre à jour le système 🦝

```bash
sudo apt-get update
sudo apt-get upgrade -y
```

> Si tu as des erreurs de clés GPG :
> ```bash
> sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys ED65462EC8D5E4C5
> sudo apt-get update --fix-missing
> ```

---

### Étape 2 — Outils de compilation

```bash
sudo apt-get install -y build-essential cmake git
```

Vérification :
```bash
cmake --version   # cmake version 3.XX.X
g++ --version     # g++ XX.X.X
git --version     # git version X.XX.X
```

---

### Étape 3 — Installer Qt5

```bash
sudo apt-get install -y qt5-default qtbase5-dev qttools5-dev qttools5-dev-tools
sudo apt-get install -y libqt5widgets5 libqt5gui5 libqt5core5a
sudo apt-get install -y libqt5serialport5-dev   # Module RS232 (S6)
```

> Si `qt5-default` est introuvable (Debian Bullseye+) :
> ```bash
> sudo apt-get install -y qtbase5-dev qtbase5-dev-tools qt5-qmake
> ```

Vérification :
```bash
qmake --version   # QMake version X.X / Using Qt version 5.X.X
```

---

### Étape 4 — Support I2C (requis à partir de S5)

```bash
sudo apt-get install -y libi2c-dev i2c-tools
```

Activer le bus I2C :
```bash
sudo modprobe i2c-dev
echo "i2c-dev" | sudo tee -a /etc/modules
sudo nano /boot/config.txt
# Ajouter ou décommenter : dtparam=i2c_arm=on
sudo reboot
```

Vérification après redémarrage :
```bash
ls /dev/i2c*          # Doit afficher /dev/i2c-1
sudo i2cdetect -y 1   # 0x40 doit apparaître
```

---

### Étape 5 — Support SPI (requis pour le TC72)

```bash
sudo nano /boot/config.txt
# Ajouter ou décommenter : dtparam=spi=on
sudo reboot
```

Vérification :
```bash
ls /dev/spi*   # Doit afficher /dev/spidev0.0 et /dev/spidev0.1
```

---

### Étape 6 — Support RS232 / UART (requis pour S6)

```bash
sudo nano /boot/config.txt
# Ajouter :
# enable_uart=1
# dtoverlay=disable-bt
sudo reboot
```

Vérification :
```bash
ls /dev/serial*   # Doit afficher /dev/serial0 → /dev/ttyAMA0
```

---

### Récapitulatif dépendances — Pi 3

| Paquet | Obligatoire | À partir de | Rôle |
|--------|-------------|-------------|------|
| `build-essential` | ✅ | S1 | Compilateur C++ |
| `cmake` ≥ 3.16 | ✅ | S1 | Système de build |
| `git` | ✅ | S1 | Cloner le dépôt |
| `qt5-default` + `qtbase5-dev` | ✅ | S1 | Qt5 |
| `libqt5widgets5` | ✅ | S1 | Affichage fenêtres |
| `libqt5serialport5-dev` | ✅ | S6 | QSerialPort RS232 |
| `libi2c-dev` | ✅ | S5 | Communication I2C |
| `i2c-tools` | ⚙️ Recommandé | S5 | Vérifier les composants I2C |

---

## Installation sur Raspberry Pi 5

> La Pi 5 utilise **Raspberry Pi OS Bookworm** et présente plusieurs différences importantes par rapport à la Pi 3. Lire cette section attentivement avant de commencer.

### ⚠️ Différences clés Pi 5 vs Pi 3

| Point | Raspberry Pi 3 | Raspberry Pi 5 |
|-------|----------------|----------------|
| Fichier config | `/boot/config.txt` | `/boot/firmware/config.txt` |
| GPIO sysfs | ✅ Fonctionne | ❌ Supprimé → utiliser `libgpiod` |
| Qt5 | `qt5-default` | `qtbase5-dev` (paquet renommé) |
| I2C | `/dev/i2c-1` | `/dev/i2c-1` (identique ✅) |
| SPI | `/dev/spidev0.0` | `/dev/spidev0.0` (identique ✅) |
| UART | `/dev/ttyAMA0` | `/dev/ttyAMA0` (identique ✅) |

---

### Étape 1 — Mettre à jour le système

```bash
sudo apt update
sudo apt upgrade -y
```

---

### Étape 2 — Outils de compilation

```bash
sudo apt install -y build-essential cmake git
```

---

### Étape 3 — Installer Qt5 (syntaxe Bookworm)

```bash
# qt5-default n'existe plus sur Bookworm → remplacé par :
sudo apt install -y qtbase5-dev qtbase5-dev-tools qt5-qmake
sudo apt install -y libqt5widgets5 libqt5gui5 libqt5core5a
sudo apt install -y libqt5serialport5-dev   # Module RS232 (S6)
```

Vérification :
```bash
qmake --version
```

---

### Étape 4 — Support I2C

```bash
sudo apt install -y libi2c-dev i2c-tools
sudo modprobe i2c-dev
echo "i2c-dev" | sudo tee -a /etc/modules

# ⚠️ Chemin différent sur Pi 5 !
sudo nano /boot/firmware/config.txt
# Ajouter : dtparam=i2c_arm=on
sudo reboot
```

Vérification :
```bash
ls /dev/i2c*
sudo i2cdetect -y 1   # 0x40 doit apparaître
```

---

### Étape 5 — Support SPI

```bash
sudo nano /boot/firmware/config.txt
# Ajouter : dtparam=spi=on
sudo reboot
```

Vérification :
```bash
ls /dev/spi*
```

---

### Étape 6 — Support RS232 / UART

```bash
sudo nano /boot/firmware/config.txt
# Ajouter :
# enable_uart=1
# dtoverlay=disable-bt
sudo reboot
```

Vérification :
```bash
ls /dev/serial*
```

---

### Étape 7 — GPIO sur Pi 5 (libgpiod — remplace sysfs) 🦝

Sur Pi 5, le sysfs GPIO (`/sys/class/gpio/`) est supprimé. Il faut utiliser `libgpiod` :

```bash
sudo apt install -y libgpiod-dev gpiod
```

Vérification :
```bash
gpiodetect
# Doit afficher : gpiochip0 [pinctrl-rp1] (54 lines)

gpioget gpiochip0 22   # Lire l'état de la broche GPIO22
```

> ⚠️ La classe `CGpio` du projet utilise sysfs et devra être adaptée pour la Pi 5.
> Voir la section résolution des problèmes pour les détails.

---

### Récapitulatif dépendances — Pi 5

| Paquet | Obligatoire | À partir de | Rôle |
|--------|-------------|-------------|------|
| `build-essential` | ✅ | S1 | Compilateur C++ |
| `cmake` ≥ 3.16 | ✅ | S1 | Système de build |
| `git` | ✅ | S1 | Cloner le dépôt |
| `qtbase5-dev` + `qt5-qmake` | ✅ | S1 | Qt5 (syntaxe Bookworm) |
| `libqt5serialport5-dev` | ✅ | S6 | QSerialPort RS232 |
| `libi2c-dev` | ✅ | S5 | Communication I2C |
| `i2c-tools` | ⚙️ Recommandé | S5 | Vérifier les composants I2C |
| `libgpiod-dev` + `gpiod` | ✅ | S4 | GPIO (remplace sysfs) |

---

## Récupérer et compiler le code

### Cloner le dépôt

```bash
cd ~
git clone https://github.com/viklib/rasberry_pi_ciel.git
cd rasberry_pi_ciel
```

### Voir les branches disponibles

```bash
git branch -a
```

### Changer de branche

```bash
git checkout Semaine_1   # ou Semaine_2, Semaine_3, etc.
```

### Mettre à jour le code

```bash
git pull origin Semaine_6   # Remplacer par la branche souhaitée
```

### Compiler — méthode universelle

```bash
cd ~/rasberry_pi_ciel
mkdir -p build && cd build
cmake ..
make
```

> Pas besoin de refaire `cmake ..` si tu modifies uniquement des `.cpp` ou `.h`.
> Refais `cmake ..` uniquement si tu modifies `CMakeLists.txt`.

---

## Mode d'emploi par semaine

### Semaines 1, 2, 3

```bash
git checkout Semaine_1   # ou Semaine_2 / Semaine_3
mkdir -p build && cd build
cmake .. && make
./appS1                  # ou ./appS2 / ./appS3
```

---

### Semaine 4 — GPIO / LED du shield

```bash
git checkout Semaine_4
mkdir -p build && cd build
cmake .. && make
sudo ./appGpio
```

**Utilisation :**
1. Sélectionner la broche **22** dans la liste (correspond à la LED D1 du shield)
2. Cliquer **Activer** — la broche GPIO est initialisée
3. Cliquer **Allumer / Eteindre** — la LED D1 physique répond

**Test manuel du câblage avant de lancer :**
```bash
echo "22" | sudo tee /sys/class/gpio/export
echo "out" | sudo tee /sys/class/gpio/gpio22/direction
echo "1" | sudo tee /sys/class/gpio/gpio22/value    # Allume D1
echo "0" | sudo tee /sys/class/gpio/gpio22/value    # Eteint D1
echo "22" | sudo tee /sys/class/gpio/unexport
```

---

### Semaine 5 — Capteur SHT20 du shield

```bash
git checkout Semaine_5
mkdir -p build && cd build
cmake .. && make
sudo ./appS5_v3
```

**Avant de lancer — vérifier la détection du capteur :**
```bash
sudo i2cdetect -y 1
# Le SHT20 du shield doit apparaître à 0x40
# 4 autres adresses → afficheur LCD du shield
```

**Utilisation :**
1. Régler les seuils **Min** et **Max** de température
2. Cliquer **Activer GPIO** puis **Demarrer acquisition**
3. La LED D1 du shield se commande automatiquement selon la température

---

### Semaine 6 — TCP + RS232 + CSV

```bash
git checkout Semaine_6
mkdir -p build && cd build
cmake .. && make
sudo ./appS6_v2
```

**Utilisation TCP :**
1. Renseigner l'**IP** du serveur et le **Port** (modifiables sans redémarrer)
2. Cliquer **Connecter TCP**

**Utilisation RS232 (port DB9 du shield) :**
1. Cliquer **Rafraichir** pour détecter les ports disponibles
2. Sélectionner `/dev/ttyAMA0` et la vitesse `9600`
3. Cliquer **Ouvrir port serie**
4. Brancher un câble DB9 vers un PC avec PuTTY en mode Serial

**Lancer l'acquisition :**
1. Cliquer **Activer GPIO** puis **Demarrer acquisition**
2. Les données sont envoyées sur TCP ET RS232 ET enregistrées en CSV

**Trouver l'IP de la Raspberry :**
```bash
hostname -I
# ex : 192.168.1.42
```

**Serveur TCP de test sur un PC :**
```bash
ncat -l 12345
# ou
nc -l -p 12345
```

**Trouver le fichier CSV :**
```bash
ls ~/rasberry_pi_ciel/build/data_*.csv
```

---

## Résolution des problèmes courants

### "cannot connect to display"

```bash
export DISPLAY=:0
sudo ./appS6_v2
```

Lance toujours l'application depuis le bureau graphique.

---

### LED jaune D2 du shield ne s'allume pas

La carte n'est pas alimentée correctement. Vérifier dans l'ordre :
1. La nappe est bien branchée (fil rouge côté broche 1)
2. La Raspberry est bien sous tension
3. La nappe est bien enfichée des deux côtés

---

### "Permission denied" sur GPIO

```bash
sudo chmod -R 777 /sys/class/gpio/
sudo ./appS6_v2
```

---

### "gpio export already exported"

La broche est déjà exportée d'une session précédente :
```bash
echo "22" | sudo tee /sys/class/gpio/unexport
sudo ./appS6_v2
```

---

### Le capteur SHT20 n'est pas détecté à 0x40

```bash
sudo i2cdetect -y 1
```

Si `0x40` n'apparaît pas :
1. Vérifier que la nappe est correctement branchée (sens du fil rouge)
2. Vérifier que I2C est activé : `ls /dev/i2c*` doit retourner `/dev/i2c-1`
3. Si absent : `sudo modprobe i2c-dev` puis réessayer
4. Vérifier que l'afficheur LCD est bien connecté au shield (câble grove) — il fournit les résistances pull-up du bus I2C
5. **Pi 3 :** vérifier `dtparam=i2c_arm=on` dans `/boot/config.txt`
6. **Pi 5 :** vérifier `dtparam=i2c_arm=on` dans `/boot/firmware/config.txt`

---

### GPIO ne fonctionne pas sur Raspberry Pi 5

Sur Pi 5, sysfs GPIO est supprimé. La classe `CGpio` doit être adaptée :

```bash
# Tester manuellement avec libgpiod
gpioset gpiochip0 22=1   # Allume la LED D1
gpioset gpiochip0 22=0   # Eteint la LED D1
```

La réécriture de `CGpio` avec `libgpiod` est prévue en S7.

---

### "could not find Qt5" lors du cmake

**Pi 3 :**
```bash
sudo apt-get install -y qt5-default qtbase5-dev
cmake ..
```

**Pi 5 (Bookworm) :**
```bash
sudo apt install -y qtbase5-dev qtbase5-dev-tools qt5-qmake
cmake ..
```

---

### Erreur TCP "Connection timed out"

1. Vérifier que le serveur TCP tourne sur la machine cible
2. Vérifier l'IP et le port dans l'IHM
3. Vérifier que les deux machines sont sur le même réseau : `ping <ip_du_serveur>`
4. Pour tester en local : utiliser `127.0.0.1` comme IP

---

### Port RS232 introuvable dans la liste

```bash
ls /dev/tty*
# Chercher /dev/ttyAMA0 ou /dev/ttyS0
```

Si absent :
1. Vérifier `enable_uart=1` dans le fichier config
2. `sudo modprobe uart` puis réessayer
3. Cliquer **Rafraichir** dans l'IHM pour rescanner les ports

---

### Recompiler après modification du code

```bash
cd ~/rasberry_pi_ciel/build
make
sudo ./appS6_v2
```

---

### Erreurs de clés GPG lors d'un apt-get update

```bash
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys ED65462EC8D5E4C5
sudo apt-get update --fix-missing
```

---

*BTS CIEL — 2025/2026* 🦝

---

> *Ce projet a été réalisé avec l'aide de l'IA Claude (Anthropic) pour la génération et l'amélioration du code, la rédaction de la documentation et le débogage. Les propositions ont été relues, comprises et validées à chaque étape.*
