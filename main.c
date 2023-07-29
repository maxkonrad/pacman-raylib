#include "raylib.h"
#include "rlgl.h"

#define TURQUOISE (Color) {72, 209, 204, 255}

typedef enum GameScreen { ME = 0, TITLE, GAMEPLAY, ENDING } GameScreen;

GameScreen currentScreen = ME;

const int screenWidth = 1080;
const int screenHeight = 720;

Vector2 center = { (float)screenWidth / 2, (float)screenHeight / 2 };
float radius = 120;
int segments = 0;
float rotation = 90.0f;
float rotation_segment = 0.6f;
float edge_angle = 65.0f;
int rising = 1;

int gap = 50;
int font_size_big = 60;
int font_size_sml = 20;

static int lettersCount = 0;
static float alpha = 1.0f;

void MeWindow();
void TitleWindow();
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
    ClearBackground(TURQUOISE);
    if (((framesCounter/15)%2) && framesCounter < 120) DrawRectangle((screenWidth/2) - MeasureText(".maxkonrad", font_size_big) / 2, screenHeight/2, 32, 32, BLACK);
    DrawText(TextSubtext(" maxkonrad", 0, lettersCount), (screenWidth/2) - MeasureText(".maxkonrad", font_size_big), screenHeight/2, font_size_big, Fade(BLACK, alpha));
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
    DrawText("Press enter to play!",(int) (screenWidth/2) - MeasureText("Press enter to play!", font_size_sml)/2, (screenHeight/2) + (int) radius + gap, font_size_sml, RED);
}

void Update(){
    switch(currentScreen)
    {
        case ME:
        {
            framesCounter++;
            if (framesCounter/12)       // Every 12 frames, one more letter!
            {
                lettersCount++;
                framesCounter = 0;
            }

            if (lettersCount >= 9)     // When all letters have appeared, just fade out everything
            {
                alpha -= 0.02f;

                if (alpha <= 0.0f) {
                    alpha = 0.0f;

                }
            }
            if (framesCounter > 360)
            {
                currentScreen = TITLE;
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
            if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
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


        } break;
        case GAMEPLAY:
        {
            TitleWindow();

        } break;
        case ENDING:
        {
            // TODO: Draw ENDING screen here!
            DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
            DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
            DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);

        } break;
        default: break;
    }
}
