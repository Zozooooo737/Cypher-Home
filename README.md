# Cypher Home

Système embarqué de surveillance et de consultation à distance d'un logement.
Projet de Bachelor Informatique (parcours Cybersécurité) à l'ESILV, équipe E200.
Échéance : 1er décembre 2026.

## Le projet en bref

Cypher Home est un prototype fixé au mur d'un logement. Il :

- mesure l'environnement : température, humidité, pression et qualité de l'air (capteur BME680) ;
- détecte la fumée ou certains gaz (capteur MQ-2) ;
- surveille la pièce avec une caméra (ArduCAM OV5642) et enregistre une image en cas d'intrusion ;
- se consulte et se commande à distance depuis un tableau de bord web, sur ordinateur ou iPhone.

Sur place, un écran couleur, cinq boutons, quatre LED, un buzzer et un interrupteur général complètent l'usage.

Ce n'est pas une maison intelligente : le système mesure, surveille et alerte, mais ne régule rien et ne commande aucun équipement.

## Vue d'ensemble

```
Carte Arduino UNO R4 WiFi --Wi-Fi--> Serveur Python (PC) --> Base de données
                                            |
                                     Tableau de bord web
                                     (ordinateur / iPhone)
```

- **La carte** (C++, PlatformIO) lit les capteurs, pilote l'écran, les LED et le buzzer, et envoie ses données par Wi-Fi.
- **Le serveur** (Python) reçoit les données, les enregistre et renvoie les commandes à la carte.
- **Le tableau de bord** (Python) affiche les mesures, l'historique et les alertes, et permet de commander la surveillance.
- Les choix techniques précis (protocole, base de données, framework du tableau de bord) sont dans `DECISIONS.md`.

## Organisation du dossier

```
CypherHome/
├── README.md                  Ce fichier
├── CLAUDE.md                  Consignes permanentes pour Claude Code
├── CONTEXTE_TECHNIQUE.md      Composants et noms de broches
├── DECISIONS.md               Décisions, périmètre, avancement des fonctions, journal
├── PLAN.md                    Étapes à réaliser, ordre, cases cochées
├── .claude/commands/          Commandes BILAN et DOC pour Claude Code (bilan.md, doc.md)
├── firmware/                  Code C++ de la carte (projet PlatformIO)
│   ├── platformio.ini         Configuration PlatformIO
│   ├── src/ include/ lib/     Code du firmware (créés par PlatformIO)
│   └── tests/                 Nos programmes de test, un sous-dossier par composant
├── server/                    Serveur Python
│   ├── dashboard/             Tableau de bord web
│   └── db/                    Base de données (modèle, accès)
├── hardware/
│   ├── kicad/                 Schéma électronique
│   └── datasheets/            Fiches techniques des composants
├── cad/                       Modélisation 3D du boîtier et de la fixation murale
├── docker/                    Configuration Docker des services (si Docker est retenu, décision D8)
└── docs/
    ├── CONVENTION_COMMITS.md  Règles pour écrire les messages de commit
    ├── E200_CypherHome_CC.pdf Cahier des charges
    ├── E200_CypherHome_BC.pdf Bon de commande
    └── tests/                 Fiches de test, une par composant
```

Chaque dossier de code contient son propre `CLAUDE.md` avec les consignes propres à cette partie.

## Les fichiers de référence

Règle : **une information n'est écrite qu'à un seul endroit.**

| Fichier | Rôle |
|---|---|
| `CLAUDE.md` | Instructions que Claude Code lit au début de chaque session : règles de travail, arborescence, conventions. Les `CLAUDE.md` des sous-dossiers précisent les règles de chaque partie. |
| `CONTEXTE_TECHNIQUE.md` | Les composants physiques : références, datasheets, noms officiels des broches. Aucune décision ici. |
| `DECISIONS.md` | Source de vérité : périmètre, décisions prises (D1, D2…), décisions encore ouvertes, tableau des fonctions avec leur avancement, journal. Prioritaire sur le cahier des charges. |
| `PLAN.md` | Les étapes (P0.1 à P12.1) dans l'ordre, leurs dépendances, les risques et plans B, et les cases cochées. |

Deux commandes à taper dans la discussion Claude :

- **BILAN** : fait le point sur l'étape en cours et donne le texte exact à reporter dans `PLAN.md` et `DECISIONS.md`.
- **DOC** : produit le texte prêt à coller dans Notion pour le compte rendu.

## Workflow de travail

**Au début d'une séance**

1. Récupérer le travail des autres : dans VS Code, Source Control → **Pull**. (Un simple *fetch* télécharge sans mettre à jour les fichiers ; *pull* fait les deux. ; pio run -d firmware -t compiledb)
2. Si `CONTEXTE_TECHNIQUE.md`, `DECISIONS.md` ou `PLAN.md` ont changé, les réimporter dans le projet Claude Desktop : Claude travaille sur des copies.
3. Ouvrir `PLAN.md` : la première étape non cochée est l'étape du jour. Vérifier ses « Dépend de » et les décisions Dn encore ouvertes.

**Pendant la séance**

4. Développer, une étape à la fois. Claude Desktop sert à réfléchir (schémas, choix, explications) ; Claude Code sert à écrire le code volumineux.
5. Tester sur le matériel et noter les résultats (mesures, photos, sortie série) dans une fiche de `docs/tests/`.

**À la fin de la séance**

6. Taper **BILAN** dans Claude Desktop. Lui fournir les faits réels (mesures, résultats, photos) : il ne voit pas ce qui a été fait dans Claude Code. À faire aussi si l'étape n'est pas finie : le bilan note ce qui est en cours.
7. Relire chaque ligne proposée, puis la reporter dans `PLAN.md` (cases) et `DECISIONS.md` (tableau des fonctions, décisions, journal).
8. Réimporter `DECISIONS.md` et `PLAN.md` mis à jour dans Claude Desktop.
9. Commit et push :
   - vérifier dans Source Control la liste des fichiers modifiés (aucun secret) ;
   - demander à Claude un message de commit conforme à `docs/CONVENTION_COMMITS.md` ;
   - commit, puis **Push**.
10. Taper **DOC** et coller le résultat dans Notion.

**Règles pour travailler à plusieurs**

- Toujours faire un pull avant de modifier `DECISIONS.md` ou `PLAN.md` (ce sont les fichiers les plus à risque de conflit), et les commit et pousser juste après.
- Prévenir l'équipe avant de modifier la même partie qu'un autre membre.
- Petits commits fréquents plutôt qu'un gros commit en fin de semaine.
- En cas de conflit, VS Code propose un éditeur de fusion : garder les deux versions utiles, ne pas écraser.

## Sécurité

- Aucun mot de passe, clé ni identifiant (Wi-Fi, MQTT, base de données, SMTP) dans Git. Les secrets vont dans des fichiers non versionnés (`.env`, fichier de configuration du firmware) listés dans `.gitignore`.
- Si un secret est commité par erreur, le changer immédiatement : le supprimer du dernier commit ne suffit pas, il reste dans l'historique.

## Récupérer le projet sur un autre poste

1. Installer Git, VS Code et l'extension **PlatformIO IDE**.
2. Cloner le dépôt : `Ctrl+Maj+P` → « Git: Clone ».
3. Recréer les fichiers de secrets en local (ils ne sont pas dans le dépôt).
