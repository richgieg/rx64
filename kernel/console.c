#include <console.h>
#include <graphics.h>

#define CELL_WIDTH_PIXELS       8
#define CELL_HEIGHT_PIXELS      19

#define FIRST_PRINTABLE_CHAR    0x20
#define LAST_PRINTABLE_CHAR     0x7e

static UINT16           mColumns;
static UINT16           mRows;
static UINT16           mHorizontalPaddingPixels;
static UINT16           mVerticalPaddingPixels;
static UINT16           mCurrentColumn;
static UINT16           mCurrentRow;
static UINT32           mForegroundColor;
static UINT32           mBackgroundColor;

VOID
CnInitializeConsole ()
{
    mColumns = GfxGetHorizontalResolution() / CELL_WIDTH_PIXELS;
    mRows = GfxGetVerticalResolution() / CELL_HEIGHT_PIXELS;
    mHorizontalPaddingPixels = GfxGetHorizontalResolution() % mColumns / 2;
    mVerticalPaddingPixels = GfxGetVerticalResolution() % mRows / 2;
    mCurrentColumn = 0;
    mCurrentRow = 0;
    mForegroundColor = 0xe8e8e8;     // gray
    mBackgroundColor = 0;            // black

    GfxFillScreen(mBackgroundColor);
}
