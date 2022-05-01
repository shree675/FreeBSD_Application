#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define BUFFER_INIT                                                            \
  { NULL, 0 }
#define CTRL_KEY(k) ((k)&0x1f)
#define MAXLEN 1024
#define MAX_PEOPLE 100

int num_faculty = 0;
int num_students = 0;
// variable initializations
char file[MAX_PEOPLE][MAX_PEOPLE][MAXLEN];
int total[MAX_PEOPLE];
bool normalMode = true;

enum editorKey { ARROW_LEFT = 1000, ARROW_RIGHT, ARROW_UP, ARROW_DOWN };

struct editorConfig {
  int cx, cy;
  int screenrows;
  int screencols;
  struct termios orig_termios;
};

/* the point of using buffers is to
   minimize the no. of write() calls */
struct writeBuffer {
  char *b;
  int len;
};

struct editorConfig E;

// clear the screen and exit
void terminate(const char *s) {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
  perror(s);
  exit(1);
}

// exit from raw mode
void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1) {
    terminate("tcsetattr");
  }
}

// enter raw mode
void enableRawMode() {
  // get the current terminal settings
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) {
    terminate("tcgetattr");
  }
  // atexit calls disableRawMode as soon as the program exits
  atexit(disableRawMode);
  struct termios raw = E.orig_termios;
  /* disabling some default settings of the terminal
     that are required to be turned off for raw mode */
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  // minimum no. of bytes to read before read() can return
  raw.c_cc[VMIN] = 0;
  // set the modified terminal settings
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
    terminate("tcsetattr");
  }
}

// read key press and distinguish betwen arrow key presses
int readKey() {
  int readStream;
  char c;
  while ((readStream = read(STDIN_FILENO, &c, 1)) != 1) {
    if (readStream == -1 && errno != EAGAIN) {
      terminate("read");
    }
  }
  if (c == '\x1b') {
    char seq[3];
    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';
    if (seq[0] == '[') {
      switch (seq[1]) {
      // up arrow: ^[A
      case 'A':
        return ARROW_UP;
      // down arrow: ^[B
      case 'B':
        return ARROW_DOWN;
      // right arrow: ^[C
      case 'C':
        return ARROW_RIGHT;
      // left arrow: ^[D
      case 'D':
        return ARROW_LEFT;
      }
    }
    return '\x1b';
  } else {
    // if it is not an escape sequence
    return c;
  }
}

// append string to buffer to display it on the screen
void appendBuf(struct writeBuffer *ab, const char *s, int len) {
  // creating space in the memory to append the string s
  char *newBlock = realloc(ab->b, ab->len + len);
  if (newBlock == NULL) {
    return;
  }
  memcpy(&newBlock[ab->len], s, len);
  ab->b = newBlock;
  ab->len += len;
}

