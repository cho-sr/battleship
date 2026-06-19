#define _CRT_SECURE_NO_WARNINGS
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAP_SIZE 10
#define SHIP_COUNT 3
#define TOTAL_SHIP_CELLS 7
#define NAME_LENGTH 31
#define MAX_RANKINGS 100
#define TOP_RANKINGS 10
#define RANKING_FILE "rankings.txt"

#define WATER '.'
#define SHIP 'S'
#define HIT 'X'
#define MISS 'O'

typedef struct {
    const char *name;
    int size;
} Ship;

typedef struct {
    char name[NAME_LENGTH];
    int score;
} Ranking;

void clearInputBuffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void initializeBoard(char board[MAP_SIZE][MAP_SIZE], char value)
{
    int row, col;

    for (row = 0; row < MAP_SIZE; row++) {
        for (col = 0; col < MAP_SIZE; col++) {
            board[row][col] = value;
        }
    }
}

int isInRange(int row, int col)
{
    return row >= 0 && row < MAP_SIZE && col >= 0 && col < MAP_SIZE;
}

int canPlaceShip(char board[MAP_SIZE][MAP_SIZE], int row, int col, int size, char direction)
{
    int i;
    int nextRow;
    int nextCol;

    if (size > 1 && direction != 'H' && direction != 'V') {
        return 0;
    }

    for (i = 0; i < size; i++) {
        nextRow = row + (direction == 'V' ? i : 0);
        nextCol = col + (direction == 'H' ? i : 0);

        if (!isInRange(nextRow, nextCol)) {
            return 0;
        }

        if (board[nextRow][nextCol] != WATER) {
            return 0;
        }
    }

    return 1;
}

void placeShip(char board[MAP_SIZE][MAP_SIZE], int row, int col, int size, char direction)
{
    int i;
    int nextRow;
    int nextCol;

    for (i = 0; i < size; i++) {
        nextRow = row + (direction == 'V' ? i : 0);
        nextCol = col + (direction == 'H' ? i : 0);
        board[nextRow][nextCol] = SHIP;
    }
}

char getPlayerDisplayCell(char playerBoard[MAP_SIZE][MAP_SIZE],
                          char computerAttackHistory[MAP_SIZE][MAP_SIZE],
                          int row,
                          int col)
{
    if (computerAttackHistory[row][col] == HIT) {
        return HIT;
    }

    if (computerAttackHistory[row][col] == MISS) {
        return MISS;
    }

    return playerBoard[row][col];
}

void printBoards(char playerBoard[MAP_SIZE][MAP_SIZE],
                 char playerAttackHistory[MAP_SIZE][MAP_SIZE],
                 char computerAttackHistory[MAP_SIZE][MAP_SIZE])
{
    int row, col;

    printf("\n");
    printf("Your Board                         Enemy Board\n");
    printf("    ");
    for (col = 0; col < MAP_SIZE; col++) {
        printf("%d ", col);
    }
    printf("          ");
    for (col = 0; col < MAP_SIZE; col++) {
        printf("%d ", col);
    }
    printf("\n");

    for (row = 0; row < MAP_SIZE; row++) {
        printf("%2d  ", row);
        for (col = 0; col < MAP_SIZE; col++) {
            printf("%c ", getPlayerDisplayCell(playerBoard, computerAttackHistory, row, col));
        }

        printf("      %2d  ", row);
        for (col = 0; col < MAP_SIZE; col++) {
            printf("%c ", playerAttackHistory[row][col]);
        }
        printf("\n");
    }

    printf("Legend: S=your ship, X=hit, O=miss, .=water/unknown\n\n");
}

void printPlacementBoard(char board[MAP_SIZE][MAP_SIZE])
{
    int row, col;

    printf("\nYour Board\n");
    printf("    ");
    for (col = 0; col < MAP_SIZE; col++) {
        printf("%d ", col);
    }
    printf("\n");

    for (row = 0; row < MAP_SIZE; row++) {
        printf("%2d  ", row);
        for (col = 0; col < MAP_SIZE; col++) {
            printf("%c ", board[row][col]);
        }
        printf("\n");
    }

    printf("Legend: S=your ship, .=water\n\n");
}

