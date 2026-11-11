# MQTT dans Cypher Home (P2.2)

> Mon cours de P2.2, écrit étape par étape : une section par étape, dans l'ordre. Ce n'est pas la fiche de test d'une page : elle sera tirée de ce fichier à la fin.

## Objectif de P2.2

Mettre en place un **broker MQTT** local (Mosquitto, lancé avec Docker), protégé par un mot de passe. La carte lui envoie un message ; le PC le reçoit.

Critères de validation (`PLAN.md`) :
- un message envoyé par la carte est reçu sur le PC ;
- une connexion sans mot de passe est refusée.

Deux décisions restent **ouvertes** dans `DECISIONS.md`. P2.2 sert à les éprouver, pas à les trancher :
- D4 : MQTT est *proposé* pour les mesures (repli : HTTP partout) ;
- D8 : Docker est *proposé* pour Mosquitto (repli : installation native sous Windows).

## Qu'est-ce que MQTT, Mosquitto, Docker

### Une image pour tout le cours : un immeuble

Le PC est un immeuble. Le broker est le **gardien** : il tient la loge où l'on dépose le courrier, et il le distribue. Un message est une **lettre** ; son sujet est l'**étiquette** collée sur l'enveloppe. Les étapes suivantes ajouteront l'adresse de l'immeuble, ses portes, le vigile de l'entrée et les badges.

### Un peu d'histoire

- **MQTT** naît en 1999, créé par Andy Stanford-Clark (IBM) et Arlen Nipper (Arcom). Le but : surveiller des oléoducs par satellite. La liaison est lente, chère, et se coupe souvent. Le protocole est donc très léger.
- Il devient un standard ouvert en 2014 (version 3.1.1), puis passe en version 5 en 2019. C'est aujourd'hui le protocole courant de l'IoT (*Internet of Things* : objets connectés qui envoient des mesures).
- **Mosquitto** est un broker MQTT libre, écrit en C, lancé par Roger Light vers 2009. Il appartient aujourd'hui à la fondation Eclipse. Il est léger et très répandu.
- **Docker** sort en 2013 et popularise les conteneurs (voir plus bas).

### Publier, s'abonner, broker

- Un **client** est tout programme qui se connecte au broker : la carte, le futur serveur Python, un outil de test.
- **Publier** : envoyer un message au broker, avec un sujet. *Déposer une lettre étiquetée à la loge.*
- **S'abonner** : demander au broker tous les messages d'un sujet. *Dire au gardien : « toute lettre portant cette étiquette, apportez-la-moi ».*
- Le **broker** reçoit chaque message et le transmet à tous les abonnés de ce sujet. L'émetteur ne connaît pas les destinataires, et inversement.
- Par défaut, le broker ne garde rien : un message publié quand personne n'est abonné est perdu.

### Le sujet (*topic*)

- Un texte découpé en niveaux par des `/`, par exemple `cypherhome/test`.
- Il n'a pas besoin d'être créé à l'avance : il existe dès qu'on publie ou qu'on s'abonne.
- Il est sensible à la casse : `cypherhome/Test` et `cypherhome/test` sont deux sujets différents.

### QoS : la garantie de livraison

La **QoS** (*Quality of Service*) se choisit pour chaque message.
- **QoS 0** : envoyé une fois, sans accusé de réception. *Une lettre simple.* Rapide, mais elle peut se perdre.
- **QoS 1** : renvoyé tant que le destinataire n'a pas répondu « reçu ». *Une lettre recommandée.* Elle arrive au moins une fois, parfois deux.
- Un niveau 2 (« exactement une fois ») existe ; plus lourd, il est inutile ici.

Pour une mesure envoyée toutes les 5 s, en perdre une de temps en temps est acceptable : QoS 0 suffit pour P2.2.

### Le message testament (LWT)

- **LWT** (*Last Will and Testament*) : à la connexion, le client confie au broker un message « testament » (un sujet et un texte).
- Le client doit donner signe de vie à intervalle régulier (*keep alive*). S'il se tait trop longtemps, ou si la connexion casse sans au revoir (courant coupé, Wi-Fi perdu), le broker publie le testament à sa place.
- *Une lettre laissée d'avance au gardien : « si je ne donne plus signe de vie, distribuez ceci ».*
- Pour Cypher Home : le dashboard saura que la carte est hors ligne (F13). Ce sera codé en P6.1, pas en P2.2.

### Pourquoi un broker, plutôt que la carte qui parle directement au PC

- La carte ne connaît qu'une adresse : celle du broker. Elle ignore qui lit ses mesures.
- On peut ajouter des lecteurs sans toucher la carte : le serveur Python et, pendant la mise au point, un outil d'écoute (`mosquitto_sub`).
- C'est toujours la carte qui ouvre la connexion vers le broker. Les commandes du serveur (P6.3) reviendront par cette même connexion : la carte n'a pas à accepter de connexion entrante.
- Le statut « en ligne / hors ligne » est fourni par le protocole (LWT).
- Le prix : un logiciel de plus à faire tourner et à protéger.

### Pourquoi Docker, plutôt qu'une installation native (D8, ouverte)

- **Docker** lance des logiciels dans des **conteneurs**. Une **image** est un modèle prêt à l'emploi : le logiciel et tout ce dont il a besoin (ici `eclipse-mosquitto:2.0.22`). Un conteneur est une image en marche, isolée du reste du PC. *Un local préfabriqué, livré meublé, posé dans l'immeuble, avec ses propres murs.*
- Sous Windows, Docker Desktop fait tourner une petite machine virtuelle Linux ; les conteneurs vivent dedans.
- Pour : la version est figée dans un fichier ; on démarre et on arrête en une commande ; rien n'est installé dans Windows ; MongoDB suivra la même méthode ; tout se supprime proprement.
- Contre : Docker Desktop consomme de la mémoire et met du temps à démarrer ; il ajoute une couche réseau (le port publié, étape 1) ; c'est un outil de plus à apprendre.
- L'installation native passe par l'installateur Windows de Mosquitto, qui l'enregistre comme service Windows (il peut alors démarrer avec le PC [H]). Le chemin réseau est plus direct, mais les mises à jour se font à la main.
- D8 reste ouverte : P2.2 dira si Docker convient.

### Docker n'est pas un `.venv`

- Un **`.venv`** isole les **bibliothèques Python** d'un projet. Il réutilise le Python installé sur Windows et ne contient que du Python.
- Un **conteneur Docker** isole un **logiciel complet** : son programme, ses fichiers système, son propre réseau.
- Mosquitto est un programme écrit en C, pas une bibliothèque Python : un `.venv` ne peut pas le faire tourner.
- Les deux se complètent : le futur serveur Python aura son `.venv` dans `server/`, Mosquitto son conteneur.

