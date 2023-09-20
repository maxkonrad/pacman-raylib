#include "raylib.h"
#include "rlgl.h"

#define TURQUOISE \
    (Color) { 72, 209, 204, 255 }
#define DARKRED \
    (Color) { 139, 0, 0, 255 }
#define GAME_WIDTH 20
#define GAME_HEIGHT 23
#define SQUARE_SIZE 30

typedef enum GameScreen
{
    ME,
    TITLE,
    GAMEPLAY,
    ENDING
} GameScreen;
typedef enum PlayScreen
{
    START,
    PAUSE,
    PLAY,
    LOSE,
    WIN
} PlayScreen;
typedef enum GhostMode
{
    LAIR,
    CHASE,
    SCATTER,
    FRIGHTENED
} GhostMode;

typedef enum Direction {
    LEFT,
    DOWN,
    RIGHT,
    UP
} Direction;

GameScreen currentScreen = ME;
PlayScreen currentPlayStatus = START;

const int screenWidth = 1080;
const int screenHeight = 720;

struct Pacman
{
    bool isMoving;
    int xpos;
    int ypos;
    int direction;
};

typedef struct Ghost
{
    int xpos;
    int ypos;
    GhostMode mode;
    bool isOnDot;
    Color color;
} Ghosts;

int DIRECTION_KEYS[] = {
    [LEFT] = KEY_LEFT,
    [RIGHT] = KEY_RIGHT,
    [UP] = KEY_UP,
    [DOWN] = KEY_DOWN,
    
};

Vector2 DIRECTION_MOVES[] = {
    [LEFT] = {-1, 0},
    [RIGHT] = {1, 0},
    [UP] = {0, -1},
    [DOWN] = {0, 1}
    
};

