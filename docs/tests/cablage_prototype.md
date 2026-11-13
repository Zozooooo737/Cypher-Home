# Fiche de test : câblage du prototype (P3.3)

Complète, sans les répéter : l'affectation broche par broche (table D3 de `DECISIONS.md`), le schéma (`hardware/kicad/cypher-home.pdf`) et l'essai d'ensemble sur breadboard (`essai_ensemble.md`). Ici : alimentation, raccordements fixes, contrôles avant mise sous tension, écarts avec P1.7.

| | |
|---|---|
| Date | 2026-11-13 |
| **Verdict** | **OK avec réserve** : courant total non mesuré ; signaux 5 V directs sur l'écran |
| Photo | `docs/tests/cablage_prototype.jpg` |

## Mode d'alimentation

- Source : USB-C du PC (mode principal, N6). Pas d'interrupteur en mode USB-C (D10) : le prototype s'éteint en débranchant le câble.
- Rail 5V de la carte : caméra (VCC), BME680 (VIN), MQ-2 (VCC), buzzer (V+), 74HC595 (VCC, SRCLR).
- Rail 3.3V de la carte : écran (VCC, BLK). Régulateur partagé avec le module Wi-Fi ESP32-S3 (conflit C10 de `docs/compatibilite.md`).
- Budget d'après les datasheets, **pas une mesure** : caméra 300 mA, MQ-2 environ 150 mA, 4 LED 74 mA au plus, soit plus de 450 mA sans la carte ; un port USB 2.0 de PC fournit normalement 500 mA : marge faible (C11).
- Non câblés, hors schéma : DS12A, convertisseur Step-Up, LiPo, SunFlower (mode batterie : P11, D6, D10).

## Raccordements fixes (sans signal du firmware)

| Composant | Broche | Reliée à | Rôle |
|---|---|---|---|
| 74HC595 | OE | GND | sorties toujours actives |
| 74HC595 | SRCLR | 5V | registre jamais remis à zéro |
| 74HC595 | QA, QF, QG, QH, QH' | non connectées | |
| LED ×4 | QB à QE → 270 Ω → anode ; cathode | GND | une résistance par LED |
| BME680 | SDO | GND | adresse I2C 0x76 |
| BME680 | 3Vo, CS | non connectées | |
| MQ-2 | D0 | non connectée | seule A0 est lue |
| Boutons ×5 | B2 | GND | A2 et B1 non connectées |
| Écran | BLK | 3.3V | rétroéclairage toujours allumé |

## Contrôles avant mise sous tension

Déclarés par l'étudiant le 2026-11-13.

| Contrôle | Résultat |
|---|---|
| Continuité de toutes les liaisons | bonne |
| Court-circuit | aucun |
| Polarité (LED, buzzer, alimentation des modules) | correcte |
| 74HC595 : VCC et SRCLR sur 5V, OE à GND | conforme |
| `firmware/include/pins.h` comparé au câblage réel | conforme |
| `self_test_run()` complet, tout branché (procédure : `docs/guide_self_test.md`) | réussi |

## Écarts avec l'essai d'ensemble (P1.7)

- Affectation des broches : identique (la table D3 a été validée d'après le câblage de P1.7).
- VCC du 74HC595 sur 5V : confirmé (« à confirmer » en P1.7).
- Câblage réalisé d'après le schéma KiCad, qui n'existait pas en P1.7.
- Wi-Fi actif : `self_test_run()` comprend désormais les tests Wi-Fi, MQTT, HTTP et photo HTTP (Wi-Fi inactif pendant P1.7).

## Limites

- Courant total non mesuré : reporté à la fin du projet.
- Signaux 5 V directs sur l'écran : voir la remarque de D3 (`DECISIONS.md`).
- Sortie série du `self_test_run()` complet : à compléter.