## Étape 1 : un broker local, sans mot de passe, joignable depuis le PC seulement

### Ce qu'on construit

Mosquitto tourne dans un conteneur. Seuls les programmes du PC peuvent le joindre. Il n'y a pas encore de mot de passe : c'est **provisoire**, jusqu'à l'étape 4.

*Le gardien s'installe dans son local. La porte du local donne sur le couloir intérieur de l'immeuble, pas sur la rue.*

Deux notions de réseau suffisent ici (détails à l'étape 2) :
- un **port** est un numéro de porte : sur une machine, chaque programme réseau attend derrière la sienne. 1883 est la porte habituelle de MQTT ;
- **`127.0.0.1`** (ou `localhost`) veut dire « cette machine-ci ». Une connexion vers 127.0.0.1 ne sort jamais de la machine.

### Fichier `docker/docker-compose.yml`

**Docker Compose** lit ce fichier et lance les conteneurs qu'il décrit (`docker compose up`).

| Ligne | Ce qu'elle fait | Ce qui casse si on l'enlève |
|---|---|---|
| `name: cypherhome` | Nomme le projet ; le conteneur s'appelle `cypherhome-mosquitto-1`. | Rien : le projet prend le nom du dossier (`docker`), moins parlant. |
| `services:` puis `mosquitto:` | Déclare un service nommé `mosquitto`. Ce nom sert dans les commandes (`docker compose logs mosquitto`). | Compose n'a rien à lancer : erreur. |
| `image: eclipse-mosquitto:2.0.22` | Choisit l'image et fige sa version. | Sans la ligne : erreur. Sans `:2.0.22` : Docker prend `latest`, qui change sans prévenir (aujourd'hui une version 2.1 [H]). |
| `ports:` `"127.0.0.1:1883:1883"` | Perce une porte : port 1883 du PC vers port 1883 du conteneur, **sur l'adresse 127.0.0.1 du PC seulement**. | Sans la ligne : le broker tourne, mais rien hors du conteneur ne l'atteint. Sans `127.0.0.1:` : la porte s'ouvre aussi sur le Wi-Fi (on le fera exprès à l'étape 2). |
| `volumes:` `./mosquitto/config:/mosquitto/config` | Fait apparaître le dossier du dépôt dans le conteneur, là où Mosquitto lit sa configuration. | Mosquitto prend la configuration par défaut de l'image, sans `listener` : il ne répond qu'à l'intérieur du conteneur. |

Il n'y a volontairement **pas** de ligne `restart:` : le broker ne redémarre pas tout seul avec Docker Desktop. À l'étape 2, il sera ouvert sans mot de passe sur le Wi-Fi ; il ne doit tourner que pendant les tests.

### Fichier `docker/mosquitto/config/mosquitto.conf`

| Ligne | Ce qu'elle fait | Ce qui casse si on l'enlève |
|---|---|---|
| `listener 1883` | Mosquitto écoute sur le port 1883, sur toutes les adresses du conteneur. | Mosquitto 2 passe en *local only mode* : seuls les clients lancés dans le conteneur sont acceptés. Ni le PC ni la carte ne peuvent se connecter. |
| `allow_anonymous true` | Accepte les clients sans identifiant. **PROVISOIRE.** | Depuis Mosquitto 2.0, la valeur par défaut est `false` : sans fichier de mots de passe, tout client est refusé (`not authorised`). |

### Le piège : pourquoi pas `listener 1883 127.0.0.1` ?

- 127.0.0.1 veut dire « cette machine-ci ». Dans un conteneur, cette machine, c'est le **conteneur**.
- Docker fait entrer les connexions du PC par la carte réseau du conteneur, pas par son 127.0.0.1.
- Avec `listener 1883 127.0.0.1`, Mosquitto n'écouterait que l'intérieur de son conteneur : les connexions venues du PC ne trouveraient personne. *Le gardien n'ouvrirait qu'aux gens déjà dans son local.*
- La restriction « PC seulement » se fait donc dans `docker-compose.yml` : là, `127.0.0.1` désigne bien le PC.
- Le piège est sournois : les tests avec `docker compose exec` (ci-dessous) marcheraient quand même, car ils tournent dans le conteneur.

### Scénario de test

Docker Desktop doit afficher « Engine running ». Les commandes se tapent dans PowerShell, depuis le dossier `docker` du dépôt (`cd docker`).

1. **Moteur** : `docker info --format "{{.ServerVersion}}"` affiche un numéro de version. L'erreur `failed to connect to the docker API` veut dire que le moteur est arrêté.
2. **Démarrer** : `docker compose up -d`. `-d` lance en arrière-plan et rend la main. La première fois, Docker télécharge l'image (quelques Mo).
3. **État** : `docker compose ps`. Attendu : statut `Up`, ports `127.0.0.1:1883->1883/tcp`.
4. **Journaux** : `docker compose logs mosquitto`. Attendu : `Config loaded from /mosquitto/config/mosquitto.conf.` puis `mosquitto version 2.0.22 running`. Le nombre en début de ligne est l'heure, en secondes Unix. Si on lit `Starting in local only mode`, notre fichier n'a pas été lu.
5. **Message de test**, deux terminaux, tous deux dans `docker` :
   - terminal A : `docker compose exec mosquitto mosquitto_sub -t cypherhome/test -v` (il attend) ;
   - terminal B : `docker compose exec mosquitto mosquitto_pub -t cypherhome/test -m "bonjour"`.

   Attendu dans A : `cypherhome/test bonjour`. `-t` donne le sujet, `-m` le message, `-v` affiche le sujet devant le message. Les journaux montrent alors `New client connected from 127.0.0.1` (ou `::1`) : les clients sont **dans** le conteneur. Ctrl+C arrête A.
