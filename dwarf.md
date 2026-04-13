# DWARF

## Introduction
DWARF (version 5) est le format des informations de debug généré par les compilateurs, assembleurs et linkers. Ces informations sont présentes dans les sections .debug_ des fichiers ELF. 

L'option ```-w``` de ```readelf``` affiche le contenu des sections relatives à DWARF (ex : debug_aranges, .debug_info, .debug_abbrev, .debug_line, .debug_str, .debug_line_str).

Debugging Information Entry (DIE) : tag + attribut, on peut les afficher avec l'option ```-wi``` de ```readelf```

Line Number Information Table (dans .debug_line), on peut l'afficher avec l'option ```-wL``` de ```readelf```
