

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define ALT 0x80
#define ESC 0x1b

static char *lowchars[] = {
    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", "BS", "HT",  "LF",  "VT", "FF", "CR", "SO", "SI", "DLE",
    "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB", "CAN", "EM", "SUB", "ESC", "FS", "GS", "RS", "US", "SP",
};

static int signalled;

static void catcher(const int sig)
{
  signalled = sig;
}

#define STARTCOOKIE '<'
#define ENDCOOKIE '>'

static void visualize(int c, /*@out@*/ char *buf)
{
  char *end;
  bool unprintable;

  buf[0] = '\0';
  if ((c & ALT) != 0)
  {
    unprintable = true;
    c &= ~ALT;
    buf[0] = STARTCOOKIE;
    buf[1] = '\0';
    (void)strcat(buf, "ALT-");
  }

  if (c <= ' ')
  {
    unprintable = true;
    end = buf + strlen(buf);
    if (buf[0] != STARTCOOKIE)
    {
      *end++ = STARTCOOKIE;
      *end++ = '\0';
    }
    if (c > 0 && c < 27)
    {
      (void)strcat(buf, "CTL-");
      end = buf + strlen(buf);
      *end++ = (c + 0x40);
      *end++ = '=';
      *end = 0;
    }
    (void)strcat(buf, lowchars[c]);
  }
  else if (c == 0x3F)
  {
    unprintable = true;
    (void)strcat(buf, "DEL");
  }
  else
  {
    unprintable = false;
    buf[0] = c;
    buf[1] = '\0';
  }

  if (unprintable)
  {
    end = buf + strlen(buf);
    *end++ = ENDCOOKIE;
    *end++ = '\0';
  }
}

main()
{
  struct termios cooked, raw;
  unsigned char c;
  unsigned int i, timeouts;
  char intrchar[32], quitchar[32];

  for (i = SIGHUP; i <= SIGIO; i++)
    (void)signal(c, catcher);

  // Get the state of the tty

  (void)tcgetattr(0, &cooked);
  // Make a copy we can mess with

  (void)memcpy(&raw, &cooked, sizeof(struct termios));
  // Turn off echoing, linebuffering, and special-character processing,
  //     // but not the SIGINT or SIGQUIT keys.

  raw.c_lflag &= ~(ICANON | ECHO);
  // Ship the raw control blts
  (void)tcsetattr(0, TCSANOW, &raw);

  (void)printf("Type any key to see the sequence it sends.\n");
  visualize(raw.c_cc[VINTR], intrchar);
  visualize(raw.c_cc[VQUIT], quitchar);
  (void)printf("Terminate with your shell interrupt %s or quit %s character.\n", intrchar, quitchar);
  signalled = 0;
  //while (!signalled)
  while (true)
  {
    char cbuf[32];

    read(0, &c, 1);
    visualize(c, cbuf);
    (void)fputs(cbuf, stdout);
    (void)fflush(stdout);
  }

  (void)printf("\nBye...\n");
  // Restore the cooked state
  (void)tcsetattr(0, TCSANOW, &cooked);
}

// clang-format off
// clang-format-9 -i showkey.cpp
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a showkey.cpp
// g++ -std=c++17 -Wall -Wextra -Wpedantic showkey.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto
// g++ -std=c++17 -Wall -Wextra -Wpedantic -Wno-deprecated-declarations showkey.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto

