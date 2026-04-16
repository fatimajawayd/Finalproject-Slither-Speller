#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <ctype.h>

#define WIDTH 100
#define HEIGHT 30
#define MAX_SNAKE 600
#define MAX_LETTERS 40
#define DICT_SIZE 40
#define WORDS_REQUIRED 6     
#define MAX_STRIKES 3

typedef struct { int x, y; } Point;

char board[HEIGHT][WIDTH+1];
Point snake[MAX_SNAKE];
int snakeLen;
int dirX, dirY;

char collected[4];
int collectedCount = 0;

char lettersOnBoard[MAX_LETTERS];
Point lettersPos[MAX_LETTERS];
int lettersCount = 0;

int score = 0;
int strikes = 0;
int correctWords = 0;

char message[128] = "";
char lastAscii[128] = "";

const char dict[DICT_SIZE][4] = {
    "CAT","DOG","SUN","MAP","CAR","RUN","HAT","BAT","RAT","MAT",
    "CAN","PAN","FAN","MAN","CUP","BOX","KEY","SKY","BUG","ANT",
    "EAT","BAR","EAR","ICE","OAK","OWL","FOX","JAM","LOG","NET",
    "PEN","RUG","SAP","TUB","VAN","WAX","YAK","ZAP","GEM","BEE"
};

const char *positiveMsgs[] = {
    "Snake: Nice!",
    "Snake: Good one!",
    "Snake: Correct!",
    "Snake: You're smart!"
};

const char *negativeMsgs[] = {
    "Snake: Wrong!",
    "Snake: Hiss... incorrect!",
    "Snake: Strike!",
    "Snake: Try again!"
};

const char *asciiPositive =
"  /\\_/\\\n ( ^.^ ) Good!\n  \\   /\n";

const char *asciiNegative =
" (\\_/)\n ( o.o ) Hiss!\n /   \\\n";

void initBoard(){
    for(int y=0;y<HEIGHT;y++){
        for(int x=0;x<WIDTH;x++){
            board[y][x] = (y==0||y==HEIGHT-1||x==0||x==WIDTH-1) ? '#' : ' ';
        }
        board[y][WIDTH] = '\0';
    }
}

void initSnake(){
    snakeLen = 3;
    int cx = WIDTH/2;
    int cy = HEIGHT/2;
    for(int i=0;i<snakeLen;i++){
        snake[i].x = cx - i;
        snake[i].y = cy;
    }
    dirX = 1;
    dirY = 0;
}

int collidesWithSnake(int x,int y,int ignore){
    for(int i=0;i<snakeLen;i++){
        if(i!=ignore && snake[i].x==x && snake[i].y==y)
            return 1;
    }
    return 0;
}

void placeRandomLetter(){
    if(lettersCount >= MAX_LETTERS) return;

    int rx, ry;
    do {
        rx = 1 + rand()%(WIDTH-2);
        ry = 1 + rand()%(HEIGHT-2);
    } while(collidesWithSnake(rx, ry, -1));

    lettersPos[lettersCount].x = rx;
    lettersPos[lettersCount].y = ry;

    int r = rand() % DICT_SIZE;
    lettersOnBoard[lettersCount] = dict[r][rand()%3];

    lettersCount++;
}

void resetLetters(){
    lettersCount = 0;
    for(int i=0;i<20;i++)
        placeRandomLetter();
}

void removeLetter(int idx){
    for(int i=idx;i<lettersCount-1;i++){
        lettersPos[i] = lettersPos[i+1];
        lettersOnBoard[i] = lettersOnBoard[i+1];
    }
    lettersCount--;
}