6. **Porte du PC** : `Test-NetConnection 127.0.0.1 -Port 1883`. Attendu : `TcpTestSucceeded : True`.
7. **Adresse d'écoute** : `Get-NetTCPConnection -LocalPort 1883 -State Listen`. Attendu : `LocalAddress` vaut `127.0.0.1`, et rien d'autre. Si on voit `0.0.0.0` ou `::`, la porte est ouverte à tous : arrêter le broker.
8. **Arrêt** : `docker compose down` supprime le conteneur (l'image et nos fichiers restent). Puis de nouveau `Test-NetConnection 127.0.0.1 -Port 1883` : attendu `TcpTestSucceeded : False`.

### Ce que ces tests prouvent, et ce qu'ils ne prouvent pas

| Test | Prouve | Ne prouve pas |
|---|---|---|
| 4. Journaux | Mosquitto a lu notre fichier et tourne. | Qu'on peut le joindre hors du conteneur. |
| 5. `exec` sub et pub | Le broker reçoit un message et le redistribue à l'abonné. | Rien sur la porte du PC : les deux outils tournent dans le conteneur. |
| 6. `Test-NetConnection` | Quelque chose accepte une connexion sur le port 1883 du PC. | Que Mosquitto est derrière : c'est le relais de Docker qui répond, et il pourrait accepter même si Mosquitto n'écoutait pas [H]. La preuve viendra à l'étape 3, quand la carte parlera MQTT par cette porte. |
| 7. Adresse d'écoute | La porte n'existe que sur 127.0.0.1 : rien ne peut entrer depuis le Wi-Fi. | Le rôle du pare-feu (pas encore concerné). |
| 8. Arrêt | C'est bien notre conteneur qui ouvrait la porte ; elle se referme à l'arrêt. | N/A |

### Résultat (2026-11-11)

- Journaux : `Config loaded from /mosquitto/config/mosquitto.conf.`, `mosquitto version 2.0.22 running`.
- `mosquitto_sub` a reçu `cypherhome/test bonjour` et `cypherhome/test ca va ?`. Les clients venaient de `::1` : l'intérieur du conteneur.
- `Test-NetConnection 127.0.0.1 -Port 1883` : `True`. Au même moment, les journaux montrent `New connection from 172.18.0.1` puis `Client <unknown> closed its connection` : la connexion du PC a bien traversé le relais de Docker jusqu'à Mosquitto. Elle arrive depuis l'adresse de Docker (`172.18.0.1`), pas depuis celle du PC.
- `Get-NetTCPConnection` : une seule adresse d'écoute, `127.0.0.1`.
- Après `docker compose down` : `TcpTestSucceeded : False`.
- Le message `What's next: Debug this Compose error with Gordon` après Ctrl+C dans le terminal A est une publicité de Docker Desktop : Ctrl+C termine `mosquitto_sub` avec un code d'erreur, ce n'est pas une erreur de configuration.

> **Ce qui reste à faire.** Le broker n'est joignable que depuis le PC. Or la carte est un autre appareil, sur le Wi-Fi de l'iPhone. À l'étape 2, on ouvre la porte sur le Wi-Fi (port Docker, pare-feu Windows), et la carte vérifie qu'elle l'atteint.

## Étape 2 : la porte s'ouvre sur le Wi-Fi, et la carte l'atteint

### Ce qu'on construit

La carte est un autre appareil, sur le Wi-Fi de l'iPhone. Elle doit atteindre la porte 1883 du PC. À cette étape, elle frappe seulement à la porte : elle ouvre une connexion, sans parler MQTT.

*La porte du gardien donne maintenant sur la rue. À l'entrée de l'immeuble, un vigile (le pare-feu) décide qui passe.*

### Les notions de l'étape

- **Adresse IP** : le numéro d'un appareil sur un réseau. *L'adresse de l'immeuble.* Sur le partage de connexion, l'iPhone est `172.20.10.1` et il distribue aux autres appareils des adresses `172.20.10.x` (c'est le **DHCP**). Elles peuvent changer à chaque session : l'IP du PC est donc dans `secrets.h`, à mettre à jour.
- **Port** : le numéro de porte d'un programme sur cet appareil. On joint un programme avec une IP **et** un port : `172.20.10.x:1883`.
- **TCP** : la façon d'ouvrir une conversation sur le réseau. Avant d'envoyer quoi que ce soit, le client frappe (« je peux entrer ? »), le serveur ouvre (« oui »), le client confirme. Ensuite, TCP garantit que les données arrivent entières et dans l'ordre. MQTT circule dans une connexion TCP. *Frapper et attendre qu'on ouvre avant de parler.*
- Deux façons d'échouer, qu'on va observer :
  - **personne derrière la porte** (broker arrêté) : le PC répond aussitôt « refusé » ; l'échec est rapide [H] ;
  - **le vigile refuse** (pare-feu) : il ne répond rien ; la carte attend jusqu'à son délai maximal, plus longtemps [H].
- **Pare-feu Windows** : il filtre les connexions qui **entrent** dans le PC. Par défaut, il les bloque, sauf si une **règle** les autorise. Les connexions qui sortent du PC ne sont pas concernées.
- **Profil réseau** : Windows classe chaque réseau en **privé** (de confiance : maison) ou **public** (café, gare). Une règle peut ne valoir que pour un profil. La nôtre ne vaut que pour **privé** : sur un Wi-Fi public, la porte reste fermée. Le partage de connexion de l'iPhone doit donc être marqué privé (Windows le retient pour ce réseau seulement).

### Danger : un broker ouvert, sans mot de passe

Jusqu'à l'étape 4, **n'importe quel appareil du même Wi-Fi peut lire et publier** sur le broker. C'est PROVISOIRE. Trois garde-fous : le partage de connexion n'accueille que mes appareils (il a un mot de passe Wi-Fi) ; les étapes 2 à 4 se font dans la même session ; le broker est arrêté (`docker compose down`) dès que je ne teste plus.

### Ce qui change

| Fichier, ligne | Ce qu'elle fait | Ce qui casse si on l'enlève |
|---|---|---|
| `docker-compose.yml` : `"1883:1883"` | Ouvre la porte 1883 sur **toutes** les adresses du PC, dont le Wi-Fi. | Avec `127.0.0.1:` devant (étape 1), la carte reçoit `KO` : la porte n'existe pas côté Wi-Fi. |
| `config.h` : `MQTT_BROKER_PORT = 1883` | Le numéro de porte du broker. | Ne compile plus : `mqtt.cpp` l'utilise. |
| `secrets.h` : `MQTT_BROKER_IP` | L'IP du PC sur le Wi-Fi de l'iPhone. Dans `secrets.h` car propre à mon réseau, hors Git. | Ne compile plus. Une IP périmée (autre session) donne `KO`. |
| `mqtt.cpp` : `static WiFiClient s_client` | L'objet qui tient une connexion TCP, via le module Wi-Fi de la carte. | Plus de connexion possible. |
| `mqtt_setup()` : `s_client.stop()` | Ferme la connexion précédente avant d'en ouvrir une neuve. | Une connexion restée ouverte passerait encore le pare-feu après la désactivation de la règle : le test dirait OK à tort. |
| `mqtt_setup()` : `s_client.connect(IP, port)` | Frappe à la porte et attend la réponse. | C'est le test lui-même. |
| `mqtt_test()` : `millis()` avant et après | Chronomètre la connexion. | Plus de durée affichée. |
| `mqtt_test()` : `Serial.println("[mqtt] KO")` | Échec = une seule ligne (convention des tests). | Un échec passerait inaperçu. |
| `main.cpp` : `mqtt_setup()` après `wifi_setup()` | Connexion au démarrage, une fois le Wi-Fi prêt. | Rien de visible à cette étape ; elle servira à l'étape 3. |
| `self_test.cpp` : bannière `MQTT` puis `mqtt_test()` | Le test passe à chaque tour de `self_test_run()`, après le Wi-Fi. | Plus de test MQTT dans la sortie série. |

Côté Mosquitto, la carte n'apparaîtra pas avec sa propre IP : comme à l'étape 1, les connexions arrivent depuis l'adresse de Docker (`172.18.0.1`) [H].

### Scénario de test

PC et carte sur le partage de connexion de l'iPhone. Les commandes marquées **(admin)** se tapent dans un PowerShell ouvert en administrateur (clic droit, « Exécuter en tant qu'administrateur »).

1. **IP du PC** : `Get-NetIPAddress -InterfaceAlias WiFi -AddressFamily IPv4`. Attendu : `172.20.10.x`. La recopier dans `MQTT_BROKER_IP` de `secrets.h`.
2. **Profil** : `Get-NetConnectionProfile`. Si le réseau de l'iPhone est `Public` : **(admin)** `Set-NetConnectionProfile -InterfaceAlias WiFi -NetworkCategory Private`.
3. **Règles déjà créées pour Docker (admin)** :
   `Get-NetFirewallApplicationFilter | Where-Object Program -like "*docker*" | Get-NetFirewallRule | Where-Object Direction -eq Inbound | Format-Table DisplayName, Enabled, Action, Profile`
   Une règle `Allow` laisserait passer la carte même sans notre règle (le test du pare-feu ne montrerait rien) ; une règle `Block` l'emporte sur toute autorisation (la carte serait toujours KO). Si une fenêtre du pare-feu apparaît pour Docker, ne pas cliquer au hasard : « Autoriser » ouvre tous les ports de Docker, « Annuler » crée une règle de blocage [H].
4. **Règle pour le port 1883 (admin)** :
   `New-NetFirewallRule -DisplayName "Cypher Home - MQTT 1883" -Direction Inbound -Protocol TCP -LocalPort 1883 -Action Allow -Profile Private`
   (`-Direction Inbound` : connexions entrantes ; `-Protocol TCP -LocalPort 1883` : seulement cette porte ; `-Action Allow` : laisser passer ; `-Profile Private` : seulement sur un réseau privé.)
5. **Broker** (dans `docker`) : `docker compose up -d`, puis `docker compose ps` (attendu : `0.0.0.0:1883->1883/tcp`) et `Get-NetTCPConnection -LocalPort 1883 -State Listen` (attendu : `0.0.0.0`).
6. **Depuis le PC** : `Test-NetConnection <IP du PC> -Port 1883`. Attendu : `True`.
7. **Carte** : téléverser (`pio run -d firmware -t upload`), puis lancer le moniteur série. Attendu :
   ```
   ----- TEST MQTT -----
   [mqtt] broker .................. 172.20.10.x:1883
   [mqtt] connexion tcp ........... NN ms
   ```
   Dans `docker compose logs mosquitto` : `New connection from 172.18.0.1` à chaque passe.
8. **KO, broker arrêté** : `docker compose down`. Attendu : `[mqtt] KO`, vite. Noter la durée approximative de la pause avant la bannière suivante. Puis `docker compose up -d` : retour à OK.
9. **KO, pare-feu (admin)** : `Disable-NetFirewallRule -DisplayName "Cypher Home - MQTT 1883"`. Attendu : `[mqtt] KO`, après une attente plus longue (la noter). Puis **(admin)** `Enable-NetFirewallRule -DisplayName "Cypher Home - MQTT 1883"` : retour à OK.
10. **Fin** : `docker compose down`.

### Ce que ces tests prouvent, et ce qu'ils ne prouvent pas

| Test | Prouve | Ne prouve pas |
|---|---|---|
| 5. Adresse d'écoute | La porte est ouverte sur toutes les adresses du PC. | Que le pare-feu la laisse passer. |
| 6. Depuis le PC | La porte répond sur l'IP Wi-Fi du PC. | Le pare-feu : une connexion du PC vers lui-même ne passe probablement pas par le filtrage des connexions entrantes [H]. Seule la carte le prouve. |
| 7. Carte OK | La carte et le PC se parlent à travers le partage de l'iPhone, et le pare-feu laisse entrer le port 1883. | Que Mosquitto comprend la carte : aucun mot MQTT n'est encore échangé. |
| 8. Broker arrêté | Le OK de l'étape 7 venait bien de notre broker. | N/A |
| 9. Règle désactivée | C'est **notre** règle qui ouvre la porte (sauf règle Docker trouvée au point 3). | N/A |

### Résultat (2026-11-11)

- Carte : `[mqtt] broker ... 172.20.10.3:1883`, `[mqtt] connexion tcp ... 32 ms`.
- `docker compose down` : `[mqtt] KO` ; `docker compose up -d` : retour à OK (26 ms).
- Règle désactivée : `[mqtt] KO` ; règle réactivée : retour à OK (26 ms).
- Conséquence : aucune règle propre à Docker ne laissait passer le port 1883 à la place de la nôtre (sinon la carte serait restée OK, règle désactivée).
- Non relevé : sorties des points 1 à 6 et durée des deux KO. L'écart « rapide / lent » entre les deux échecs reste donc une hypothèse.

> **Ce qui reste à faire.** La carte frappe à la porte, mais ne dit rien. À l'étape 3, elle parle MQTT : elle se présente au broker (CONNECT), attend sa réponse (CONNACK) et publie un message sur `cypherhome/test`.

## Étape 3 : la carte parle MQTT et publie un message

### Ce qu'on construit

La carte ne se contente plus de frapper : elle se présente au gardien, attend sa réponse, puis dépose une lettre sur `cypherhome/test`. Un abonné sur le PC la reçoit. Toujours sans mot de passe (PROVISOIRE, jusqu'à l'étape 4).

### Les notions de l'étape

- **CONNECT** : le premier message MQTT, envoyé par le client juste après l'ouverture de la connexion TCP. Il contient l'**identifiant client** (`cypherhome-board`), le délai de keep alive et, plus tard, l'utilisateur et le mot de passe. *Se présenter au gardien.*
- **CONNACK** : la réponse du broker. Elle contient un code : 0 = accepté, sinon la raison du refus (détaillée à l'étape 4). *Le gardien dit « entrez » ou « non ».*
- **Identifiant client** : le nom du client pour le broker. Il doit être unique : si un second client se présente avec le même nom, le broker coupe le premier.
- **PUBLISH** : le message qui transporte une lettre : un sujet et un contenu. En **QoS 0**, il part une fois, sans accusé de réception : la carte ne sait pas si un abonné l'a reçu.
- **Keep alive** : la carte promet de donner signe de vie au moins toutes les 60 s (valeur par défaut de la bibliothèque). Au-delà d'une fois et demie ce délai sans nouvelles, le broker la considère comme partie.

### Bibliothèque : ArduinoMqttClient 0.1.8

| Critère | ArduinoMqttClient 0.1.8 (retenue) | PubSubClient 2.8 |
|---|---|---|
| Origine, dernière version | Arduino, 2024 | un auteur, 2020 |
| Plateformes déclarées | toutes (`*`) | AVR et ESP seulement (pas la R4) |
| QoS en publication | 0, 1, 2 | 0 seulement |
| Taille d'un message | envoyé au fil de l'eau, sans tampon fixe | tampon de 256 octets par défaut |
| Testament (LWT) | oui | oui |
| Code d'erreur de connexion | oui (`connectError()`) | oui (`state()`) |

Elle se branche sur le `WiFiClient` de WiFiS3 : le client MQTT parle **à travers** la connexion TCP de l'étape 2. Ligne ajoutée à `platformio.ini` : `arduino-libraries/ArduinoMqttClient@0.1.8` (version figée).

### Ce qui change

| Fichier, ligne                                                 | Ce qu'elle fait                                                                                                                                                                                         | Ce qui casse si on l'enlève                                                                       |
| -------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------- |
| `config.h` : `MQTT_CLIENT_ID = "cypherhome-board"`             | Nom de la carte auprès du broker.                                                                                                                                                                       | La bibliothèque invente un nom (`Arduino-…`) qui change à chaque démarrage : journaux illisibles. |
| `config.h` : `MQTT_CONNECT_TIMEOUT_MS = 5000`                  | Attente maximale de CONNACK.                                                                                                                                                                            | La bibliothèque attend 30 s par défaut : une passe peut rester bloquée 30 s.                      |
| `mqtt.cpp` : `MqttClient s_mqtt_client(s_wifi_client)`         | Crée le client MQTT au-dessus de la connexion TCP.                                                                                                                                                      | Plus de MQTT.                                                                                     |
| `mqtt_setup()` : `setId()`, `setConnectionTimeout()`           | Règle l'identifiant et le délai avant chaque connexion.                                                                                                                                                 | Voir les deux lignes de `config.h`.                                                               |
| `mqtt_setup()` : `connect(IP, port)`                           | Ouvre une connexion TCP neuve (et ferme l'ancienne), envoie CONNECT, attend CONNACK. Le délai de 5 s ne couvre que CONNACK ; l'ouverture TCP garde le délai du module Wi-Fi (jusqu'à 10 s environ [H]). | Plus de connexion.                                                                                |
| `mqtt_ensure_connected()` : `poll()`                           | Lit ce que le broker a envoyé, entretient le keep alive, détecte une connexion fermée.                                                                                                                  | La perte de connexion n'est pas vue ; le keep alive n'est plus entretenu.                         |
| `mqtt_ensure_connected()` : `connected() \|\| mqtt_setup()`    | **Reconnexion automatique** : si la connexion est perdue, on la rouvre.                                                                                                                                 | Après un arrêt du broker, la carte resterait KO pour toujours.                                    |
| `mqtt_publish()` : `beginMessage()`, `print()`, `endMessage()` | Construit le PUBLISH (sujet, QoS 0) et l'envoie.                                                                                                                                                        | Plus de message.                                                                                  |
| `mqtt_test()` : `TEST_TOPIC`, contenu daté                     | Publie `bonjour de la carte, NN s` : le nombre de se condes distingue chaque passe.                                                                                                                     | Sans date, impossible de savoir si un message est neuf.                                           |

`TEST_TOPIC` reste une constante locale de `mqtt.cpp` : ce sujet n'existe que pour le test.

### Scénario de test

PC et carte sur le partage de connexion. Profil privé, règle de pare-feu active, `MQTT_BROKER_IP` à jour dans `secrets.h` (l'IP du PC peut avoir changé).

1. **Broker** (dans `docker`) : `docker compose up -d`.
2. **Abonné**, terminal A : `docker compose exec mosquitto mosquitto_sub -t cypherhome/test -v`.
3. **Carte** : téléverser (`pio run -d firmware -t upload`), lancer le moniteur série. Attendu :
   ```
   ----- TEST MQTT -----
   [mqtt] broker .................. 172.20.10.x:1883
   [mqtt] connexion ............... NN ms
   [mqtt] sujet ................... cypherhome/test
   [mqtt] message ................. bonjour de la carte, NN s
   ```
   Et dans le terminal A, à chaque passe : `cypherhome/test bonjour de la carte, NN s`.
4. **Journaux** : `docker compose logs mosquitto`. Attendu : `New client connected from 172.18.0.1:… as cypherhome-board`.
5. **KO, broker arrêté** : `docker compose down`. Attendu : `[mqtt] KO`, sur une seule ligne. **Durée de blocage** : comparer l'écart entre deux lignes `=== Self-test : passe N, X s depuis le demarrage ===` pendant le KO et en temps normal ; la différence est le temps perdu dans le test MQTT.
6. **Retour** : `docker compose up -d`, puis relancer l'abonné (il a été coupé avec le conteneur). Attendu : OK à la passe suivante, avec une nouvelle durée de connexion (reconnexion automatique), et le message de nouveau reçu dans A.
7. **Fin** : `docker compose down`.

### Ce que ces tests prouvent, et ce qu'ils ne prouvent pas

| Test | Prouve | Ne prouve pas |
|---|---|---|
| 3. Message reçu dans A | La carte parle MQTT ; le broker accepte sa connexion et redistribue son message. | Qu'un programme Windows hors du conteneur le reçoit (l'abonné tourne dans le conteneur ; ce sera le serveur Python, P5.2). Qu'aucun message ne se perd : en QoS 0, la carte ne le saurait pas. |
| 4. Journaux | La carte est identifiée par `cypherhome-board`. | N/A |
| 5. Broker arrêté | Un échec donne une seule ligne ; durée de blocage mesurée. | Le cas « broker qui ne répond plus sans fermer la connexion » (non testé). |
| 6. Retour | La reconnexion automatique fonctionne. | Sa tenue sur une longue durée. |

### Résultat (2026-11-11)

- Passe 1 : `[mqtt] connexion ... 68 ms`, message `bonjour de la carte, 20 s` reçu dans le terminal A.
- Journaux : `New client connected from 172.18.0.1:42488 as cypherhome-board (p2, c1, k60)`. `k60` = keep alive de 60 s ; `c1` = session neuve à chaque connexion (*clean session*).
- Passe 2, broker arrêté : `[mqtt] KO` sur une seule ligne.
- Passe 3, broker relancé : reconnexion automatique (85 ms), message `bonjour de la carte, 71 s` reçu.
- Durée de blocage : 21 s entre les passes 1 et 2 (normal), 31 s entre les passes 2 et 3 (KO). Le KO coûte donc **environ 10 s**. C'est la durée du délai du module Wi-Fi (10 s) : l'échec « rapide » prévu à l'étape 2 n'est pas observé ici. Cause exacte non vérifiée [H].

> **Ce qui reste à faire.** N'importe qui sur le Wi-Fi peut encore lire et publier, y compris une fausse commande. À l'étape 4, le broker exige un utilisateur et un mot de passe, et la carte présente les siens dans CONNECT.

## Étape 4 : authentification

### Pourquoi un mot de passe

Sans lui, tout appareil du même Wi-Fi peut lire les mesures et **publier** à la place de la carte ou du serveur : de fausses mesures, ou plus tard une fausse commande « désactiver la surveillance ». *Le gardien ne remet plus de lettre et n'en accepte plus sans badge.*

### Les notions de l'étape

- **Authentification** : prouver qui l'on est. Ici, un **utilisateur** et un **mot de passe**, envoyés dans CONNECT. Le broker répond par CONNACK : 0 = accepté, sinon refus et fermeture de la connexion.
- **Hachage** : le broker ne garde pas les mots de passe, mais leur **empreinte**. Une fonction de hachage transforme un mot de passe en une suite de caractères ; on ne peut pas revenir en arrière. À la connexion, le broker calcule l'empreinte du mot de passe reçu et la compare à celle du fichier. Un **sel** (valeur aléatoire propre à chaque utilisateur) rend deux empreintes différentes même pour deux mots de passe identiques. *Le gardien garde un moulage du badge, pas le badge.*
- Une ligne du fichier ressemble à `cypherhome-board:$7$101$<sel>$<empreinte>` : `7` désigne la méthode (PBKDF2-SHA512), `101` le nombre de répétitions du calcul [H].
- **Un utilisateur par usage** : `cypherhome-board` pour la carte, `cypherhome-server` pour le futur serveur Python (et pour les tests depuis le PC). Si un mot de passe fuit, on change celui-là seul, et les journaux disent qui s'est connecté.

### Ce qui change

| Fichier, ligne | Ce qu'elle fait | Ce qui casse si on l'enlève |
|---|---|---|
| `mosquitto.conf` : `allow_anonymous false` | Refuse tout client sans utilisateur valide. | C'est déjà la valeur par défaut de Mosquitto 2 ; l'écrire rend la règle visible. Avec `true`, on revient à l'étape 3 : tout le monde entre. |
| `mosquitto.conf` : `password_file /mosquitto/config/passwd` | Indique où lire les utilisateurs et leurs empreintes. | Sans fichier et sans anonymes, plus personne ne peut se connecter. |
| `docker/mosquitto/config/passwd` | Le fichier des empreintes, créé par `mosquitto_passwd`. Ignoré par Git (règle `**/passwd` du `.gitignore`). | Fichier absent : Mosquitto refuse de démarrer [H]. |
| `secrets.h` : `MQTT_USERNAME`, `MQTT_PASSWORD` | Identifiants de la carte, hors Git. | Ne compile plus. |
| `mqtt.cpp` : `setUsernamePassword(...)` | Ajoute utilisateur et mot de passe à CONNECT. | La carte se présente sans badge : `[mqtt] KO` à chaque passe. |

### Créer les utilisateurs : `mosquitto_passwd`

- `-c` (*create*) **crée** le fichier, et écrase celui qui existe. On l'utilise pour le **premier** utilisateur seulement. Pour le second, on l'omet : sinon le fichier est recréé et le premier utilisateur disparaît.
- `docker compose run --rm` plutôt que `exec` : `exec` lance une commande dans le conteneur **en marche** ; or, sans fichier de mots de passe, le broker ne peut pas tourner. `run` crée un conteneur temporaire avec le même dossier monté, y lance `mosquitto_passwd`, et `--rm` le supprime à la fin. Le fichier reste, car il est écrit dans le dossier du dépôt.

### Codes de connexion d'ArduinoMqttClient (`connectError()`)

| Code | Nom                                  | Sens                                                                         |
| ---- | ------------------------------------ | ---------------------------------------------------------------------------- |
| -2   | `MQTT_CONNECTION_REFUSED`            | La connexion TCP n'a pas abouti (malgré le nom, ce n'est pas un refus MQTT). |
| -1   | `MQTT_CONNECTION_TIMEOUT`            | Pas de CONNACK dans le délai (5 s).                                          |
| 0    | `MQTT_SUCCESS`                       | Accepté.                                                                     |
| 1    | `MQTT_UNACCEPTABLE_PROTOCOL_VERSION` | Version de MQTT refusée.                                                     |
| 2    | `MQTT_IDENTIFIER_REJECTED`           | Identifiant client refusé.                                                   |
| 3    | `MQTT_SERVER_UNAVAILABLE`            | Broker indisponible.                                                         |
| 4    | `MQTT_BAD_USER_NAME_OR_PASSWORD`     | Utilisateur ou mot de passe mal formé.                                       |
| 5    | `MQTT_NOT_AUTHORIZED`                | Refusé. Mosquitto renvoie ce code pour un mauvais mot de passe [H].          |

En cas d'échec, la carte affiche `[mqtt] KO` puis une ligne `[mqtt] code erreur ..... N` avec ce code. C'est une **exception choisie** à la convention des tests (« une seule ligne KO ») : le code dit tout de suite si la carte n'a pas atteint le broker (-2, -1) ou si le broker l'a refusée (1 à 5). Les journaux du broker confirment : `not authorised` = mauvais identifiants ; aucune ligne = la carte n'a pas atteint le broker.

| Ligne de `mqtt_test()` | Ce qu'elle fait | Ce qui casse si on l'enlève |
|---|---|---|
| `snprintf(code, …, "%d", connectError())` puis `self_test_print(...)` | Affiche le code de la dernière tentative de connexion, en texte (`self_test_print()` n'affiche pas les entiers négatifs). | Retour au seul `[mqtt] KO` : il faut ouvrir les journaux du broker pour savoir pourquoi. |

Limite [H] : si la connexion réussit mais que l'envoi du message échoue, le code affiché est `0` (connexion acceptée) ; ce cas n'a pas été observé.

### Ce qui n'est PAS protégé

**Sans TLS, le mot de passe passe en clair sur le Wi-Fi** : un appareil qui écoute le réseau peut le lire, ainsi que tous les messages (TLS = P10.2, bonus). **Il n'y a pas de droits par sujet** : tout utilisateur accepté peut lire et publier sur tous les sujets. Enfin, le mot de passe tapé dans une commande `mosquitto_pub -P` reste dans l'historique de PowerShell.

### Scénario de test

1. **Mots de passe** : en choisir deux, longs (20 caractères ou plus), différents, sans `"` ni `\`. Ajouter `MQTT_USERNAME` et `MQTT_PASSWORD` dans `secrets.h` (voir `secrets.example.h`).
2. **Utilisateurs** (dans `docker`, broker arrêté avec `docker compose down`). Chaque commande demande le mot de passe deux fois :
   - `docker compose run --rm mosquitto mosquitto_passwd -c /mosquitto/config/passwd cypherhome-board`
   - `docker compose run --rm mosquitto mosquitto_passwd /mosquitto/config/passwd cypherhome-server`
   - `Get-Content mosquitto\config\passwd` : deux lignes, `cypherhome-board:$7$…` et `cypherhome-server:$7$…`.
   - Depuis la racine du dépôt : `git status --short` ne doit **pas** lister `passwd`.
3. **Broker** : `docker compose up -d`, puis `docker compose logs mosquitto`. Attendu : `running`. Un avertissement sur les droits du fichier (`world readable`) est possible [H].
4. **Abonné**, terminal A : `docker compose exec mosquitto mosquitto_sub -u cypherhome-server -P <mot de passe serveur> -t cypherhome/test -v`.
5. **Trois essais**, terminal B :
   - bon mot de passe : `docker compose exec mosquitto mosquitto_pub -u cypherhome-server -P <mot de passe serveur> -t cypherhome/test -m "avec mot de passe"`. Attendu : reçu dans A ;
   - sans mot de passe : `docker compose exec mosquitto mosquitto_pub -t cypherhome/test -m "sans mot de passe"`. Attendu : `Connection Refused: not authorised`, rien dans A ;
   - mauvais mot de passe : `docker compose exec mosquitto mosquitto_pub -u cypherhome-server -P faux -t cypherhome/test -m "mauvais mot de passe"`. Attendu : même refus, rien dans A.
6. **Carte, bons identifiants** : téléverser, moniteur série. Attendu : 4 lignes `[mqtt]` comme à l'étape 3, message reçu dans A, `as cypherhome-board` dans les journaux.
7. **Carte, mauvais mot de passe** : changer `MQTT_PASSWORD` dans `secrets.h`, téléverser. Attendu : `[mqtt] KO` à chaque passe ; `Client cypherhome-board disconnected, not authorised` dans les journaux [H] ; noter l'écart entre deux passes. Remettre le bon mot de passe, téléverser : retour à OK.
8. **Fin** : `docker compose down`.

### Ce que ces tests prouvent, et ce qu'ils ne prouvent pas

| Test | Prouve | Ne prouve pas |
|---|---|---|
| 2. Fichier `passwd` | Les deux utilisateurs existent, sous forme d'empreintes ; le fichier reste hors Git. | N/A |
| 5. Trois essais `mosquitto_pub` | Le broker n'accepte que les bons identifiants : **critère « connexion refusée sans mot de passe » de PLAN.md**. | Le chemin réseau : ces outils tournent dans le conteneur. Mais la règle s'applique à tous les clients, d'où qu'ils viennent. |
| 6. Carte OK | La carte s'authentifie et publie : **critère « message reçu » de PLAN.md**, avec mot de passe. | Que le mot de passe est protégé en route (il ne l'est pas, voir plus haut). |
| 7. Carte KO | Un mauvais mot de passe donne `[mqtt] KO`, et le broker dit pourquoi. | N/A |

### Résultat (2026-11-11)

- Abonné (`cypherhome-server`) : a reçu `avec mot de passe`, puis `bonjour de la carte, 19 s`, `40 s` et `20 s`. Il n'a reçu ni `sans mot de passe` ni `mauvais mot de passe`.
- Journaux : `u'cypherhome-server'` pour l'abonné et le bon `mosquitto_pub` ; deux fois `disconnected, not authorised` pour les essais sans mot de passe et avec un mauvais ; la carte apparaît `from 172.18.0.1 … as cypherhome-board (p2, c1, k60, u'cypherhome-board')`.
- Carte, bons identifiants : 4 lignes `[mqtt]`, connexion en 74 ms.
- Carte, mauvais mot de passe : `[mqtt] KO` (avant l'ajout de la ligne du code d'erreur).
- Non relevé : la ligne des journaux du refus de la carte et le code d'erreur renvoyé (le code 5 reste une hypothèse [H]).
- Défaut constaté : le mot de passe de `cypherhome-server` est très court et a été tapé en clair (`-P`), donc il est resté dans l'historique de PowerShell. À changer (voir étape 5).

> **Ce qui reste à faire.** Les secrets existent maintenant à trois endroits : `secrets.h`, `passwd`, et bientôt le `.env` du serveur. À l'étape 5, on vérifie qu'aucun n'entre dans Git, ni aujourd'hui ni dans l'historique, et on prépare l'endroit où le serveur Python lira ses identifiants.

## Étape 5 : ranger les secrets hors de Git

### L'idée

Un **secret** est une information qui donne un accès : un mot de passe Wi-Fi, un mot de passe MQTT, une empreinte de mot de passe. Il en existe maintenant à trois endroits : `secrets.h` (la carte), `passwd` (le broker) et, plus tard, `server/.env` (le serveur). Aucun ne doit entrer dans **Git**, l'outil qui garde l'historique du dépôt.

*Dans l'immeuble, Git est le registre du gardien : chaque page est datée et ne s'arrache pas. Rayer une ligne plus tard ne l'efface pas des pages précédentes.*

C'est le piège : un fichier supprimé dans un commit récent reste lisible dans les commits anciens. Il faut donc vérifier le dépôt **et** son historique.

### Les outils

- **`.gitignore`** : la liste des fichiers que Git doit ignorer. Il ne protège que les fichiers **jamais commités** : un fichier déjà suivi reste suivi (il faudrait `git rm --cached`).
- **`git check-ignore -v <fichier>`** : dit si un fichier est ignoré, et par quelle règle.
- **`.env`** : un fichier texte de lignes `NOM=valeur`, lu au démarrage par un programme. C'est la convention habituelle pour les secrets d'un serveur Python. Son modèle **`.env.example`** est versionné, avec des valeurs factices : il montre le format, comme `secrets.example.h` pour la carte.

### Ce que couvre le `.gitignore`

| Règle | Fichier protégé | Ce qui casse si on l'enlève |
|---|---|---|
| `**/secrets.h` | `firmware/include/secrets.h` | Le mot de passe Wi-Fi, celui de la carte et l'IP du PC partent au prochain `git add`. |
| `**/passwd` | `docker/mosquitto/config/passwd` | Les empreintes partent. Elles ne sont pas les mots de passe, mais un mot de passe court se retrouve par essais. |
| `*.env` (et `.env`) | `server/.env`, et tout `.env` du dépôt | Le mot de passe du serveur part. |

`*.env` ne touche pas `.env.example` : le nom ne finit pas par `.env`. Le modèle reste donc versionné, et c'est voulu.

### Vérifications faites (2026-11-11)

Aucune valeur n'a été affichée : seulement des noms de fichiers et des comptes.

| Vérification | Résultat |
|---|---|
| `git check-ignore -v` sur `secrets.h`, `passwd`, `server/.env`, `docker/.env`, `.env` | les cinq sont ignorés, chacun par la règle attendue |
| `git check-ignore` sur `secrets.example.h` et `server/.env.example` | suivis, comme voulu |
| Fichiers `secrets.h`, `passwd`, `.env`, `.pem`, `.key` suivis par Git | aucun |
| Les mêmes noms dans **tout** l'historique (toutes branches) | aucun |
| Empreintes Mosquitto (`$7$…$`) dans l'historique | un seul commit, dans `mqtt.md` : c'est le modèle `$7$101$<sel>$<empreinte>` de l'étape 4, pas une vraie empreinte |
| Lignes `PASSWORD = "…"` hors fichiers `*example*`, dans tous les commits | aucune |

Ce que ces vérifications **ne prouvent pas** : elles cherchent des noms et des formes connus. Un mot de passe recopié ailleurs (dans un `.md`, un commentaire) ne serait pas trouvé. Pour le chercher, il faut sa valeur ; c'est à moi de le faire (voir le scénario).

### Si un secret fuit

1. **D'abord, changer le secret** (mot de passe Wi-Fi, `mosquitto_passwd`). Dès qu'il a été poussé sur GitHub, il faut le considérer comme lu : des robots parcourent les dépôts publics en quelques minutes, et des copies peuvent exister ailleurs.
2. **Ensuite seulement, nettoyer l'historique** (outil `git filter-repo`, puis envoi forcé). Cela réécrit tous les commits suivants : c'est lourd, et cela ne rend pas secret ce qui a déjà été lu.

L'ordre compte : nettoyer sans changer le mot de passe laisse la porte ouverte.

### Où le serveur lira ses identifiants

`server/.env.example` (versionné) donne le format ; `server/.env` (ignoré) contiendra les vraies valeurs, sur mon PC seulement. Le serveur Python n'existe pas encore : la façon de lire ce fichier sera choisie quand il sera écrit.

| Ligne | Ce qu'elle dit | Ce qui casse si on l'enlève |
|---|---|---|
| `MQTT_HOST=127.0.0.1` | Le broker tourne sur le même PC que le serveur. | Le serveur ne sait pas où se connecter. |
| `MQTT_PORT=1883` | La porte du broker. | Idem. |
| `MQTT_USERNAME=cypherhome-server` | L'utilisateur créé pour le serveur à l'étape 4 (pas celui de la carte). | Refus `not authorised`. |
| `MQTT_PASSWORD=…` | Son mot de passe (valeur factice dans le modèle). | Idem. |

Pour le serveur, `127.0.0.1` suffit : il tourne sur le PC, le port publié par Docker y répond (vu à l'étape 1). Seule la carte a besoin de l'IP du Wi-Fi.

### Scénario de test

1. **Changer le mot de passe de `cypherhome-server`** (trop court, et visible dans l'historique de PowerShell) : broker arrêté, dans `docker` : `docker compose run --rm mosquitto mosquitto_passwd /mosquitto/config/passwd cypherhome-server` (**sans `-c`** : on remplace la ligne de cet utilisateur, la carte garde la sienne).
2. **Effacer l'ancien mot de passe de l'historique de PowerShell** : ouvrir le fichier donné par `(Get-PSReadLineOption).HistorySavePath` et supprimer les lignes `mosquitto_sub` / `mosquitto_pub` qui contiennent `-P`.
3. **Créer `server/.env`** : copier `server/.env.example` en `server/.env`, mettre le nouveau mot de passe. Puis, depuis la racine : `git status --short` ne doit **pas** lister `server/.env`, mais doit lister `server/.env.example`.
4. **Chercher mes vraies valeurs dans l'historique**, sans les écrire dans une commande :
   `$v = Read-Host "Valeur"; (git log --all -S $v --oneline | Measure-Object).Count`
   À faire pour le mot de passe Wi-Fi, celui de la carte et celui du serveur. Attendu : `0`. La valeur s'affiche pendant la saisie, mais l'historique ne garde que la commande.
5. **Contrôle** : broker démarré, `mosquitto_sub` avec le nouveau mot de passe reçoit le message de la carte ; l'ancien est refusé (`not authorised`).

| Test | Prouve | Ne prouve pas |
|---|---|---|
| 1 et 5 | Le mot de passe du serveur est changé, celui de la carte n'a pas bougé. | N/A |
| 3 | `server/.env` est ignoré, son modèle est suivi. | Que le serveur saura le lire (il n'existe pas). |
| 4 | Mes valeurs actuelles ne sont dans aucun commit. | Qu'elles ne sont nulle part ailleurs (copie, capture d'écran, conversation). |

> **Ce qui reste à faire.** Tous les morceaux existent : carte, Wi-Fi, pare-feu, Docker, broker avec mots de passe, secrets hors Git. L'étape 6 vérifie les deux critères de P2.2 sur les sorties relevées, et retrace le chemin complet d'un message.
