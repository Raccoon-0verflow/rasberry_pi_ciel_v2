# Semaine 6 — Communication TCP + Sauvegarde CSV

> Projet pédagogique | BTS CIEL | Qt / C++ / Raspberry Pi

---

## Objectif

Sixième semaine de formation Qt. L'objectif est d'envoyer les données du capteur **SHT20** (température + humidité) vers un serveur externe via le protocole **TCP**, tout en les sauvegardant simultanément dans un **fichier CSV horodaté** sur la Raspberry Pi. L'application reprend intégralement le code de la Semaine 5 (seuils Min/Max, commande LED, enum `EtatLed`) et y ajoute la couche communication.

---

## Ce que fait l'application

```
┌──────────────────────────────────────────────────────┐
│       Semaine 6 — Capteur SHT20 I2C + TCP            │
│                                                      │
│  Capteur SHT20                                       │
│    Temperature :      24.5 °C                        │
│    Humidite    :      58.2 %                         │
│                                                      │
│  Seuils temperature                                  │
│    Seuil Min (°C) :  [ 20  ]                         │
│    Seuil Max (°C) :  [ 30  ]                         │
│                                                      │
│  Commande LED                                        │
│    Broche GPIO :  [ 22 ▼ ]   [ Activer GPIO ]        │
│                                                      │
│  Communication TCP                                   │
│    IP :  [ 192.168.1.100 ]   Port :  [ 12345 ]       │
│    [ Connecter ]   Non connecte                      │
│                                                      │
│         [ Demarrer acquisition ]                     │
│                                                      │
│     LED ETEINTE — Temperature normale                │
│  ─────────────────────────────────────────────────   │
│  Derniere trame :                                    │
│  15/01/2024;14:32:05;24,5;58,2;NORMAL                │
│  Trames envoyees : 3  |  Enregistrees CSV : 3        │
└──────────────────────────────────────────────────────┘
```

**Fonctionnement :**
1. Renseigner l'**IP** et le **Port** du serveur TCP (modifiables à tout moment)
2. Cliquer sur **Connecter** pour établir la connexion TCP
3. Cliquer sur **Activer GPIO** pour initialiser la broche LED
4. Cliquer sur **Demarrer acquisition** pour lancer la lecture toutes les 2 secondes
5. À chaque acquisition, les données sont **envoyées via TCP** ET **écrites dans le CSV**
6. Les **seuils Min/Max** et l'**IP/Port** sont modifiables en direct, sans arrêter l'acquisition

---

## Historique des versions

### v1.0 — Reprise du code S5 + structure TCP de base

> *Code écrit en classe à partir du projet S5*

**Objectif :** intégrer `QTcpSocket` dans le projet S5 existant et établir une première connexion TCP fonctionnelle.

**Ce qui a été développé :**
- Ajout de `Qt::Network` dans `CMakeLists.txt`
- Ajout de `QTcpSocket` dans `cihmapps6.h`
- Bouton Connecter + champs IP et Port dans l'IHM
- Tentative d'envoi de la trame dans `slotAcquisition()`
- Reprise complète des classes `CSht20`, `CGpio`, `EtatLed` depuis S5

**Limitations identifiées :**
- Pas de fichier CSV — les données envoyées via TCP disparaissent si personne n'écoute
- Pas de retour visuel du statut de connexion (connecté / déconnecté)
- Pas de gestion des erreurs socket (hôte injoignable, timeout, déconnexion)
- Format de trame non structuré

---

### v2.0 — Ajout de la sauvegarde CSV

> *Travail maison — ajout du livrable planification*

**Objectif :** sauvegarder les données localement sur la Raspberry en parallèle de l'envoi TCP.

**Ce qui a été ajouté :**
- Classe `QFile` + `QTextStream` pour l'écriture sur disque
- Nom de fichier horodaté automatique (`data_YYYYMMDD_HHMMSS.csv`) pour éviter tout écrasement entre sessions
- En-tête CSV avec nommage explicite des colonnes : `datetime;temperature_c;humidite_pct;etat_led`
- Compteurs de trames TCP envoyées et de lignes CSV enregistrées affichés dans l'IHM
- Flush immédiat après chaque écriture pour éviter toute perte de données en cas de crash

**Limitations identifiées :**
- Format `datetime` en une seule colonne → Excel affichait `##########` (colonne non reconnue)
- Séparateur décimal `.` incompatible avec Excel en français (attend `,`)
- Gestion des erreurs socket encore absente

---

### v3.0 — Gestion des erreurs socket + statut TCP

> *Corrections après tests sous Windows*

**Objectif :** rendre la communication TCP robuste et informative pour l'utilisateur.

**Ce qui a été corrigé et ajouté :**
- Connexion du signal `errorOccurred` de `QTcpSocket` → affichage d'un message d'erreur clair (timeout, hôte injoignable, refus de connexion)
- Ajout des slots `slotTcpConnecte()` et `slotTcpDeconnecte()` pour suivre l'état en temps réel
- Label statut TCP coloré : 🟢 vert *"Connecté"* / 🔴 rouge *"Non connecté"*
- Le bouton **Connecter** devient **Déconnecter** quand la connexion est active
- Fermeture propre du socket dans le destructeur `~CIhmAppS6()`

---

### v4.0 — Corrections et améliorations proposées par Claude (IA)

> *Améliorations suggérées par l'IA Claude (Anthropic), adaptées et intégrées par l'élève*

**Objectif :** corriger les problèmes de compatibilité CSV et renforcer la qualité du code.

#### Correction 1 — Format CSV compatible Excel FR

Le format `datetime` en une seule colonne posait deux problèmes : Excel ne reconnaissait pas le format et affichait `##########`, et le séparateur décimal `.` était incompatible avec Excel en français.

**Solution proposée par Claude :** séparer la date et l'heure en deux colonnes distinctes, utiliser le format `dd/MM/yyyy` pour la date, et remplacer le point décimal par une virgule :

```cpp
// Avant (v3) — une seule colonne, point décimal
QString datetime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
QString temp     = QString::number(temperature, 'f', 1);  // ex: 24.5

// Après (v4) — deux colonnes, virgule décimale
QString date = now.toString("dd/MM/yyyy");   // ex: 15/01/2024
QString heure = now.toString("HH:mm:ss");   // ex: 14:32:05
QString temp = QString::number(temperature, 'f', 1).replace('.', ',');  // ex: 24,5
```

En-tête CSV mis à jour :
```
date;heure;temperature_c;humidite_pct;etat_led
15/01/2024;14:32:05;24,5;58,2;NORMAL
```

#### Correction 2 — Connexion du signal `errorOccurred` avec surcharge explicite

Qt6 possède plusieurs surcharges du signal d'erreur socket. Sans préciser laquelle, le compilateur ne sait pas laquelle connecter et produit une erreur ambiguë.

**Solution proposée par Claude :**

```cpp
// Connexion explicite avec QOverload pour lever l'ambiguïté Qt6
connect(m_socket,
    QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
    this, &CIhmAppS6::slotTcpErreur);
```

#### Correction 3 — Flush immédiat du fichier CSV

Sans appel à `flush()` après chaque écriture, Qt bufferise les données et peut ne pas les écrire sur le disque immédiatement. En cas de coupure de courant ou de crash, les dernières mesures sont perdues.

**Solution proposée par Claude :**

```cpp
out << trame << "\n";
m_fichierCsv->flush(); // Ecriture immédiate — pas de perte en cas de crash
```

#### Correction 4 — Adresse IP et port modifiables en temps réel

Claude a suggéré de lire l'IP et le port directement depuis les champs IHM au moment de la connexion, sans nécessiter de redémarrage de l'application. Il suffit de modifier les champs, cliquer Déconnecter puis Connecter pour changer de serveur cible à la volée.

#### Bilan des tests

