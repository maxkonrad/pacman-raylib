#include "raylib.h"
#include "rlgl.h"

#define TURQUOISE (Color) {72, 209, 204, 255}
#define DARKRED (Color) {139, 0, 0, 255}
#define GAME_WIDTH 20
#define GAME_HEIGHT 23
#define SQUARE_SIZE 30


typedef enum GameScreen { ME , TITLE, GAMEPLAY, ENDING } GameScreen;
typedef enum PlayScreen {STOP, PLAY, LOSE} PlayScreen;
typedef enum GhostMode {LAIR, CHASE, SCATTER, FRIGHTENED} GhostMode;

GameScreen currentScreen = ME;
PlayScreen currentPlayStatus = STOP;

const int screenWidth = 1080;
const int screenHeight = 720;

struct Pacman {
    int xpos;
    int ypos;
    int direction;
};

typedef struct Ghost {
    int xpos;
    int ypos;
    GhostMode mode;
    int isOnDot;
} Ghosts;



int LEVEL[23][20] = {
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
        { 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
        { 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1 },
        { 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1 },
        { 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
        { 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1 },
        { 1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1 },
        { 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1 },
        { 0, 0, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 2, 1, 2, 1, 10, 10, 10, 10, 1, 2, 1, 2, 1, 0, 0, 0 },
        { 1, 1, 1, 1, 2, 1, 2, 1, 9, 9, 9, 9, 1, 2, 1, 2, 1, 1, 1, 1 },
        { 1, 0, 0, 0, 2, 2, 2, 1, 9, 9, 9, 9, 1, 2, 2, 2, 0, 0, 0, 1 },
        { 1, 1, 1, 1, 2, 1, 2, 1, 9, 9, 9, 9, 1, 2, 1, 2, 1, 1, 1, 1 },
        { 0, 0, 0, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 0, 0, 0 },
        { 0, 0, 0, 1, 2, 2, 2, 0, 3, 8, 4, 5, 6, 2, 2, 2, 1, 0, 0, 0 },
        { 1, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1 },
        { 1, 2, 2, 2, 2, 1, 2, 2, 2, 1, 1, 2, 2, 2, 1, 2, 2, 2, 2, 1 },
        { 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1 },
        { 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
        { 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1 },
        { 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1, 2, 1 },
        { 1, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 1 },
        { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
};

Vector2 center = { (float)screenWidth / 2, (float)screenHeight / 2 };
Vector2 ZerothSqPx = {(float) (screenWidth / 2) - SQUARE_SIZE * GAME_WIDTH / 2, (float) (screenHeight / 2) - SQUARE_SIZE * GAME_HEIGHT / 2};
float radius = 120;
int segments = 0;
float rotation = 90.0f;
float rotation_segment = 0.6f;
float edge_angle = 65.0f;
int rising = 1;

int gap = 50;
int font_size_big = 80;
int font_size_sml = 30;

int lettersCount = 0;
static float alpha = 1.0f;

int GhostStatus = 0;
int PacmanDirection = 0;

void DrawBlank(int column, int row);
void DrawWall(int column, int row);
void DrawDot(int column, int row);
void DrawBlinky(int column , int row);
void DrawPinky(int column , int row);
void DrawInky(int column , int row);
void DrawClyde(int column , int row);
void DrawPill(int column, int row);
void DrawPacman(int column, int row);
void DrawLairDoor(int column, int row);

int LevelCounter = 0;
Ghosts Blinky = {11, 10, LAIR, 0};
Ghosts Pinky = {10, 10, LAIR, 0};
Ghosts Inky = {9, 10, LAIR, 0};
Ghosts Clyde = {8, 10, LAIR, 0};
struct Pacman pacman = {9, 14, 0};
double time;
int score = 0;

void MeWindow();
void TitleWindow();
void RenderGameplay();

void UpdateGameplay();


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
        Update();

        Render();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void MeWindow(){
    ClearBackground(BLACK);
    if (((framesCounter/15)%2)) DrawText(TextSubtext("maxkonrad", 0, lettersCount), (screenWidth/2) - MeasureText("maxkonrad", font_size_big)/2, screenHeight/2, font_size_big, Fade(TURQUOISE, alpha));
}

void TitleWindow(){
    if (rising){
        rotation -= rotation_segment;
        if (rotation <= edge_angle) rising = 0;
    } else{
        rotation += rotation_segment;
        if (rotation >= 90.0f) rising = 1;
    }

    ClearBackground(BLACK);

    rlPushMatrix();
    rlTranslatef(center.x, center.y, 0);
    rlRotatef(rotation, 0, 0, -1);
    DrawCircleSector((Vector2){0, 0}, (float) radius, 0, 180, segments, YELLOW);
    DrawCircle((int) radius/2, - (int) radius/2, radius/10, BLACK);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(center.x, center.y, 0);
    rlRotatef(-rotation, 0, 0, -1);
    DrawCircleSector((Vector2){0, 0}, radius, 0, 180, segments, YELLOW);
    rlPopMatrix();

    DrawText("Pac-Man", (int) (screenWidth/2) - MeasureText("Pac-Man", font_size_big)/2,  (screenHeight/2) - (int) radius - 2 * gap, font_size_big, DARKBLUE);
    DrawText("Press enter to play!",(int) (screenWidth/2) - MeasureText("Press enter to play!", font_size_sml)/2, (screenHeight/2) + (int) radius + gap / 2, font_size_sml, RED);
}

void Update(){
    switch(currentScreen)
    {
        case ME:
        {
            framesCounter++;
            if (framesCounter%15 == 0)       // Every 12 frames, one more letter!
            {
                lettersCount++;
            }

            if (lettersCount >= 9)     // When all letters have appeared, just fade out everything
            {
                alpha -= 0.01f;

                if (alpha <= 0.0f) {
                    alpha = 0.0f;

                }
            }
            if (framesCounter == 300)
            {
                currentScreen = TITLE;
                framesCounter = 0;
            }
        } break;
        case TITLE:
        {

            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                currentScreen = GAMEPLAY;
            }
        } break;
        case GAMEPLAY:
        {
            UpdateGameplay();
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = ENDING;
            }
        } break;
        case ENDING:
        {
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
            {
                currentScreen = TITLE;
            }
        } break;
        default: break;
    }
}

void Render(){
    switch(currentScreen)
    {
        case ME:
        {
            MeWindow();
        } break;
        case TITLE:
        {
            TitleWindow();
        } break;
        case GAMEPLAY:
        {
            RenderGameplay();
        } break;
        case ENDING:
        {

        } break;
        default: break;
    }
}

void RenderGameplay(){
    int i, j;
    ClearBackground(BLACK);
    for (i = 0; i < GAME_WIDTH; i++){
        for (j = 0; j < GAME_HEIGHT; j++){
            if (LEVEL[j][i] == 0) DrawBlank(i, j);
            if (LEVEL[j][i] == 1) DrawWall(i, j);
            if (LEVEL[j][i] == 2) DrawDot(i, j);
            if (LEVEL[j][i] == 3) DrawBlinky(i, j);
            if (LEVEL[j][i] == 4) DrawPinky(i, j);
            if (LEVEL[j][i] == 5) DrawInky(i, j);
            if (LEVEL[j][i] == 6) DrawClyde(i, j);
            if (LEVEL[j][i] == 7) DrawPill(i, j);
            if (LEVEL[j][i] == 8) DrawPacman(i, j);
            if (LEVEL[j][i] == 9) DrawBlank(i, j);
            if (LEVEL[j][i] == 10) DrawLairDoor(i, j);
        }
    }
}

void DrawBlank(int column, int row){
    DrawRectangle(ZerothSqPx.x + column * SQUARE_SIZE, ZerothSqPx.y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, BLACK);
}

void DrawWall(int column, int row){
    DrawRectangle(ZerothSqPx.x + column * SQUARE_SIZE, ZerothSqPx.y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, DARKRED);
    DrawRectangleLines(ZerothSqPx.x + column * SQUARE_SIZE, ZerothSqPx.y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, RED);
}

void DrawDot(int column, int row){
    DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/8, YELLOW);
}

void DrawBlinky(int column , int row){
    if (GhostStatus == 1) DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/3, DARKBLUE);
    else DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/3, RED);
}

