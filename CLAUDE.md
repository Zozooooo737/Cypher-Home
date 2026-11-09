# Cypher Home : instructions pour Claude Code

<!-- Maintenance : les blocs « Règles de travail », « Commande BILAN » et « Commande DOC » ont un jumeau identique dans les Instructions du projet Claude Desktop (fichier 01_instructions_projet_claude_ai.md). Toute modification se fait des deux côtés. -->

Cypher Home : système embarqué fixé au mur d'un logement, qui mesure l'environnement (BME680, MQ-2) et surveille par caméra (ArduCAM OV5642), le tout consultable et commandable à distance via un tableau de bord web. **Pas une maison intelligente** : aucune régulation ni commande d'équipements. Projet de Bachelor Informatique (cybersécurité), équipe E200. L'étudiant est débutant en électronique, réseau et MQTT.

## Fichiers de référence
Une information vit dans un seul fichier. Ne les contredis pas ; en cas de conflit, `DECISIONS.md` l'emporte sur le cahier des charges.

| Fichier | Contient |
|---|---|
| `CONTEXTE_TECHNIQUE.md` | composants, références, noms de broches officiels (à utiliser partout) |
| `DECISIONS.md` | périmètre, échéance, décisions prises, décisions ouvertes (D1…), tableau des fonctions et avancement, journal |
| `PLAN.md` | étapes ordonnées (P0.1 à P12.1), dépendances, risques et plans B, cases cochées |
| `docs/` | cahier des charges (CC, visé, plus large que le périmètre réel) et bon de commande (BC) ; fiches de test des composants dans `docs/tests/` |

À lire au début de chaque session : `DECISIONS.md`, `PLAN.md`, `CONTEXTE_TECHNIQUE.md` ; `docs/` seulement si un détail fonctionnel manque. Pour tout travail dans `firmware/`, lis aussi `firmware/CLAUDE.md` : il contient la structure du firmware et les règles à appliquer à chaque composant.
Le dépôt est la copie de référence. Une copie de `CONTEXTE_TECHNIQUE.md`, `DECISIONS.md` et `PLAN.md` existe dans le projet Claude Desktop : l'étudiant la remet à jour à la main après chaque modification.

## Organisation du dépôt
```text
CypherHome/
├── CLAUDE.md                # ce fichier (commun)
├── CONTEXTE_TECHNIQUE.md
├── DECISIONS.md
├── PLAN.md
├── docs/                    # CC, BC, CONVENTION_COMMITS.md
│   └── tests/               # _modele.md, fiches de test (une par composant), matrice de compatibilité
├── hardware/                # schéma KiCad, datasheets (voir hardware/CLAUDE.md)
│   ├── kicad/
│   └── datasheets/
├── firmware/                # C++ PlatformIO (voir firmware/CLAUDE.md : structure et règles)
│   ├── include/             # pins.h, config.h, secrets.h (ignoré par Git)
│   ├── src/                 # main.cpp, self_test, components/ (un module par composant)
│   ├── scripts/             # serial_monitor.py (Python, son propre .venv)
│   └── output/              # images de test (ignoré par Git)
├── server/                  # Python, un seul .venv pour tout server/ (voir server/CLAUDE.md)
│   ├── db/                  # couche MongoDB / PyMongo (voir server/db/CLAUDE.md)
│   └── dashboard/           # tableau de bord web (voir server/dashboard/CLAUDE.md)
├── docker/                  # MongoDB + Mosquitto (voir docker/CLAUDE.md)
└── cad/                     # boîtier 3D
```
Travaille dans le dossier concerné par la tâche (hardware, firmware, server, docker) ; ne modifie pas un autre dossier sans le dire.

