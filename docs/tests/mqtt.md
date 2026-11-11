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
4. **Journaux** : `docker compose logs mosquitto`. Attendu : `Config loaded from /mosquitto/config/mosquitto.conf.` puis `mosquitto version 2.0.22 running`. Le nombre en début de ligne est l'heure, en secondes Unix. Si on lit `Starting in local ounly mode`, notre fichier n'a pas été lu.
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

> **Ce qui reste à faire.** Le broker n'est joignable que depuis le PC. Or la carte est un autre appareil, sur le Wi-Fi de l'iPhone. À l'étape 2, on ouvre la porte sur le Wi-Fi (port Docker, pare-feu Windows), et la carte vérifie qu'elle l'atteint.
