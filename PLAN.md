# PLAN DE RÉALISATION : Cypher Home

> Fichier de suivi. Il contient l'ordre des étapes, leurs dépendances, les risques et plans B, et les cases cochées. Il ne contient ni décisions ni journal : voir `DECISIONS.md` (décisions Dn, périmètre, tableau des fonctions, journal).
> Priorités : ESSENTIEL / IMPORTANT / BONUS. Marqueur : [H] hypothèse à vérifier.
> Aucune date ni durée : seul l'ordre compte.
> Cases : une étape se coche `[x]` quand son critère de validation est atteint. Une phase se coche quand toutes ses étapes ESSENTIEL et IMPORTANT sont cochées (les étapes BONUS ne sont pas requises).
> « Dn » dans « Dépend de » renvoie à une décision ouverte de `DECISIONS.md` : elle doit être tranchée avant l'étape.

## Tableau de bord d'avancement
| Phase | Sujet                              | Priorité          | État |
| ----- | ---------------------------------- | ----------------- | ---- |
| P0    | Cadrage                            | ESSENTIEL         | [x]  |
| P1    | Test des composants                | ESSENTIEL         | [x]  |
| P2    | Test réseau précoce                | ESSENTIEL         | [ ]  |
| P3    | Câblage et schéma KiCad            | ESSENTIEL         | [ ]  |
| P4    | Code de vérification au démarrage  | ESSENTIEL         | [ ]  |
| P5    | Modèle de données et serveur       | ESSENTIEL         | [ ]  |
| P6    | Firmware                           | ESSENTIEL         | [ ]  |
| P7    | Dashboard                          | ESSENTIEL         | [ ]  |
| P8    | Intégration et tests               | ESSENTIEL         | [ ]  |
| P9    | Boîtier 3D et fixation (parallèle) | IMPORTANT         | [ ]  |
| P10   | Sécurité (au fil de l'eau)         | ESSENTIEL / BONUS | [ ]  |
| P11   | Batterie et solaire                | BONUS             | [ ]  |
| P12   | Compte rendu final                 | ESSENTIEL         | [ ]  |

Ordre : P0 → P1 → (P2 en parallèle dès P1.1 faite) → P3 → P4 → P5/P6 → P7 → P8 → P11. P9, P10 et P12 avancent en parallèle.

---

## P0 : Cadrage

- [x] **P0.1 Fixer le périmètre minimal** (ESSENTIEL)
  - Objectif : donner un statut à chaque fonction du CC.
  - Livrable : sections « Périmètre » et « Tableau des fonctions » de DECISIONS.md.
  - Validation : chaque fonction F1 à F18 a un statut.
  - Dépend de : rien.
- [x] **P0.2 Dépôt Git et projet PlatformIO** (ESSENTIEL)
  - Objectif : base de travail propre.
  - Tâches : dépôt Git, arborescence (voir CLAUDE.md), `.gitignore` (secrets, `.env`), projet PlatformIO vide pour UNO R4 WiFi.
  - Livrable : dépôt + `firmware/platformio.ini`.
  - Validation : `pio run` compile un programme vide.
  - Dépend de : rien.
- [x] **P0.3 Modèle de fiche de test et structure du firmware** (IMPORTANT)
  - Objectif : documenter chaque composant de la même façon et fixer la structure du firmware avant d'y ajouter le premier composant.
  - Tâches : modèle de fiche ; squelette du firmware (dossiers, `pins.h`, `config.h`, `self_test`) ; `.gitignore` ; arborescence mise à jour dans CLAUDE.md.
  - Livrable : `docs/tests/_modele.md` + squelette dans `firmware/` (convention dans DECISIONS.md, section Méthode).
  - Validation : le modèle est réutilisable tel quel ; `pio run -d firmware` compile le squelette.
  - Dépend de : P0.2.

**À collecter :** structure du dépôt, `platformio.ini`, modèle de fiche.

---

## P1 : Test des composants un par un

Chaque composant validé produit : un module `firmware/src/components/<composant>.h/.cpp` (setup, fonctions principales réutilisées par le firmware final, fonction de test) ; ses broches dans `firmware/include/pins.h` et ses paramètres utiles au produit dans `config.h` ; une ligne dans `self_test_run()` une fois validé sur le matériel ; une fiche d'une page `docs/tests/<composant>.md` (modèle `_modele.md`, rédigée après validation, sans code). Code et fiche sont committés ensemble, par l'étudiant. Il n'y a pas de dossier `firmware/tests/`. Le texte et les images reçus par le port série passent par `firmware/scripts/serial_monitor.py` (images enregistrées dans `firmware/output/`, ignoré par Git).

- [x] **P1.1 Caméra OV5642** (ESSENTIEL, à faire en premier)
  - Objectif : savoir si la caméra est exploitable sur cette carte.
  - Tâches : lire les docs ArduCAM, câbler (SPI + I2C), tester la communication, capturer une image, mesurer taille du JPEG, temps de capture et de transfert, tester les hautes résolutions. Noter la bibliothèque utilisée (alimente D2). [H] RAM limitée : lecture de l'image par blocs depuis le tampon de la caméra, à confirmer. Écrire `firmware/scripts/serial_monitor.py` ; l'image est envoyée au PC par le port série USB (pas de Wi-Fi ni de HTTP à ce stade).
  - Livrable : `firmware/src/components/camera.h/.cpp` + `firmware/scripts/serial_monitor.py` + fiche `docs/tests/camera.md` + 1 photo choisie copiée dans `docs/tests/`.
  - Validation : un JPEG valide arrive sur le PC, mesures notées.
  - Dépend de : P0.2, P0.3.
- [x] **P1.2 BME680** (ESSENTIEL)
  - Objectif : lire température, humidité, pression, gaz.
  - Tâches : trancher D1 (SPI ou I2C) ; vérifier dans le manuel Joy-IT le rôle de VIN et 3Vo ; programme de lecture.
  - Livrable : module `components/bme680` + fiche.
  - Validation : valeurs plausibles et stables.
  - Dépend de : P0.2, D1.
- [x] **P1.3 MQ-2** (IMPORTANT)
  - Objectif : détecter une variation fumée/gaz.
  - Tâches : câbler VCC/GND/A0 ; enregistrer un démarrage à froid d'environ 25 min ; fixer la durée de préchauffage ; observer A0 en air normal puis avec une source faible ; tester une source faible (fumée d'allumette éteinte). Sortie A0 compatible avec l'ADC de la carte : vérifié (0,41 à 2,83 V mesurés, sans saturation).
  - Livrable : module `components/mq2` + fiche (valeurs notées).
  - Validation : variation nette entre air normal et source ; seuil initial proposé.
  - Dépend de : P0.2.
