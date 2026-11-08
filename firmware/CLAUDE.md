# Firmware (C++ / PlatformIO / Arduino UNO R4 WiFi)

Complète le `CLAUDE.md` racine. Ce fichier fixe la structure du firmware et les règles à appliquer à **chaque** composant (étapes P1.1 à P1.5, puis P6), pour que l'étudiant n'ait pas à les répéter à chaque session. Les décisions résumées sont dans `DECISIONS.md` (section Méthode) ; en cas de conflit, `DECISIONS.md` l'emporte.

## Principe
Aucun code jetable : chaque composant est codé une seule fois, dans un module qui sert d'abord à le valider (P1), puis au firmware final (P6). Il n'y a pas de dossier `tests/` : la fonction de test vit dans le module du composant.

## Règles de code du firmware
Complètent les « Règles de code » du `CLAUDE.md` racine (code minimal).
- **Commentaires** : aucun commentaire dans les `.cpp` de `src/`. Dans les `.h` de `src/`, une seule ligne de commentaire au-dessus de chaque signature de fonction, en français, simple. `pins.h` et `config.h` gardent leurs commentaires de regroupement.
- **`config.h`** : une constante n'y entre que si le code de la tâche l'utilise.

## Commandes
- Compiler : `pio run -d firmware` (depuis la racine du dépôt ; autorisé sans demander). Le framework compile avec `-w` (avertissements masqués) : pour les voir, compiler une copie temporaire du projet hors du dépôt avec `build_unflags = -w` et `build_flags = -Wall -Wextra`.
- Téléverser : `pio run -d firmware -t upload`, uniquement quand l'étudiant dit que le câblage est vérifié et que la carte est branchée. Un seul programme peut tenir le port COM : arrêter le script série avant, ou le relancer pendant le téléversement (il attend que le port se libère).
- Sortie série : `firmware\scripts\.venv\Scripts\python firmware\scripts\serial_monitor.py` (option `--port COMx` si la détection échoue). Il remplace `pio device monitor`. Il est lancé par l'étudiant (interactif) ; Claude Code lui demande le résultat.
- Ne jamais déclarer qu'un test physique (LED, buzzer, écran, caméra, capteur) a réussi sans retour de l'étudiant.

## Arborescence
```text
firmware/
├── platformio.ini
├── include/
│   ├── pins.h                  # broches : un bloc par composant + index des broches de la carte
│   ├── config.h                # paramètres par défaut utiles au produit (constexpr ; #define pour la résolution caméra)
│   └── secrets.h               # créé au premier secret, ignoré par Git (+ secrets.example.h versionné)
├── lib/
│   └── ArduCAM/                # copie locale versionnée (D2) ; origine et correctifs : CYPHER_SOURCE.md
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
│   ├── serial_monitor.py       # P1.1 : texte + images
│   ├── requirements.txt        # pyserial (version figée)
│   └── .venv/                  # ignoré par Git
└── output/                     # images de test, ignoré par Git (créé par le script)
```
Les modules apparaissent **un par un**, à l'étape du composant : ne crée pas un module, un script ou un fichier avant l'étape correspondante. Le DS12A (interrupteur général) n'a pas de module : il coupe l'alimentation, il a seulement une fiche. Les 4 LED forment un seul module `leds`.

## Un module par composant (`src/components/<composant>.h/.cpp`)
Noms de fichiers, de fonctions et de variables en anglais (`snake_case`), commentaires selon « Règles de code du firmware ». Fonctions préfixées par le nom du composant, pas de classes. Messages affichés en français, sans accents (comme `self_test.cpp`).

