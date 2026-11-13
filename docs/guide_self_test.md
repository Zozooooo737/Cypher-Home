# Guide : lancer `self_test_run()`

But : les 12 tests réussis dans une même passe. La carte relance `self_test_run()` en boucle, avec 5 s de pause entre deux passes. Commandes PowerShell, lancées depuis la racine du dépôt sauf indication contraire.

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
Attendu : `[moniteur] Connecté à COMx (460800 bauds)`, puis les tests. Pour téléverser de nouveau : Ctrl+C sur le moniteur d'abord. Carte non trouvée : ajouter `--port COM5` (numéro du port de la carte).
Première fois seulement, avant le moniteur :
```powershell
py -m venv firmware\scripts\.venv
firmware\scripts\.venv\Scripts\python -m pip install -r firmware\scripts\requirements.txt
```

## 3. Les tests, dans l'ordre

| # | Test | Prérequis | Validé si |
|---|---|---|---|
| 1 | CAMERA | moniteur lancé (terminal 4), sinon l'image s'affiche en caractères illisibles | `[moniteur] photo enregistree ... cam_<date>.jpg` (dans `firmware/output/`, s'ouvre), puis `[camera] resolution 640x480`, `format JPEG`, `capture` |
| 2 | BME680 | aucun | `temperature`, `humidite`, `pression`, `gaz` affichés, valeurs plausibles |
| 3 | MQ2 | aucun ; état `PRET` seulement 300 s après la mise sous tension | `etat PRECHAUFFAGE` ou `PRET`, `alerte NON` en air normal |
| 4 | DISPLAY | aucun | `resolution 160x128` ; à l'écran : « Cypher Home », « Ecran OK », puis Rouge, Vert, Bleu dans leur couleur |
| 5 | LEDS | aucun | rouge, bleue, jaune, orange s'allument seules dans cet ordre, puis les 4 ensemble |
| 6 | BUTTONS | ne toucher aucun bouton au début ; dès les 5 lignes `pas appuye`, appuyer puis relâcher chaque bouton dans les 7 s | chaque bouton affiche `appuye` puis `pas appuye`, sous le bon nom |
| 7 | BUZZER | aucun | deux sons entendus, grave (1000 Hz) puis aigu (2000 Hz) |
| 8 | WIFI | § 1, étapes 2 et 4 | `adresse ip ... 172.20.10.x` |
| 9 | MQTT | terminaux 1 et 2 | carte : `broker <IP du PC>:1883` et `message` ; terminal 2 : `cypherhome/test bonjour de la carte, NN s` |
| 10 | HTTP | terminal 3 | carte : `code http 200` pour 16384, 32768, 65536 et 131072 octets ; terminal 3 : `motif : conforme` 4 fois |
| 11 | PHOTO HTTP | terminal 3 | carte : lignes `[photo]` (taille, capture, envoi, total, debit) ; terminal 3 : `photo recue : N octets -> photo_<date>_<taille>.jpg`, fichier dans `server/output/` qui s'ouvre |
| 12 | ENSEMBLE | tous les prérequis ci-dessus | 4 LED allumées et un son pendant le test, mesures affichées à l'écran, puis photo enregistrée par le moniteur comme au test 1 |

Un test raté affiche `[composant] KO` (suivi de `code erreur` pour MQTT et HTTP).

## 4. Causes connues d'un KO

| KO | Vérifier |
|---|---|
| `[wifi] KO` | partage de connexion actif, « Maximiser la compatibilité », SSID et mot de passe dans `secrets.h` |
| `[mqtt] KO` | terminal 1 (broker démarré), IP du PC dans `secrets.h`, profil privé, règle de pare-feu « Cypher Home - MQTT 1883 » ; après un redémarrage du broker, relancer le terminal 2 |
| `[http] KO`, code -1 | terminal 3 (serveur lancé), IP du PC dans `secrets.h`, règle de pare-feu « Cypher Home - HTTP 8000 » |
| `[http] KO`, code -3 | serveur coupé pendant un envoi : le relancer |

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