- [x] **P1.4 Écran ST7735S** (IMPORTANT)
  - Objectif : afficher texte et couleurs.
  - Tâches : câbler en tenant compte du 3,3 V ; déterminer si une adaptation de niveaux est nécessaire sur SCL/SDA/RES/DC/CS ; câbler BLK. Adaptation retenue : diviseurs 1 kOhm / 2 kOhm sur les 5 signaux ; VCC et BLK sur 3.3V.
  - Livrable : module `components/display` + fiche.
  - Validation : affichage correct, aucun composant chaud.
  - Dépend de : P0.2.
- [x] **P1.5 LED, buzzer, boutons, interrupteur DS12A** (ESSENTIEL : buzzer et interrupteur ; IMPORTANT : LED et boutons)
  - Note : DS12A : test de continuité et fiche non faits, reportés avant P3.2 (décision de l'étudiant, 2026-11-09).
  - Objectif : valider les entrées/sorties simples.
  - Tâches : calculer les résistances des LED ; buzzer passif avec `tone()` ; boutons en `INPUT_PULLUP` + anti-rebond ; test de continuité des pattes des boutons (4) et du DS12A (3) pour fixer leur nommage dans CONTEXTE_TECHNIQUE.md.
  - Livrable : modules `components/leds`, `buzzer`, `buttons` + fiches (le DS12A a une fiche, sans module) + noms de pattes validés.
  - Validation : chaque actionneur réagit, chaque bouton est lu proprement.
  - Dépend de : P0.2.
- [x] **P1.6 Matrice de compatibilité électrique** (ESSENTIEL)
  - Objectif : éviter les erreurs avant le câblage.
  - Tâches : tableau composant / tension d'alimentation / niveau logique / courant (datasheet ; mesure du courant total en P1.7) / bus / broche CS ou adresse I2C.
  - Livrable : `docs/compatibilite.md`.
  - Validation : aucune cellule vide, chaque conflit a une solution.
  - Dépend de : P1.1 à P1.5.
- [x] **P1.7 Essai d'ensemble sur breadboard** (ESSENTIEL)
  - Note : courant total non mesuré, reporté à la fin du projet (décision de l'étudiant, 2026-11-08) ; validation fondée sur le fonctionnement simultané sans reset.
  - Objectif : vérifier que tout fonctionne en même temps.
  - Tâches : tous les modules alimentés, bus partagés actifs, lecture simultanée capteurs + écran + caméra ; surveiller resets et valeurs erratiques ; vérifier le courant total ; proposer l'affectation des broches (soumise à D3) ; relire l'index de `pins.h` (aucune broche utilisée deux fois, tout tient sur la carte).
  - Livrable : photo + fiche + `pins.h` complet + table d'affectation des broches proposée.
  - Validation : fonctionnement simultané sans reset ; `self_test_run()` passe avec tous les composants branchés. 
  - Dépend de : P1.6, D2.

**À collecter :** datasheets, fiches de test, mesures de courant, taille d'image, photos des montages, matrice de compatibilité.

---

## P2 : Test réseau précoce (dès que P1.1 est faite, en parallèle de P1.2 à P1.5)

- [x] **P2.1 Wi-Fi via partage de connexion iPhone (2,4 GHz)** (ESSENTIEL)
  - Note : critère atteint le 2026-11-10 (IP 172.20.10.2, reconnexion seule après débranchement de l'USB, pertes et reprises affichées) ; fiche `docs/tests/wifi.md` encore à rédiger.
  - Tâches : connecter la carte ; [H] activer « Maximiser la compatibilité » sur l'iPhone si le réseau n'est pas vu.
  - Livrable : programme + fiche.
  - Validation : la carte obtient une IP et se reconnecte après redémarrage.
  - Dépend de : P0.2.
- [x] **P2.2 Broker MQTT local (Mosquitto) avec authentification** (ESSENTIEL)
  - Note : critère atteint le 2026-11-11 (message de la carte reçu par mosquitto_sub, connexions sans mot de passe et avec mauvais mot de passe refusées : not authorised) ; Mosquitto sous Docker ; cours dans docs/cours/mqtt.md.
  - Rappel : MQTT = la carte publie des messages sur des « sujets », un broker les redistribue aux abonnés.
  - Tâches : installer Mosquitto (D8 : Docker ou natif), utilisateur/mot de passe hors Git, publier depuis la carte, recevoir sur le PC.
  - Livrable : config Mosquitto + programme.
  - Validation : message reçu ; connexion refusée sans mot de passe.
  - Dépend de : P2.1, D8.
- [x] **P2.3 Envoi HTTP d'un gros fichier** (ESSENTIEL)
  - Note : critère atteint le 2026-11-11 (16, 32, 64 et 128 ko reçus intacts : taille, SHA-256 et motif vérifiés par le serveur ; débit d'environ 10,3 ko/s en blocs de 512 octets, 10,8 ko/s en blocs de 2048) ; fiche `docs/tests/http.md`.
  - Tâches : POST de plusieurs dizaines de ko vers un petit serveur Python.
  - Livrable : script serveur de test + fiche.
  - Validation : fichier reçu intact, débit noté.
  - Dépend de : P2.1.
- [x] **P2.4 Accès à une page du PC depuis l'iPhone** (IMPORTANT)
  - Note : critère atteint le 2026-11-11 (page de `server/test_page_server.py` affichée par localhost, par l'IP du PC et par `<nom du PC>.local`, sur le PC, sur l'iPhone qui émet le partage et sur un second téléphone connecté au partage) ; changement d'IP non testé.
  - Tâches : page de test ; [H] un second iPhone connecté au partage de connexion accède au serveur du PC ; [H] gérer le changement d'IP éventuel.
  - Validation : page affichée sur l'iPhone.
  - Dépend de : P2.1.
- [ ] **P2.5 Photo de bout en bout** (ESSENTIEL)
  - Tâches : photo de P1.1 envoyée de la carte au PC via HTTP.
  - Validation : image ouvrable, temps total noté.
  - Dépend de : P1.1, P2.3.

**À collecter :** schéma du réseau, config du broker, mesures de débit.

---

## P3 : Câblage et schéma KiCad

- [ ] **P3.1 Bibliothèque KiCad du projet** (ESSENTIEL)
  - Objectif : symboles et empreintes des modules avec les noms de broches officiels.
  - Livrable : bibliothèque KiCad du projet dans `hardware/kicad/`.
  - Validation : chaque module a son symbole.
  - Dépend de : P1.7.
- [ ] **P3.2 Schéma complet** (ESSENTIEL)
  - Tâches : suivre la table de broches validée (D3) ; ajouter résistances, adaptation de niveaux éventuelle, interrupteur.
  - Livrable : `hardware/kicad/cypher-home.kicad_sch` + export PDF.
  - Validation : revue ligne par ligne contre la matrice de compatibilité.
  - Dépend de : P3.1, D3, D10.
- [ ] **P3.3 Câblage du prototype** (ESSENTIEL)
  - Tâches : câbler selon le schéma ; liste de contrôle (continuité, polarité, pas de court-circuit) avant mise sous tension ; vérifier que pins.h correspond au câblage réel.
  - Livrable : photo + liste de contrôle.
  - Validation : prototype conforme au schéma.
  - Dépend de : P3.2.

**À collecter :** schéma KiCad (PDF), table des broches finale.

---

## P4 : Code de vérification au démarrage

- [ ] **P4.1 Finaliser `self_test_run()`** (ESSENTIEL)
  - Tâches : `self_test_run()` existe depuis P0.3 et reçoit une ligne par composant validé en P1 ; ajouter le test Wi-Fi (P2.1) ; afficher un récapitulatif OK/KO par composant ; déplacer l'appel de `loop()` vers `setup()` (exécution unique) ; un composant débranché donne un KO clair sans bloquer le démarrage ; l'image du test caméra n'est pas envoyée si aucun PC n'est connecté au port série. [H]
  - Livrable : `firmware/src/self_test.cpp`.
  - Validation : tout passe ; un composant débranché donne un KO clair.
  - Dépend de : P3.3, P2.1.

**À collecter :** capture de la sortie série.

---

## P5 : Modèle de données et serveur

- [ ] **P5.1 Modèle de données** (ESSENTIEL)
  - Tâches : confirmer la base (D7), puis collections (mesures, événements, configuration, utilisateurs, images), champs, index, expiration automatique à 30 jours.
  - Livrable : entités et attributs consignés dans DECISIONS.md, section « Modèle de données (D7) ».
  - Validation : chaque fonction retenue (ESSENTIEL et IMPORTANT) a sa collection.
  - Dépend de : P0.1, P2.2, P2.3, D4, D7.
- [ ] **P5.2 Serveur Python** (ESSENTIEL)
  - Tâches : réception MQTT et HTTP, validation des entrées, écriture en base, API pour le dashboard (code dans Claude Code).
  - Livrable : `server/`.
  - Validation : mesures simulées enregistrées, valeurs invalides rejetées.
  - Dépend de : P5.1, P2.2.
- [ ] **P5.3 Authentification du dashboard** (ESSENTIEL)
  - Tâches : identifiant + mot de passe haché, secrets dans `.env`.
  - Validation : accès refusé sans connexion.
  - Dépend de : P5.2.

**À collecter :** schéma de la base, architecture du serveur.

---

## P6 : Firmware (en parallèle de P5.2)

- [ ] **P6.1 Mesures BME680 (5 s), conformité par valeurs par défaut, LED d'état, publication MQTT, statut de connexion (LWT)** (ESSENTIEL ; LED : IMPORTANT). Validation : valeurs reçues par le serveur, statut visible. Dépend de : P4.1, P2.2.
- [ ] **P6.2 MQ-2 : lecture, seuil, alerte, buzzer** (IMPORTANT). Validation : dépassement détecté et publié. Dépend de : P6.1.
- [ ] **P6.3 Surveillance, détection de présence, délai de 10 s, intrusion, réception des commandes du serveur (activation/désactivation, PIN)** (ESSENTIEL ; saisie du PIN avec les boutons : IMPORTANT). PIN : haut=1, droite=2, bas=3, gauche=4, centre=valide. Validation : scénario local complet (détection, délai de 10 s, neutralisation ou intrusion), puis commande reçue du serveur. Dépend de : P6.1, P1.1.
- [ ] **P6.4 Envoi de l'image d'intrusion (HTTP)** (ESSENTIEL). Validation : image reçue et stockée. Dépend de : P6.3, P2.5.
- [ ] **P6.5 Écran et menus** (IMPORTANT). Validation : navigation fonctionnelle. Dépend de : P6.1.
- [ ] **P6.6 Réglages reçus du serveur (consignes : IMPORTANT ; veille écran et mode « respect de l'environnement » : BONUS)**. Validation : réglage appliqué sur la carte. Dépend de : P6.5, P5.2.
- [ ] **P6.7 Estimation du nombre de personnes** (BONUS). Validation : estimation publiée à partir des images. Dépend de : P6.4.

---

## P7 : Dashboard

- [ ] **P7.1 Mesures, états de conformité, état du système (connexion, fraîcheur des données)** (ESSENTIEL). Validation : lisible sur PC ; sur iPhone (IMPORTANT). Dépend de : D5, P5.2.
- [ ] **P7.2 Commande de surveillance, fenêtre PIN, alertes, image d'intrusion** (ESSENTIEL). Validation : activation/désactivation à distance. Dépend de : P7.1, P6.3.
- [ ] **P7.3 Consignes, tolérances, seuils** (IMPORTANT). Validation : modification visible sur la carte. Dépend de : P7.1, P6.6.
- [ ] **P7.4 Historique (graphiques d'évolution, liste d'événements), notifications web** (IMPORTANT ; réglage de la durée de conservation : BONUS). Validation : graphiques et événements visibles, notification affichée. Dépend de : P7.1.
- [ ] **P7.5 Courriel d'alerte avec image ; notifications configurables par type** (BONUS). Validation : courriel reçu après une intrusion. Dépend de : P8.1.

---

## P8 : Intégration et tests

- [ ] **P8.1 Scénario d'intrusion complet** (ESSENTIEL)
  - Tâches : activer la surveillance, détecter, délai de 10 s, PIN (cas neutralisé et cas non neutralisé), image enregistrée, notification dans l'app.
  - Livrable : fiche de test + vidéo.
  - Validation : les deux cas suivent la section 11.2 du CC.
  - Dépend de : P6.4, P7.2.
- [ ] **P8.2 Autres scénarios** (IMPORTANT) : fumée/gaz, perte de communication, commandes contradictoires (distant prioritaire). Dépend de : P8.1.
- [ ] **P8.3 Critères d'acceptation A1 à A19 retenus** (ESSENTIEL) : tableau avec statut et preuve pour chaque critère (A20, batterie, relève de P11). Dépend de : P8.2.

---

## P9 : Boîtier 3D et fixation (en parallèle, après P1.7)

- [ ] **P9.1 Relevé des cotes et disposition** (IMPORTANT) : dimensions réelles des modules (pied à coulisse), disposition écran, boutons, LED, caméra, interrupteur (perçage 20,5 mm), accès USB. Livrable : croquis + cotes. Dépend de : P1.7.
- [ ] **P9.2 Modélisation, impression, itérations** (IMPORTANT) : fixation murale, ventilation vers BME680 et MQ-2, champ de la caméra. Livrable : fichiers 3D dans `cad/` + photos. Dépend de : P9.1, P3.3.
- [ ] **P9.3 Montage final et test de chauffe** (IMPORTANT) : [H] le chauffage du MQ-2 peut fausser la température du BME680, à mesurer. Livrable : photo + mesure. Dépend de : P9.2, P8.3.

---

## P10 : Sécurité

- [ ] **P10.1 Revue de sécurité simple** (ESSENTIEL) : secrets hors Git, auth MQTT, validation des entrées, mots de passe hachés, accès aux images réservé. Validation : aucun secret dans l'historique Git. Dépend de : P5.3.
- [ ] **P10.2 TLS (MQTT et HTTPS) et durcissement** (BONUS). Dépend de : P8.3.

---

## P11 : Batterie et solaire (BONUS, après P8.3)

- [ ] **P11.1 Vérifications du CC 19.2** : polarité JST, compatibilité BMS / SunFlower, tension du convertisseur réglée à vide avant de le relier à la carte. Livrable : fiche de mesures. Dépend de : P8.3.
- [ ] **P11.2 Raccordement et mesure de consommation** : puis mesure réelle. Validation : prototype fonctionne sur batterie (critère A20). Dépend de : P11.1, D6.

---

## P12 : Compte rendu final (collecte continue)

- [ ] **P12.1 Assembler le compte rendu** (ESSENTIEL) : composants et datasheets, matrice de compatibilité, schéma KiCad, étapes de développement, architecture PlatformIO, architecture du projet, modèle de base, résultats des tests (A1 à A19), limites. Utiliser la commande DOC après chaque phase pour alimenter Notion. Dépend de : P8.3.

---

## Risques et plans B
| Risque                                                    | Plan B                                                                                                                                                           |
| --------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Image trop lourde ou trop lente                           | Réduire résolution ou débit ; envoi par blocs.                                                                                                                   |
| Détection de présence trop gourmande sur la carte         | Comparaison d'images sur le serveur (photos périodiques en mode surveillance). [H] à tester.                                                                     |
| Surveillance caméra irréaliste                            | Photo sur événement ou à la demande, sans détection ; sinon retirer F7 et envisager un autre capteur de présence (achat à valider).                              |
| Partage SPI conflictuel                                   | BME680 en I2C ; un CS par périphérique SPI ; réduire la fréquence SPI.                                                                                           |
| Écran 3,3 V abîmé par du 5 V                              | Adaptation de niveaux dès P1.4.                                                                                                                                  |
| Chutes de tension ou resets                               | Courant total mesuré en fin de projet (reporté) ; surveiller les resets dès l'activation du Wi-Fi (P2.1) ; vérifier le point d'alimentation du MQ-2 ; découpler. |
| Wi-Fi iPhone instable ou IP changeante                    | Noter l'IP au démarrage ; reconnexion automatique dans le firmware.                                                                                              |
| Dérive des valeurs MQ-2                                   | Préchauffage ; seuil relatif à la mesure initiale.                                                                                                               |
| Port série USB bloquant ou perte d'octets sans PC branché | Tester l'état du port avant d'envoyer ; image non envoyée si aucun PC. [H] à vérifier sur la R4.                                                                 |
