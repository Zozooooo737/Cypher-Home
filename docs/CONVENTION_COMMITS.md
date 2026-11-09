# Convention de nommage des commits Git

Quand je décris des modifications que je viens d'effectuer, propose directement le message de commit, prêt à copier dans Git. Respecte systématiquement cette convention.

## Format

`TYPE (scope) : Description courte et explicite`

Le scope est optionnel : `firmware`, `server`, `dashboard`, `docker`, `docs`, `hardware`, `cad`.
À utiliser seulement si le dépôt contient plusieurs briques. Exemple : `FEAT (server) : ...`

## Types

| Préfixe    | Utilisation                                                            |
|------------|------------------------------------------------------------------------|
| `INIT`     | Initialisation du projet ou configuration initiale                     |
| `FEAT`     | Ajout d'une nouvelle fonctionnalité                                    |
| `FIX`      | Correction d'un bug ou d'un problème                                   |
| `TEST`     | Ajout ou modification de tests                                         |
| `REFACTOR` | Réorganisation du code sans changer son comportement                   |
| `DOCS`     | Ajout ou modification de documentation                                 |
| `CHORE`    | Maintenance : dépendances, .gitignore, fichiers de config, nettoyage   |

## Règles de rédaction

- Commencer par l'un des préfixes, en majuscules, suivi de ` : `.
- Rédiger en français, à l'infinitif ou au nom d'action, de façon cohérente (ex. « Ajout de… », « Correction de… »).
- Description concrète, 72 caractères maximum, sans point final.
- Un commit = une version fonctionnelle.
- Choisir le préfixe correspondant à la nature réelle des changements.
- Pas de numéro de version dans le message.
- Ne jamais committer de secrets (`secrets.h`, `.env`) : signale-le avant de proposer le commit si un tel fichier est modifié.
- Corps du message obligatoire : une ligne vide après le titre, puis un paragraphe de 4 lignes maximum (72 caractères par ligne environ). Il énumère en phrases ce qui a été ajouté, modifié ou retiré (« Ajout de X, Y et Z ; mise à jour de... »), sans puces ni tirets, sans détail ligne à ligne.

## Livraison des commits par Claude Code

Claude Code ne commite jamais ; en fin de tâche, il fournit :
- un découpage en plusieurs commits thématiques (un sujet par commit : code, correctif, fiches, fichiers de référence), numérotés dans l'ordre où les faire ;
- pour chaque commit, les commandes `git add` exactes dans un bloc de code ; si un fichier mélange deux sujets, `git add -p <fichier>` avec les réponses à taper (`s` pour scinder, `y` / `n` par bloc) ;
- le message (titre, ligne vide, paragraphe) dans un bloc de code séparé, prêt à coller ;
- en fin de liste, un signalement si un fichier de secrets est modifié.

Exemple de message :

```text
FEAT (firmware) : Ajout des modules LED (74HC595), boutons et buzzer

Ajout des modules leds (74HC595 par shiftOut), buttons (INPUT_PULLUP,
anti-rebond de 20 ms) et buzzer (tone() non bloquant sur D0), chacun avec
sa fonction de test. Mise à jour de pins.h et config.h, appel des setup
dans main.cpp et des trois tests dans self_test_run().
```

## Exemples

- `INIT (firmware) : Initialisation du projet et configuration de PlatformIO`
- `TEST (firmware) : Ajout du test du capteur BME680`
- `FEAT (firmware) : Implémentation de la lecture des données du MQ-2`
- `FIX (server) : Correction de l'horodatage des mesures reçues`
- `REFACTOR (firmware) : Réorganisation du module sensors`
- `DOCS : Ajout de la documentation du câblage`
- `CHORE (server) : Ajout du fichier .env.example et mise à jour du .gitignore`