// create table with 5 rows and 6 columns
void fillTable(struct writeBuffer *ab) {
  int x = (E.cx - 3) / 6;
  int y = E.cy;
  // stores dynamic cell information
  char information[64] = {'\0'};
  snprintf(information, sizeof(information),
           "Current cell: \x1b[47;1m\x1b[30;1m (faculty%d,student%d) \x1b[0m",
           x, y);
  char faculty[num_faculty][5];
  char student[num_students][5];
  char concatStr[32];
  for(int i=0;i<num_faculty;i++){
  	snprintf(concatStr, sizeof(concatStr), "Fac%d", i);
  	strcpy(faculty[i], concatStr);
  	snprintf(concatStr, 32, "");
  }
  for(int i=0;i<num_students;i++){
  	snprintf(concatStr, sizeof(concatStr), "Stu%d", i);
  	strcpy(student[i], concatStr);
  	snprintf(concatStr, 32, "");
  }
  // strcpy(faculty[0], "Fac1");
  // strcpy(faculty[1], "Fac2");
  // strcpy(faculty[2], "Fac3");
  // strcpy(faculty[3], "Fac4");
  // strcpy(student[0], "Stu1");
  // strcpy(student[1], "Stu2");
  // strcpy(student[2], "Stu3");
  // strcpy(student[3], "Stu4");
  char totalString[MAXLEN] = "\x1b[37;1mTotal\x1b[0m";
  char empty[MAXLEN] = "      ";
  char fiveSpaces[MAXLEN] = "     ";
  char space[MAXLEN] = "  ";
  char newLine[MAXLEN] = "\r\n";
  char temp[MAXLEN] = {'\0'};
  // populate table and format it
  appendBuf(ab, empty, sizeof(empty));
  for (int i = 0; i < num_faculty; i++) {
    appendBuf(ab, faculty[i], sizeof(faculty[i]));
    appendBuf(ab, space, sizeof(space));
  }
  appendBuf(ab, totalString, sizeof(totalString));
  appendBuf(ab, newLine, sizeof(newLine));
  for (int i = 0; i < num_students; i++) {
    appendBuf(ab, student[i], sizeof(student[i]));
    for (int j = 0; j < num_faculty; j++) {
      appendBuf(ab, fiveSpaces, sizeof(fiveSpaces));
      appendBuf(ab, file[j][i], sizeof(file[j][i]));
    }
    appendBuf(ab, fiveSpaces, sizeof(fiveSpaces));
    sprintf(temp, "\x1b[37;1m%2d\x1b[0m", total[i]);
    appendBuf(ab, temp, sizeof(temp));
    appendBuf(ab, newLine, sizeof(newLine));
  }
  if (y < num_students + 1) {
    char instructions[MAXLEN] = {'\0'};
    appendBuf(ab, information, sizeof(information));
    appendBuf(ab, newLine, sizeof(newLine));
    appendBuf(ab, newLine, sizeof(newLine));
    snprintf(instructions, sizeof(instructions),
             "Instructions:\r\n* Navigate up, down, left and right using arrow "
             "keys\r\n* Click on Enter to initiate edit mode and edit a "
             "cell\r\n* In edit mode, type a number between 0 and 9 to update "
             "the value of the cell\r\n* Then click on Enter to save the "
             "updated value and exit edit mode\r\n* To cancel an edit, hit Esc "
             "while in edit mode\r\n* To print the table and exit, hit "
             "Esc\r\n* Use Ctrl+C to exit without printing the table\r\n");
    appendBuf(ab, instructions, sizeof(instructions));
  } else {
    write(STDOUT_FILENO, "\x1b[J", 3);
  }
}

// restrict cursor movement to 16 cells
void moveCursor(int key) {
  switch (key) {
  case ARROW_LEFT:
    if (E.cx != 9) {
      E.cx = E.cx - 6;
    }
    break;
  case ARROW_RIGHT:
    if (E.cx != 3 + 6 * num_faculty) {
      E.cx = E.cx + 6;
    }
    break;
  case ARROW_UP:
    if (E.cy != 1) {
      E.cy--;
    }
    break;
  case ARROW_DOWN:
    if (E.cy != num_students) {
      E.cy++;
    }
    break;
  }
}

// read values from file and calculate total
void readFromFile() {
  FILE *fp;
  char buf[32];
  for (int i = 1; i <= num_faculty; i++) {
    for (int j = 1; j <= num_students; j++) {
      snprintf(buf, sizeof(buf), "data/data%d%d/data%d%d.txt", i, j, i, j);
      if ((fp = fopen(buf, "r")) == NULL) {
        terminate(buf);
      }
      fscanf(fp, "%s", file[i - 1][j - 1]);
      fclose(fp);
    }
  }
  for (int i = 0; i < num_students; i++) {
    total[i] = 0;
    for (int j = 0; j < num_faculty; j++) {
      total[i] = total[i] + (file[j][i][0] - 48);
    }
  }
}

