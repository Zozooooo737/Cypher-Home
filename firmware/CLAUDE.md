# Firmware (C++ / PlatformIO / Arduino UNO R4 WiFi)

Complète le `CLAUDE.md` racine.

## Commandes PlatformIO (depuis la racine du dépôt)
- Compiler : `pio run -d firmware` (autorisé sans demander).
- Téléverser : `pio run -d firmware -t upload`, uniquement quand l'étudiant dit que le câblage est vérifié et que la carte est branchée.
- Moniteur série : lancé par l'étudiant (interactif). Claude Code lui demande le résultat.
- Ne jamais déclarer qu'un test physique (LED, buzzer, écran, caméra) a réussi sans retour de l'étudiant.

## Informations supplémentaires

- Plateforme PlatformIO pour l'UNO R4 WiFi (Renesas RA4M1) ; Wi-Fi via le module ESP32-S3 de la carte. Vérifie la compatibilité de chaque bibliothèque avant de l'ajouter à `platformio.ini`.
- Développement composant par composant : un petit programme de test par composant avant toute intégration. Pas de câblage ni de broches supposés : utilise uniquement les affectations de `DECISIONS.md`, sinon propose-les.
- Les noms de broches des modules sont ceux de `CONTEXTE_TECHNIQUE.md`.
- Points de vigilance matériel à rappeler quand c'est pertinent : la carte fonctionne en logique 5 V alors que l'écran TFT et certains modules sont en 3,3 V ; le BME680 et la caméra utilisent le SPI (un CS par périphérique) ; la caméra a besoin de beaucoup de mémoire et d'un débit SPI suffisant : priorité à des photos exploitables.
- Secrets (SSID, mot de passe Wi-Fi, identifiants MQTT) dans un fichier `secrets.h` non versionné, avec un `secrets.example.h` versionné.
- Code non bloquant (pas de `delay` long dans la boucle principale), structure en modules (un fichier par composant).
- Cadences visées par le CC : mesure toutes les 5 s, enregistrement serveur toutes les 30 s, événements immédiats.
- Termine toujours par une liste « À vérifier sur la carte ».
