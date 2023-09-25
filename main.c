#include "raylib.h"
#include "rlgl.h"

#define TURQUOISE \
    (Color) { 72, 209, 204, 255 }
#define DARKRED \
    (Color) { 139, 0, 0, 255 }
#define GAME_WIDTH 20
#define GAME_HEIGHT 23
#define SQUARE_SIZE 30

typedef enum MapNumbers
{
    BLANKSPACE = 0,
    WALL,
    DOT,
    BLINKY,
    PINKY,
    INKY,
    CLYDE,
    PILL,
    PACMAN,
    LAIR
} MapNumbers;

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
    CHASE,
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
    Color color;
    int nextDirection;
    MapNumbers num;
    MapNumbers lastNum;
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
    {1, 7, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 7, 1},
    {1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1},
    {1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1},
    {1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1},
    {1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1},
    {0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0},
    {0, 0, 0, 1, 2, 1, 2, 1, 9, 9, 9, 9, 1, 2, 1, 2, 1, 0, 0, 0},
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
    {1, 7, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 7, 1},
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
int i, j;

int lettersCount = 0;
static float alpha = 1.0f;
int abs(int n);
int ghostMoveRandomPossibility = 15;

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
Ghosts ghosts[4] = {
    {11, 10, CHASE, RED, BLINKY, LAIR},
    {10, 10, CHASE, PINK, PINKY, LAIR},
    {9, 10, CHASE, ORANGE, INKY, LAIR},
    {8, 10, CHASE, TURQUOISE, CLYDE, LAIR}
};
struct Pacman pacman = {false, 9, 14, 0};

int score = 0;
double time;
double start_time;
double passed_time;
double frightened_time;
double frightened_start_time;

void MeWindow();
void TitleWindow();
void RenderMap();

void UpdateGameplay();
void CheckPlayPause();
void CheckGhostsFrightened();
void MoveGhost(Ghosts ghost, int randomPercent);
void UpdatePacman();
void UpdateGhosts();

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
    ClearBackground(BLACK);
    for (i = 0; i < GAME_WIDTH; i++)
    {
        for (j = 0; j < GAME_HEIGHT; j++)
        {
            switch (LEVEL[j][i])
            {
            case BLANKSPACE:
                DrawBlank(i, j);
                break;
            case WALL:
                DrawWall(i, j);
                break;
            case DOT:
                DrawDot(i, j);
                break;
            case BLINKY:
                DrawGhost(i, j, ghosts[BLINKY - 3]);
                break;
            case PINKY:
                DrawGhost(i, j, ghosts[PINKY - 3]);
                break;
            case INKY:
                DrawGhost(i, j, ghosts[INKY - 3]);
                break;
            case CLYDE:
                DrawGhost(i, j, ghosts[CLYDE - 3]);
                break;
            case PILL:
                DrawPill(i, j);
                break;
            case PACMAN:
                DrawPacman(i, j);
                break;
            case LAIR:
                DrawBlank(i, j);
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
    if (ghost.mode == FRIGHTENED)
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
            start_time = GetTime();
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
        time = GetTime();
        passed_time = time - start_time;
        CheckPlayPause();
        UpdatePacman();
        CheckGhostsFrightened();
        // Move Ghosts
        UpdateGhosts();
        // Check collision with ghosts again
        
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
                case BLINKY:
                case PINKY:
                case INKY:
                case CLYDE:
                    currentPlayStatus = LOSE;
                    break;
                case BLANKSPACE:
                    LEVEL[pacman.ypos][pacman.xpos] = 0;
                    LEVEL[newY][newX] = 8;
                    pacman.xpos = newX;
                    pacman.ypos = newY;
                    pacman.direction = dir;
                    break;
                case DOT:
                    LEVEL[pacman.ypos][pacman.xpos] = 0;
                    LEVEL[newY][newX] = 8;
                    pacman.xpos = newX;
                    pacman.ypos = newY;
                    pacman.direction = dir;
                    GameScore += 200;
                    break;
                case PILL:
                    LEVEL[pacman.ypos][pacman.xpos] = 0;
                    LEVEL[newY][newX] = 8;
                    pacman.xpos = newX;
                    pacman.ypos = newY;
                    pacman.direction = dir;
                    GameScore += 500;
                    frightened_start_time = GetTime();
                    for (int c = 0; c < 4; c++){
                        ghosts[c].mode = FRIGHTENED;
                    }
                    
                    break;
                default:
                    break;
            }
            break;
        }
    }
}

void CheckGhostsFrightened(){
    if (ghosts[0].mode == FRIGHTENED){
        frightened_time = time - frightened_start_time;
        if (frightened_time >= 4){
            for (int c = 0; c < 4; c++){
                ghosts[c].mode = CHASE;
            }
        }
    }
}

void UpdateGhosts(){
    if (!((int) time % 2) && time == (int) time){
        for (int c = 0; c < 4; c++){
                MoveGhost(ghosts[c], GetRandomValue(1,100));
            }
    }
}

void MoveGhost(Ghosts ghost, int randomPercent){
    Vector2 move;
    if (randomPercent < ghostMoveRandomPossibility){
        int dir = GetRandomValue(LEFT, UP);
        move = DIRECTION_MOVES[dir];       
    } else {
        if ((abs(pacman.xpos - ghost.xpos) - abs(pacman.ypos - ghost.ypos)) >= 0){
            if ((pacman.ypos - ghost.ypos) > 0){
                move = DIRECTION_MOVES[UP];
            } else {
                move = DIRECTION_MOVES[DOWN];
            }
        } else{
            if ((pacman.xpos - ghost.xpos) > 0){
                move = DIRECTION_MOVES[RIGHT];
            } else {
                move = DIRECTION_MOVES[LEFT];
            }
        }
    }
    if (ghost.mode == FRIGHTENED) {
        move.x *= -1;
        move.y *= -1;
    }
    
    int newX = ghost.xpos + move.x;
    int newY = ghost.ypos + move.y;
    int nextCell = LEVEL[newY][newX];
    
    switch (nextCell) {
                case 8:
                    currentPlayStatus = LOSE;
                    break;
                case 0:
                case 2:
                case 7:
                    LEVEL[ghost.ypos][ghost.xpos] = ghost.lastNum;
                    LEVEL[newY][newX] = ghost.num;
                    ghost.xpos = newX;
                    ghost.ypos = newY;
                    break;
                default:
                    break;
            }
}

int abs(int n) {
    return (n < 0) ? -n : n;
}