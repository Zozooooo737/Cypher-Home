# Fiche de test : écran TFT ST7735S

## En-tête

| Composant                    | Écran TFT ST7735S 1,8" 128x160, 3,3 V (DollaTek)              |
| ---------------------------- | ------------------------------------------------------------- |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.12                                                        |
| Étape du plan                | P1.4                                                          |
| Date du test                 | 2026-11-09 (décalage constaté le 2026-11-08)                  |
| Datasheet                    | https://www.buydisplay.com/download/ic/ST7735S.pdf            |
| **Verdict**                  | **OK**                                                        |

## Alimentation

| Élément                   | Valeur                                                                 | Source        |
| ------------------------- | ---------------------------------------------------------------------- | ------------- |
| Tension d'alimentation    | VCC sur la broche 3.3V de la carte                                     | câblage testé |
| Niveau logique            | 3,3 V : adaptation nécessaire, la carte sort du 5 V (diviseurs, voir câblage) | module 3,3 V  |
| Courant                   | Non mesuré : mesure reportée à la fin du projet (courant total, P1.7)  | —             |
| Démarrage ou préchauffage | N/A                                                                    | —             |

Aucun composant chaud pendant les essais (vérifié au toucher).

## Câblage testé

| Broche du composant (nom officiel) | Broche de la carte | Remarque |
|---|---|---|
| GND | GND | |
| VCC | 3.3V | |
| SCL | D13 | Bus SPI (SCK), partagé avec la caméra ; via diviseur |
| SDA | D11 | Bus SPI (MOSI), partagé avec la caméra ; via diviseur |
| RES | D8 | Via diviseur |
| DC | D9 | Via diviseur |
| CS | D7 | Via diviseur |
| BLK | 3.3V | Rétroéclairage toujours allumé |

- Bus et adressage : SPI mode 0, CS sur D7 ; MISO (D12) non utilisé par l'écran.
- Éléments ajoutés : un diviseur par signal (SCL, SDA, RES, DC, CS) : 1 kOhm côté carte, 2 kOhm vers GND ; 5 V × 2 / 3 = environ 3,3 V.
- Cette table est le câblage **testé** le 2026-11-09. `pins.h` donne le câblage courant, et la table D3 validée fait foi.

## Logiciel

- Bibliothèques : Adafruit ST7735 and ST7789 Library 1.12.0 (MIT), Adafruit GFX Library 1.13.0 (BSD).
- Lignes `lib_deps` : `adafruit/Adafruit ST7735 and ST7789 Library@1.12.0`, `adafruit/Adafruit GFX Library@1.13.0` (plus seesaw 1.7.9 et SD 1.3.0, déclarées par la bibliothèque, non utilisées).
- Compatibilité UNO R4 WiFi : oui (compilation et affichage sur la carte).
- Fichier du dépôt : `firmware/src/components/display.cpp`.
- Paramètres clés (`config.h`) : variante `INITR_BLACKTAB`, SPI à 4 MHz après l'initialisation, décalage 1 colonne et 1 ligne, rotation 3 (paysage 160x128).
- Alternatives écartées : aucune essayée.

## Résultat

- Intégrité : fonctionnel ; constaté à l'œil : paysage dans le bon sens, fond entièrement noir sans pixels parasites, texte complet, « Rouge », « Vert » et « Bleu » dans leur couleur.
- Sortie type de `display_test()` :

```
----- TEST DISPLAY -----
[display] resolution ........... 160x128
[display] affichage ............ 162 ms
```

- Photo ou mesure : à compléter.

## Pièges et limites

- Avec le décalage 0 et 0 de `INITR_BLACKTAB`, affichage décalé de 1 pixel à gauche et en haut, pixels multicolores sur la dernière colonne et la dernière ligne → décalage de 1 colonne et 1 ligne (`config.h`).
- `setColRowStart()` est protégée dans la bibliothèque : une classe interne à `display.cpp` la rend accessible.
- La bibliothèque envoie la séquence d'initialisation à sa vitesse par défaut (32 MHz demandés), sans réglage possible ; la fréquence de `config.h` ne s'applique qu'après.
- Les diviseurs tirent CS, DC et RES à LOW tant que les broches ne sont pas configurées : CS de l'écran mis à HIGH au début de `setup()`, sinon l'écran reçoit les échanges SPI de la caméra.
- Pas de ligne `[display] KO` : sans MISO, l'absence de l'écran n'est pas détectable.
- Non testé :
  - courant consommé, dont le rétroéclairage (reporté en P1.7) ;
  - fiabilité de l'initialisation sur de nombreux démarrages ;
  - rotations 0, 1 et 2 avec le décalage actuel (un bord peut redevenir bruité).

## À reporter (si applicable)

- [ ] Valeurs des résistances (diviseurs 1 kOhm / 2 kOhm) → `CONTEXTE_TECHNIQUE.md` § 3.16
- [x] Décision (bibliothèque, réglages) → `DECISIONS.md`
- [ ] Ligne du composant → `docs/compatibilite.md` (P1.6)
- [ ] Éléments pour le compte rendu (P12.1) : photo
