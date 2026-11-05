# Firmware (C++ / PlatformIO / Arduino UNO R4 WiFi)

Complète le `CLAUDE.md` racine. Ce fichier fixe la structure du firmware et les règles à appliquer à **chaque** composant (étapes P1.1 à P1.5, puis P6), pour que l'étudiant n'ait pas à les répéter à chaque session. Les décisions résumées sont dans `DECISIONS.md` (section Méthode) ; en cas de conflit, `DECISIONS.md` l'emporte.

## Principe
Aucun code jetable : chaque composant est codé une seule fois, dans un module qui sert d'abord à le valider (P1), puis au firmware final (P6). Il n'y a pas de dossier `tests/` : la fonction de test vit dans le module du composant.

## Commandes
- Compiler : `pio run -d firmware` (depuis la racine du dépôt ; autorisé sans demander).
- Téléverser : `pio run -d firmware -t upload`, uniquement quand l'étudiant dit que le câblage est vérifié et que la carte est branchée. Le script série doit être arrêté avant : un seul programme peut tenir le port COM.
- Sortie série : `firmware/scripts/serial_monitor.py` remplace `pio device monitor` dès qu'il existe (il est écrit au début de P1.1). Il est lancé par l'étudiant (interactif) ; Claude Code lui demande le résultat.
- Ne jamais déclarer qu'un test physique (LED, buzzer, écran, caméra, capteur) a réussi sans retour de l'étudiant.

## Arborescence
```text
firmware/
├── platformio.ini
├── include/
│   ├── pins.h                  # broches : un bloc par composant + index des broches de la carte
│   ├── config.h                # paramètres par défaut utiles au produit (constexpr)
│   └── secrets.h               # créé au premier secret, ignoré par Git (+ secrets.example.h versionné)
├── src/
│   ├── main.cpp
│   ├── self_test.h / .cpp      # self_test_run() : enchaîne les tests validés
│   └── components/
│       ├── camera.h / .cpp     # P1.1
│       ├── bme680.h / .cpp     # P1.2
│       ├── mq2.h / .cpp        # P1.3
│       ├── display.h / .cpp    # P1.4
│       └── leds, buzzer, buttons   # P1.5 (même structure)
├── scripts/
│   ├── serial_monitor.py       # écrit au début de P1.1
│   ├── requirements.txt        # pyserial
│   └── .venv/                  # ignoré par Git
└── output/                     # images de test, ignoré par Git (créé par le script)
```
Les modules apparaissent **un par un**, à l'étape du composant : ne crée pas un module, un script ou un fichier avant l'étape correspondante. Le DS12A (interrupteur général) n'a pas de module : il coupe l'alimentation, il a seulement une fiche. Les 4 LED forment un seul module `leds`.

## Un module par composant (`src/components/<composant>.h/.cpp`)
Noms de fichiers, de fonctions et de variables en anglais (`snake_case`), commentaires en français, simples, en tête de bloc. Fonctions préfixées par le nom du composant, pas de classes.

Chaque module contient :
1. **`bool <composant>_setup()`** : met en place le composant (modes des broches, bus SPI ou I2C, paramètres initiaux). Renvoie `false` si le composant ne répond pas. Ne bloque jamais : délai maximal.
2. **Les fonctions principales** : celles dont le firmware final aura besoin (ex. caméra : capturer une photo ; BME680 : une lecture qui renvoie une structure température, humidité, pression, gaz). Elles ne contiennent ni affichage de test ni constante de test, car elles seront réutilisées telles quelles en P6. [H] Pour le BME680, une lecture groupée évite de relancer plusieurs cycles de mesure : à confirmer avec la bibliothèque retenue.
3. **`bool <composant>_test()`** : appelle les fonctions principales, affiche le résultat sur le port série (valeurs, ou action visible pour LED et buzzer) et renvoie `true` ou `false`. C'est la preuve que le composant fonctionne. Ses paramètres de test (nombre de mesures, durée, compte à rebours) sont des constantes **locales** en haut du `.cpp`, jamais dans `config.h`. Le code de test peut utiliser `delay` ; pas les fonctions principales.

Aucun numéro de broche en dur dans un module : tout vient de `pins.h`.

