# Serveur (Python)

Sous-dossiers : `db/` (accès MongoDB) et `dashboard/` (interface web), chacun avec son `CLAUDE.md`.

Complète le `CLAUDE.md` racine.

- Environnement `.venv` + `requirements.txt` à jour. Python uniquement.
- Composants : réception des mesures (MQTT, repli HTTP), réception des photos (HTTP), base MongoDB (PyMongo), API pour le dashboard, dashboard web (NiceGUI ou Streamlit, non tranché).
- Le serveur écoute sur `0.0.0.0`. Dashboard accessible depuis PC et iPhone.
- Données horodatées : distinguer l'heure de mesure et l'heure de réception. Structure des collections : à proposer et faire valider avant d'implémenter (mesures, événements, configuration, images ou références d'images).
- Authentification simple : un seul compte administrateur (identifiant + mot de passe haché). Code PIN distinct du mot de passe pour neutraliser une alerte. Validation de toutes les entrées.
- Intrusion : détection → délai de 10 s pour saisir le PIN → sinon événement d'intrusion + image enregistrée + notification dans le dashboard (courriel en option, bonus).
- Secrets (MQTT, base, SMTP) dans un `.env` non versionné, avec un `.env.example` versionné.
- Tests : fournis des scripts simples qui simulent la carte (publication de fausses mesures) pour tester le serveur sans matériel. Ne dis pas « testé » pour ce qui n'a pas été exécuté.
- Priorité de développement : 1) réception + stockage des mesures, 2) dashboard de lecture, 3) alertes et surveillance, 4) photos, 5) commandes vers la carte, 6) options.
