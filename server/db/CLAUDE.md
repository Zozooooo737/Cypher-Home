# Couche base de données (PyMongo)

Complète `CLAUDE.md` (racine) et `server/CLAUDE.md`. MongoDB est lancé par Docker (voir `docker/`) ; ici se trouve seulement le code Python qui l'utilise.

- Une seule couche d'accès : le serveur et le dashboard passent par ce module, jamais par des requêtes MongoDB dispersées dans le code.
- **Le schéma n'est pas défini.** Avant d'écrire du code, propose la structure des collections (mesures, événements, configuration, notifications, images ou références d'images, utilisateur) avec un exemple de document par collection, puis attends la validation. Une fois validé, l'étudiant le consigne dans `DECISIONS.md`, section « Décidé » (étape P5.1).
- Chaque document de mesure ou d'événement porte l'heure de mesure et l'heure de réception.
- Conservation : suppression des données au-delà de la durée choisie (30 jours par défaut, réglable). Envisage un index TTL MongoDB.
- Images : choisis et justifie entre stockage dans la base et fichiers sur disque référencés par la base (limite de taille des documents MongoDB : 16 Mo).
- Connexion via variables d'environnement (`.env` non versionné). Pas de mot de passe en dur.
- Fournis un petit script de remplissage avec de fausses données pour tester serveur et dashboard.

## Schéma validé
Le modèle de données validé (entités et attributs) est dans `DECISIONS.md`, section « Décidé ». Ne l'implémente que s'il y figure.
