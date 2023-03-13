#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>
#include <math.h>

//
//This modified function is used to display the console using UTF8 mode,
//so the symbol π can actually be shown

int _wprintf(const wchar_t* format, ...)
{
  int r;
  static int utf8ModeSet = 0;
  static wchar_t* bufWchar = NULL;
  static size_t bufWcharCount = 256;
  static char* bufMchar = NULL;
  static size_t bufMcharCount = 256;
  va_list vl;
  int mcharCount = 0;

  if (utf8ModeSet == 0)
  {
    if (!SetConsoleOutputCP(CP_UTF8))
    {
      DWORD err = GetLastError();
      fprintf(stderr, "SetConsoleOutputCP(CP_UTF8) failed with error 0x%X\n", err);
      utf8ModeSet = -1;
    }
    else
    {
      utf8ModeSet = 1;
    }
  }

  if (utf8ModeSet != 1)
  {
    va_start(vl, format);
    r = vwprintf(format, vl);
    va_end(vl);
    return r;
  }

  if (bufWchar == NULL)
  {
    if ((bufWchar = malloc(bufWcharCount * sizeof(wchar_t))) == NULL)
    {
      return -1;
    }
  }

  for (;;)
  {
    va_start(vl, format);
    r = vswprintf(bufWchar, bufWcharCount, format, vl);
    va_end(vl);

    if (r < 0)
    {
      break;
    }

    if (r + 2 <= bufWcharCount)
    {
      break;
    }

    free(bufWchar);
    if ((bufWchar = malloc(bufWcharCount * sizeof(wchar_t) * 2)) == NULL)
    {
      return -1;
    }
    bufWcharCount *= 2;
  }

  if (r > 0)
  {
    if (bufMchar == NULL)
    {
      if ((bufMchar = malloc(bufMcharCount)) == NULL)
      {
        return -1;
      }
    }

    for (;;)
    {
      mcharCount = WideCharToMultiByte(CP_UTF8,
                                       0,
                                       bufWchar,
                                       -1,
                                       bufMchar,
                                       bufMcharCount,
                                       NULL,
                                       NULL);
      if (mcharCount > 0)
      {
        break;
      }

      if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
      {
        return -1;
      }

      free(bufMchar);
      if ((bufMchar = malloc(bufMcharCount * 2)) == NULL)
      {
        return -1;
      }
      bufMcharCount *= 2;
    }
  }

  if (mcharCount > 1)
  {
    DWORD numberOfCharsWritten, consoleMode;

    if (GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &consoleMode))
    {
      fflush(stdout);
      if (!WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE),
                         bufMchar,
                         mcharCount - 1,
                         &numberOfCharsWritten,
                         NULL))
      {
        return -1;
      }
    }
    else
    {
      if (fputs(bufMchar, stdout) == EOF)
      {
        return -1;
      }
    }
  }

  return r;
}


int main()
{
    const double pi = 4.0 * atan(1.0);
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE); // Get the console handle
    CONSOLE_SCREEN_BUFFER_INFO console_info; // Initialize the console information struct
    clock_t start_time = clock(); // Get the current time
    int elapsed_time_ms = 0; // Initialize the elapsed time
    time_t now = time(0);

    while (elapsed_time_ms < 50000) { // Update output for 50 seconds
        elapsed_time_ms = (clock() - start_time) * 1000 / CLOCKS_PER_SEC; // Calculate elapsed time in milliseconds

        GetConsoleScreenBufferInfo(console, &console_info); // Get console information

        SYSTEMTIME st;
        GetSystemTime(&st);

        time_t now = time(0);

        struct tm *tm_struct = localtime(&now);
        float hour = tm_struct->tm_hour;
        float minute = tm_struct->tm_min;
        float second = tm_struct->tm_sec;

        //Join the minutes and seconds into one variable
        float min_sec = minute + second/100;

        //Convert the hours and minutes/seconds to powers of pi
        double x = log(hour)/log(pi);
        double y = log(min_sec)/log(pi);

        // Update output every millisecond
        _wprintf(L"\rThe current time is: \u03C0^(%lf) : \u03C0^(%lf)\n\n\nFor the uncultured: %.f : %.2f", x, y, hour, min_sec);

        fflush(stdout); // Flush the output buffer

        // Move the cursor back to the beginning of the line
        console_info.dwCursorPosition.X = 0;
        SetConsoleCursorPosition(console, console_info.dwCursorPosition);

        Sleep(1); // Sleep for 1 millisecond
    }

    return 0;
}
