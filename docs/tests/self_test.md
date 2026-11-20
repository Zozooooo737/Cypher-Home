# Fiche de test : self-test au démarrage

## En-tête

| | |
|---|---|
| Composant | `self_test_run()` : enchaînement des tests de tous les composants au démarrage, avec récapitulatif OK/KO |
| Réf. `CONTEXTE_TECHNIQUE.md` | § 3 (tous les composants du prototype) |
| Étape du plan | P4.1 |
| Date du test | 2026-11-20 |
| Datasheet | N/A (voir les fiches de chaque composant) |
| **Verdict** | **OK** |

## Alimentation

N/A : carte sur USB (PC, ou chargeur USB pour l'essai sans PC), comme dans `cablage_prototype.md`.

## Câblage testé

Prototype complet de P3.3, conforme à la table D3 : voir `cablage_prototype.md`. Pour les essais « débranché », le composant est retiré carte hors tension (USB débranché).

## Logiciel

- Fichiers du dépôt : `firmware/src/self_test.cpp` (appelé une fois à la fin de `setup()` de `firmware/src/main.cpp`), modules de `firmware/src/components/`. Guide de lancement : `docs/guide_self_test.md`.
- Ordre : CAMERA, BME680, MQ2, WIFI, MQTT, PHOTO HTTP, DISPLAY, LEDS, BUZZER, BUTTONS, puis RECAPITULATIF (port série seulement : OK ou KO par composant, total, durée).
- Tests sautés : Wi-Fi KO → `[mqtt] KO (wifi)` et `[http] KO (wifi)` ; caméra KO → `[http] KO (camera)`.
- Toujours OK (absence non détectable) : MQ-2 (entrée analogique), écran (pas de MISO), LED et buzzer (sorties). Boutons : OK seulement si les 5 sont appuyés pendant les 7 s.
- Test caméra sans image sur le port série (contrôle FF D8 seulement) ; image complète contrôlée par le serveur dans PHOTO HTTP.
- Prérequis réseau : partage iPhone, Mosquitto (Docker), `server/test_upload_server.py`.

## Résultat

| Essai | Lignes KO | Total | Durée |
|---|---|---|---|
| Tout branché, 5 boutons appuyés | aucune | 10 OK, 0 KO | 21 s |
| Caméra débranchée | `[camera] KO`, `[http] KO (camera)` | 8 OK, 2 KO | 17 s |
| BME680 débranché | `[bme680] KO` | 9 OK, 1 KO | 19 s |
| Écran débranché | aucune (limite) | 10 OK, 0 KO | 21 s |
| MQ-2 débranché | aucune (limite) ; `A0 brut` 3531 | 10 OK, 0 KO | 24 s |
| Partage de connexion coupé | `[wifi] KO`, `[mqtt] KO (wifi)`, `[http] KO (wifi)` | 7 OK, 3 KO | 47 s |
| Bouton gauche jamais appuyé | `[buttons] KO` | 9 OK, 1 KO | 23 s |
| Sans PC (chargeur USB) | déroulement complet constaté par l'étudiant | — | — |

- Chaque passe avec réseau : photo reçue par le serveur (code 200) et message reçu par `mosquitto_sub`.
- Capture complète de l'essai « tout branché » : `self_test_capture.txt` (SSID masqué). Récapitulatif :

```
[recap] photo http ............. OK
[recap] boutons ................ OK
[recap] total .................. 10 OK, 0 KO
[recap] duree .................. 21 s
```

- Photo ou mesure : N/A

## Pièges et limites

- MQ-2 débranché : entrée flottante, `A0 brut` 3531, alors que le capteur branché donne 950 à 1207 juste après une remise sous tension et 2824 au démarrage à froid (`mq2.md`) : plages trop proches, pas de seuil de détection.
- Écran, LED, buzzer : leur absence ne se voit qu'à l'œil ou à l'oreille.
- Wi-Fi absent : environ 30 s d'attente (3 tentatives de 10 s, `config.h`) avant les tests sautés.
- RESET appuyé trop tôt (passe précédente interrompue) : une passe avec `[mqtt] KO` / code -2, isolée ; le serveur signale l'envoi coupé (`ConnectionResetError`) et continue.
- Non testé : broker arrêté et serveur HTTP arrêté pendant le self-test (codes vus en P2.2 et P2.3).

## À reporter (si applicable)

- [x] Self-test au démarrage, test HTTP, port série sans PC → `DECISIONS.md`
- [ ] Éléments pour le compte rendu (P12.1) : tableau des essais ci-dessus
