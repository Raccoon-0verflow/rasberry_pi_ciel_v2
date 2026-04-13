# Projet Qt / Raspberry Pi — BTS CIEL

> Projet pédagogique annuel | BTS CIEL | Qt / C++ / Raspberry Pi
> Année scolaire **2025/2026**

---

## Présentation

Ce dépôt contient l'ensemble des projets développés semaine par semaine dans le cadre du cours **Qt / C++ sur Raspberry Pi**. Chaque semaine est sur une branche Git dédiée. Le projet monte en compétences progressivement : de la première fenêtre Qt jusqu'à une application complète avec capteur I2C, commande GPIO, communication TCP et sauvegarde CSV.

---

## Sommaire

- [Progression du projet](#progression-du-projet)
- [Aperçu des semaines](#aperçu-des-semaines)
- [Câblage](#câblage)
- [Prérequis matériel](#prérequis-matériel)
- [Installation de l'environnement](#installation-de-lenvironnement)
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
| S6 | [`Semaine_6`](../../tree/Semaine_6) | Communication TCP + sauvegarde CSV | ✅ Terminé |
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

Première interaction avec le matériel. Une LED est commandée depuis l'IHM Qt via le système de fichiers **sysfs** Linux (`/sys/class/gpio/`). L'utilisateur choisit la broche GPIO dans une liste déroulante, active la broche, puis allume ou éteint la LED avec un bouton.

**Matériel requis :** LED + résistance 330Ω branchée sur GPIO22 (broche 15).

---

### Semaine 5 — Capteur I2C SHT20

> Branche : [`Semaine_5`](../../tree/Semaine_5)

Lecture de la **température** et de l'**humidité** depuis le capteur **SHT20** via le protocole **I2C**. La LED est commandée automatiquement selon deux seuils Min/Max réglables en temps réel. Trois états visuels distincts dans l'IHM (vert / orange / rouge).

**Matériel requis :** capteur SHT20 sur SDA/SCL + LED sur GPIO22 (voir section câblage).

---

### Semaine 6 — Communication TCP + CSV

> Branche : [`Semaine_6`](../../tree/Semaine_6)

Ajout d'une couche **communication TCP** : les données du capteur sont envoyées toutes les 2 secondes vers un serveur distant. En parallèle, chaque mesure est sauvegardée dans un **fichier CSV horodaté** sur la Raspberry. L'adresse IP, le port et les seuils sont modifiables en direct sans redémarrer l'application.

Format CSV généré :
```
date;heure;temperature_c;humidite_pct;etat_led
15/01/2026;14:32:05;24,5;58,2;NORMAL
```

**Matériel requis :** même câblage que S5 + réseau Wi-Fi ou Ethernet.

---

## Câblage

### Semaine 4 — Brancher la LED

> Une seule LED + une résistance. C'est tout.

```
RASPBERRY PI                                    COMPOSANTS
                                               
  Broche 15 ──────────────────── patte 1 ──[  330Ω  ]── patte 2 ──── Anode (+) ──[LED]
  (GPIO22)                                  résistance                longue patte
                                               
  Broche 9  ──────────────────────────────────────────────────────── Cathode (−)──[LED]
  (GND)                                                               courte patte
```

> ⚠️ **La résistance 330Ω est obligatoire** — sans elle la LED grille et la Raspberry peut être endommagée.
> Les bandes de couleur de la résistance 330Ω : **orange – orange – marron – or**

---

### Semaine 5 et 6 — Ajouter le capteur SHT20

> On garde le câblage de la LED (S4) et on branche le SHT20 en plus.

```
RASPBERRY PI                                    CAPTEUR SHT20
                                               
  Broche 1  ──────────────────────────────────── VCC   (alimentation)
  (3.3V)                                        
                                               
  Broche 3  ──────────────────────────────────── SDA   (données)
  (GPIO2)                                       
                                               
  Broche 5  ──────────────────────────────────── SCL   (horloge)
  (GPIO3)                                       
                                               
  Broche 6  ──────────────────────────────────── GND   (masse)
  (GND)                                         
```

```
RASPBERRY PI                                    LED  (identique S4)

  Broche 15 ──── patte 1 ──[  330Ω  ]── patte 2 ──── Anode (+) ──[LED]
  (GPIO22)                résistance                 longue patte

  Broche 9  ──────────────────────────────────────── Cathode (−)──[LED]
  (GND)                                              courte patte
```

> ⚠️ **Brancher le SHT20 sur la broche 3.3V uniquement** (broche 1 ou 17).
> Le brancher sur le 5V peut endommager le capteur définitivement.

---

### Résumé — toutes les connexions en un coup d'œil

```
RASPBERRY PI 3 — Connecteur 40 broches
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   Broche  1  (3.3V)  ──────────────────────── SHT20 VCC     │  ← S5, S6
│   Broche  3  (SDA)   ──────────────────────── SHT20 SDA     │  ← S5, S6
│   Broche  5  (SCL)   ──────────────────────── SHT20 SCL     │  ← S5, S6
│   Broche  6  (GND)   ──────────────────────── SHT20 GND     │  ← S5, S6
│                                                             │
│   Broche  9  (GND)   ──────────────────────── LED (−)       │  ← S4, S5, S6
│   Broche 15  (GPIO22)──── [330Ω] ─────────── LED (+)        │  ← S4, S5, S6
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## Prérequis matériel

| Composant | Requis à partir de |
|-----------|-------------------|
| Raspberry Pi 3 (ou compatible) | S1 |
| Carte SD + Kali Linux ou Raspberry Pi OS | S1 |
| Connexion internet (WiFi ou Ethernet) | S1 |
| Écran + clavier + souris | S1 |
| LED + résistance 330Ω + fils | S4 |
| Capteur SHT20 (adresse I2C 0x40) | S5 |

---

## Installation de l'environnement

### Étape 1 — Mettre à jour le système

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

### Étape 2 — Installer les outils de compilation

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
```

> **Pourquoi Qt5 et pas Qt6 ?**
> Qt6 n'est pas disponible dans les dépôts Kali Linux sur Raspberry Pi.
> Qt5 est parfaitement compatible avec tous les projets de ce cours.

Vérification :
```bash
qmake --version   # QMake version X.X / Using Qt version 5.X.X
```

---

### Étape 4 — Installer le support I2C (requis à partir de S5)

```bash
sudo apt-get install -y libi2c-dev i2c-tools
```

Activer le bus I2C :
```bash
sudo modprobe i2c-dev
echo "i2c-dev" | sudo tee -a /etc/modules
sudo nano /boot/config.txt   # Ajouter la ligne : dtparam=i2c_arm=on
sudo reboot
```

Vérifier que le capteur SHT20 est bien détecté après redémarrage :
```bash
sudo i2cdetect -y 1
# Le capteur doit apparaître à l'adresse 0x40
```

---

### Récapitulatif des dépendances

| Paquet | Obligatoire | À partir de | Rôle |
|--------|-------------|-------------|------|
| `build-essential` | ✅ | S1 | Compilateur C++ |
| `cmake` ≥ 3.16 | ✅ | S1 | Système de build |
| `git` | ✅ | S1 | Cloner le dépôt |
| `qt5-default` + `qtbase5-dev` | ✅ | S1 | Qt5 |
| `libqt5widgets5` | ✅ | S1 | Affichage fenêtres |
| `libi2c-dev` | ✅ | S5 | Communication I2C |
| `i2c-tools` | ⚙️ Recommandé | S5 | Vérifier le capteur (`i2cdetect`) |

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

### Semaine 4 — GPIO

```bash
git checkout Semaine_4
mkdir -p build && cd build
cmake .. && make
sudo ./appGpio
```

**Utilisation :**
1. Sélectionner la broche **22** dans la liste déroulante
2. Cliquer **Activer** — la broche GPIO est initialisée
3. Cliquer **Allumer / Eteindre** — la LED physique répond

**Test manuel du câblage avant de lancer :**
```bash
echo "22" | sudo tee /sys/class/gpio/export
echo "out" | sudo tee /sys/class/gpio/gpio22/direction
echo "1" | sudo tee /sys/class/gpio/gpio22/value    # Allume
echo "0" | sudo tee /sys/class/gpio/gpio22/value    # Eteint
echo "22" | sudo tee /sys/class/gpio/unexport
```

---

### Semaine 5 — Capteur SHT20

```bash
git checkout Semaine_5
mkdir -p build && cd build
cmake .. && make
sudo ./appS5_v3
```

**Utilisation :**
1. Vérifier que le capteur est détecté : `sudo i2cdetect -y 1` → doit afficher `40`
2. Régler les seuils **Min** et **Max** de température dans les champs IHM
3. Cliquer **Activer GPIO** puis **Demarrer acquisition**
4. La LED se commande automatiquement selon la température mesurée

---

### Semaine 6 — TCP + CSV

```bash
git checkout Semaine_6
mkdir -p build && cd build
cmake .. && make
sudo ./appS6_v1
```

**Utilisation :**
1. Renseigner l'**IP** du serveur TCP et le **Port** (modifiables sans redémarrer)
2. Cliquer **Connecter** — le statut passe en vert si le serveur répond
3. Cliquer **Activer GPIO** puis **Demarrer acquisition**
4. Les données sont envoyées via TCP **et** enregistrées en CSV automatiquement

**Trouver l'IP de la Raspberry :**
```bash
hostname -I
# ex : 192.168.1.42
```

**Lancer un serveur TCP de test sur un PC :**
```bash
ncat -l 12345
# ou
nc -l -p 12345
```

**Trouver le fichier CSV généré :**
```bash
ls ~/rasberry_pi_ciel/build/data_*.csv
```

---

## Résolution des problèmes courants

### "cannot connect to display"

```bash
export DISPLAY=:0
sudo ./appS6_v1
```

Lance toujours l'application depuis le bureau graphique de Kali Linux.

---

### "Permission denied" sur GPIO

```bash
sudo chmod -R 777 /sys/class/gpio/
sudo ./appS6_v1
```

---

### "could not find Qt5" lors du cmake

```bash
sudo apt-get install -y qt5-default qtbase5-dev
cmake ..
```

---

### "gpio export already exported"

La broche est déjà exportée d'une session précédente :
```bash
echo "22" | sudo tee /sys/class/gpio/unexport
sudo ./appS6_v1
```

---

### La LED ne s'allume pas

Vérifier dans l'ordre :
1. Le câblage — résistance bien branchée, longue patte LED côté GPIO
2. La broche sélectionnée dans l'IHM correspond à celle câblée physiquement
3. Tester manuellement avec les commandes `echo` de la section Semaine 4
4. Vérifier que l'application est lancée avec `sudo`

---

### Le capteur SHT20 n'est pas détecté

```bash
sudo i2cdetect -y 1
```

Si `0x40` n'apparaît pas :
1. Vérifier le câblage SDA (broche 3) et SCL (broche 5)
2. Vérifier que I2C est activé : `ls /dev/i2c*` doit retourner `/dev/i2c-1`
3. Si absent : `sudo modprobe i2c-dev` puis réessayer
4. Vérifier que `dtparam=i2c_arm=on` est dans `/boot/config.txt`

---

### Erreur TCP "Connection timed out"

1. Vérifier que le serveur TCP tourne bien sur la machine cible
2. Vérifier l'IP et le port saisis dans l'IHM
3. Vérifier que les deux machines sont sur le même réseau : `ping <ip_du_serveur>`
4. Pour tester en local sur la Raspberry : utiliser `127.0.0.1` comme IP

---

### Recompiler après modification du code

```bash
cd ~/rasberry_pi_ciel/build
make
sudo ./appS6_v1
```

---

### Erreurs de clés GPG lors d'un apt-get update

```bash
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys ED65462EC8D5E4C5
sudo apt-get update --fix-missing
```

---

*BTS CIEL — 2025/2026*