**Cas de la caméra.** `camera_test()` fait un compte à rebours (3, 2, 1) sur le port série, capture une photo, vérifie qu'il s'agit d'un JPEG valide (taille non nulle, marqueur de début `FF D8`, marqueur de fin `FF D9`), l'envoie au PC puis affiche « photo envoyée, N octets, JPEG valide ». La capture lit l'image **par blocs** depuis la mémoire de la caméra et les confie à une fonction de sortie, sans garder l'image entière en RAM [H : RAM de la carte limitée]. En P1 la sortie est le port série USB ; plus tard le transport changera (HTTP, P2.5) sans toucher à la capture. Pas de Wi-Fi ni de HTTP en P1. La bibliothèque dépend de la décision D2 (ouverte) : ne pas la figer avant les tests. Pour distinguer un problème de câblage d'un problème de code, valider d'abord la communication avec un exemple fourni par la bibliothèque, dans un projet temporaire hors du dépôt (ne pas le committer, ne pas l'ajouter à `src/`).

## Enchaînement des tests (`src/self_test.h/.cpp`)
`void self_test_run()` appelle les `<composant>_test()` validés. On y ajoute **une ligne par composant, seulement après sa validation sur le matériel par l'étudiant** (évite les faux KO sur un composant non câblé). Le récapitulatif OK/KO est finalisé en P4.1.

- **Développement (P1 à P3)** : `setup()` de `main.cpp` appelle les `<composant>_setup()` ; `loop()` appelle `self_test_run()` suivi d'une pause (constante locale dans `main.cpp`, supprimée avec l'appel). Pendant la mise au point d'un composant, on peut n'appeler que son `<composant>_test()`.
- **Version finale (P4.1)** : `self_test_run()` passe dans `setup()` (exécution unique au démarrage) ; `loop()` contient le firmware réel.
- **Code jetable assumé** : pendant la mise au point de la caméra, un bloc temporaire dans `loop()` lit une touche envoyée par le script (touche `d`) et déclenche une photo à la demande. Il vit dans `main.cpp` et disparaît quand la caméra est validée.
- `Serial` de la R4 passe par l'USB natif : dans `setup()`, attendre l'ouverture du port avec un délai maximal, sinon les premières lignes sont perdues [H].
- Broches CS des périphériques SPI : à mettre à `HIGH` dès le début de `setup()`, avant tout `<composant>_setup()`. À traiter dès le deuxième périphérique SPI (P1.4 ou P1.7).

## Broches : `include/pins.h`
- **En tête** : un index des broches de la carte utilisées, une ligne par broche (`// D10 : CAMERA_CS`), mis à jour à chaque composant. Il sert à repérer les doublons et à préparer D3.
- **Ensuite** : un bloc par composant listant **toutes** ses broches, avec les noms officiels de `CONTEXTE_TECHNIQUE.md` et la broche de la carte. Seules les broches à définir dans le code sont des `constexpr uint8_t` (ex. `CAMERA_CS_PIN`). Les autres (bus SPI ou I2C imposés par la carte, alimentation, masse) sont notées en **commentaire** : on voit en un coup d'œil comment brancher le composant.
- Tant que D3 est ouverte, ces numéros sont **provisoires** : propose-les à l'étudiant et fais-lui valider chaque affectation avant câblage. [H] Les broches des bus (MOSI, MISO, SCK, SDA, SCL) sont fixées par la carte : confirmer dans la datasheet ABX00087 avant de les écrire.

