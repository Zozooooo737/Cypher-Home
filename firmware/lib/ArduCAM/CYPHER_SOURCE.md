# Bibliothèque ArduCAM : copie locale (Cypher Home)

Copie versionnée pour une compilation reproductible. Choix provisoire : la décision D2 (`DECISIONS.md`) reste ouverte jusqu'aux tests sur le matériel.

| | |
|---|---|
| Source | fork communautaire https://github.com/keeeal/ArduCAM-Arduino-Uno-R4 (dossier `ArduCAM/`) |
| Commit | `8b276d39420b5d57f31b2f9a290a2fc9586e527f` (2024-09-28), version annoncée V4.1.3 |
| Base | original https://github.com/ArduCAM/Arduino, commit `066a7ea` (2022-07-13), dernière version de l'original à la date de la copie |
| Apport du fork | un seul bloc dans `ArduCAM.h` : macros d'accès à la broche CS pour `ARDUINO_UNOR4_MINIMA` / `ARDUINO_UNOR4_WIFI` (registre PODR 16 bits) |
| Licence | LGPL 2.1 (en-têtes des fichiers et `LICENSE.txt`). Le dépôt d'origine porte aussi un fichier MIT à sa racine, conservé ici sous `LICENSE_REPO_MIT.txt`. |
| Copie | 2026-11-05 |

## Contenu
- Copiés tels quels, **identiques à l'amont** (commit `8b276d3`, vérifié fichier par fichier le 2026-11-05) : `ArduCAM.cpp`, `ArduCAM.h`, `memorysaver.h`, fichiers `*_regs.h`, `library.json`, `keywords.txt`, `LICENSE.txt`.
- Ajoutés : ce fichier et `LICENSE_REPO_MIT.txt` (licence de la racine du dépôt d'origine).
- Non copiés : dossier `examples/` et dossiers voisins du dépôt (`ArduCAM_Touch`, `UTFT4ArduCAM_SPI`, `OV7670FIFO`), inutiles ici.
- `memorysaver.h` : seule la plateforme `OV5642_MINI_5MP_PLUS` est active (réglage d'origine du fork). Ne pas ajouter `-D OV5642_MINI_5MP_PLUS` dans `platformio.ini` (double définition).

## Correctifs
Aucun. Un correctif éventuel serait marqué `CYPHER-PATCH: <raison>` dans le fichier modifié et listé ici.

Point connu, non corrigé : dans `wrSensorRegs16_8()`, `reg_addr` et `reg_val` sont lus avant d'être affectés au premier tour de boucle (avertissement GCC `-Wmaybe-uninitialized`, visible seulement sans `-w`). Aucun défaut observé : la caméra fonctionne sur la carte, et un projet de référence de l'étudiant fonctionne avec cette bibliothèque non modifiée. Un correctif ajouté le 2026-11-05 a été retiré le même jour pour garder la copie identique à l'amont.
