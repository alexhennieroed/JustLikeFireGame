//Structs
typedef struct {
    int row;
    int col;
    int rd;
    int cd;
    u16 color;
    int size;
} FIRE;

typedef struct {
    int row;
    int col;
    int rd;
    int cd;
    u16 color;
    int size;
} PLAYER;

//Prototypes
void drawRectangle(int row, int col, int height, int width, u16 color);
void waitForVblank();
void fillScreen(u16 color);
void setPixel(int row, int col, u16 color);
void drawImage3(int row, int col, int height, int width, const u16* image);
void getItStartedHot();
int gameLoop(int level);