void DrawPinky(int column , int row){
    if (GhostStatus == 1) DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/3, DARKBLUE);
    else DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/3, PINK);
}

void DrawInky(int column , int row){
    if (GhostStatus == 1) DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/3, DARKBLUE);
    else DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/3, TURQUOISE);
}

void DrawClyde(int column , int row){
    if (GhostStatus == 1) DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/3, DARKBLUE);
    else DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/3, ORANGE);
}

void DrawPill(int column, int row){
    DrawCircle(ZerothSqPx.x + (SQUARE_SIZE/2) + column * SQUARE_SIZE, ZerothSqPx.y + (SQUARE_SIZE/2) + row * SQUARE_SIZE, SQUARE_SIZE/5, WHITE);
}

void DrawPacman(int column, int row){
    if (rising){
        rotation -= rotation_segment;
        if (rotation <= edge_angle) rising = 0;
    } else {
        rotation += rotation_segment;
        if (rotation >= 90.0f) rising = 1;
    }

    rlPushMatrix();
    rlTranslatef(ZerothSqPx.x + (SQUARE_SIZE / 2) + column * SQUARE_SIZE,
                 ZerothSqPx.y + (SQUARE_SIZE / 2) + row * SQUARE_SIZE, 0);
    rlRotatef(rotation + PacmanDirection * 90, 0, 0, -1);
    DrawCircleSector((Vector2) {0, 0}, (float) SQUARE_SIZE / 3, 0, 180, segments, YELLOW);
    rlPopMatrix();

    rlPushMatrix();
    rlTranslatef(ZerothSqPx.x + (SQUARE_SIZE / 2) + column * SQUARE_SIZE,
                 ZerothSqPx.y + (SQUARE_SIZE / 2) + row * SQUARE_SIZE, 0);
    rlRotatef(-rotation + PacmanDirection * 90, 0, 0, -1);
    DrawCircleSector((Vector2) {0, 0}, SQUARE_SIZE / 3, 0, 180, segments, YELLOW);
    rlPopMatrix();
}

