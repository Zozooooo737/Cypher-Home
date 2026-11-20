#pragma once

// Règle les délais de connexion au serveur et d'attente de sa réponse
void http_setup();

// Envoie length octets en POST, lus bloc par bloc avec read_block ; status = code HTTP, ou -1 connexion impossible, -2 envoi interrompu, -3 pas de réponse ; false si status n'est pas 200
bool http_upload(uint32_t length, size_t (*read_block)(uint8_t *buf, size_t max_len), int &status);
