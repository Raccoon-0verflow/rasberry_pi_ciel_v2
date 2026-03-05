# Guide d'installation et de compilation — Raspberry Pi

> BTS CIEL | Qt / C++ / Raspberry Pi | Guide complet

---

## Sommaire

1. [Prérequis matériel](#1-prérequis-matériel)
2. [Installation de l'environnement](#2-installation-de-lenvironnement)
3. [Récupérer le code source](#3-récupérer-le-code-source)
4. [Compiler et lancer chaque projet](#4-compiler-et-lancer-chaque-projet)
5. [Semaine 4 — Câblage et test de la LED](#5-semaine-4--câblage-et-test-de-la-led)
6. [Résolution des problèmes courants](#6-résolution-des-problèmes-courants)

---

## 1. Prérequis matériel

- Raspberry Pi 3 (ou compatible)
- Carte SD avec **Kali Linux** (ou Raspberry Pi OS) installé
- Connexion internet (WiFi ou câble Ethernet)
- Ecran + clavier + souris branchés sur la Raspberry
- **Pour la semaine 4 :** une LED + une résistance 330 ohms + fils de connexion

---

## 2. Installation de l'environnement

### 2.1 Mettre à jour le système

Ouvre un terminal et tape ces commandes **dans l'ordre** :

```bash
sudo apt-get update
sudo apt-get upgrade -y
```

> Si tu as des erreurs de clés GPG, tape d'abord :
> ```bash
> sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys ED65462EC8D5E4C5
> sudo apt-get update --fix-missing
> ```

---

### 2.2 Installer les outils de compilation

```bash
sudo apt-get install -y build-essential
sudo apt-get install -y cmake
sudo apt-get install -y git
```

Vérifie que cmake est bien installé :
```bash
cmake --version
# Doit afficher : cmake version X.XX.X
```

---

### 2.3 Installer Qt5 et ses modules

```bash
sudo apt-get install -y qt5-default
sudo apt-get install -y qtbase5-dev
sudo apt-get install -y qttools5-dev
sudo apt-get install -y qttools5-dev-tools
```

> **Pourquoi Qt5 et pas Qt6 ?**
> Qt6 n'est pas disponible dans les dépôts Kali Linux sur Raspberry Pi.
> Qt5 est parfaitement compatible avec tous les projets de ce cours.

Vérifie que Qt est bien installé :
```bash
qmake --version
# Doit afficher : QMake version X.X / Using Qt version 5.X.X
```

---

### 2.4 Installer le support graphique Qt (interface visuelle)

Pour que les fenêtres Qt s'affichent correctement sur l'écran de la Raspberry :

```bash
sudo apt-get install -y libqt5widgets5
sudo apt-get install -y libqt5gui5
sudo apt-get install -y libqt5core5a
```

---

### 2.5 Vérification complète de l'installation

```bash
# Vérifier cmake
cmake --version

# Vérifier Qt
qmake --version

# Vérifier le compilateur C++
g++ --version

# Vérifier git
git --version
```

Si toutes ces commandes affichent un numéro de version, l'installation est complète.

---

## 3. Récupérer le code source

### 3.1 Cloner le dépôt GitHub

```bash
cd ~
git clone https://github.com/viklib/rasberry_pi_ciel.git
cd rasberry_pi_ciel
```

### 3.2 Voir les branches disponibles

```bash
git branch -a
# Affiche toutes les branches : Semaine_1, Semaine_2, Semaine_3, Semaine_4...
```

### 3.3 Changer de branche selon la semaine

```bash
# Semaine 1
git checkout Semaine_1

# Semaine 2
git checkout Semaine_2

# Semaine 3
git checkout Semaine_3

# Semaine 4
git checkout Semaine_4
```

### 3.4 Mettre à jour le code (si modifications sur GitHub)

```bash
git pull origin Semaine_4
# Remplace Semaine_4 par la branche souhaitée
```

---

## 4. Compiler et lancer chaque projet

### Méthode universelle (fonctionne pour toutes les semaines)

```bash
# 1. Aller dans le dossier du projet
cd ~/rasberry_pi_ciel

# 2. Créer un dossier de compilation propre
mkdir -p build
cd build

# 3. Configurer avec cmake
cmake ..

# 4. Compiler
make

# 5. Lancer l'application
./appS1      # Pour la semaine 1
./appS2      # Pour la semaine 2
./appS3      # Pour la semaine 3
./appGpio    # Pour la semaine 4
```

---

### Semaine 1 — appS1

```bash
git checkout Semaine_1
mkdir -p build && cd build
cmake ..
make
./appS1
```

**Résultat attendu :** Une fenêtre s'ouvre avec un bouton "Afficher un message".

---

### Semaine 2 — appS2

```bash
git checkout Semaine_2
mkdir -p build && cd build
cmake ..
make
./appS2
```

**Résultat attendu :** Une fenêtre avec un champ prénom, une liste déroulante et un bouton Valider.

---

### Semaine 3 — appS3

```bash
git checkout Semaine_3
mkdir -p build && cd build
cmake ..
make
./appS3
```

**Résultat attendu :** Une fenêtre avec un timer configurable et un compteur qui s'incrémente.

---

### Semaine 4 — appGpio

```bash
git checkout Semaine_4
mkdir -p build && cd build
cmake ..
make
sudo ./appGpio
```

> **Important :** Le `sudo` est nécessaire pour accéder aux broches GPIO via sysfs.

**Résultat attendu :** Une fenêtre avec une liste de broches GPIO, un bouton Activer et un bouton Allumer/Eteindre.

---

## 5. Semaine 4 — Câblage et test de la LED

### 5.1 Brochage Raspberry Pi (numérotation BCM)

```
Raspberry Pi 3 — Connecteur GPIO 40 broches

 3.3V  [ 1] [ 2]  5V
 GPIO2 [ 3] [ 4]  5V
 GPIO3 [ 5] [ 6]  GND
 GPIO4 [ 7] [ 8]  GPIO14
  GND  [ 9] [10]  GPIO15
GPIO17 [11] [12]  GPIO18
GPIO27 [13] [14]  GND
GPIO22 [15] [16]  GPIO23    <-- Broche 15 = GPIO22
  3.3V [17] [18]  GPIO24
GPIO10 [19] [20]  GND
 GPIO9 [21] [22]  GPIO25
GPIO11 [23] [24]  GPIO8
  GND  [25] [26]  GPIO7
```

---

### 5.2 Câblage de la LED sur GPIO22

```
Raspberry Pi                    LED
                                 +
Broche 15 (GPIO22) ----[330Ω]---|>|---- Broche 9 (GND)
                    résistance  anode  cathode
```

**Etapes de câblage :**
1. Branche la résistance 330 ohms sur la broche **15** (GPIO22) de la Raspberry
2. Branche l'**anode** (longue patte) de la LED sur l'autre côté de la résistance
3. Branche la **cathode** (courte patte) de la LED sur la broche **9** (GND)

> **Attention :** Ne jamais brancher une LED sans résistance, cela pourrait endommager la Raspberry Pi.

---

### 5.3 Configurer les droits GPIO

Avant de lancer l'application, configure les droits d'accès au système GPIO :

```bash
sudo chmod 777 /sys/class/gpio/export
sudo chmod 777 /sys/class/gpio/unexport
```

---

### 5.4 Tester la LED manuellement (sans l'application Qt)

Pour vérifier que le câblage est correct avant de lancer l'IHM :

```bash
# Exporter la broche GPIO22
echo "22" | sudo tee /sys/class/gpio/export

# Configurer en sortie
echo "out" | sudo tee /sys/class/gpio/gpio22/direction

# Allumer la LED
echo "1" | sudo tee /sys/class/gpio/gpio22/value

# Eteindre la LED
echo "0" | sudo tee /sys/class/gpio/gpio22/value

# Libérer la broche
echo "22" | sudo tee /sys/class/gpio/unexport
```

Si la LED s'allume et s'éteint, le câblage est correct et l'application Qt fonctionnera.

---

### 5.5 Lancer et utiliser l'application GPIO

```bash
cd ~/rasberry_pi_ciel/build
sudo ./appGpio
```

**Utilisation :**
1. Dans la liste déroulante, sélectionner **22** (ou la broche câblée)
2. Cliquer sur **Activer** — la broche GPIO est initialisée
3. Cliquer sur **Allumer** — la LED physique s'allume, le label passe en orange
4. Cliquer sur **Eteindre** — la LED s'éteint, le label repasse en gris

---

## 6. Résolution des problèmes courants

### L'application ne se lance pas — "cannot connect to display"

```bash
export DISPLAY=:0
sudo ./appGpio
```

Ou lance l'application directement depuis le bureau de Kali Linux.

---

### Erreur GPIO — "Permission denied"

```bash
sudo chmod -R 777 /sys/class/gpio/
sudo ./appGpio
```

---

### Erreur cmake — "could not find Qt5"

```bash
sudo apt-get install -y qt5-default qtbase5-dev
cmake ..
```

---

### La LED ne s'allume pas

Vérifie dans cet ordre :
1. Le câblage (résistance bien branchée, sens de la LED correct)
2. La broche sélectionnée dans l'IHM correspond à celle câblée physiquement
3. Tester manuellement avec les commandes `echo` de la section 5.4
4. Vérifier que `sudo` est bien utilisé pour lancer l'application

---

### Erreur "gpio export already exported"

La broche est déjà exportée d'une session précédente :

```bash
echo "22" | sudo tee /sys/class/gpio/unexport
# Puis relancer l'application
sudo ./appGpio
```

---

### Recompiler après une modification du code

```bash
cd ~/rasberry_pi_ciel/build
make
sudo ./appGpio
```

> Pas besoin de refaire `cmake ..` si tu n'as modifié que des fichiers `.cpp` ou `.h`.
> Refais `cmake ..` uniquement si tu as modifié `CMakeLists.txt`.

---

*BTS CIEL — 2024/2025*
