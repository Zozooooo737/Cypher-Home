# Docker : MongoDB et Mosquitto

Complète `CLAUDE.md` (racine). Docker est un choix d'outillage, pas une exigence du cahier des charges. Repli possible : installation native sous Windows.

- `docker-compose.yml` : un service MongoDB et un service Mosquitto (broker MQTT), avec volumes pour conserver les données.
- Explique simplement Docker, les ports publiés et les volumes : l'étudiant débute.
- Mosquitto : authentification par identifiant et mot de passe dès le départ (pas d'accès anonyme). Identifiants dans un `.env` non versionné, avec un `.env.example`.
- MongoDB : utilisateur et mot de passe via variables d'environnement. Les ports ne sont exposés qu'au nécessaire ; rappelle le pare-feu Windows et le profil réseau privé pour l'accès depuis la carte.
- Si Docker pose problème pendant plus d'une demi-journée, propose l'installation native.