Chaque module contient :
1. **`bool <composant>_setup()`** : met en place le composant (modes des broches, bus SPI ou I2C, paramètres initiaux). Renvoie `false` si le composant ne répond pas. Ne bloque jamais : délai maximal.
2. **Les fonctions principales** : celles dont le firmware final aura besoin (ex. caméra : capturer une photo ; BME680 : une lecture qui renvoie une structure température, humidité, pression, gaz). Elles ne contiennent ni affichage de test ni constante de test, car elles seront réutilisées telles quelles en P6. En cas d'échec, elles renvoient simplement `false` (pas de `<composant>_last_error()`). Un capteur à plusieurs valeurs a une seule lecture groupée (BME680 : `bme680_read()`, car chaque lecture relance un cycle de mesure complet), pas une fonction par valeur.
3. **`bool <composant>_test()`** : appelle les fonctions principales, affiche le résultat sur le port série (valeurs, ou action visible pour LED et buzzer) selon la « Convention d'affichage des tests » du `CLAUDE.md` racine, et renvoie `true` ou `false`. C'est la preuve que le composant fonctionne. Ses paramètres de test (nombre de mesures, durée, pause, délais) sont des constantes **locales** en haut du `.cpp`, jamais dans `config.h`. Le code de test peut utiliser `delay` ; pas les fonctions principales (sauf délais matériels courts et bornés imposés par le composant, en `setup`).

Aucun numéro de broche en dur dans un module : tout vient de `pins.h`.

**Cas de la caméra (P1.1).**
- Bibliothèque : copie locale `lib/ArduCAM/` (fork R4 de la bibliothèque ArduCAM, voir `CYPHER_SOURCE.md`), choix D2 de `DECISIONS.md`.
- API : `camera_setup()`, `camera_capture(uint32_t &jpeg_length)`, lecture en rafale `camera_read_begin()` / `camera_read_block(buf, max_len)` / `camera_read_end()`, `camera_test()`. La lecture ne code aucune destination en dur : la même API servira à l'envoi HTTP (P2.5).
- L'image n'est jamais entièrement en RAM (32 ko de SRAM) : lecture par blocs de `CAMERA_READ_BLOCK_BYTES` depuis le tampon de 8 Mo de la caméra, envoi au fil de l'eau. Entre `camera_read_begin()` et `camera_read_end()`, le bus SPI est réservé à la caméra.
- `camera_test()` : une capture à la résolution de `config.h`, envoi de la trame image, puis une ligne par caractéristique (résolution, format, temps de capture). Contrôle : les 2 premiers octets du premier bloc doivent être FF D8 ; l'image est envoyée en entier dans tous les cas, puis, si FF D8 est absent, ligne `[camera] KO` à la place des caractéristiques et retour `false`. Échec de mise en place ou de capture : ligne `[camera] KO`. La pause de 5 s entre deux tests est dans `main.cpp` (`SELF_TEST_PAUSE_MS`). Pas de compte à rebours ni de commande clavier. Si `camera_setup()` a échoué au démarrage, ou après un délai de capture dépassé (drapeau de fin remis à zéro), `camera_test()` la retente.
- La longueur lue dans le tampon de la caméra dépasse la fin du JPEG (dernier FF D9) de quelques octets de remplissage (12 à 19 octets observés en 640x480) : ce n'est pas une erreur. La carte les envoie ; le script les retire du fichier enregistré, sans l'afficher.
- Pour distinguer un problème de câblage d'un problème de code : un exemple de la bibliothèque dans un projet temporaire hors du dépôt (ni committé, ni dans `src/`).

## Bibliothèques tierces (`lib/`)
- Une bibliothèque copiée localement va dans `firmware/lib/<Nom>/` (versionnée, reproductible ; ne jamais modifier `.pio/libdeps`). Garder son fichier de licence et ajouter `CYPHER_SOURCE.md` : source, commit, licence, contenu copié, tableau des correctifs.
- Chaque correctif est marqué dans le fichier modifié par un commentaire `CYPHER-PATCH: <raison>`.
- Condition d'arrêt : si le portage demande plus que de petits correctifs, ou si le composant n'est pas détecté alors que le câblage est correct, s'arrêter et rapporter l'erreur exacte avec les options. Ne jamais réécrire une bibliothèque sans l'accord de l'étudiant.