## Conventions du dépôt
- **Langue** : noms de fichiers, de fonctions et de variables du code en anglais (`snake_case`, fonctions préfixées par le nom du composant, par exemple `camera_setup()`). Commentaires de code et documents `.md` en français.
- **Pas de dossier de tests séparé** : chaque composant du firmware a un module qui contient sa propre fonction de test (voir `firmware/CLAUDE.md`).
- **Fiches de test** : une par composant dans `docs/tests/<composant>.md` (les 4 LED partagent une fiche), nom identique à celui du module, modèle `docs/tests/_modele.md`. Une page, sans code (elle renvoie au fichier du dépôt), rédigée une fois le composant validé sur le matériel par l'étudiant. Claude Code ne la rédige que si l'étudiant le demande, à partir des seules informations fournies (mesures, sortie série, photo) : jamais de valeur inventée, section non renseignée = « à compléter ». Comme elle est hors du dossier de travail, le dire.
- **Câblage, trois niveaux** : la fiche garde le câblage *testé* (daté) ; `firmware/include/pins.h` garde le câblage *courant* ; la table D3 validée dans `DECISIONS.md` fait foi.
- **Python** : un `.venv` par dossier Python (`firmware/scripts/`, `server/`), jamais committé. VS Code n'utilise qu'un interpréteur à la fois : l'étudiant sélectionne celui du dossier courant.
- **Fichiers ignorés par Git** : secrets (`secrets.h`, `.env`), `.venv`, `firmware/output/` (les images de test montrent l'intérieur du logement). Pour une fiche, une seule photo choisie, sans élément personnel, est copiée dans `docs/tests/`.

## Git : seul l'étudiant commite
**Claude Code ne lance JAMAIS** `git add`, `git commit` (y compris `git commit --amend`), `git push`, `git reset`, `git stash` ni `git rebase`. Seul l'étudiant commite.
- Autorisé : `git status`, `git diff`, `git log` (lecture seule).
- À la fin de chaque tâche, Claude Code fournit **uniquement** les commits à faire, selon `docs/CONVENTION_COMMITS.md` (section « Livraison des commits par Claude Code ») : commits thématiques numérotés, commandes `git add` exactes, puis titre et paragraphe de 4 lignes maximum, sans puces, dans des blocs de code. Il signale tout fichier de secrets modifié (`secrets.h`, `.env`).
- Filet de sécurité : règles `deny` dans `.claude/settings.json`. Elles ne remplacent pas cette règle.

## Règles de travail
- Explications courtes ; développe seulement sur demande. Code fonctionnel directement, sans pédagogie ligne à ligne (code et commentaires : voir « Règles de code »).
- **Ne jamais prétendre qu'un montage ou un code a été testé.** Indique ce qui reste à vérifier sur le matériel, en fin de réponse.
- Sépare toujours faits vérifiés / hypothèses / choix à valider par l'étudiant. N'invente ni valeurs, ni prix, ni résultats de tests, ni décisions. Si une information manque, pose peu de questions, mais ciblées.
- Une décision ouverte (liste « À valider ou à trancher » de `DECISIONS.md`) n'est pas définitive : compare vite les options, recommande, puis attends la validation. Aucune affectation de broche ni choix de protocole n'est définitif tant qu'il n'est pas dans « Décidé ».
- Dis franchement si une idée est irréaliste ou trop ambitieuse vu l'échéance (voir `DECISIONS.md`) et propose une version réduite (ordre de réduction du périmètre dans `DECISIONS.md`).
- Électronique : avant tout schéma ou câblage, vérifie les niveaux logiques (la carte est en 5 V, plusieurs modules sont en 3,3 V), les courants et le partage des bus (SPI notamment).
- Vérifie la compatibilité de toute bibliothèque avec l'Arduino UNO R4 WiFi (Renesas RA4M1, Wi-Fi via ESP32-S3).
- Explique brièvement toute notion de cybersécurité, réseau, MQTT ou électronique que tu introduis (pare-feu Windows, profil réseau privé, `.venv` qui isole les bibliothèques Python, Docker qui isole des logiciels tiers complets : ce ne sont pas des alternatives équivalentes).
- Sécurité, dès le développement : secrets dans un fichier de configuration non versionné (`.gitignore`), identifiants MQTT, validation des entrées, aucun mot de passe en dur. TLS et durcissement avancé : bonus de fin de projet.
- Avant un gros développement, propose un plan court et attends la validation.
- Commits : voir « Git : seul l'étudiant commite » ; messages selon `docs/CONVENTION_COMMITS.md`, jamais de secrets.

## Règles de code
- **Code minimal** : aller droit à l'essentiel ; ne produire que ce que la tâche demande. Pas de fonction, de constante, d'include, de test, de message, de cas d'erreur ni de paramètre « au cas où ». Une fonction n'existe que si elle est appelée.
- Avant de rendre le travail : le relire et supprimer tout ce qui est inutile. Niveau attendu : `firmware/src/components/bme680.cpp`.
- En cas de doute entre ajouter et ne pas ajouter : ne pas ajouter, et poser la question à l'étudiant.
- **Commentaires** : en français, simples, en tête de bloc. Firmware C++ : règle plus stricte dans `firmware/CLAUDE.md` (« Règles de code du firmware »), avec celle de `config.h`.

## Convention d'affichage des tests
S'applique à tous les composants du firmware (`<composant>_test()`) et aux lignes `[moniteur]` de `firmware/scripts/serial_monitor.py`.
- Chaque test commence par la bannière `----- TEST NOM -----` (affichée par `self_test.cpp`) et se termine par une ligne vide.
- Une valeur par ligne : `[étiquette] libellé ..... valeur unité`. Les points alignent toutes les valeurs sur une même colonne (`SELF_TEST_VALUE_COLUMN` dans `self_test.cpp`, `VALUE_COLUMN` dans `serial_monitor.py` : même valeur).
- Les lignes sont écrites uniquement avec `self_test_print()` (`self_test.h` : texte, entier avec unité, flottant avec unité) ; côté Python, `print_value()`. Personne ne recopie la logique des points.
- Échec (composant absent, bus muet, lecture invalide) : **une seule ligne**, `[étiquette] KO`, sans explication, sans conseil de câblage ni scan. Le diagnostic se fait avec la fiche `docs/tests/<composant>.md`.
- Pas de fonction `<composant>_last_error()` : les fonctions principales renvoient simplement `false`.

```text
----- TEST CAMERA -----
[moniteur] photo enregistree ... cam_20261108-125603.jpg
[moniteur] taille .............. 52604 octets
[moniteur] transfert carte ..... 1265 ms
[camera] resolution ............ 1024x768
[camera] format ................ JPEG
[camera] capture ............... 218 ms

----- TEST BME680 -----
[bme680] temperature ........... 23.03 C
[bme680] humidite .............. 47.59 %
[bme680] pression .............. 1009.37 hPa
[bme680] gaz ................... 29.40 kOhm
```

## Commande BILAN
Quand l'étudiant écrit « BILAN », réponds en trois blocs, dans cet ordre. Appuie-toi sur PLAN.md (étapes Px.x), DECISIONS.md et sur ce qui a été fait ou fourni pendant la session.

Étape de référence : c'est la première étape non cochée de PLAN.md, en lisant le fichier de haut en bas (P0.1, P0.2, P0.3, P1.1, etc.), en ignorant les étapes BONUS. Détermine-la uniquement d'après les cases de PLAN.md, jamais d'après l'activité récente, le journal ou la colonne Impl. de DECISIONS.md. Avant de la traiter, vérifie ses « Dépend de » ; si une dépendance n'est pas cochée, dis-le.

**1. Bilan**
- Fait récemment : ce qui a réellement été réalisé ou mesuré (faits fournis par l'étudiant), avec le travail hors séquence signalé comme tel.
- Reste à faire dans l'étape de référence (Px.x) : uniquement les tâches, le livrable et le critère de validation de cette étape dans PLAN.md, en indiquant ce qui est déjà acquis. Ne parle pas des sous-missions d'une autre étape.
- Prochaine étape : si l'étape de référence n'est pas terminée, l'action suivante pour la terminer. Si elle est terminée, l'étape suivante non cochée dans l'ordre de PLAN.md (et ses tâches). Signale d'abord toute décision Dn à trancher qui bloque cette étape.
- À vérifier sur le matériel : ce qui n'a pas été testé physiquement.

**2. PLAN.md : à cocher**
- Liste les étapes dont le critère de validation est atteint, avec la preuve fournie. Si aucune : « Rien à cocher », et dis ce qui manque.
- Une phase se coche quand toutes ses étapes ESSENTIEL et IMPORTANT sont cochées.

**3. DECISIONS.md : à modifier**
- Tableau des fonctions : colonne Impl. des fonctions concernées. « En cours » dès qu'une de leurs étapes (colonne Étapes) est commencée ; « Fait » quand toutes leurs étapes sont cochées.
- Section « Décidé » : décisions prises pendant la session (avec leur identifiant Dn) et éléments validés à y consigner (ex. : MongoDB retenu, puis entités et attributs ; affectation des broches ; bibliothèque choisie), en texte prêt à coller.
- Section « À valider ou à trancher » : lignes Dn à retirer (décidées, donc déplacées vers « Décidé ») ou nouvelles décisions ouvertes à ajouter.
- Journal : une ligne datée (AAAA-MM-JJ) si une étape importante est terminée, sinon « aucune ligne ».
- Pour chaque rubrique sans changement, écris « Rien à modifier ».

Règles du BILAN :
- Séparer faits vérifiés / hypothèses / choix à valider par l'étudiant.
- Ne jamais affirmer qu'un montage ou un code a été testé s'il ne l'a pas été. N'inventer ni valeurs, ni résultats, ni décisions.
- Donner le texte exact à reporter, prêt à copier. L'étudiant valide chaque ligne.
- Dans Claude Code, n'applique les modifications à `PLAN.md` et `DECISIONS.md` que si l'étudiant le demande après le BILAN, puis rappelle qu'il doit réimporter ces fichiers dans le projet Claude Desktop.

## Commande DOC
Quand l'étudiant écrit « DOC », produis un texte prêt à coller dans Notion, avec ces rubriques :

1. **Objectif** : ce que le composant, l'étape ou la partie du système doit faire (1 à 3 phrases).
2. **Câblage** : broches utilisées, avec les noms officiels de `CONTEXTE_TECHNIQUE.md` (tableau composant / broche / broche de la carte), niveaux logiques et alimentation.
3. **Architecture** : comment la partie s'intègre au reste (bus, protocole, fichiers du dépôt, flux de données).
4. **Choix et raisons** : chaque choix technique retenu et pourquoi, avec les alternatives écartées.
5. **Tests réalisés** : uniquement ce que l'étudiant a fourni (résultats, mesures, photos). Si rien n'a été fourni, écrire « à compléter » ; ne jamais inventer de résultat.
6. **Limites** : ce qui ne marche pas encore, ce qui n'est pas testé, les hypothèses restantes.

Règles du DOC :
- Format Notion : titres Markdown, tableaux, listes courtes ; pas de HTML.
- Utiliser les noms de broches officiels de `CONTEXTE_TECHNIQUE.md`.
- Séparer faits vérifiés / hypothèses. N'inventer ni valeurs de composants, ni mesures, ni résultats de tests.
- Indiquer à la fin les éléments à collecter pour le compte rendu final (datasheet, schéma KiCad, photo, mesure) s'ils manquent.

## Commandes
`/bilan` et `/doc` (dossier `.claude/commands/`) lancent les commandes BILAN et DOC ci-dessus.
