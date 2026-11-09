# Fiche de test : boutons poussoirs

## En-tête

| Composant                    | 5 boutons poussoirs à 4 pattes (référence commerciale : à compléter) |
| ---------------------------- | -------------------------------------------------------------------- |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.10                                                               |
| Étape du plan                | P1.5                                                                 |
| Date du test                 | 2026-11-09                                                           |
| Datasheet                    | à compléter                                                          |
| **Verdict**                  | **OK**                                                               |

## Alimentation

| Élément                   | Valeur                                                                 | Source        |
| ------------------------- | ---------------------------------------------------------------------- | ------------- |
| Tension d'alimentation    | Aucune : contact entre la broche de la carte et GND                    | câblage testé |
| Niveau logique            | Résistance de tirage interne de la carte (`INPUT_PULLUP`) : relâché = HIGH, appuyé = LOW | câblage testé |
| Courant                   | Non mesuré                                                             | —             |
| Démarrage ou préchauffage | N/A                                                                    | —             |

## Câblage testé

| Bouton (rôle)       | Patte A1 | Patte B2 |
| ------------------- | -------- | -------- |
| Centre (validation) | D2       | GND      |
| Droite              | D3       | GND      |
| Gauche              | D4       | GND      |
| Haut                | D5       | GND      |
| Bas                 | D6       | GND      |

- Bus et adressage : GPIO en entrée.
- Éléments ajoutés : aucun (tirage interne de la carte).
- Noms de pattes fixés par le test de continuité du 2026-11-09 : A1 et B2. Noms des deux autres pattes et paires reliées en interne : à compléter.
- Cette table est le câblage **testé** le 2026-11-09. `pins.h` donne le câblage courant, et la table D3 validée fait foi.

## Logiciel

- Bibliothèque : aucune (cœur Arduino Renesas 1.6.0).
- Ligne `lib_deps` : N/A.
- Compatibilité UNO R4 WiFi : oui.
- Fichier du dépôt : `firmware/src/components/buttons.cpp`.
- Paramètres clés : `BUTTONS_DEBOUNCE_MS` = 20 (`config.h`) : une lecture doit rester stable 20 ms pour changer l'état d'un bouton. `buttons_update()` est à appeler souvent.
- Alternatives écartées : à compléter.

## Résultat

- Intégrité : fonctionnel ; les 5 boutons sont lus correctement.

## Pièges et limites

- `[buttons] KO` au départ du test = un bouton lu appuyé sans appui : vérifier que ce sont bien les pattes A1 et B2 qui sont câblées.
- Un bouton débranché se lit comme relâché : pas détectable.

## À reporter (si applicable)

- [x] Noms de pattes → `CONTEXTE_TECHNIQUE.md` § 3.10
- [ ] Ligne du composant → `docs/compatibilite.md` (P1.6)
