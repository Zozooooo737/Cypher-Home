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
- Corps du message optionnel (ligne vide puis explication), seulement si la raison du changement n'est pas évidente.

## Exemples

- `INIT (firmware) : Initialisation du projet et configuration de PlatformIO`
- `TEST (firmware) : Ajout du test du capteur BME680`
- `FEAT (firmware) : Implémentation de la lecture des données du MQ-2`
- `FIX (server) : Correction de l'horodatage des mesures reçues`
- `REFACTOR (firmware) : Réorganisation du module sensors`
- `DOCS : Ajout de la documentation du câblage`
- `CHORE (server) : Ajout du fichier .env.example et mise à jour du .gitignore`
