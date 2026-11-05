# Fiche de test : [nom du composant]

> Modèle P0.3. Copier ce fichier vers `docs/tests/<composant>.md`. Le nom du fichier est en anglais et identique à celui du module (`camera`, `bme680`, `mq2`, `display`, `buttons`, `leds`, `buzzer`, `ds12a`). Les 4 LED partagent une fiche.
> À rédiger **une fois le composant validé sur le matériel**. Une page maximum, **sans code** : renvoyer vers le fichier du dépôt. Section sans objet : écrire « N/A ».
> Les pièges se notent au fil du test dans un brouillon, pas de mémoire après coup.
> Supprimer cette note et les consignes entre crochets avant de committer.

## En-tête

| | |
|---|---|
| Composant | [nom et référence commerciale] |
| Réf. `CONTEXTE_TECHNIQUE.md` | [§ 3.x] |
| Étape du plan | [P1.x] |
| Date du test | [AAAA-MM-JJ] |
| Datasheet | [lien] |
| **Verdict** | **[OK / OK avec réserve / KO / non testé]** |

[Si réserve ou KO : une phrase sur la nature du problème.]

## Alimentation

| Élément | Valeur | Source |
|---|---|---|
| Tension d'alimentation | [V, broche de la carte utilisée] | [datasheet / mesure] |
| Niveau logique | [3,3 V / 5 V] | [datasheet / mesure] |
| Courant | [mA] | [datasheet / mesuré au multimètre] |
| Démarrage ou préchauffage | [s, ou N/A] | [datasheet / mesure] |

[La carte est en 5 V : préciser si une adaptation de niveau est nécessaire.]

## Câblage testé

| Broche du composant (nom officiel) | Broche de la carte | Remarque |
|---|---|---|
| [ex. CS] | [ex. D10] | [ex. broche libre, choisie] |

- Bus et adressage : [SPI (broche CS) / I2C (adresse) / GPIO / analogique]
- Éléments ajoutés : [résistances : valeur et formule de calcul ; adaptation de niveau ; condensateur]
- Cette table est le câblage **testé** à la date ci-dessus. `pins.h` donne le câblage courant, et la table D3 validée fait foi.

## Logiciel

- Bibliothèque : [nom, version]
- Ligne `lib_deps` : `[ligne]`
- Compatibilité UNO R4 WiFi : [oui / non / avec réserve : laquelle]
- Fichier du dépôt : `firmware/src/components/[composant].cpp`
- Paramètres clés : [adresse, fréquence du bus, résolution, mode...] (valeurs par défaut utiles au produit : voir `config.h`)
- Formules utiles : [conversion de la valeur brute, calcul d'un seuil...]
- Alternatives écartées : [une ligne par alternative, avec la raison]

## Résultat

- Intégrité : [fonctionnel / défectueux, et comment c'est constaté : continuité, lecture, visuel]
- Sortie type de `[composant]_test()` (3 à 5 lignes) :

```
[coller ici]
```

- Photo ou mesure : [chemin dans `docs/tests/`, ou N/A. Pas de photo montrant l'intérieur du logement.]

## Pièges et limites

- [Problème rencontré → solution]
- Non testé : [ce qui reste à vérifier]

## À reporter (si applicable)

- [ ] Noms de pattes → `CONTEXTE_TECHNIQUE.md` (boutons, DS12A)
- [ ] Valeurs des résistances → `CONTEXTE_TECHNIQUE.md` § 3.16
- [ ] Décision D1 ou D2 → `DECISIONS.md`
- [ ] Ligne du composant → `docs/compatibilite.md` (P1.6)
- [ ] Éléments pour le compte rendu (P12.1) : [photo, mesure, schéma]
