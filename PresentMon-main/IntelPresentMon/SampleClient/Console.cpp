// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: MIT
#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

static HANDLE gConsoleHandle;
static char gConsoleWriteBuffer[32 * 1024];
static uint32_t gConsoleWriteBufferIndex;
static uint32_t gConsolePrevWriteBufferSize;
static SHORT gConsoleTop;
static SHORT gConsoleWidth;
static SHORT gConsoleBufferHeight;
static bool gConsoleFirstCommit;

bool InitializeConsole()
{
    gConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (gConsoleHandle == INVALID_HANDLE_VALUE) {
        return false;
    }

    CONSOLE_SCREEN_BUFFER_INFO info = {};
    if (GetConsoleScreenBufferInfo(gConsoleHandle, &info) == 0) {
        return false;
    }

    gConsoleTop = info.dwCursorPosition.Y;
    gConsoleWidth = info.srWindow.Right - info.srWindow.Left + 1;
    gConsoleBufferHeight = info.dwSize.Y;
    gConsoleWriteBufferIndex = 0;
    gConsolePrevWriteBufferSize = 0;
    gConsoleFirstCommit = true;

    return true;
}

static void vConsolePrint(char const* format, va_list args)
{
    auto s = gConsoleWriteBuffer + gConsoleWriteBufferIndex;
    auto n = sizeof(gConsoleWriteBuffer) - gConsoleWriteBufferIndex;

    int r = vsnprintf(s, n, format, args);
    if (r > 0) {
        gConsoleWriteBufferIndex = min((uint32_t) (n - 1), gConsoleWriteBufferIndex + r);
    }
}

void ConsolePrint(char const* format, ...)
{
    va_list args;
    va_start(args, format);
    vConsolePrint(format, args);
    va_end(args);
}

void ConsolePrintLn(char const* format, ...)
{
    va_list args;
    va_start(args, format);
    vConsolePrint(format, args);
    va_end(args);

    auto x = gConsoleWriteBufferIndex % gConsoleWidth;
    auto s = gConsoleWidth - x;
    memset(gConsoleWriteBuffer + gConsoleWriteBufferIndex, ' ', s);
    gConsoleWriteBufferIndex += s;
}

void CommitConsole()
{
    auto sizeWritten = gConsoleWriteBufferIndex;
    auto linesWritten = static_cast<SHORT>(sizeWritten / gConsoleWidth);

    // Reset gConsoleTop on the first commit so we don't overwrite any warning
    // messages.
    auto size = sizeWritten;
    if (gConsoleFirstCommit) {
        gConsoleFirstCommit = false;

        CONSOLE_SCREEN_BUFFER_INFO info = {};
        GetConsoleScreenBufferInfo(gConsoleHandle, &info);
        gConsoleTop = info.dwCursorPosition.Y;
    } else {
        // Write some extra empty lines to make sure we clear anything from
        // last time.
        if (size < gConsolePrevWriteBufferSize) {
            memset(gConsoleWriteBuffer + size, ' ', gConsolePrevWriteBufferSize - size);
            size = gConsolePrevWriteBufferSize;
        }
    }

    // If we're at the end of the console buffer, issue some new lines to make
    // some space.
    auto maxCursorY = gConsoleBufferHeight - linesWritten;
    if (gConsoleTop > maxCursorY) {
        COORD bottom = { 0, static_cast<SHORT>(gConsoleBufferHeight - 1)};
        SetConsoleCursorPosition(gConsoleHandle, bottom);
        printf("\n");
        for (--gConsoleTop; gConsoleTop > maxCursorY; --gConsoleTop) {
            printf("\n");
        }
    }

    // Write to the console.
    DWORD dwCharsWritten = 0;
    COORD cursor = { 0, gConsoleTop };
    WriteConsoleOutputCharacterA(gConsoleHandle, gConsoleWriteBuffer, static_cast<DWORD>(size), cursor, &dwCharsWritten);

    // Put the cursor at the end of the written text.
    cursor.Y += linesWritten;
    SetConsoleCursorPosition(gConsoleHandle, cursor);

    // Update console info in case it was resized.
    CONSOLE_SCREEN_BUFFER_INFO info = {};
    GetConsoleScreenBufferInfo(gConsoleHandle, &info);
    gConsoleWidth = info.srWindow.Right - info.srWindow.Left + 1;
    gConsoleBufferHeight = info.dwSize.Y;
    gConsoleWriteBufferIndex = 0;
    gConsolePrevWriteBufferSize = sizeWritten;
}