| Scénario testé | Résultat |
|----------------|----------|
| Envoi TCP vers serveur `ncat` sous Windows | ✅ OK |
| Serveur injoignable (mauvaise IP) | ✅ Message d'erreur affiché |
| Déconnexion pendant acquisition | ✅ Acquisition continue, CSV toujours écrit |
| Reconnexion sans relancer l'appli | ✅ OK |
| Fichier CSV ouvert dans Excel FR | ✅ OK (date, heure, virgule décimale) |
| Modification IP/Port sans redémarrer | ✅ OK |
| Fermeture application pendant acquisition | ✅ Arrêt propre |

---

## Structure du projet

```
appS6/
├── CMakeLists.txt          # Configuration Qt — inclut Qt::Network
├── main.cpp                # Point d'entrée de l'application
├── cihmapps6.h             # Déclaration de la classe IHM
├── cihmapps6.cpp           # Implémentation IHM + TCP + CSV
├── cihmapps6.ui            # Interface graphique (Qt Designer)
├── csht20.h                # Réutilisé depuis S5 — inchangé
├── csht20.cpp              # Réutilisé depuis S5 — inchangé
├── cgpio.h                 # Réutilisé depuis S5 — inchangé
└── cgpio.cpp               # Réutilisé depuis S5 — inchangé
```

---

## Architecture POO

```
CIhmAppS6      (IHM — affichage, TCP, CSV)
      │
      ├── CSht20       (lecture température/humidité via I2C — S5)
      ├── CGpio        (commande LED via sysfs — S5)
      ├── QTimer       (acquisition périodique toutes les 2 secondes)
      ├── QTcpSocket   (envoi des données vers serveur TCP)
      └── QFile        (sauvegarde CSV locale horodatée)
```

| Classe | Fichiers | Responsabilité |
|--------|----------|----------------|
| `CIhmAppS6` | `cihmapps6.h/.cpp/.ui` | IHM, TCP, CSV |
| `CSht20` | `csht20.h/.cpp` | Lecture capteur SHT20 via I2C |
| `CGpio` | `cgpio.h/.cpp` | Commande LED via sysfs Linux |

---

## Le protocole TCP

TCP est un protocole de communication réseau qui garantit que les données arrivent **dans l'ordre** et **sans perte**. Avant tout envoi, une connexion est établie en 3 étapes (handshake) :

```
Raspberry (client)          PC / Serveur
──────────────────          ────────────
  SYN          ──────────►
               ◄──────────  SYN-ACK
  ACK          ──────────►
               ✓ Connexion établie
  "24,5;58,2"  ──────────►  (toutes les 2s)
```

Dans Qt, `QTcpSocket` gère tout cela automatiquement — il suffit d'appeler `connectToHost(ip, port)` et d'écrire dans le socket comme dans un fichier.

---

## Format des données

### Trame envoyée via TCP et écrite dans le CSV

```
date;heure;temperature_c;humidite_pct;etat_led
15/01/2024;14:32:05;24,5;58,2;NORMAL
15/01/2024;14:32:07;24,7;58,5;INTERMEDIAIRE
15/01/2024;14:32:09;31,2;60,1;SEUIL_MAX
```

### Valeurs possibles de `etat_led`

| Valeur | Signification |
|--------|--------------|
| `NORMAL` | Température sous le seuil Min — LED éteinte |
| `INTERMEDIAIRE` | Température entre Min et Max — LED orange |
| `SEUIL_MAX` | Température au dessus du seuil Max — LED rouge |

### Nom du fichier CSV

```
data_YYYYMMDD_HHMMSS.csv
ex : data_20240115_143205.csv
```

Le nom horodaté garantit qu'un nouveau fichier est créé à chaque lancement — aucun écrasement possible.

---

## Notions abordées

