#include "console.h"
#include "graphics.h"

#define FONT_WIDTH_PIXELS      8
#define FONT_HEIGHT_PIXELS     19

static UINT16 mColumns;
static UINT16 mRows;
static UINT16 mCurrentColumn;
static UINT16 mCurrentRow;
static UINT32 mForegroundColor;
static UINT32 mBackgroundColor;

VOID
InitializeConsole ()
{
    mColumns = GetHorizontalResolution() / FONT_WIDTH_PIXELS;
    mRows = GetVerticalResolution() / FONT_HEIGHT_PIXELS;
    mForegroundColor = 0xe8e8e8;     // gray
    mBackgroundColor = 0;            // black

    FillScreen(mBackgroundColor);
}

VOID
PutChar (
    UINT16 Column,
    UINT16 Row,
    CHAR16 Char
    )
{
    UINT32 X;
    UINT32 Y;

    X = Column * FONT_WIDTH_PIXELS;
    Y = Row * FONT_HEIGHT_PIXELS;

    FillBlock(X, Y, FONT_WIDTH_PIXELS, FONT_HEIGHT_PIXELS, mForegroundColor);
}
