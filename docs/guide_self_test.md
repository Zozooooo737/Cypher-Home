# Guide : lancer `self_test_run()`

But : les 10 tests et le récapitulatif réussis. La carte exécute `self_test_run()` une seule fois, à la mise sous tension ou après un appui sur RESET (environ 20 s [estimation]). Commandes PowerShell, lancées depuis la racine du dépôt sauf indication contraire.

## 1. Avant de commencer (à chaque session)

1. Brancher la carte au PC (USB-C).
2. iPhone : partage de connexion activé, « Maximiser la compatibilité » activé (2,4 GHz). Connecter le PC à ce partage.
3. Ouvrir Docker Desktop et attendre « Engine running ».
4. IP du PC :
   ```powershell
   Get-NetIPAddress -InterfaceAlias WiFi -AddressFamily IPv4
   ```
   Comparer `IPAddress` (`172.20.10.x`) à `MQTT_BROKER_IP` et `HTTP_SERVER_IP` dans `firmware/include/secrets.h`. Si elle diffère : corriger les deux lignes (le téléversement du terminal 4 recompile).
5. Profil réseau :
   ```powershell
   Get-NetConnectionProfile
   ```
   `NetworkCategory` doit valoir `Private`. Sinon, dans un PowerShell ouvert en administrateur :
   ```powershell
   Set-NetConnectionProfile -InterfaceAlias WiFi -NetworkCategory Private
   ```

## 2. Terminaux à ouvrir, dans cet ordre

**Terminal 1 : broker MQTT**
```powershell
cd docker
docker compose up -d
docker compose ps
```
Attendu : statut `Up`, `0.0.0.0:1883->1883/tcp`.

**Terminal 2 : abonné MQTT** (remplacer `<MQTT_PASSWORD>` par la valeur de `MQTT_PASSWORD` dans `server/.env`)
```powershell
cd docker
docker compose exec mosquitto mosquitto_sub -u cypherhome-server -P <MQTT_PASSWORD> -t cypherhome/test -v
```
Attendu : le terminal attend, sans message d'erreur.

**Terminal 3 : serveur HTTP de test**
```powershell
server\.venv\Scripts\python server\test_upload_server.py
```
Attendu : `ecoute sur 0.0.0.0:8000, POST /upload (Ctrl+C pour arreter)`.

**Terminal 4 : carte**
```powershell
pio run -d firmware -t upload
firmware\scripts\.venv\Scripts\python firmware\scripts\serial_monitor.py
```
Attendu : `[moniteur] Connecté à COMx (460800 bauds)`. Appuyer alors sur le bouton RESET de la carte : les tests défilent (sans cet appui, le début peut être manqué pendant le démarrage du moniteur). Pour téléverser de nouveau : Ctrl+C sur le moniteur d'abord. Carte non trouvée : ajouter `--port COM5` (numéro du port de la carte).
Première fois seulement, avant le moniteur :
```powershell
py -m venv firmware\scripts\.venv
firmware\scripts\.venv\Scripts\python -m pip install -r firmware\scripts\requirements.txt
```

## 3. Les tests, dans l'ordre

| # | Test | Prérequis | Validé si |
|---|---|---|---|
| 1 | CAMERA | aucun (l'image n'est plus envoyée sur le port série) | `[camera] resolution 640x480`, `format JPEG`, `capture` |
| 2 | BME680 | aucun | `temperature`, `humidite`, `pression`, `gaz` affichés, valeurs plausibles |
| 3 | MQ2 | aucun ; état `PRET` seulement 300 s après la mise sous tension | `etat PRECHAUFFAGE` ou `PRET`, `alerte NON` en air normal |
| 4 | WIFI | § 1, étapes 2 et 4 | `adresse ip ... 172.20.10.x` |
| 5 | MQTT | terminaux 1 et 2 | carte : `broker <IP du PC>:1883` et `message` ; terminal 2 : `cypherhome/test bonjour de la carte, NN s` |
| 6 | PHOTO HTTP | terminal 3 | carte : lignes `[photo]` (taille, capture, envoi, total, debit) ; terminal 3 : `photo recue : N octets -> photo_<date>_<taille>.jpg`, fichier dans `server/output/` qui s'ouvre |
| 7 | DISPLAY | aucun | `resolution 160x128` ; à l'écran : « Cypher Home », « Ecran OK », puis Rouge, Vert, Bleu dans leur couleur |
| 8 | LEDS | aucun | rouge, bleue, jaune, orange s'allument seules dans cet ordre, puis les 4 ensemble |
| 9 | BUZZER | aucun | deux sons entendus, grave (1000 Hz) puis aigu (2000 Hz) |
| 10 | BUTTONS | ne toucher aucun bouton au début ; dès les 5 lignes `pas appuye` (juste après les deux sons), appuyer puis relâcher **les 5 boutons** dans les 7 s | chaque bouton affiche `appuye` puis `pas appuye`, sous le bon nom |
| — | RECAPITULATIF | aucun | une ligne `[recap]` par composant, `total 10 OK, 0 KO` et `duree` |

Un test raté affiche `[composant] KO` (suivi de `code erreur` pour MQTT et HTTP). Wi-Fi KO : MQTT et PHOTO HTTP sont sautés (`[mqtt] KO (wifi)`, `[http] KO (wifi)`) ; caméra KO : PHOTO HTTP est sauté (`[http] KO (camera)`).

Récapitulatif : écran, LED, buzzer et MQ-2 sont toujours OK, car la carte ne peut pas détecter leur absence : les vérifier à l'oeil, à l'oreille ou par la valeur affichée. Boutons : KO si l'un des 5 n'a pas été appuyé pendant les 7 s.

## 4. Causes connues d'un KO

| KO | Vérifier |
|---|---|
| `[wifi] KO` | partage de connexion actif, « Maximiser la compatibilité », SSID et mot de passe dans `secrets.h` |
| `[mqtt] KO` | terminal 1 (broker démarré), IP du PC dans `secrets.h`, profil privé, règle de pare-feu « Cypher Home - MQTT 1883 » ; après un redémarrage du broker, relancer le terminal 2 |
| `[http] KO`, code -1 | terminal 3 (serveur lancé), IP du PC dans `secrets.h`, règle de pare-feu « Cypher Home - HTTP 8000 » |
| `[http] KO`, code -3 | serveur coupé pendant un envoi : le relancer |
| `[mqtt] KO (wifi)`, `[http] KO (wifi)` | voir `[wifi] KO` |
| `[http] KO (camera)` | voir `[camera] KO` (fiche `docs/tests/camera.md`) |

Réactiver une règle de pare-feu (PowerShell administrateur) :
```powershell
Enable-NetFirewallRule -DisplayName "Cypher Home - MQTT 1883"
Enable-NetFirewallRule -DisplayName "Cypher Home - HTTP 8000"
```

## 5. Fin de session

1. Ctrl+C dans les terminaux 2, 3 et 4.
2. Terminal 1 (dossier `docker`) : `docker compose down`.
3. Le mot de passe tapé dans le terminal 2 reste dans l'historique de PowerShell : `(Get-PSReadLineOption).HistorySavePath` donne le chemin du fichier ; l'ouvrir et supprimer la ligne `mosquitto_sub`.

## À confirmer

- Création du `.venv` de `server/` : aucune commande dans le dépôt (il existe déjà ; les serveurs de test n'utilisent que la bibliothèque standard de Python).
- Commande pour vérifier qu'une règle de pare-feu est active : non documentée.