void DrawLairDoor(int column, int row){
    DrawRectangle(ZerothSqPx.x + column * SQUARE_SIZE, ZerothSqPx.y + row * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, YELLOW);
}

void UpdateGameplay(){
    switch (currentPlayStatus) {
        case STOP:
        {
            if (IsKeyPressed(KEY_ENTER)){
                currentPlayStatus = PLAY;
            }
        } break;
        case PLAY:
        {


            if(IsKeyPressed(KEY_ENTER)){
                currentPlayStatus = STOP;
            }
        }
        case LOSE:
        {

        }
    }
}

void UpdatePacman(){
    if(IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT)){
        if(LEVEL[pacman.ypos][pacman.xpos - 1] == 3) currentPlayStatus = LOSE;
        if(LEVEL[pacman.ypos][pacman.xpos - 1] == 4) currentPlayStatus = LOSE;
        if(LEVEL[pacman.ypos][pacman.xpos - 1] == 5) currentPlayStatus = LOSE;
        if(LEVEL[pacman.ypos][pacman.xpos - 1] == 6) currentPlayStatus = LOSE;
        if(LEVEL[pacman.ypos][pacman.xpos - 1] == 0) {
            LEVEL[pacman.ypos][pacman.xpos] = 0;
            LEVEL[pacman.ypos][pacman.xpos - 1] = 8;
            pacman.xpos = pacman.xpos - 1;
            pacman.direction = 0;
        }
    }
}

