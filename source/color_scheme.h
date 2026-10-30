#ifndef COLOR_SCHEME_H
# define COLOR_SCHEME_H

#include <raylib.h>

// Color Scheme Structure
typedef struct {
    const char *name;
    Color background;
    Color text;
    Color cursor;
    Color lineNumbersBg;
    Color lineNumbersText;
    Color currentLineHighlight;
	Color secondaryText;
	Color secondaryBackground;
	Color highlight;
	Color visual;
	Color dime;
} ColorScheme;

// Available Color Schemes
ColorScheme colorSchemes[] = {
    {
        .name = "Dark (Default)",
        .background = (Color){24, 24, 24, 255},
        .text = (Color){220, 220, 220, 255},
        .cursor = (Color){255, 255, 255, 255},
        .lineNumbersBg = (Color){32, 32, 32, 255},
        .lineNumbersText = (Color){100, 100, 100, 255},
        .currentLineHighlight = (Color){40, 40, 40, 255}
    },
    {
        .name = "Light",
        .background = (Color){250, 250, 250, 255},
        .text = (Color){30, 30, 30, 255},
        .cursor = (Color){0, 0, 0, 255},
        .lineNumbersBg = (Color){230, 230, 230, 255},
        .lineNumbersText = (Color){140, 140, 140, 255},
        .currentLineHighlight = (Color){240, 240, 245, 255}
    },
    {
        .name = "Monokai",
        .background = (Color){39, 40, 34, 255},
        .text = (Color){248, 248, 242, 255},
        .cursor = (Color){248, 248, 240, 255},
        .lineNumbersBg = (Color){49, 50, 44, 255},
        .lineNumbersText = (Color){144, 144, 138, 255},
        .currentLineHighlight = (Color){62, 61, 50, 255}
    },
    {
        .name = "Matrix",
        .background = (Color){5, 15, 5, 255},
        .text = (Color){0, 255, 65, 255},
        .cursor = (Color){0, 255, 65, 255},
        .lineNumbersBg = (Color){10, 25, 10, 255},
        .lineNumbersText = (Color){0, 120, 30, 255},
        .currentLineHighlight = (Color){15, 35, 15, 255}
    }
};

#endif
