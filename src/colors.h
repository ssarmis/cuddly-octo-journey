#pragma once

#include "general.h"

#include "math.h"


static v4 rgbaToNormalized(u8 r, u8 g, u8 b, u8 a){
    v4 result = {};

    result.x = (r32)(r) / 255.0;
    result.y = (r32)(g) / 255.0;
    result.z = (r32)(b) / 255.0;
    result.w = (r32)(a) / 255.0;

    return result;
}

static v3 rgbToNormalized(u8 r, u8 g, u8 b){
    v3 result = {};

    result.x = (r32)(r) / 255.0;
    result.y = (r32)(g) / 255.0;
    result.z = (r32)(b) / 255.0;

    return result;
}

// #define TEMPLE_OS_THEME
#define DEFAULT_THEME
#if defined(TEMPLE_OS_THEME)

static v3 DEFAULT_COLOR_COMMENT = {0, 0.5, 0};

static v3 DEFAULT_COLOR_BACKGROUND = {0.4, 0.4, 0.4);
static v3 DEFAULT_COLOR_LINE = DEFAULT_COLOR_BACKGROUND;
static v3 DEFAULT_COLOR_TEXT = {0, 0, 0};

static v3 DEFAULT_COLOR_CURSOR = {0, 0, 0};

static v3 DEFAULT_COLOR_PANEL_TEXT = DEFAULT_COLOR_BACKGROUND;
static v3 DEFAULT_COLOR_PANEL_BACKGROUND = {0, 0, 0.8};
static v3 DEFAULT_COLOR_PANEL_CURSOR = DEFAULT_COLOR_CURSOR;
static v3 DEFAULT_COLOR_TEXT_PANEL_DESCRIPTION = {0.9, 0.9, 0.9};
static v3 DEFAULT_COLOR_TEXT_PANEL_SUGGESTION = {0.9, 0.9, 0};
static v3 DEFAULT_COLOR_PANEL_BACKGROUND_SUGGESTION = DEFAULT_COLOR_PANEL_BACKGROUND;

static v3 SELECTION_COLOR_TEXT = {0, 0, 0};
static v3 STRING_COLOR_TEXT = {0.7, 0.3, 0};

static v3 KEYWORD_COLOR_TYPE = {0, 0, 1};
static v3 KEYWORD_COLOR_SPECIAL = KEYWORD_COLOR_TYPE;
#elif defined(DEFAULT_THEME)

static v4 DEFAULT_COLOR_COMMENT = {0, 0.5, 0, 1};

static v4 DEFAULT_COLOR_BACKGROUND = {0.1, 0.1, 0.1, 1};
static v4 DEFAULT_COLOR_LINE = {0, 0, 0, 1};
static v4 DEFAULT_COLOR_CURSOR = {0.6, 0, 0, 0.6};

static v4 SELECTION_SECONDARY_COLOR_TEXT = {0, 0.5, 1, 0.3};

static v4 DEFAULT_COLOR_TEXT = {0.9, 0.9, 0.9, 1};
static v4 SELECTION_COLOR_TEXT = {0, 0, 0.8, 0.6};
static v4 STRING_COLOR_TEXT = {1, 0.5, 0.5, 1};

static v4 DEFAULT_COLOR_PANEL_TEXT = DEFAULT_COLOR_TEXT;
static v4 DEFAULT_COLOR_PANEL_BACKGROUND = {0, 0, 0, 1};
static v4 DEFAULT_COLOR_PANEL_CURSOR = DEFAULT_COLOR_CURSOR;
static v4 DEFAULT_COLOR_TEXT_PANEL_DESCRIPTION = {0.6, 0.6, 0.6, 1};
static v4 DEFAULT_COLOR_TEXT_PANEL_SUGGESTION = {0.6, 0.6, 0, 1};
static v4 DEFAULT_COLOR_PANEL_BACKGROUND_SUGGESTION = DEFAULT_COLOR_PANEL_BACKGROUND;

// v4 DEFAULT_COLOR_STATUS_BAR_BACKGROUND = {0.05, 0.05, 0.05, 1};
static v4 DEFAULT_COLOR_STATUS_BAR_BACKGROUND = {0.3, 0.3, 0.3, 1};
static v4 DEFAULT_COLOR_STATUS_BAR_TEXT_COLOR = {0, 0, 0, 1};

static v4 KEYWORD_COLOR_TYPE = {0, 0.8, 0.5, 1};
static v4 KEYWORD_COLOR_SPECIAL = KEYWORD_COLOR_TYPE;

#else
static v3 DEFAULT_COLOR_BACKGROUND = rgbToNormalized(0x85, 0xdc, 0xb0);
static v3 DEFAULT_COLOR_LINE = {0, 0, 0};
static v3 DEFAULT_COLOR_CURSOR = {1, 1, 0};

static v3 DEFAULT_COLOR_TEXT = rgbToNormalized(0xee, 0xee, 0xee);
static v3 SELECTION_COLOR_TEXT = rgbToNormalized(0xc3, 0x8d, 0x9e);
static v3 STRING_COLOR_TEXT = rgbToNormalized(0xc3, 0x8d, 0x9e);

static v3 KEYWORD_COLOR_TYPE = rgbToNormalized(0xe2, 0x7d, 0x60);
static v3 KEYWORD_COLOR_SPECIAL = KEYWORD_COLOR_TYPE;
#endif 