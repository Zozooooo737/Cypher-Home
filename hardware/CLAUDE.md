# Matériel : schéma, datasheets, tests de composants

Complète `CLAUDE.md` (racine). Pas de code de production ici.

- `kicad/` : projet KiCad (schéma électronique officiel). `datasheets/` : PDF des composants. Noms de broches : ceux de `CONTEXTE_TECHNIQUE.md`.
- Aucune affectation de broche n'est définie : les composants sont testés un par un avant tout câblage. Propose les affectations, l'étudiant valide, puis elles entrent dans `DECISIONS.md`.
- Avant de proposer un montage ou un schéma, vérifie et signale : niveaux logiques (carte en 5 V, écran et certains modules en 3,3 V), courants consommés, résistances série des LED, partage du bus SPI (BME680 et caméra, un CS par périphérique), alimentation et masses communes.
- Pour chaque composant, consigne dans une fiche courte (dans `docs/tests/`, sur le modèle `_modele.md`) : montage testé, bibliothèque utilisée, résultat observé fourni par l'étudiant, problèmes rencontrés. N'invente aucun résultat.
- Valeurs des résistances : à définir (voir `CONTEXTE_TECHNIQUE.md`, 3.17). Raccordement du convertisseur pour le mode batterie : décision D6 dans `DECISIONS.md`.
- Pour KiCad, fournis des listes de connexions claires (composant, broche, vers carte, broche) que l'étudiant reporte dans le schéma. Ne dis pas qu'un montage fonctionne sans test.
