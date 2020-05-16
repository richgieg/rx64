#include "console.h"
#include "graphics.h"
#include "kernel.h"
#include "memory.h"

#define CELL_WIDTH_PIXELS      8
#define CELL_HEIGHT_PIXELS     19

static UINT16           mColumns;
static UINT16           mRows;
static UINT16           mHorizontalPaddingPixels;
static UINT16           mVerticalPaddingPixels;
static UINT16           mCurrentColumn;
static UINT16           mCurrentRow;
static UINT32           mForegroundColor;
static UINT32           mBackgroundColor;
static GFX_FRAME_BUFFER *mFrameBuffer;

VOID
ScrollToNewRow ();

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
    mFrameBuffer = GfxCreateBuffer();

    GfxFillScreen(mBackgroundColor);
    GfxFillBuffer(mFrameBuffer, mBackgroundColor);
}

VOID
CnPrint(
    IN CONST CHAR16 *String
    )
{
    CHAR16 Char;

    while (Char = *String++) {
        switch (Char) {
            case L'\n': {
                mCurrentColumn = 0;
                mCurrentRow++;
                break;
            }
            default: {
                CnPutChar(mCurrentColumn, mCurrentRow, Char);
                mCurrentColumn++;
                if (mCurrentColumn >= mColumns) {
                    mCurrentColumn = 0;
                    mCurrentRow++;
                }
            }
        }
        if (mCurrentRow == mRows) {
            ScrollToNewRow();
            mCurrentRow--;
        }
    }
}

VOID
ScrollToNewRow ()
{
    UINT32  DestinationY;
    UINT32  SourceY;
    UINTN   NoLines;
    UINT32  LastRowX;
    UINT32  LastRowY;
    UINT32  LastRowWidth;

    DestinationY =  mVerticalPaddingPixels;
    SourceY = CELL_HEIGHT_PIXELS + mVerticalPaddingPixels;
    NoLines = CELL_HEIGHT_PIXELS * (mRows - 1);
    LastRowX = 0;
    LastRowY = NoLines + mVerticalPaddingPixels;
    LastRowWidth = GfxGetHorizontalResolution();

    // Scrolls rows (second through last) up to the first row.
    GfxBltLinesInBuffer(mFrameBuffer, DestinationY, SourceY, NoLines);
    GfxFillBlockInBuffer(mFrameBuffer, LastRowX, LastRowY, LastRowWidth, CELL_HEIGHT_PIXELS, mBackgroundColor);
    GfxCopyBufferToScreen(mFrameBuffer);

    // NOTE: Below is my original method. It's fast in a virtual machine,
    // but very slow when running on my Dell G5 laptop. Writing to the
    // screen's framebuffer is fine, but it seems that reading from it is
    // very slow. This is why I decided that the console should maintain
    // its own backup framebuffer. CnPutChar draws on the screen as well
    // as the backup framebuffer, so they both have the same content.
    // The scrolling is performed only in the backup framebuffer so we
    // don't have to read from the screen's framebuffer. The contents of
    // the backup framebuffer are then copied to the screen.

    // GfxBltLinesOnScreen(DestinationY, SourceY, NoLines);
    // GfxFillBlockOnScreen(LastRowX, LastRowY, LastRowWidth, CELL_HEIGHT_PIXELS, mBackgroundColor);
}

VOID
CnPutChar (
    IN UINT16 Column,
    IN UINT16 Row,
    IN CHAR16 Char
    )
{
    if (Column >= mColumns || Row >= mRows) {
        KeBugCheck();
    }

    UINT32 X;
    UINT32 Y;

    X = Column * CELL_WIDTH_PIXELS + mHorizontalPaddingPixels;
    Y = Row * CELL_HEIGHT_PIXELS + mVerticalPaddingPixels;

    GfxFillBlockOnScreen(X, Y, CELL_WIDTH_PIXELS, CELL_HEIGHT_PIXELS, mForegroundColor);
    GfxFillBlockInBuffer(mFrameBuffer, X, Y, CELL_WIDTH_PIXELS, CELL_HEIGHT_PIXELS, mForegroundColor);
}

VOID
CnSetForegroundColor (
    IN UINT32 Color
    )
{
    mForegroundColor = Color;
}
