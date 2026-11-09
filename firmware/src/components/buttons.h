#pragma once
#include <Arduino.h>

// Les 5 boutons, dans l'ordre du tableau des broches de buttons.cpp
enum Button : uint8_t { BUTTON_CENTER, BUTTON_RIGHT, BUTTON_LEFT, BUTTON_UP, BUTTON_DOWN, BUTTON_COUNT };

// Met les 5 broches en INPUT_PULLUP ; toujours true : un bouton absent se lit comme relâché
bool buttons_setup();

// Lit les 5 boutons et met à jour leur état stable (anti-rebond BUTTONS_DEBOUNCE_MS) ; à appeler souvent
void buttons_update();

// true si le bouton est appuyé, d'après le dernier buttons_update()
bool buttons_is_pressed(Button button);

// Test : KO si un bouton est lu appuyé au départ, sinon état des 5 boutons puis une ligne par changement pendant 15 s
bool buttons_test();
