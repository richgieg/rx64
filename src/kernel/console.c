#include "console.h"
#include "graphics.h"

#define CELL_WIDTH_PIXELS      8
#define CELL_HEIGHT_PIXELS     19

static UINT16 mColumns;
static UINT16 mRows;
static UINT16 mHorizontalPaddingPixels;
static UINT16 mVerticalPaddingPixels;
static UINT16 mCurrentColumn;
static UINT16 mCurrentRow;
static UINT32 mForegroundColor;
static UINT32 mBackgroundColor;

VOID
InitializeConsole ()
{
    mColumns = GetHorizontalResolution() / CELL_WIDTH_PIXELS;
    mRows = GetVerticalResolution() / CELL_HEIGHT_PIXELS;
    mHorizontalPaddingPixels = GetHorizontalResolution() % mColumns / 2;
    mVerticalPaddingPixels = GetVerticalResolution() % mRows / 2;
    mCurrentColumn = 0;
    mCurrentRow = 0;
    mForegroundColor = 0xe8e8e8;     // gray
    mBackgroundColor = 0;            // black
    
    FillScreen(mBackgroundColor);
}

VOID
Print(
    const CHAR16 *String
    )
{
    while (*String) {
        PutChar(mCurrentColumn, mCurrentRow, *String);
        mCurrentColumn++;
        if (mCurrentColumn >= mColumns) {
            mCurrentColumn = 0;
            mCurrentRow++;
        }
        String++;
    }
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

    X = Column * CELL_WIDTH_PIXELS + mHorizontalPaddingPixels;
    Y = Row * CELL_HEIGHT_PIXELS + mVerticalPaddingPixels;

    FillBlock(X, Y, CELL_WIDTH_PIXELS, CELL_HEIGHT_PIXELS, mForegroundColor);
}