int LEVEL[23][20] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1},
    {1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1},
    {0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0},
    {0, 0, 0, 1, 2, 1, 2, 1, 10, 10, 10, 10, 1, 2, 1, 2, 1, 0, 0, 0},
    {1, 1, 1, 1, 2, 1, 2, 1, 3, 4, 5, 6, 1, 2, 1, 2, 1, 1, 1, 1},
    {1, 0, 0, 0, 2, 2, 2, 1, 9, 9, 9, 9, 1, 2, 2, 2, 0, 0, 0, 1},
    {1, 1, 1, 1, 2, 1, 2, 1, 9, 9, 9, 9, 1, 2, 1, 2, 1, 1, 1, 1},
    {0, 0, 0, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 0, 0, 0},
    {0, 0, 0, 1, 2, 2, 2, 0, 0, 8, 0, 0, 0, 2, 2, 2, 1, 0, 0, 0},
    {1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1},
    {1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1},
    {1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

Vector2 center = {(float)screenWidth / 2, (float)screenHeight / 2};
Vector2 ZerothSqPx = {(float)(screenWidth / 2) - SQUARE_SIZE * GAME_WIDTH / 2, (float)(screenHeight / 2) - SQUARE_SIZE *GAME_HEIGHT / 2};
float radius = 120;
int segments = 0;
float rotation = 90.0f;
float rotation_segment = 0.7f;
float edge_angle = 50.0f;
int rising = 1;

int gap = 50;
int font_size_big = 80;
int font_size_sml = 30;

int lettersCount = 0;
static float alpha = 1.0f;

void DrawBlank(int column, int row);
void DrawWall(int column, int row);
void DrawDot(int column, int row);
void DrawGhost(int column, int row, Ghosts ghost);
void DrawPill(int column, int row);
void DrawPacman(int column, int row);
void DrawLairDoor(int column, int row);

void DrawOutlinedText(const char *text, int posX, int posY, int fontSize, Color color, int outlineSize, Color outlineColor);

int LevelCounter = 0;
int GameScore = 0;
Ghosts Blinky = {11, 10, LAIR, false, RED};
Ghosts Pinky = {10, 10, LAIR, false, PINK};
Ghosts Inky = {9, 10, LAIR, false, ORANGE};
Ghosts Clyde = {8, 10, LAIR, false, TURQUOISE};
struct Pacman pacman = {false, 9, 14, 0};
double time;
int score = 0;

void MeWindow();
void TitleWindow();
void RenderMap();

void UpdateGameplay();
void CheckPlayPause();

void UpdatePacman();

void Update();
void Render();

static int framesCounter = 0;


int main(void)
{
    InitWindow(screenWidth, screenHeight, "Pac-Man");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        Render();

        Update();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void MeWindow()
{
    ClearBackground(BLACK);
    if (((framesCounter / 15) % 2))
        DrawText(TextSubtext("maxkonrad", 0, lettersCount), (screenWidth / 2) - MeasureText("maxkonrad", font_size_big) / 2, screenHeight / 2, font_size_big, Fade(TURQUOISE, alpha));
}

void TitleWindow()
{
    if (rising)
    {
        rotation -= rotation_segment;
        if (rotation <= edge_angle)
            rising = 0;
    }
    else
    {
        rotation += rotation_segment;
        if (rotation >= 90.0f)
            rising = 1;
    }

    ClearBackground(BLACK);

    rlPushMatrix();
    rlTranslatef(center.x, center.y, 0);
    rlRotatef(rotation, 0, 0, -1);
    DrawCircleSector((Vector2){0, 0}, (float)radius, 0, 180, segments, YELLOW);
    DrawCircle((int)radius / 2, -(int)radius / 2, radius / 10, BLACK);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(center.x, center.y, 0);
    rlRotatef(-rotation, 0, 0, -1);
    DrawCircleSector((Vector2){0, 0}, radius, 0, 180, segments, YELLOW);
    rlPopMatrix();

    DrawText("Pac-Man", (int)(screenWidth / 2) - MeasureText("Pac-Man", font_size_big) / 2, (screenHeight / 2) - (int)radius - 2 * gap, font_size_big, DARKBLUE);
    DrawText("Press enter to play!", (int)(screenWidth / 2) - MeasureText("Press enter to play!", font_size_sml) / 2, (screenHeight / 2) + (int)radius + gap / 2, font_size_sml, RED);
}

void Update()
{
    switch (currentScreen)
    {
    case ME:
    {
        framesCounter++;
        if (framesCounter % 15 == 0) // Every 12 frames, one more letter!
        {
            lettersCount++;
        }

        if (lettersCount >= 9) // When all letters have appeared, just fade out everything
        {
            alpha -= 0.01f;

            if (alpha <= 0.0f)
            {
                alpha = 0.0f;
            }
        }
        if (framesCounter == 300)
        {
            currentScreen = TITLE;
            framesCounter = 0;
        }
    }
    break;
    case TITLE:
    {

        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
        {
            currentScreen = GAMEPLAY;
        }
    }
    break;
    case GAMEPLAY:
    {
        UpdateGameplay();
        if (IsKeyPressed(KEY_ESCAPE))
        {
            currentScreen = ENDING;
        }
    }
    break;
    case ENDING:
    {
        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
        {
            currentScreen = TITLE;
        }
    }
    break;
    default:
        break;
    }
}

void Render()
{
    switch (currentScreen)
    {
    case ME:
    {
        MeWindow();
    }
    break;
    case TITLE:
    {
        TitleWindow();
    }
    break;
    case GAMEPLAY:
    {
        RenderMap();
    }
    break;
    case ENDING:
    {
    }
    break;
    default:
        break;
    }
}

void RenderMap()
{
    int i, j;
    ClearBackground(BLACK);
    for (i = 0; i < GAME_WIDTH; i++)
    {
        for (j = 0; j < GAME_HEIGHT; j++)
        {
            switch (LEVEL[j][i])
            {
            case 0:
                DrawBlank(i, j);
                break;
            case 1:
                DrawWall(i, j);
                break;
            case 2:
                DrawDot(i, j);
                break;
            case 3:
                DrawGhost(i, j, Blinky);
                break;
            case 4:
                DrawGhost(i, j, Pinky);
                break;
            case 5:
                DrawGhost(i, j, Inky);
                break;
            case 6:
                DrawGhost(i, j, Clyde);
                break;
            case 7:
                DrawPill(i, j);
                break;
            case 8:
                DrawPacman(i, j);
                break;
            case 9:
                DrawBlank(i, j);
                break;
            case 10:
                DrawLairDoor(i, j);
                break;
            default:
                break;
            }
        }
    }
}

void DrawBlank(int column, int row)
{
    DrawRectangle(ZerothSqPx.x + column * SQUARE_SIZE, ZerothSqPx.y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, BLACK);
}

void DrawWall(int column, int row)
{
    DrawRectangle(ZerothSqPx.x + column * SQUARE_SIZE, ZerothSqPx.y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, DARKRED);
    DrawRectangleLines(ZerothSqPx.x + column * SQUARE_SIZE, ZerothSqPx.y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, RED);
}

void DrawDot(int column, int row)
{
    DrawCircle(ZerothSqPx.x + (SQUARE_SIZE / 2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE / 2) + row * SQUARE_SIZE, SQUARE_SIZE / 8, YELLOW);
}

void DrawGhost(int column, int row, Ghosts ghost)
{
    if (ghost.mode == 3)
        DrawCircle(ZerothSqPx.x + (SQUARE_SIZE / 2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE / 2) + row * SQUARE_SIZE, SQUARE_SIZE / 3, DARKBLUE);
    else
        DrawCircle(ZerothSqPx.x + (SQUARE_SIZE / 2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE / 2) + row * SQUARE_SIZE, SQUARE_SIZE / 3, ghost.color);
}

void DrawPill(int column, int row)
{
    DrawCircle(ZerothSqPx.x + (SQUARE_SIZE / 2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE / 2) + row * SQUARE_SIZE, SQUARE_SIZE / 5, WHITE);
}

void DrawPacman(int column, int row)
{
    if (pacman.isMoving)
    {
        if (rising)
        {
            rotation -= rotation_segment;
            if (rotation <= edge_angle)
                rising = 0;
        }
        else
        {
            rotation += rotation_segment;
            if (rotation >= 90.0f)
                rising = 1;
        }
    }
    rlPushMatrix();
    rlTranslatef(ZerothSqPx.x + (SQUARE_SIZE / 2) + column * SQUARE_SIZE,
                 ZerothSqPx.y + (SQUARE_SIZE / 2) + row * SQUARE_SIZE, 0);
    rlRotatef(rotation + pacman.direction * 90, 0, 0, -1);
    DrawCircleSector((Vector2){0, 0}, (float)SQUARE_SIZE / 3, 0, 180, segments, YELLOW);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(ZerothSqPx.x + (SQUARE_SIZE / 2) + column * SQUARE_SIZE,
                 ZerothSqPx.y + (SQUARE_SIZE / 2) + row * SQUARE_SIZE, 0);
    rlRotatef(-rotation + pacman.direction * 90, 0, 0, -1);
    DrawCircleSector((Vector2){0, 0}, SQUARE_SIZE / 3, 0, 180, segments, YELLOW);
    rlPopMatrix();
}

void DrawLairDoor(int column, int row)
{
    DrawRectangle(ZerothSqPx.x + column * SQUARE_SIZE, ZerothSqPx.y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, YELLOW);
}

void DrawOutlinedText(const char *text, int posX, int posY, int fontSize, Color color, int outlineSize, Color outlineColor)
{
    DrawText(text, posX - outlineSize, posY - outlineSize, fontSize, outlineColor);
    DrawText(text, posX + outlineSize, posY - outlineSize, fontSize, outlineColor);
    DrawText(text, posX - outlineSize, posY + outlineSize, fontSize, outlineColor);
    DrawText(text, posX + outlineSize, posY + outlineSize, fontSize, outlineColor);
    DrawText(text, posX, posY, fontSize, color);
}

void UpdateGameplay()
{
    switch (currentPlayStatus)
    {
    case START:
    {
        DrawOutlinedText("Press Enter to Start the Game!", center.x - MeasureText("Press Enter to Start the Game!", font_size_sml) / 2, center.y, font_size_sml, BLACK, 1, WHITE);
        if (IsKeyPressed(KEY_ENTER)){
            currentPlayStatus = PLAY;
            pacman.isMoving = true;
        }
    }
    break;
    case PAUSE:
    {
        CheckPlayPause();
        DrawOutlinedText("| | GAME PAUSED | |", center.x - MeasureText("| | GAME PAUSED | |", font_size_sml) / 2, center.y, font_size_sml, BLACK, 1, WHITE);
        
    }
    break;
    case PLAY:
    {
        CheckPlayPause();
        UpdatePacman();
        
        // Move Ghosts
        // Check collision with ghosts again
                
        // Check ghost mode
        
        // Check finish
    }
    case LOSE:
    {
    } break;
    
    case WIN:{
        
    } break;
    }
}

void CheckPlayPause()
{
    if (IsKeyPressed(KEY_ENTER))
    {
        if (currentPlayStatus == PLAY){
            currentPlayStatus = PAUSE;
            pacman.isMoving = false;
        }
        else{
            currentPlayStatus = PLAY;
            pacman.isMoving = true;
        }
    }
}

void UpdatePacman() {
    for (Direction dir = LEFT; dir <= UP; dir++) {
        if (IsKeyPressed(DIRECTION_KEYS[dir])) {
            Vector2 move = DIRECTION_MOVES[dir];
            int newX = pacman.xpos + move.x;
            int newY = pacman.ypos + move.y;
            
            int adjacentCell = LEVEL[newY][newX];
            
            switch (adjacentCell) {
                case 3:
                case 4:
                case 5:
                case 6:
                    currentPlayStatus = LOSE;
                    break;
                case 0:
                    LEVEL[pacman.ypos][pacman.xpos] = 0;
                    LEVEL[newY][newX] = 8;
                    pacman.xpos = newX;
                    pacman.ypos = newY;
                    pacman.direction = dir;
                    break;
                case 2:
                    LEVEL[pacman.ypos][pacman.xpos] = 0;
                    LEVEL[newY][newX] = 8;
                    pacman.xpos = newX;
                    pacman.ypos = newY;
                    pacman.direction = dir;
                    GameScore += 200;
                    break;
                case 7:
                    LEVEL[pacman.ypos][pacman.xpos] = 0;
                    LEVEL[newY][newX] = 8;
                    pacman.xpos = newX;
                    pacman.ypos = newY;
                    pacman.direction = dir;
                    GameScore += 500;
                    // TODO: Implement ghost scare mode
                    break;
                default:
                    break;
            }
            break;
        }
    }
}