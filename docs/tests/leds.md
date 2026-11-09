# Fiche de test : LED rouge, bleue, jaune, orange

## En-tête

| Composant                    | 4 LED 5 mm : rouge LED5RL, bleue LED5BT, jaune LED5YL, orange LED5OR |
| ---------------------------- | -------------------------------------------------------------------- |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.4 à 3.7 (commande : registre 74HC595, § 3.16)                    |
| Étape du plan                | P1.5                                                                 |
| Date du test                 | 2026-11-09                                                           |
| Datasheet                    |  https://learn.adafruit.com/all-about-leds/the-led-datasheet         |
| **Verdict**                  | **OK**                                                               |

## Alimentation

| Élément                   | Valeur                                                                  | Source        |
| ------------------------- | ----------------------------------------------------------------------- | ------------- |
| Tension d'alimentation    | Sorties QB à QE du 74HC595, actives à l'état haut                       | câblage testé |
| Niveau logique            | Celui du 74HC595 (voir `74hc595.md`) ; aucune adaptation sur les LED     | câblage testé |
| Courant                   | Non mesuré (par LED et total) : reporté à la fin du projet (P1.7)       | —             |
| Démarrage ou préchauffage | N/A                                                                     | —             |

## Câblage testé

| LED (rôle)                    | Sortie du 74HC595 | Bit de l'octet envoyé | Remarque            |
| ----------------------------- | ----------------- | --------------------- | ------------------- |
| Rouge (température)           | QB                | 1                     | Anode côté sortie   |
| Bleue (humidité)              | QC                | 2                     |                     |
| Jaune (qualité de l'air)      | QD                | 3                     |                     |
| Orange (mode sécurité)        | QE                | 4                     |                     |

- Bus et adressage : GPIO, par le registre à décalage 74HC595 (SER A1, RCLK A2, SRCLK A3 ; voir `74hc595.md`).
- Éléments ajoutés : une résistance de 270 Ω en série par LED, vers GND. Formule : I = (tension de sortie − tension directe de la LED) / 270 Ω ; tension directe et courant : à compléter.
- Cette table est le câblage **testé** le 2026-11-09. `pins.h` donne le câblage courant, et la table D3 validée fait foi.

## Logiciel

- Bibliothèque : aucune (`shiftOut()` du cœur Arduino Renesas 1.6.0).
- Ligne `lib_deps` : N/A.
- Compatibilité UNO R4 WiFi : oui.
- Fichier du dépôt : `firmware/src/components/leds.cpp`.
- Paramètres clés : correspondance LED → bit dans `pins.h` (`LEDS_RED_BIT` à `LEDS_ORANGE_BIT`). Le module garde une copie de l'octet du registre : changer une LED ne modifie pas les autres.
- Alternatives écartées : 4 broches de la carte, une par LED (le 74HC595 en utilise 3 pour 8 sorties).

## Résultat

- Intégrité : fonctionnel ; constat visuel : chaque LED s'allume seule dans l'ordre rouge, bleue, jaune, orange, puis les quatre ensemble, puis toutes éteintes.

## Pièges et limites

- Avec `shiftOut` en MSBFIRST, le dernier bit envoyé (bit 0) arrive sur QA : une LED sur QB correspond au bit 1.
- Pas de ligne `[leds] KO` : l'absence d'une LED ou du registre n'est pas détectable ; résultat à vérifier à l'œil.
- Non testé : courant par LED et courant total, luminosité.

## À reporter (si applicable)

- [x] Valeurs des résistances → `CONTEXTE_TECHNIQUE.md` § 3.17
- [ ] Ligne du composant → `docs/compatibilite.md` (P1.6)