int readPlayerPlacement(const Ship *ship, int *row, int *col, char *direction)
{
    if (ship->size == 1) {
        printf("Place %s. Enter row and column (0-9): ", ship->name);
        if (scanf("%d %d", row, col) != 2) {
            clearInputBuffer();
            printf("Please enter two numbers.\n");
            return 0;
        }
        clearInputBuffer();
        *direction = 'H';
        return 1;
    }

    printf("Place %s. Enter row column direction(H/V): ", ship->name);
    if (scanf("%d %d %c", row, col, direction) != 3) {
        clearInputBuffer();
        printf("Please enter two numbers and one direction.\n");
        return 0;
    }
    clearInputBuffer();

    *direction = (char)toupper((unsigned char)*direction);
    return 1;
}

void placePlayerShips(char board[MAP_SIZE][MAP_SIZE], const Ship ships[SHIP_COUNT])
{
    int i;
    int row;
    int col;
    char direction;

    printf("Place your ships on the 10x10 sea map.\n");
    printf("Coordinates are row and column numbers from 0 to 9.\n\n");

    for (i = 0; i < SHIP_COUNT; i++) {
        while (1) {
            printPlacementBoard(board);

            if (!readPlayerPlacement(&ships[i], &row, &col, &direction)) {
                continue;
            }

            if (!canPlaceShip(board, row, col, ships[i].size, direction)) {
                printf("Invalid placement. Check the map boundary, direction, or overlap.\n");
                continue;
            }

            placeShip(board, row, col, ships[i].size, direction);
            printf("%s placed successfully.\n\n", ships[i].name);
            break;
        }
    }
}

void placeComputerShips(char board[MAP_SIZE][MAP_SIZE], const Ship ships[SHIP_COUNT])
{
    int i;
    int row;
    int col;
    char direction;

    for (i = 0; i < SHIP_COUNT; i++) {
        while (1) {
            row = rand() % MAP_SIZE;
            col = rand() % MAP_SIZE;
            direction = (rand() % 2 == 0) ? 'H' : 'V';

            if (ships[i].size == 1) {
                direction = 'H';
            }

            if (canPlaceShip(board, row, col, ships[i].size, direction)) {
                placeShip(board, row, col, ships[i].size, direction);
                break;
            }
        }
    }
}

void playerAttack(char computerBoard[MAP_SIZE][MAP_SIZE],
                  char playerAttackHistory[MAP_SIZE][MAP_SIZE],
                  int *computerRemaining,
                  int *hitCount)
{
    int row;
    int col;

    while (1) {
        printf("Your attack. Enter row and column (0-9): ");
        if (scanf("%d %d", &row, &col) != 2) {
            clearInputBuffer();
            printf("Please enter two numbers.\n");
            continue;
        }
        clearInputBuffer();

        if (!isInRange(row, col)) {
            printf("That coordinate is outside the map.\n");
            continue;
        }

        if (playerAttackHistory[row][col] != WATER) {
            printf("You already attacked that coordinate. Choose another one.\n");
            continue;
        }

        break;
    }

    if (computerBoard[row][col] == SHIP) {
        printf("Hit!\n");
        computerBoard[row][col] = HIT;
        playerAttackHistory[row][col] = HIT;
        (*computerRemaining)--;
        (*hitCount)++;
    } else {
        printf("Miss!\n");
        playerAttackHistory[row][col] = MISS;
    }
}

void computerAttack(char playerBoard[MAP_SIZE][MAP_SIZE],
                    char computerAttackHistory[MAP_SIZE][MAP_SIZE],
                    int *playerRemaining)
{
    int row;
    int col;

    do {
        row = rand() % MAP_SIZE;
        col = rand() % MAP_SIZE;
    } while (computerAttackHistory[row][col] != WATER);

    printf("Computer attacks (%d, %d): ", row, col);

    if (playerBoard[row][col] == SHIP) {
        printf("Hit!\n");
        playerBoard[row][col] = HIT;
        computerAttackHistory[row][col] = HIT;
        (*playerRemaining)--;
    } else {
        printf("Miss!\n");
        computerAttackHistory[row][col] = MISS;
    }
}

int calculateScore(int turnCount, int hitCount)
{
    return 1000 - turnCount * 10 + hitCount * 50;
}

void sortRankings(Ranking rankings[], int count)
{
    int i, j;
    Ranking temp;

    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count - i - 1; j++) {
            if (rankings[j].score < rankings[j + 1].score) {
                temp = rankings[j];
                rankings[j] = rankings[j + 1];
                rankings[j + 1] = temp;
            }
        }
    }
}

