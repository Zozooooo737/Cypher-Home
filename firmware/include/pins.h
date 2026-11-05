#pragma once
#include <Arduino.h>

// Broches de la carte Arduino UNO R4 WiFi : câblage courant.
// Un bloc par composant, avec les noms officiels de CONTEXTE_TECHNIQUE.md.
// Tant que D3 est ouverte (DECISIONS.md), les numéros sont provisoires et
// chaque affectation est validée par l'étudiant avant câblage.
// Aucun numéro de broche en dur dans les modules : tout vient d'ici.

// ---------------------------------------------------------------------------
// Index des broches de la carte (une ligne par broche, ex. « D10 : CAMERA_CS »)
// Sert à repérer les doublons et à préparer D3.
// ---------------------------------------------------------------------------
// (vide)

// ---------------------------------------------------------------------------
// Modèle de bloc par composant (à recopier, ne pas décommenter tel quel)
// ---------------------------------------------------------------------------
// <Composant> (CONTEXTE_TECHNIQUE.md, § x.x)
// Broche officielle -> broche de la carte
//   <BROCHE_1> -> Dx   : définie dans le code
//   <BROCHE_2> -> D13  : bus imposé par la carte (commentaire seulement)
//   VCC        -> 3V3 ou 5V
//   GND        -> GND
// constexpr uint8_t <COMPOSANT>_<BROCHE_1>_PIN = x;
