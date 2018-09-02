/**
 * Sapir Yamin 316251818
 */


#include <stdio.h>
#include "ex52.h"
char Board[20][20];
enum Position {HORIZON, VERTICAL} typedef Position;
struct Block
{
    int i;
    int j;
    Position e;
} typedef Block;
void initiliazeBoard(){
    for (int i = 0; i < 20; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            if(i==0||i==19||j==0 || j==19){
                Board[i][j] = '*';
            }
            else{
                Board[i][j] = ' ';
            }
        }
    }
}

void printBoard(){
    for (int i = 0; i < 20; ++i)
    {
        for (int j = 0; j < 20; ++j)
        {
            printf("%c", Board[i][j]);
        }
        printf("\n");
    }
}

void changeBoard(Block b , char Board[20][20]){
    initiliazeBoard();
    if(b.e == HORIZON){
        Board[b.i][b.j] = '-';
        Board[b.i][b.j+1] = '-';
        Board[b.i][b.j+2] = '-';
    } else{
        Board[b.i][b.j] = '|';
        Board[b.i+1][b.j] = '|';
        Board[b.i+1][b.j] = '|';
    }
}