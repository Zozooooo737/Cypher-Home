# Docker : MongoDB et Mosquitto

Complète `CLAUDE.md` (racine). Docker est un choix d'outillage, pas une exigence du cahier des charges. Repli possible : installation native sous Windows.

- `docker-compose.yml` : un service MongoDB et un service Mosquitto (broker MQTT), avec volumes pour conserver les données.
- Explique simplement Docker, les ports publiés et les volumes : l'étudiant débute.
- Mosquitto : authentification par identifiant et mot de passe (`allow_anonymous false`, `password_file`). Accès anonyme toléré seulement pendant la mise en place de P2.2 (étapes 1 à 3), broker arrêté hors des tests. Le broker ne garde que des empreintes, dans `docker/mosquitto/config/passwd` (créé avec `mosquitto_passwd`, ignoré par Git). Les mots de passe des clients sont hors Git : `firmware/include/secrets.h` pour la carte, `server/.env` (modèle `.env.example`) pour le serveur.
- MongoDB : utilisateur et mot de passe via variables d'environnement. Les ports ne sont exposés qu'au nécessaire ; rappelle le pare-feu Windows et le profil réseau privé pour l'accès depuis la carte.
- Si Docker pose problème pendant plus d'une demi-journée, propose l'installation native.