## Enchaînement des tests (`src/self_test.h/.cpp`)
`void self_test_run()` appelle les `<composant>_test()`. On y ajoute **une ligne par composant après sa validation sur le matériel par l'étudiant** (évite les faux KO sur un composant non câblé). Exception, sur demande de l'étudiant : le composant en cours de mise au point peut y figurer (cas de la caméra en P1.1). Le récapitulatif OK/KO est finalisé en P4.1.

- **Développement (P1 à P3)** : `setup()` de `main.cpp` appelle les `<composant>_setup()` ; `loop()` appelle `self_test_run()` suivi d'une pause (constante locale dans `main.cpp`, supprimée avec l'appel). Pendant la mise au point d'un composant, on peut n'appeler que son `<composant>_test()`.
- **Version finale (P4.1)** : `self_test_run()` passe dans `setup()` (exécution unique au démarrage) ; `loop()` contient le firmware réel.
- Broches CS des périphériques SPI : à mettre à `HIGH` dès le début de `setup()`, avant tout `<composant>_setup()`. À traiter dès le deuxième périphérique SPI (P1.4 ou P1.7).

## Port série de la carte (`Serial`)
Faits vérifiés dans le framework (cœur Renesas 1.6.0, `libfsp.a`) :
- `Serial` n'est **pas** l'USB natif : la carte PlatformIO `uno_r4_wifi` définit `NO_USB`, donc `Serial` est l'UART SCI9 (P109/P110) vers l'ESP32-S3, qui fait le pont USB. Le débit réel est celui de `Serial.begin()` : `SERIAL_BAUD` de `config.h` (460800 bauds ; mesuré en P1.1 : 11,1 ko/s à 115200 bauds, environ 41 ko/s à 460800 bauds), à garder égal au débit par défaut du script série.
- `while (!Serial)` n'attend rien (`operator bool` renvoie toujours `true`). Un envoi ne bloque jamais faute de PC ; [H] les octets sont alors perdus.
- **Piège** : SCI9 n'a pas de FIFO. `Serial.write(buf, n)` rend la main dès que le dernier octet est chargé dans le registre d'émission, et l'écriture suivante y place son premier octet sans attendre. Si elle arrive moins d'une durée d'octet plus tard (environ 22 µs à 460800 bauds), le dernier octet du bloc est écrasé (constaté en P1.1 à 115200 bauds : 1 octet manquant par image). Règle : appeler `Serial.flush()` après tout `Serial.write(buf, n)` suivi d'une autre écriture. Sur ce cœur, `flush()` attend l'interruption de fin d'émission (TEI, registre à décalage vide : vérifié dans le code). Les écritures octet par octet (`print`, `println`, `write(c)`) ne sont pas concernées : chacune rend la main quand le registre d'émission est vide. Dans `camera_test()` : `flush()` après chaque bloc, y compris le dernier, avant la ligne `IMG_END`.