| Notion | Description |
|--------|-------------|
| Protocole TCP | Communication réseau fiable via `QTcpSocket` |
| Handshake TCP | Établissement de connexion en 3 étapes (SYN/SYN-ACK/ACK) |
| `QTcpSocket` | Classe Qt pour la communication TCP client |
| `QFile` + `QTextStream` | Écriture du fichier CSV local |
| Horodatage | `QDateTime::currentDateTime()` pour dater chaque mesure |
| Format CSV | Séparateur `;`, virgule décimale, colonnes nommées |
| Gestion erreurs socket | Signal `errorOccurred`, slots connecté/déconnecté |
| Modification en temps réel | IP, port et seuils modifiables sans redémarrer |
| Réutilisation POO | `CSht20` et `CGpio` réutilisés sans aucune modification |

---

## Portabilité Windows / Linux

| Comportement | Windows | Raspberry Pi |
|---|---|---|
| Capteur SHT20 | Valeurs simulées aléatoirement | Lecture I2C réelle |
| GPIO / LED | Simulé (`qDebug`) | Commande sysfs réelle |
| TCP | Fonctionnel (`127.0.0.1` pour test local) | Fonctionnel sur réseau Wi-Fi |
| CSV | Créé dans le dossier `build/debug/` | Créé dans le répertoire d'exécution |

---

## Dépendances et installation

### Sur Windows (développement / simulation)

Aucune dépendance supplémentaire par rapport à S5. Qt Network est inclus dans l'installation de base de Qt — si `#include <QTcpSocket>` génère une erreur, vérifier que `Qt::Network` est bien dans `target_link_libraries` du `CMakeLists.txt` et faire **Build → Clear CMake Configuration** puis recompiler.

---

### Sur Raspberry Pi

Mêmes dépendances que S5, plus aucune bibliothèque supplémentaire nécessaire — Qt Network fait partie des modules de base de Qt5.

```bash
# Dépendances S5 (déjà installées normalement)
sudo apt install qtbase5-dev qtbase5-dev-tools qt5-qmake cmake libi2c-dev git

# Qt Network est inclus dans qtbase5-dev — rien à installer en plus
```

---

## Test TCP sous Windows

Pour tester l'envoi TCP sans Raspberry, ouvrir un terminal et lancer un serveur d'écoute :

```bash
# Avec ncat (Nmap)
ncat -l 12345

# Avec netcat
nc -l -p 12345
```

Puis dans l'IHM, mettre `127.0.0.1` comme IP et `12345` comme port, cliquer **Connecter**. Les trames apparaissent dans le terminal à chaque acquisition.

---

## Démarche de travail avec l'IA Claude

Durant cette semaine, l'IA **Claude** (Anthropic) a de nouveau été utilisée comme outil d'aide au développement. La démarche a été la suivante :

1. **Explication du protocole TCP** — Claude a expliqué le fonctionnement du handshake, la différence entre client et serveur, et ce que ça donne concrètement sur une Raspberry Pi
2. **Identification des problèmes** — Après les premiers tests, Claude a identifié les bugs : erreur de surcharge de signal Qt6, fichier CSV mal formaté pour Excel FR, absence de gestion de déconnexion
3. **Propositions d'améliorations** — Claude a suggéré plusieurs idées dont certaines non demandées initialement : le flush immédiat du CSV, la séparation date/heure en deux colonnes, la virgule décimale pour Excel FR, et la modification de l'IP en temps réel sans redémarrage
4. **Adaptation et intégration** — Les propositions ont été relues, comprises et intégrées après validation

---

## Progression du projet

| Semaine | Contenu | Statut |
|---------|---------|--------|
| S1 | Découverte Qt Creator, première fenêtre | ✅ Terminé |
| S2 | IHM complète, signaux/slots | ✅ Terminé |
| S3 | QTimer, acquisition périodique | ✅ Terminé |
| S4 | GPIO, commande LED | ✅ Terminé |
| S5 | Capteur I2C SHT20, seuils Min/Max | ✅ Terminé |
| S6 | Communication TCP + sauvegarde CSV | ✅ Terminé |
| S7 | Structuration logicielle | ⏳ À venir |
| S8 | Finalisation + Soutenance | ⏳ À venir |

---

*BTS CIEL — 2024/2025*
