# Fiche de test : capteur de gaz et fumée MQ-2

## En-tête

| Composant                    | Module capteur de gaz MQ-2 (sorties A0 et D0)                |
| ---------------------------- | ------------------------------------------------------------ |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3.8                                                        |
| Étape du plan                | P1.3                                                         |
| Date du test                 | 2026-11-08                                                   |
| Datasheet                    | https://www.handsontec.com/dataspecs/MQ2-Gas%20Sensor.pdf    |
| **Verdict**                  | **OK**                                                       |

## Alimentation

| Élément                   | Valeur                                                                                         | Source                     |
| ------------------------- | ---------------------------------------------------------------------------------------------- | -------------------------- |
| Tension d'alimentation    | VCC sur la broche 5V (alimente aussi le chauffage du capteur)                                  | câblage testé              |
| Niveau logique            | Sortie analogique A0, au plus la tension d'alimentation : dans la plage de l'ADC (référence = alimentation 5 V de la carte) ; aucune adaptation | mesure : 0,41 à 2,83 V     |
| Courant                   | Non mesuré : mesure reportée à la fin du projet (courant total, P1.7)                         | —                          |
| Démarrage ou préchauffage | 300 s retenus (A0 baisse fortement pendant les premières minutes, stable après 20 min environ) | mesure, démarrage à froid  |

## Câblage testé

| Broche du composant (nom officiel) | Broche de la carte | Remarque |
|---|---|---|
| VCC | 5V | |
| GND | GND | |
| A0 | A0 | Sortie analogique, lue par l'ADC |
| D0 | non connectée | Sortie tout-ou-rien, seuil réglé par le potentiomètre du module |

- Bus et adressage : entrée analogique (A0 est aussi la sortie DAC de la carte, non utilisée).
- Éléments ajoutés : aucun.
- Cette table est le câblage **testé** le 2026-11-08. `pins.h` donne le câblage courant, et la table D3 validée fait foi.

## Logiciel

- Bibliothèque : aucune (`analogRead()` du cœur Arduino Renesas 1.6.0).
- Ligne `lib_deps` : N/A.
- Compatibilité UNO R4 WiFi : oui. L'ADC de la carte est toujours en 14 bits ; `analogReadResolution(14)` évite la remise à l'échelle en 10 bits faite par défaut.
- Fichier du dépôt : `firmware/src/components/mq2.cpp`.
- Paramètres clés : moyenne de 16 lectures par échantillon ; `MQ2_PREHEAT_S` = 300, `MQ2_ALERT_THRESHOLD` = 3000, `MQ2_ALERT_END_THRESHOLD` = 2500, `MQ2_ALERT_CONFIRM_SAMPLES` = 2 (`config.h`).
- Formules utiles : tension de A0 = valeur brute × tension d'alimentation mesurée par la carte / 16384 (environ 4,67 V déduits du log : 9926 pour 2,83 V).
- Alternatives écartées :
  - Seuil relatif à une référence glissante d'air normal : essayé ; référence en retard au passage en prêt (2001 pour A0 = 1600) et alerte restée active après le gaz.
  - État prêt sur la dérive de A0 : plus complexe qu'une durée fixe, pour un résultat proche (déclenché à 424 s).
  - Sortie D0 : seuil réglé à la main par le potentiomètre, pas de valeur lisible.

## Résultat

- Intégrité : fonctionnel ; A0 lu à chaque passe, réaction nette au gaz et à la fumée.
- Sortie type de `mq2_test()` (briquet, deuxième échantillon au-dessus du seuil) :

```
[mq2] depuis demarrage ......... 414 s
[mq2] A0 brut .................. 6166 / 16383
[mq2] A0 tension ............... 1.76 V
[mq2] etat ..................... PRET
[mq2] alerte ................... OUI
```

- Mesures (deux enregistrements du 2026-11-08, échelle 0 à 16383) :

| Situation | A0 brut | Remarque |
|---|---|---|
| Démarrage à froid, 3 s → 5 min → 20 min et plus | 2824 → 1651 → 1420 environ | Baisse continue, puis stable |
| Air normal stable (20 à 30 min) | 1295 à 1599, la plupart vers 1400 à 1440 | Creux isolés de −130 sur un échantillon |
| Allumette éteinte, 10 cm à côté | aucune variation nette | La fumée n'atteint pas le capteur |
| Allumette éteinte, 10 cm en dessous | 1409 → 6394 → pic 8326 | Sous 2500 en 50 s |
| Briquet ouvert 10 s, 10 cm en dessous | pic 9538 | Sous 2500 en 65 s |
| Briquet 5 s, code final | 1512 → 9926 | Alerte au 2e échantillon (14 s après le gaz), fin après 45 s environ |

- Code final (second enregistrement) : passage en prêt à 306 s, aucune alerte avant ni en air normal (1475 à 1544).

## Pièges et limites

- Un reset ou un téléversement remet `depuis demarrage` à zéro sans couper le chauffage : un démarrage à froid demande de débrancher l'USB.
- Après le gaz, A0 reste vers 2000 à 2100 pendant plusieurs minutes : une fin d'alerte trop basse la garderait active.
- `Tee-Object` (PowerShell 5.1) enregistre le log en UTF-16.
- Pas de ligne `[mq2] KO` : une entrée analogique ne permet pas de détecter un capteur débranché (à traiter en P4.1).
- Second enregistrement : capteur débranché environ 13 min seulement avant le démarrage [H : pas totalement froid].
- Non testé :
  - courant consommé (reporté en P1.7) ;
  - absence de fausse alerte sur une longue durée (plus de 30 min) et selon l'humidité ou la température ;
  - allumette avec le code final ;
  - fumée dans le boîtier final (ventilation, P9.2).

## À reporter (si applicable)

- [x] Décision (préchauffage et seuil) → `DECISIONS.md`
- [ ] Ligne du composant → `docs/compatibilite.md` (P1.6)