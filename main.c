#include <stdio.h>
#include <stdlib.h>

#define INPUT_FILENAME "input.txt"

typedef struct tile{
    int row;
    int col;
    int wall;
    struct tile *neighbors[4];
}tile;

tile *tilesMap[256][256];
int nrows;
int ncols;

tile *getTile(int row, int col)
{
    tile* thisTile = tilesMap[row][col];
    if(!thisTile){
        thisTile = (tile *) calloc(1, sizeof(tile));
        thisTile->col = col;
        thisTile->row = row;
        tilesMap[row][col] = thisTile;
    }
    return thisTile;
}

typedef enum 
{ 
    RIGHT, 
    DOWN, 
    LEFT, 
    UP, 
    NDIRECTIONS
}DIRECTION;

typedef struct player{
    tile* position;
    DIRECTION dir;
}player;

char line[6000];

int loadInputFile(char *filename);
int nextLine(char *dst);

void readBoard()
{
    for(int len = nextLine(line); len > 1; len = nextLine(line))
    {
        nrows++;
        if(len > ncols)
            ncols = len;
        
        int j = 0; 
        while(line[j] < '#') //empty tile
            j++;
        tile *firsTile = getTile(nrows, j+1);
        tile *currTile = firsTile;
        currTile->wall = (line[j]=='#');
        tile *prevTile;
        
        for(j++; j < len && line[j]>= '#'; j++){
            prevTile = currTile;
            currTile = getTile(nrows, j+1);
            currTile->wall = (line[j]=='#');
            currTile->neighbors[LEFT] = prevTile;
            prevTile->neighbors[RIGHT] = currTile;
        }
        currTile->neighbors[RIGHT] = firsTile;
        firsTile->neighbors[LEFT] = currTile;
    }

    for(int j = 1; j <= ncols; j++)
    {
        int i = 1;
        tile* firstTile;
        while (!(firstTile = tilesMap[i][j]) && i <= nrows)
            i++;
        if(i > nrows) continue; //empty column

        tile* currTile = firstTile;
        tile* prevTile;
        for(i++; tilesMap[i][j]; i++)
        {
            prevTile = currTile;
            currTile = tilesMap[i][j];
            currTile->neighbors[UP] = prevTile;
            prevTile->neighbors[DOWN] = currTile;
        }
        currTile->neighbors[DOWN] = firstTile;
        firstTile->neighbors[UP] = currTile;
    }
}

void forward(player* myPlayer, int n)
{
    tile *nextTile = myPlayer->position->neighbors[myPlayer->dir];
    for(int k = 0; k < n && !nextTile->wall; k++){
        myPlayer->position = nextTile;
        nextTile = myPlayer->position->neighbors[myPlayer->dir];
    }
}

int main()
{
    player myPlayer = {NULL, RIGHT};

    if(loadInputFile(INPUT_FILENAME) < 0){
        fprintf(stderr, "Can't open input file:%s\n", INPUT_FILENAME);
        return 1;
    }

    readBoard();

    for(int j = 1; myPlayer.position == NULL; j++){
        if(tilesMap[1][j] && !tilesMap[1][j]->wall)
            myPlayer.position = tilesMap[1][j];
    }


    int len = nextLine(line);
    int i = 0;
    while(i < len)
    {
        switch (line[i])
        {
        case 'R':
            myPlayer.dir = (myPlayer.dir + 1)%NDIRECTIONS;
            i++;
            break;
        
        case 'L':
            myPlayer.dir = (myPlayer.dir - 1 + NDIRECTIONS)%NDIRECTIONS;
            i++;

        default:
            int n = 0;
            while (i < len && line[i]<='9')
            {
                n = 10*n + (line[i]-'0');
                i++;
            }
            
            forward(&myPlayer, n);
        }

    }

    int solution = 1000*myPlayer.position->row + 4*myPlayer.position->col + myPlayer.dir;

    printf("%d\n", solution);
}

#define BUFSIZE 26000

char fileBuf[BUFSIZE];
char *pbuf = fileBuf;
char *pbufend = fileBuf;

int loadInputFile(char* filename)
{
    FILE *pf = fopen(filename, "r");
    if (!pf)
        return -1;
    pbuf = fileBuf;
    pbufend = fileBuf + fread(fileBuf, 1, BUFSIZE, pf);
    fclose(pf);

    return 0;
}

int nextLine(char *dst)
{
    char c;
    char *orig = dst;

    // read line:
    while (pbuf < pbufend && (c = *(pbuf++)) != '\n')
    {
        *(dst++) = c;
    }
    *dst = '\0';
    // return line length
    return dst - orig;
}