#pragma once

// Met en place l'écran sur le bus SPI ; toujours true : sans MISO, l'absence de l'écran n'est pas détectable
bool display_setup();

// Test : fond noir, texte, puis « Rouge », « Vert », « Bleu » dans leur couleur ; résultat à vérifier à l'oeil
bool display_test();