## Broches : `include/pins.h`
- **En tête** : un index des broches de la carte utilisées, une ligne par broche (`// D10 : CAMERA_CS`), mis à jour à chaque composant. Il sert à repérer les doublons et à préparer D3.
- **Ensuite** : un bloc par composant listant **toutes** ses broches, avec les noms officiels de `CONTEXTE_TECHNIQUE.md` et la broche de la carte. Seules les broches à définir dans le code sont des `constexpr uint8_t` (ex. `CAMERA_CS_PIN`). Les autres (bus SPI ou I2C imposés par la carte, alimentation, masse) sont notées en **commentaire** : on voit en un coup d'œil comment brancher le composant.
- Tant que D3 est ouverte, ces numéros sont **provisoires** : propose-les à l'étudiant et fais-lui valider chaque affectation avant câblage.
- Bus imposés par la carte (vérifiés dans le variant `UNOWIFIR4`) : SPI = D11 (MOSI), D12 (MISO), D13 (SCK, aussi la LED intégrée) ; I2C `Wire` = broches 18/19 du variant, c'est-à-dire SDA et SCL près d'AREF, reliées à A4 et A5 [H : d'après le brochage officiel ; continuité à confirmer au multimètre] : A4 et A5 ne sont alors plus utilisables en analogique. `Wire1` = connecteur Qwiic.

## Paramètres : `include/config.h`
- `constexpr` regroupés par composant, unité dans le nom (`MQ2_ALERT_THRESHOLD`, `BME680_READ_PERIOD_MS`).
- **Règle** : une valeur qui a un sens dans le produit final (résolution de la photo, seuil MQ-2, période de mesure de 5 s, délai d'intrusion de 10 s, consignes et tolérances par défaut du CC) va dans `config.h`, au moment où le code l'utilise. Une valeur qui n'existe que pour le test reste une constante locale du `.cpp`.
- Valeurs lisibles, converties dans le module vers les constantes de la bibliothèque. Cas de la résolution caméra : `config.h` contient directement la constante de la bibliothèque (`#define CAMERA_RESOLUTION OV5642_1024x768`, liste en commentaire) ; `camera.cpp` en tire la largeur et la hauteur par un tableau, et un `static_assert` refuse une valeur hors du tableau.
- Ce sont des **valeurs par défaut**. Quand le serveur pourra les modifier (P6.6), le module aura une variable initialisée avec ce défaut, que le serveur écrasera. Rien à anticiper avant.
- Aucune valeur inventée : sinon propose-la à l'étudiant.

## Secrets : `include/secrets.h`
Créé seulement quand le premier secret apparaît (SSID et mot de passe Wi-Fi, identifiants MQTT, IP du PC). Ignoré par Git, avec un `secrets.example.h` versionné qui montre le format sans valeur réelle. Aucun secret dans `config.h` ni dans le code.

## Sortie série et images : `scripts/serial_monitor.py`
Un **seul script Python remplace le moniteur série PlatformIO**, pour tous les composants : un seul programme peut tenir le port COM à la fois, et le moniteur PlatformIO ne sait pas traiter du binaire. Mode d'emploi en tête du script.

- Il affiche en direct les lignes de texte de la carte ; ses propres messages commencent par `[moniteur]`.
- Trame image (provisoire, phase P1, transport de test sans lien avec D4). Pendant l'envoi d'une image, aucun autre texte ne passe sur `Serial` :
  ```text
  IMG_BEGIN <taille_octets> <largeur>x<hauteur>\n
  <exactement taille_octets octets bruts>
  IMG_END <temps_transfert_ms>\n
  ```
  Pas de CRC. Le PC lit exactement `<taille_octets>` octets puis la ligne suivante, qui doit commencer par `IMG_END` : sinon, un octet manque ou est en trop.
- Seul contrôle d'intégrité : la position de `IMG_END`. Aucun rejet lié à FF D8 / FF D9. Il affiche le nom du fichier, sa taille enregistrée et le temps de transfert mesuré par la carte, une ligne chacun (convention d'affichage des tests, `print_value()`).
- Enregistrement : `firmware/output/cam_<AAAAMMJJ-HHMMSS>.jpg` (dossier créé si absent), sans compteur ni résolution dans le nom, coupé au dernier FF D9 (remplissage retiré, sans affichage) ; sans FF D9, enregistré tel quel.
- Robustesse : taille plafonnée à 8 Mo. Si la ligne qui suit les octets annoncés ne commence pas par `IMG_END`, il affiche « IMG_END absent à la position annoncée » et n'enregistre pas l'image. Plus aucun octet pendant 2 s au milieu d'une image = image interrompue (redémarrage de la carte) : elle est abandonnée. Dans tous les cas, il reprend sur la ligne suivante.
- Port : détection automatique d'une carte Arduino (identifiant USB 0x2341), option `--port COMx`. Débit : constante `DEFAULT_BAUD` du script, à garder égale à `SERIAL_BAUD`, option `--baud`. Il n'ouvre jamais le port à 1200 bauds (passage en bootloader). Il se reconnecte seul quand le port disparaît (reset, téléversement, câble), signale un port occupé et s'arrête proprement sur Ctrl+C. Aucune commande clavier n'est transmise à la carte.
- Environnement : `.venv` dans `firmware/scripts/`, `requirements.txt` versionné (`pyserial`, version figée).
- Option, plus tard : une tâche VS Code qui arrête le script, téléverse, puis relance le script.

`firmware/output/` contient les images de test, ignoré par Git. Pour la fiche d'un composant, une seule photo choisie par l'étudiant est copiée dans `docs/tests/`.

## Procédure quand l'étudiant dit « on passe à <composant> » (étape P1.x)
1. Lis l'étape dans `PLAN.md` (tâches, livrable, validation, « Dépend de ») et les décisions ouvertes liées (D1 pour le BME680). Si une dépendance ou une décision bloque, dis-le avant de coder.
2. Lis la section du composant dans `CONTEXTE_TECHNIQUE.md` (noms de broches, documentation). Vérifie niveaux logiques, courant et bus partagés. Vérifie la compatibilité de la bibliothèque avec la R4 avant de l'ajouter à `platformio.ini` ou à `lib/` (voir « Bibliothèques tierces »).
3. Propose un plan court : broches provisoires, bibliothèque, fonctions du module, liste de contrôle de câblage. Attends la validation.
4. Une fois validé : crée `components/<composant>.h/.cpp`, ajoute le bloc du composant dans `pins.h` (et l'index) et les paramètres utiles au produit que le code utilise dans `config.h`, puis compile.
5. Téléverse seulement quand l'étudiant confirme le câblage. Demande-lui la sortie de `<composant>_test()` et, pour un composant visible (LED, buzzer, écran), ce qu'il constate.
6. Ne considère pas le composant validé tant que l'étudiant n'a pas confirmé le résultat sur le matériel. Alors seulement : ajoute **une ligne** dans `self_test_run()` (sauf exception demandée par l'étudiant, voir plus haut).
7. La fiche `docs/tests/<composant>.md` n'est rédigée que si l'étudiant le demande (règles dans le `CLAUDE.md` racine).
8. Termine par « À vérifier sur la carte » et par ce que l'étudiant doit reporter dans `PLAN.md`, `DECISIONS.md` et `CONTEXTE_TECHNIQUE.md` (noms de pattes, valeurs de résistances, D1).

## Informations supplémentaires
- Plateforme PlatformIO pour l'UNO R4 WiFi (Renesas RA4M1) ; Wi-Fi via le module ESP32-S3 de la carte. Vérifie la compatibilité de chaque bibliothèque avant de l'ajouter.
- Les noms de broches des modules sont ceux de `CONTEXTE_TECHNIQUE.md`. Pas de câblage ni de broches supposés : utilise les affectations de `DECISIONS.md` ; tant que D3 est ouverte, seules celles de `pins.h` validées par l'étudiant font foi.
- Points de vigilance matériel à rappeler quand c'est pertinent : la carte fonctionne en logique 5 V alors que l'écran TFT et certains modules sont en 3,3 V ; la caméra (SPI + I2C) et l'écran (SPI) partagent le bus SPI, avec un CS par périphérique ; le BME680 est en SPI ou en I2C selon D1 (ouverte) ; la caméra a besoin de beaucoup de mémoire et d'un débit SPI suffisant : priorité à des photos exploitables.
- Secrets : voir plus haut (`secrets.h` non versionné, `secrets.example.h` versionné).
- Code de production non bloquant : pas de `delay` long dans la boucle principale. Seule exception : la pause entre deux passes de `self_test_run()` en développement, supprimée en P4.1.
- Cadences visées par le CC : mesure toutes les 5 s, enregistrement serveur toutes les 30 s, événements immédiats.
- Termine toujours par une liste « À vérifier sur la carte ».