// re-render the screen using buffer
void refreshTable() {
  struct writeBuffer ab = BUFFER_INIT;
  appendBuf(&ab, "\x1b[H", 3);
  if (normalMode) {
    readFromFile();
    fillTable(&ab);
  }
  char buf[32];
  // move cursor and display it
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.cy + 1, E.cx + 1);
  appendBuf(&ab, buf, strlen(buf));
  write(STDOUT_FILENO, ab.b, ab.len);
  free(ab.b);
}

void processKeypress() {
  int c = readKey();
  char num[1];
  char ch[2];
  char buf[32];
  bool enterKey = false;
  switch (c) {
  // Enter Key
  case CTRL_KEY('m'):
    // disable movement of cursor an screen refresh
    normalMode = false;
    write(STDOUT_FILENO, "\x1b[33;1m ", 8);
    // move cursor 1 character to the left
    write(STDOUT_FILENO, "\x1b[1D ", 4);
    enterKey = true;
    ch[0] = '\0';
    break;
  // ESC key
  case CTRL_KEY('['):
    E.cx = 0;
    E.cy = num_students + 1;
    refreshTable();
    exit(0);
    write(STDOUT_FILENO, "\x1b[0m", 4);
    break;
  // Ctrl+C
  case CTRL_KEY('c'):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    write(STDOUT_FILENO, "\x1b[0m", 4);
    exit(0);
    break;
  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_LEFT:
  case ARROW_RIGHT:
    if (normalMode) {
      moveCursor(c);
    }
    break;
  }
  if (enterKey) {
    // write(STDOUT_FILENO, "\x1b[1D", 4);
    while (1) {
      c = readKey();

      // if ESC key is pressed while in edit mode
      if (c == CTRL_KEY('[')) {
        write(STDOUT_FILENO, "\x1b[0m", 4);
        break;
      }
      // if enter key is pressed while in edit mode
      if (c == CTRL_KEY('m')) {
        if (ch[0] == '\0') {
          write(STDOUT_FILENO, "\x1b[0m", 4);
          break;
        }
        int x = (E.cx - 3) / 6;
        int y = E.cy;
        FILE *fp;
        snprintf(buf, sizeof(buf), "data/data%d%d/data%d%d.txt", x, y, x, y);
        if ((fp = fopen(buf, "w")) == NULL) {
          terminate(buf);
        }
        write(STDOUT_FILENO, ch, strlen(ch));
        // save the new value to the file
        fprintf(fp, "%c", ch[0]);
        fclose(fp);
        // calculate the new total
        readFromFile();
        write(STDOUT_FILENO, "\x1b[0m", 4);
        break;
      } else if (c == CTRL_KEY('c')) {
        // clear the screen
        write(STDOUT_FILENO, "\x1b[2J", 4);
        // move cursor to the top left
        write(STDOUT_FILENO, "\x1b[H", 3);
        write(STDOUT_FILENO, "\x1b[0m", 4);
        exit(0);
        break;
      } else {
        // check if the key is a number only
        if (c >= 48 && c <= 57) {
        //   write(STDOUT_FILENO, "\x1b[1D ", 4);
          ch[0] = (char)c;
          write(STDOUT_FILENO, ch, strlen(ch));
        }
      }
    }
  }
  // allow cursor movement and screen refresh
  normalMode = true;
}

// initialize the screen
void initialize() {
  normalMode = true;
  E.cx = 9;
  E.cy = 1;
  write(STDOUT_FILENO, "\x1b[=1C", 5);
}

int main(int argc, char **argv) {
  num_faculty = atoi(argv[0]);
  num_students = atoi(argv[1]);
  enableRawMode();
  initialize();
  readFromFile();
  write(STDOUT_FILENO, "\x1b[0m", 4);
  // clear the screen
  write(STDOUT_FILENO, "\x1b[2J", 4);
  // move cursor to the top left
  write(STDOUT_FILENO, "\x1b[H", 3);
  while (true) {
    refreshTable();
    processKeypress();
  }
  // remove all colors
  write(STDOUT_FILENO, "\x1b[0m", 4);
  return 0;
}
