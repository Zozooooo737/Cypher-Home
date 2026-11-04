# Tableau de bord web (Python)

Complète `CLAUDE.md` (racine) et `server/CLAUDE.md`. Même `.venv` et même `requirements.txt` que le serveur.

- Framework : NiceGUI ou Streamlit (non tranché, voir `DECISIONS.md`). Tant que ce n'est pas décidé, propose une comparaison courte puis attends la validation ; ne mélange pas les deux.
- Le dashboard lit les données via la couche `server/db/` et envoie les commandes via le serveur. Il n'accède jamais directement à MongoDB ni à la carte.
- Accessible depuis PC et iPhone (`http://IP_DU_PC:port`, écoute sur `0.0.0.0`). Interface lisible sur petit écran, utilisée d'abord sur iPhone en démonstration.
- Écrans prioritaires : 1) connexion (identifiant + mot de passe), 2) mesures courantes avec état (conforme / hors tolérance / critique), 3) graphiques d'évolution, 4) état du système (connexion, dernière communication, surveillance), 5) alertes et historique des événements, 6) activation/désactivation de la surveillance et fenêtre de saisie du code PIN, 7) images d'intrusion, 8) réglages (consignes, tolérances, seuils, mise en veille de l'écran, mode « respect de l'environnement », durée de conservation).
- Afficher clairement l'heure de mesure, l'heure de réception et la fraîcheur des données.
- Valider toutes les saisies (consignes, seuils, PIN). Aucun secret dans le code.
- Fournis un mode démo alimenté par de fausses données pour développer l'interface sans carte. Ne dis pas « testé » pour ce qui n'a pas été vu dans un navigateur.
