#ifndef GAME_H
#define GAME_H

#include <raylib.h>

#define WIDTH 1000
#define HEIGHT 1000
#define BLOCK_LEN 40
#define BLOCK_SIZE ((Vector2){BLOCK_LEN, BLOCK_LEN})
#define BUFFER_ROWS 2
#define BUFFER_AREA (BUFFER_ROWS * BLOCK_LEN)
#define PLAYFIELD_ROWS 20
#define ROWS (BUFFER_ROWS + PLAYFIELD_ROWS)
#define COLUMNS 10
#define PIECE_COUNT 7
#define INITIAL_ROTATION 0
#define INITIAL_BOARD_POSITION ((Vector2){3, 0})
#define FONT_SIZE 60
#define KEY_DOWN_TIMER_SPEED 0.03333f
#define KEY_TIMER_SPEED (2 * KEY_DOWN_TIMER_SPEED)
#define ENTRY_DELAY -1.5f
#define MUSIC_COUNT 3

typedef enum {
  KEY_DOWN_TIMER,
  KEY_LEFT_TIMER,
  KEY_RIGHT_TIMER,
  KEY_TIMERS_COUNT,
} KeyTimers;

typedef enum {
  SCREEN_START,
  SCREEN_PLAY,
  SCREEN_GAMEOVER,
} ScreenState;

typedef enum {
  SOUND_GAMEOVER,
  SOUND_LINECLEAR,
  SOUND_COUNT,
} SOUNDS;

typedef struct {
  Vector2 points[4];
} PieceConfiguration;

typedef struct {
  Color color;
  PieceConfiguration rotations[4];
  // the offset required so that the piece is centered when displayed on the "next piece" rectangle
  Vector2 displayOffset;
} PieceType;

typedef struct {
  const PieceType *tetromino;
  Vector2 position;
  int rotationIndex;
} Piece;

typedef struct {
  Color color;
  bool occupied;
} Block;

typedef struct {
  Block board[ROWS][COLUMNS];
  ScreenState screenState;
  Piece currentPiece;
  Piece nextPiece;
  float fallingTimer;
  float keyTimers[KEY_TIMERS_COUNT];
  float ARETimer;
  float animationTimer;
  int linesCleared;
  int startingLevel;
  int currentLevel;
  int score;
  int softDropCounter;
  Music music[MUSIC_COUNT];
  Sound sounds[SOUND_COUNT];
  int currentMusicIndex;
  bool isPaused;
  bool isMusicPaused;
} GameState;

void GameCleanup(void);
void GameInit(void);
void GameUpdate(void);
void GameDraw(void);

#endif // GAME_H
