#pragma once
#include <Arduino.h>

// Paramètres par défaut du firmware.
// Règle : une valeur utile au produit final va ici, en constexpr, groupée par
// composant, unité dans le nom (ex. BME680_READ_PERIOD_MS).
// Une valeur qui n'existe que pour le test reste une constante locale du .cpp.
// Aucun secret ici (voir secrets.h, ignoré par Git).
// Une ligne est ajoutée seulement quand on en a besoin, valeur validée par l'étudiant.
