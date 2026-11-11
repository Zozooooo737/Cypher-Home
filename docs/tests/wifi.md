# Fiche de test : Wi-Fi de la carte

## En-tête

| | |
|---|---|
| Composant | Module Wi-Fi intégré de l'Arduino UNO R4 WiFi (ESP32-S3) |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.1 |
| Étape du plan | P2.1 |
| Date du test | 2026-11-10 |
| Datasheet | https://docs.arduino.cc/resources/datasheets/ABX00087-datasheet.pdf |
| **Verdict** | **OK** |

## Alimentation

| Élément | Valeur | Source |
|---|---|---|
| Tension d'alimentation | N/A (module intégré, alimenté par la carte) | N/A |
| Niveau logique | N/A (liaison interne à la carte) | N/A |
| Courant | non mesuré (reporté à la fin du projet, courant total) | N/A |
| Démarrage ou préchauffage | N/A | N/A |

## Câblage testé

N/A : aucune broche des connecteurs n'est utilisée.

- Bus et adressage : le RA4M1 parle à l'ESP32-S3 par `Serial2` (UART3, broches internes P501/P502).
- Éléments ajoutés : aucun.
- Réseau testé : partage de connexion de l'iPhone en 2,4 GHz, « Maximiser la compatibilité » activé ; IP de la carte 172.20.10.2 (attribuée par l'iPhone).

## Logiciel

- Bibliothèque : WiFiS3, fournie par le cœur Renesas 1.6.0.
- Ligne `lib_deps` : aucune (bibliothèque du cœur).
- Compatibilité UNO R4 WiFi : oui (bibliothèque officielle de la carte).
- Firmware du module Wi-Fi : 0.6.1, mis à jour depuis 0.4.1 (Arduino IDE 2, Firmware Updater).
- Fichier du dépôt : `firmware/src/components/wifi.cpp`.
- Paramètres clés : 3 tentatives de 10 s au plus (`WIFI_CONNECT_ATTEMPTS`, `WIFI_CONNECT_TIMEOUT_MS` dans `config.h`) ; connexion réussie seulement quand le DHCP a donné une IP ; SSID et mot de passe dans `secrets.h` (ignoré par Git, modèle `secrets.example.h`).
- Alternatives écartées : firmware Wi-Fi 0.4.1, avec lequel les pertes de liaison n'étaient pas toujours vues.

## Résultat

- Intégrité : fonctionnel ; la carte obtient une IP (172.20.10.2) en 1 tentative et se reconnecte seule après débranchement de l'USB (754 ms).
- Pertes et reprises de liaison (surveillance temporaire dans `loop()` pendant la mise au point, retirée depuis) : affichées avec le firmware 0.6.1, reprises en 182 ms et 3708 ms.
- Sortie type de `wifi_test()` (relevée le 2026-11-11 ; nom du réseau masqué) :

```
[wifi] firmware module ......... 0.6.1
[wifi] ssid .................... (nom du partage)
[wifi] connexion ............... 733 ms
[wifi] tentatives .............. 1
[wifi] adresse ip .............. 172.20.10.2
[wifi] rssi .................... -44 dBm
```

- Photo ou mesure : N/A

## Pièges et limites

- Firmware Wi-Fi 0.4.1 → pertes de liaison pas toujours vues ; mise à jour en 0.6.1.
- L'IP du PC change à chaque session de partage : à reporter dans `secrets.h` pour les étapes réseau (P2.2).
- Non testé : courant consommé ; durée d'un échec complet (réseau absent) ; autre réseau que le partage de l'iPhone.

## À reporter (si applicable)

- [x] Bibliothèque, firmware du module et réglage de la connexion → `DECISIONS.md` (Réseau et serveur)
- [ ] Éléments pour le compte rendu (P12.1) : à compléter