void render(){
    initBoard();

    for(int i=0;i<lettersCount;i++)
        board[lettersPos[i].y][lettersPos[i].x] = lettersOnBoard[i];

    board[snake[0].y][snake[0].x] = 'O';
    for(int i=1;i<snakeLen;i++)
        board[snake[i].y][snake[i].x] = 'o';

    system("cls");
    printf("=== SlitherSpeller ===\n");
    printf("Score: %d | Correct words: %d/%d | Strikes: %d/%d\n",
           score, correctWords, WORDS_REQUIRED, strikes, MAX_STRIKES);

    printf("Collected: ");
    for(int i=0;i<collectedCount;i++) printf("%c", collected[i]);
    for(int i=collectedCount;i<3;i++) printf("_");
    printf("   (Press ENTER to submit)\n\n");

    for(int y=0;y<HEIGHT;y++)
        printf("%s\n", board[y]);

    if(message[0]) printf("\n%s\n", message);
    if(lastAscii[0]) printf("%s\n", lastAscii);
}

int inDictionary(char *w){
    for(int i=0;i<DICT_SIZE;i++)
        if(strcmp(w, dict[i]) == 0)
            return 1;
    return 0;
}

void clearCollected(){
    collectedCount = 0;
    collected[0] = '\0';
}

int updateSnake(){
    int nx = snake[0].x + dirX;
    int ny = snake[0].y + dirY;

    if(nx<=0||nx>=WIDTH-1||ny<=0||ny>=HEIGHT-1)
        return 1;
    if(collidesWithSnake(nx,ny,snakeLen-1))
        return 1;

    for(int i=snakeLen-1;i>0;i--)
        snake[i] = snake[i-1];

    snake[0].x = nx;
    snake[0].y = ny;

    for(int i=0;i<lettersCount;i++){
        if(lettersPos[i].x==nx && lettersPos[i].y==ny){
            if(snakeLen<MAX_SNAKE){
                snake[snakeLen] = snake[snakeLen-1];
                snakeLen++;
            }
            if(collectedCount<3){
                collected[collectedCount++] = toupper(lettersOnBoard[i]);
                collected[collectedCount] = '\0';
            }

            removeLetter(i);
            placeRandomLetter();
            break;
        }
    }

    return 0;
}

void gameLoop(){
    resetLetters();

    while(1){
        if(_kbhit()){
            int c = _getch();

            if(c==13 && collectedCount==3){
                char w[4] = {
                    toupper(collected[0]),
                    toupper(collected[1]),
                    toupper(collected[2]),
                    '\0'
                };

                if(inDictionary(w)){
                    correctWords++;
                    score += 10;
                    strcpy(message, positiveMsgs[rand()%4]);
                    strcpy(lastAscii, asciiPositive);
                    clearCollected();

                    if(correctWords >= WORDS_REQUIRED){
                        render();
                        printf("\nYou found %d correct words! YOU WIN!\n", WORDS_REQUIRED);
                        _getch();
                        exit(0);
                    }
                }
                else {
                    strikes++;
                    strcpy(message, negativeMsgs[rand()%4]);
                    strcpy(lastAscii, asciiNegative);
                    clearCollected();

                    if(strikes >= MAX_STRIKES){
                        render();
                        printf("\n3 strikes! ROUND FAILED.\n");
                        _getch();
                        exit(0);
                    }
                }
            }

            if(c==0 || c==224){
                int k=_getch();
                if(k==72 && dirY!=1){dirX=0; dirY=-1;}
                else if(k==80 && dirY!=-1){dirX=0; dirY=1;}
                else if(k==75 && dirX!=1){dirX=-1; dirY=0;}
                else if(k==77 && dirX!=-1){dirX=1; dirY=0;}
            }
        }

        if(updateSnake()){
            printf("\nYou crashed! Final Score: %d\n", score);
            _getch();
            return;
        }

        render();
        Sleep(150);
    }
}

int main(){
    srand(time(NULL));

    initBoard();
    initSnake();

    system("cls");
    printf("Welcome to SlitherSpeller!\n");
    printf("Collect letters ? Press ENTER to submit a word.\n");
    printf("Find %d correct words to win.\n", WORDS_REQUIRED);
    printf("Make %d wrong submissions ? you lose.\n\n", MAX_STRIKES);
    printf("Press any key to start...");
    _getch();

    gameLoop();
    return 0;
}