int loadRankings(Ranking rankings[])
{
    FILE *file;
    int count = 0;

    file = fopen(RANKING_FILE, "r");
    if (file == NULL) {
        return 0;
    }

    while (count < MAX_RANKINGS &&
           fscanf(file, "%30s %d", rankings[count].name, &rankings[count].score) == 2) {
        count++;
    }

    fclose(file);
    return count;
}

void saveTopRankings(Ranking rankings[], int count)
{
    FILE *file;
    int i;
    int limit = count < TOP_RANKINGS ? count : TOP_RANKINGS;

    file = fopen(RANKING_FILE, "w");
    if (file == NULL) {
        printf("Could not save rankings.\n");
        return;
    }

    for (i = 0; i < limit; i++) {
        fprintf(file, "%s %d\n", rankings[i].name, rankings[i].score);
    }

    fclose(file);
}

void printRankings(Ranking rankings[], int count)
{
    int i;
    int limit = count < TOP_RANKINGS ? count : TOP_RANKINGS;

    printf("\n===== Top 10 Rankings =====\n");
    if (limit == 0) {
        printf("No rankings yet.\n");
        return;
    }

    for (i = 0; i < limit; i++) {
        printf("%2d. %-30s %5d\n", i + 1, rankings[i].name, rankings[i].score);
    }
}

void addRankingAndPrint(const char *name, int score)
{
    Ranking rankings[MAX_RANKINGS + 1];
    int count;

    count = loadRankings(rankings);
    strncpy(rankings[count].name, name, NAME_LENGTH - 1);
    rankings[count].name[NAME_LENGTH - 1] = '\0';
    rankings[count].score = score;
    count++;

    sortRankings(rankings, count);
    saveTopRankings(rankings, count);
    printRankings(rankings, count);
}

int main(void)
{
    const Ship ships[SHIP_COUNT] = {
        {"Scout (1 cell)", 1},
        {"Destroyer (2 cells)", 2},
        {"Battleship (4 cells)", 4}
    };

    char playerBoard[MAP_SIZE][MAP_SIZE];
    char computerBoard[MAP_SIZE][MAP_SIZE];
    char playerAttackHistory[MAP_SIZE][MAP_SIZE];
    char computerAttackHistory[MAP_SIZE][MAP_SIZE];
    char playerName[NAME_LENGTH];
    int playerRemaining = TOTAL_SHIP_CELLS;
    int computerRemaining = TOTAL_SHIP_CELLS;
    int turnCount = 0;
    int hitCount = 0;
    int score;

    srand((unsigned int)time(NULL));

    initializeBoard(playerBoard, WATER);
    initializeBoard(computerBoard, WATER);
    initializeBoard(playerAttackHistory, WATER);
    initializeBoard(computerAttackHistory, WATER);

    printf("===== Battleship Console Game =====\n\n");

    placePlayerShips(playerBoard, ships);
    placeComputerShips(computerBoard, ships);
    printf("Computer ships are ready. Battle start!\n");

    while (playerRemaining > 0 && computerRemaining > 0) {
        printf("\nTurn %d\n", turnCount + 1);
        printf("Your ship cells: %d | Enemy ship cells: %d | Your hits: %d\n",
               playerRemaining, computerRemaining, hitCount);
        printBoards(playerBoard, playerAttackHistory, computerAttackHistory);

        playerAttack(computerBoard, playerAttackHistory, &computerRemaining, &hitCount);
        turnCount++;

        if (computerRemaining <= 0) {
            break;
        }

        computerAttack(playerBoard, computerAttackHistory, &playerRemaining);
    }

    printBoards(playerBoard, playerAttackHistory, computerAttackHistory);

    if (computerRemaining <= 0) {
        printf("You win! All enemy ship cells were destroyed.\n");
    } else {
        printf("You lose. The computer destroyed all your ship cells.\n");
    }

    score = calculateScore(turnCount, hitCount);
    printf("Final score: %d\n", score);
    printf("Turns used: %d | Hits: %d\n", turnCount, hitCount);

    printf("Enter your name for the ranking (no spaces, max 30 chars): ");
    if (scanf("%30s", playerName) != 1) {
        strcpy(playerName, "Player");
    }

    addRankingAndPrint(playerName, score);

    return 0;
}
