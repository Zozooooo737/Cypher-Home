# Fiche de test : buzzer passif SVM06N

## En-tête

| Composant                    | Module buzzer passif SVM06N                                          |
| ---------------------------- | -------------------------------------------------------------------- |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.9                                                                |
| Étape du plan                | P1.5                                                                 |
| Date du test                 | 2026-11-09                                                           |
| Datasheet                    | https://www.handsontec.com/dataspecs/module/passive%20buzzer.pdf     |
| **Verdict**                  | **OK**                                                               |

## Alimentation

| Élément                   | Valeur                                         | Source        |
| ------------------------- | ---------------------------------------------- | ------------- |
| Tension d'alimentation    | V+ : à compléter (`pins.h` : 5V, à confirmer)  | —             |
| Niveau logique            | Signal commandé en 5 V par la carte (D0)       | câblage testé |
| Démarrage ou préchauffage | N/A                                            | —             |

## Câblage testé

| Broche du composant (nom officiel) | Broche de la carte | Remarque                                    |
| ---------------------------------- | ------------------ | ------------------------------------------- |
| Signal                             | D0                 | Aussi le RX de `Serial1`, jamais initialisé |
| V+                                 | 5V                 |                                             |
| GND                                | GND                |                                             |

- Bus et adressage : GPIO, signal carré produit par `tone()`.
- Éléments ajoutés : aucun.
- Cette table est le câblage **testé** le 2026-11-09. `pins.h` donne le câblage courant, et la table D3 validée fait foi.

## Logiciel

- Bibliothèque : aucune (`tone()` du cœur Arduino Renesas 1.6.0).
- Ligne `lib_deps` : N/A.
- Compatibilité UNO R4 WiFi : oui. Dans le cœur 1.6.0, `tone()` ne bloque pas : une minuterie matérielle libre bascule la broche par interruption et arrête le son à la fin de la durée. Toute broche numérique convient.
- Fichier du dépôt : `firmware/src/components/buzzer.cpp`.
- Paramètres clés : `buzzer_play(freq_hz, duration_ms)` ; un nouveau son remplace le précédent.
- Alternatives écartées : à compléter.

## Résultat

- Intégrité : fonctionnel ; constat à l'oreille : deux sons distincts, 1000 Hz puis 2000 Hz.
- Sortie type de `buzzer_test()` : à compléter.

## Pièges et limites

- D0 est le RX de `Serial1` : ne jamais appeler `Serial1.begin()`. `Serial` (USB) passe par d'autres broches.
- Pas de ligne `[buzzer] KO` : l'absence du buzzer n'est pas détectable ; résultat à vérifier à l'oreille.
- Non testé : courant, bruit éventuel du buzzer pendant un téléversement.

## À reporter (si applicable)

- [ ] Ligne du composant → `docs/compatibilite.md` (P1.6)