## Paramètres : `include/config.h`
- `constexpr` regroupés par composant, unité dans le nom (`MQ2_ALERT_THRESHOLD`, `BME680_READ_PERIOD_MS`).
- **Règle** : une valeur qui a un sens dans le produit final (résolution de la photo, seuil MQ-2, période de mesure de 5 s, délai d'intrusion de 10 s, consignes et tolérances par défaut du CC) va dans `config.h`. Une valeur qui n'existe que pour le test reste une constante locale du `.cpp`.
- Ce sont des **valeurs par défaut**. Quand le serveur pourra les modifier (P6.6), le module aura une variable initialisée avec ce défaut, que le serveur écrasera. Rien à anticiper avant.
- Ajoute une ligne seulement quand on en a besoin (ex. la résolution caméra, une fois D2 tranchée). Aucune valeur inventée : sinon propose-la à l'étudiant.

## Secrets : `include/secrets.h`
Créé seulement quand le premier secret apparaît (SSID et mot de passe Wi-Fi, identifiants MQTT, IP du PC). Ignoré par Git, avec un `secrets.example.h` versionné qui montre le format sans valeur réelle. Aucun secret dans `config.h` ni dans le code.

## Sortie série et images : `scripts/serial_monitor.py`
Un **seul script Python remplace le moniteur série PlatformIO**, pour tous les composants : un seul programme peut tenir le port COM à la fois, et le moniteur PlatformIO ne sait pas traiter du binaire. Il est écrit au début de P1.1, pas avant.

- Il affiche les lignes de texte envoyées par la carte.
- Protocole image (provisoire, à confirmer en P1.1) : la carte envoie une ligne `IMG_START <taille>`, puis exactement ce nombre d'octets, puis une ligne `IMG_END`.
- À la ligne de début, le script lit la taille annoncée, vérifie les marqueurs JPEG, écrit `firmware/output/camera_AAAAMMJJ_HHMMSS.jpg` (il crée le dossier si besoin), affiche « image enregistrée : <chemin> » et reprend la lecture normale. Si la taille reçue ne correspond pas, il abandonne l'image et attend le prochain marqueur.
- Il transmet les touches du clavier à la carte (touche `d` de la mise au point de la caméra).
- Il se reconnecte seul quand le port disparaît (flash, reset).
- Il n'ouvre jamais le port à 1200 bauds, qui est la commande de passage en bootloader de la R4 [H].
- La carte ne doit pas dépendre de la présence du script : sans PC branché, un envoi de plusieurs Ko sur `Serial` pourrait bloquer [H]. Tester l'état du port avant d'envoyer.
- Environnement : `.venv` dans `firmware/scripts/`, `requirements.txt` versionné (`pyserial`).
- Option, plus tard : une tâche VS Code qui arrête le script, téléverse, puis relance le script.

`firmware/output/` contient les images de test, ignoré par Git. Pour la fiche d'un composant, une seule photo choisie par l'étudiant est copiée dans `docs/tests/`.

## Procédure quand l'étudiant dit « on passe à <composant> » (étape P1.x)
1. Lis l'étape dans `PLAN.md` (tâches, livrable, validation, « Dépend de ») et les décisions ouvertes liées (D1 pour le BME680, D2 pour la caméra). Si une dépendance ou une décision bloque, dis-le avant de coder.
2. Lis la section du composant dans `CONTEXTE_TECHNIQUE.md` (noms de broches, documentation). Vérifie niveaux logiques, courant et bus partagés. Vérifie la compatibilité de la bibliothèque avec la R4 avant de l'ajouter à `platformio.ini`.
3. Propose un plan court : broches provisoires, bibliothèque, fonctions du module, liste de contrôle de câblage. Attends la validation.
4. Une fois validé : crée `components/<composant>.h/.cpp`, ajoute le bloc du composant dans `pins.h` (et l'index) et les paramètres utiles au produit dans `config.h`, puis compile.
5. Téléverse seulement quand l'étudiant confirme le câblage. Demande-lui la sortie de `<composant>_test()` et, pour un composant visible (LED, buzzer, écran), ce qu'il constate.
6. Ne considère pas le composant validé tant que l'étudiant n'a pas confirmé le résultat sur le matériel. Alors seulement : ajoute **une ligne** dans `self_test_run()`.
7. La fiche `docs/tests/<composant>.md` n'est rédigée que si l'étudiant le demande (règles dans le `CLAUDE.md` racine).
8. Termine par « À vérifier sur la carte » et par ce que l'étudiant doit reporter dans `PLAN.md`, `DECISIONS.md` et `CONTEXTE_TECHNIQUE.md` (noms de pattes, valeurs de résistances, D1, D2).

## Informations supplémentaires
- Plateforme PlatformIO pour l'UNO R4 WiFi (Renesas RA4M1) ; Wi-Fi via le module ESP32-S3 de la carte. Vérifie la compatibilité de chaque bibliothèque avant de l'ajouter à `platformio.ini`.
- Les noms de broches des modules sont ceux de `CONTEXTE_TECHNIQUE.md`. Pas de câblage ni de broches supposés : utilise les affectations de `DECISIONS.md` ; tant que D3 est ouverte, seules celles de `pins.h` validées par l'étudiant font foi.
- Points de vigilance matériel à rappeler quand c'est pertinent : la carte fonctionne en logique 5 V alors que l'écran TFT et certains modules sont en 3,3 V ; la caméra (SPI + I2C) et l'écran (SPI) partagent le bus SPI, avec un CS par périphérique ; le BME680 est en SPI ou en I2C selon D1 (ouverte) ; la caméra a besoin de beaucoup de mémoire et d'un débit SPI suffisant : priorité à des photos exploitables.
- Secrets : voir plus haut (`secrets.h` non versionné, `secrets.example.h` versionné).
- Code de production non bloquant : pas de `delay` long dans la boucle principale. Seule exception : la pause entre deux passes de `self_test_run()` en développement, supprimée en P4.1.
- Cadences visées par le CC : mesure toutes les 5 s, enregistrement serveur toutes les 30 s, événements immédiats.
- Termine toujours par une liste « À vérifier sur la carte ».
