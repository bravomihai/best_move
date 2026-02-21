#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "poe.h"
#include <unistd.h>
#define negative -10000

typedef struct
{
    char row;
    int col;
} point;

void create_empty_map(int H, int W, char a[H][W])
{
    for(int i = 0 ; i < H ; i++)
    {
        for(int j = 0 ; j < W ; j++)
        {
            a[i][j]='.';
        }
    }
    a[0][0]='A';
    a[H-1][W-1]='B';
}

void print_map(int H, int W, char a[H][W])
{
    int p = 10;
    for(int i = 0 ; i < 2 ; i++)
    {
        printf(" ");
        for(int j = 1 ; j <= W ; j++)
        {
            printf("%d", j/(p)%10);
        }
        p/=10;
        printf("\n");
    }


    for(int i = 0 ; i < H ; i++)
    {
        printf("%c", ('A'^(32*(i/26)))+i%26);

        for(int j = 0 ; j < W ; j++)
        {
            printf("%c",a[i][j]);
        }
        printf("\n");
    }
}

int find_in_map(int H, int W, char a[H][W], char x, char rows[], int cols[])
{
    int id=0;
    for(int i = 0 ; i < H ; i++)
    {
        for(int j = 0 ; j < W ; j++)
        {
            if(a[i][j]==x)
            {
                cols[id]=j+1;
                rows[id]=('A'^(32*(i/26)))+i%26;
                id++;
            }
        }
    }
    return id;
}

void encode(int H, int W, char a[H][W], char* s)
{
    int p=0, d, col;
    for(int i = 0 ; i < H ; i++)
    {
        for(int j = 0 ; j < W ; j++)
        {
            d = isdigit(a[i][j]);
            if(d || isupper(a[i][j]) || islower(a[i][j]))
            {
                if(d) s[p++]='o';
                s[p++]=a[i][j];
                s[p++]=' ';
                s[p++]=('A'^(32*(i/26)))+i%26;
                col=j+1;
                if(col/10) s[p++]='0'+col/10%10;
                s[p++]='0'+col%10;
                s[p++]=' ';
            }
        }
    }
}
void decode(int H, int W, char a[H][W], char* s)
{
    for(int i = 0 ; i < H ; i++)
    {
        for(int j = 0 ; j < W ; j++)
        {
            a[i][j]='.';
        }
    }
    int n = strlen(s), row, col, i = 0;
    char c;
    while(i < n)
    {
        if(!strchr("o ",s[i]))
        {
            col=0;
            c=s[i];
            i=i+2;
            row=(int)((s[i]^(32*(s[i]/'a')))-'A'+26*(s[i++]/'a'));
            if(s[i+1]!=' ' && s[i+1]!='\0')
            {
                col=((int)(s[i++]-'0'))*10;
            }
            col=col+(int)(s[i++]-'0');
            a[row][col-1]=c;
        }
        i++;
    }
}

char getrow(char* s, int* start)
{
    *start = *start + 1;
    return s[*(start)-1];
}

int getcol(char* s, int* start)
{
    int col = 0;
    if(s[(*start)+1] != ' ' && s[(*start)+1] != '\0')
    {
        col = ((int)(s[(*start)++] - '0')) * 10;
    }
    col = col + (int)(s[(*start)++] - '0');
    return col;
}

int add_c(char* s, int start,int len)
{
    int str_len = strlen(s);
    if(start < 0 || len <= 0)
    {
        //printf("pozitie sau lungime invalida in add_c %d %d\n",start, len);
        return -1;
    }
    memmove(s+start+len, s+start, str_len - start + 1);
    return 1;
}

int delete_c(char* s, int start,int len)
{
    int str_len = strlen(s);
    if(start < 0 || len <= 0)
    {
        //printf("pozitie sau lungime invalida in delete_c %d %d\n",start, len);
        return -1;
    }
    if(start+len>str_len)
    {
        len = str_len - start;
    }
    memmove(s+start, s+start+len, str_len - start - len + 1);
    return 1;
}
///manhattan distance
int distance(char row, int col, char np_row, int np_col)
{
    if(islower(row))
    {
        row = row - 'a' + 'Z' + 1;
    }
    if(islower(np_row))
    {
        np_row = np_row - 'a' + 'Z' + 1;
    }
    int vert = ((int)(row - np_row) > 0) ? (int)(row - np_row) : (int)(np_row - row);
    int diag = (col - np_col > 0) ? col - np_col : np_col - col;
    return diag + vert;
}

point closest_point(int row, int col, int np_row, int np_col, int s, int s2)
{
    point closest;
    closest.row = np_row;
    closest.col = np_col;
    ///going down until obstacle
    while(np_row < row && s)
    {
        np_row++;
        if(np_row == 'Z' + 1)
        {
            np_row = 'a';
        }
        s--;
    }
    ///going right until obstacle
    while(np_col < col && s)
    {
        np_col++;
        s--;
    }
    ///going up until obstacle
    while(np_row > row && s)
    {
        np_row--;
        if(np_row == 'a' - 1)
        {
            np_row = 'Z';
        }
        s--;
    }
    ///going left until obstacle
    while(np_col > col && s)
    {
        np_col--;
        s--;
    }

    ///going right to avoid obstacle
    while(np_col < col && s2)
    {
        np_col++;
        s2--;
    }
    ///going left to avoid obstacle
    while(np_col > col && s2)
    {
        np_col--;
        s2--;
    }
    ///going down to avoid obstacle
    while(np_row < row && s2)
    {
        np_row++;
        if(np_row>'Z')
        {
            np_row = 'a';
        }
        s2--;
    }
    ///going up to avoid obstacle
    while(np_row > row && s2)
    {
        np_row--;
        if(np_row<'a')
        {
            np_row = 'Z';
        }
        s2--;
    }
    closest.row = np_row;
    closest.col = np_col;
    return closest;
}

int valid_position(char row, int col, int s, int H, int W)
{
    int my_row = (int)((row^(32*(row/'a')))-'A'+26*(row/'a'));

    if(!(my_row < H && my_row >= 0 && col > 0 && col <= W))
    {
        return 0;
    }
    char substr[10];
    substr[0]=row;
    int i = 1;
    if(col>9)
    {
        substr[i++] = '0' + col / 10;
    }
    substr[i++] = '0' + col % 10;
    substr[i] = '\0';
    char *result = strstr(s,substr);
    if(result == NULL)
    {
        return 1;
    }
    return 0;
}

void end_round(game_state* ngs, game_state* gs, move* moves, int np, int otherp,int state)
{

    moves[state].type = 'p';
    moves[state].tocol = 0;
    moves[state].torow = '.';

    ///string nemodificat
    memmove(ngs[state].s, gs->s, strlen(gs->s) + 1);

    ///stats nemodificate
    ngs[state].players[otherp].s = gs->players[otherp].S;
    ngs[state].players[otherp].H = gs->players[otherp].H;
    ngs[state].players[otherp].A = gs->players[otherp].A;
    ngs[state].players[otherp].D = gs->players[otherp].D;
    ngs[state].players[otherp].S = gs->players[otherp].S;

    ngs[state].players[np].s = gs->players[np].S;
    ngs[state].players[np].H = gs->players[np].H;
    ngs[state].players[np].A = gs->players[np].A;
    ngs[state].players[np].D = gs->players[np].D;
    ngs[state].players[np].S = gs->players[np].S;
}

void move_player(game_state* ngs, game_state* gs, move* moves,int state, char row, int col, char np_row, int np_col, char next_player)
{
    moves[state].type = 'm';
    moves[state].tocol = col;
    moves[state].torow = row;

    ///incep modificarea stringului
    memmove(ngs[state].s, gs->s, strlen(gs->s) + 1);

    ///modificam pozitia jucatorului

    //cautam jucatorul
    int seek_np = 0;
    while(ngs[state].s[seek_np] != next_player || ngs[state].s[seek_np+1]!=' ')
    {
        seek_np++;
    }
    seek_np+=2;
    //marim
    if(col>9 && ngs[state].s[seek_np+2] == ' ')
    {
        if(add_c(ngs[state].s, seek_np, 1) == -1)
        {
            return -1;
        }
    }
    //micsoram
    if(col<10 && ngs[state].s[seek_np+2] != ' ')
    {
        if(delete_c(ngs[state].s, seek_np, 1));
    }
    //modificam
    ngs[state].s[seek_np++] = row;
    if(col>9)
    {
        ngs[state].s[seek_np++] = '0' + col/10;
    }
    ngs[state].s[seek_np++] = '0' + col%10;
}

point closest_valid_point(int row, int col, int np_row, int np_col, int s, char* str, int H, int W)
{
    point closest = closest_point(row, col, np_row, np_col, s, 0);
    int s2;
    while(!valid_position(closest.row, closest.col, str, H, W) && s > 0)
    {
        s2 = 0;
        while(!valid_position(closest.row, closest.col, str, H, W) && s2 <= s)
        {
            closest = closest_point(row, col, np_row, np_col, s-s2, s2);
            s2++;
        }
        s--;
    }
    return closest;
}

short int maxim(short int a, short int b)
{
    short int m = (a > b) ? a : b;
    return m;
}

int isobject(int i, char* s)
{
    if(s[i] != 'o')
    {
        return 0;
    }
    if(i==0)
    {
        return 1;
    }
    if(s[i-2] == 'm' || s[i-2] == 'A' || s[i-2] == 'B')
    {
        return 0;
    }
    return 1;
}
/*
count_m_nearby(char* s, int H, int W, char next_player)
{
    int a[H][W];
    decode(H, W, s);
    int npi, npj;
    for(int i = 0 ; i < H ; i++){
        for(int j = 0 ; j < W ; j++){
            if(a[i][j] == next_player){
                npi = i;
                npj = j;
            }
        }
    }
    for(int i = 0 ; i < 4 ; i++){

    }
}
*/
int next_states(int H, int W, game_state* gs, char next_player, item* items, game_state* ngs, move* moves)
{
    int i = 0, state = 0, np_pos = 0, np_col;
    char np_row;
    char opp = (next_player == 'A') ? 'B' : 'A';
    int np = (next_player == 'A') ? 0 : 1;
    int otherp = !np;
    if(gs->players[np].s == 0)
    {
        end_round(ngs, gs, moves, np, otherp, state);
        return 1;
    }
    while((gs->s[np_pos] != next_player || gs->s[np_pos+1] != ' ') && gs->s[np_pos] != '\0')
    {
        np_pos++;
    }
    np_pos+=2;
    np_row = getrow(gs->s, &np_pos);
    np_col = getcol(gs->s, &np_pos);
    int o = 0, m=0;
    while(gs->s[i]!='\0')
    {
        if(gs->s[i] == 'm' && gs->s[i+1] == ' ')
        {
            m++;
        }
        if(isobject(i,gs->s))
        {
            o++;
        }
        i++;
    }
    i = 0;
    ///LUCRAM CU OPONENTUL
    while(gs->s[i]!=opp || gs->s[i+1]!=' ')
    {
        i++;
    }
    ///luam coloana si randul oponentului si setam de unde incepe si unde se termina
    i+=2;
    int start = i-2, end = i+1;
    while(gs->s[end-1]!=' ' && gs->s[end-1] != '\0')
    {
        end++;
    }
    char row = getrow(gs->s, &i);
    int col = getcol(gs->s, &i);
    ///verificam distanta
    int dtop = distance(row, col, np_row, np_col);

    if(dtop == 1)
    {
        if(gs->players[np].s >= 10)
        {
            moves[state].type = 'a';
            moves[state].tocol = col;
            moves[state].torow = row;

            ///incep modificarea stringului
            memmove(ngs[state].s, gs->s, strlen(gs->s) + 1);

            ngs[state].players[otherp].s = gs->players[otherp].s;
            ngs[state].players[otherp].H = gs->players[otherp].H - maxim(gs->players[np].A - gs->players[otherp].D, 0);
            ngs[state].players[otherp].A = gs->players[otherp].A;
            ngs[state].players[otherp].D = gs->players[otherp].D;
            ngs[state].players[otherp].S = gs->players[otherp].S;

            ngs[state].players[np].s = gs->players[np].s - 10;
            ngs[state].players[np].H = gs->players[np].H;
            ngs[state].players[np].A = gs->players[np].A;
            ngs[state].players[np].D = gs->players[np].D;
            ngs[state].players[np].S = gs->players[np].S;
            state++;
        }
        else
        {
            int di[] = {-1, 1, -1, 1}, dj[] = {-1, 1, 1, -1};
            char next_row;
            int next_col;
            for(int dir = 0 ; dir < 4 ; dir++)
            {
                next_row = row + di[dir];
                next_col = col + dj[dir];
                if(next_row == 'Z' + 1)
                {
                    next_row ='a';
                }
                if(next_row == 'a' - 1)
                {
                    next_row = 'Z';
                }
                if(valid_position(next_row, next_col, gs->s, H, W))
                {
                    int dist = distance(next_row, next_col, np_row, np_col);
                    if(dist<= gs->players[np].s)
                    {
                        move_player(ngs, gs, moves, state, next_row, next_col, np_row, np_col, next_player);
                        ngs[state].players[otherp].s = gs->players[otherp].s;
                        ngs[state].players[otherp].H = gs->players[otherp].H;
                        ngs[state].players[otherp].A = gs->players[otherp].A;
                        ngs[state].players[otherp].D = gs->players[otherp].D;
                        ngs[state].players[otherp].S = gs->players[otherp].S;

                        ngs[state].players[np].s = gs->players[np].s - dist;
                        ngs[state].players[np].H = gs->players[np].H;
                        ngs[state].players[np].A = gs->players[np].A;
                        ngs[state].players[np].D = gs->players[np].D;
                        ngs[state].players[np].S = gs->players[np].S;
                        dir = 4;
                        state++;
                    }
                }
            }
        }
    }
    else
    {

        int di[] = {0, 0, 1, -1};
        int dj[] = {1, -1, 0, 0};
        char next_row;
        int next_col;
        for(int dir = 0 ; dir < 4 ; dir++)
        {
            next_row = row + di[dir];
            next_col = col + dj[dir];
            if(next_row == 'Z' + 1)
            {
                next_row ='a';
            }
            if(next_row == 'a' - 1)
            {
                next_row = 'Z';
            }
            if(valid_position(next_row, next_col, gs->s, H, W))
            {
                int dist = distance(next_row, next_col, np_row, np_col);
                if(dist + 10 <= gs->players[np].s)///putem merge si ataca
                {
                    move_player(ngs, gs, moves, state, next_row, next_col, np_row, np_col, next_player);
                    ngs[state].players[otherp].s = gs->players[otherp].s;
                    ngs[state].players[otherp].H = gs->players[otherp].H;
                    ngs[state].players[otherp].A = gs->players[otherp].A;
                    ngs[state].players[otherp].D = gs->players[otherp].D;
                    ngs[state].players[otherp].S = gs->players[otherp].S;

                    ngs[state].players[np].s = gs->players[np].s - dist;
                    ngs[state].players[np].H = gs->players[np].H;
                    ngs[state].players[np].A = gs->players[np].A;
                    ngs[state].players[np].D = gs->players[np].D;
                    ngs[state].players[np].S = gs->players[np].S;
                    dir = 4;

                    state++;
                }
                else if(dtop - 1 > gs->players[np].s)
                {
                    point closest = closest_valid_point(next_row, next_col, np_row, np_col, gs->players[np].s, gs->s, H, W);
                    int d = distance(closest.row, closest.col, np_row, np_col);
                    if(valid_position(closest.row, closest.col, gs->s, H, W) && d <= gs->players[np].s)
                    {
                        move_player(ngs, gs, moves, state, closest.row, closest.col, np_row, np_col, next_player);

                        ngs[state].players[otherp].s = gs->players[otherp].s;
                        ngs[state].players[otherp].H = gs->players[otherp].H;
                        ngs[state].players[otherp].A = gs->players[otherp].A;
                        ngs[state].players[otherp].D = gs->players[otherp].D;
                        ngs[state].players[otherp].S = gs->players[otherp].S;

                        ngs[state].players[np].s = 0;
                        ngs[state].players[np].H = gs->players[np].H;
                        ngs[state].players[np].A = gs->players[np].A;
                        ngs[state].players[np].D = gs->players[np].D;
                        ngs[state].players[np].S = gs->players[np].S;
                        dir=4;
                    }///daca nu putem innainta
                    else
                    {
                        end_round(ngs, gs, moves, np, otherp, state);
                        dir = 4;
                    }

                    state++;
                }
                else
                {
                    end_round(ngs, gs, moves, np, otherp, state);
                    state++;
                    dir = 4;
                }
            }

        }
    }
    i = 0;
    ///LUCRAM CU OBIECTELE
    while(state<=o+1)
    {
        ///gasim inceputul obiectului in string
        while(!isobject(i,gs->s) && gs->s[i]!='\0')
        {
            i++;
        }
        if(gs->s[i]=='\0')
        {
            break;
        }
        int nr_o = (int)(gs->s[i+1] - '0');
        ///luam coloana si randul obiectului si setam de unde incepe si unde se termina
        i+=3;
        int start = i-3, end = i+1;
        while(gs->s[end-1]!=' ' && gs->s[end-1] != '\0')
        {
            end++;
        }
        char row = getrow(gs->s, &i);
        int col = getcol(gs->s, &i);
        ///verificam distanta
        int dist = distance(row, col, np_row, np_col);
        if(dist <= gs->players[np].s)
        {
            moves[state].type = 'm';
            moves[state].tocol = col;
            moves[state].torow = row;

            ///incep modificarea stringului
            memmove(ngs[state].s, gs->s, strlen(gs->s) + 1);

            //sterg din string obiectul luat de jucator
            if(delete_c(ngs[state].s, start, end-start)==-1)
            {
                return -1;
            }

            ///modificam pozitia jucatorului

            //cautam jucatorul
            int seek_np = 0;
            while(ngs[state].s[seek_np] != next_player || ngs[state].s[seek_np+1]!=' ')
            {
                seek_np++;
            }
            seek_np+=2;
            //marim
            if(col>9 && ngs[state].s[seek_np+2] == ' ')
            {
                if(add_c(ngs[state].s, seek_np, 1) == -1)
                {
                    return -1;
                }
            }
            //micsoram
            if(col<10 && ngs[state].s[seek_np+2] != ' ')
            {
                if(delete_c(ngs[state].s, seek_np, 1));
            }
            //modificam
            ngs[state].s[seek_np++] = row;
            if(col>9)
            {
                ngs[state].s[seek_np++] = '0' + col/10;
            }
            ngs[state].s[seek_np++] = '0' + col%10;
            ///stringul ar trebui sa fie bine modificat
            ///modificam STATISTICILE

            ngs[state].players[otherp].s = gs->players[otherp].s;
            ngs[state].players[otherp].H = gs->players[otherp].H;
            ngs[state].players[otherp].A = gs->players[otherp].A;
            ngs[state].players[otherp].D = gs->players[otherp].D;
            ngs[state].players[otherp].S = gs->players[otherp].S;

            ngs[state].players[np].s = gs->players[np].s - dist;
            ngs[state].players[np].H = gs->players[np].H + items[nr_o].dH;
            ngs[state].players[np].A = gs->players[np].A + items[nr_o].dA;
            ngs[state].players[np].D = gs->players[np].D + items[nr_o].dD;
            ngs[state].players[np].S = gs->players[np].S + items[nr_o].dS;
        }///altfel mergem la cea mai apropiata pozitie de obiect
        else
        {
            point closest = closest_valid_point(row, col, np_row, np_col, gs->players[np].s, gs->s, H, W);

            if(valid_position(closest.row, closest.col, gs->s, H, W))
            {
                move_player(ngs, gs, moves, state, closest.row, closest.col, np_row, np_col, next_player);
                ngs[state].players[otherp].s = gs->players[otherp].s;
                ngs[state].players[otherp].H = gs->players[otherp].H;
                ngs[state].players[otherp].A = gs->players[otherp].A;
                ngs[state].players[otherp].D = gs->players[otherp].D;
                ngs[state].players[otherp].S = gs->players[otherp].S;

                ngs[state].players[np].s = 0;
                ngs[state].players[np].H = gs->players[np].H;
                ngs[state].players[np].A = gs->players[np].A;
                ngs[state].players[np].D = gs->players[np].D;
                ngs[state].players[np].S = gs->players[np].S;
            }///daca nu putem innainta
            else
            {
                end_round(ngs, gs, moves, np, otherp, state);
            }
        }

        state++;
    }

    i = 0;
    ///LUCRAM CU MONSTRII
    while(state<=o+m+1)
    {
        ///gasim monstrul in string
        while((gs->s[i]!='m' || gs->s[i+1]!=' '))
        {
            i++;
            if(gs->s[i]=='\0')
            {
                break;
            }
        }
        if(gs->s[i]=='\0')
        {
            break;
        }
        ///luam coloana si randul monstrului si setam de unde incepe si unde se termina
        i+=2;
        int start = i-2, end = i+1;
        while(gs->s[end-1]!=' ' && gs->s[end-1] != '\0')
        {
            end++;
        }
        char row = getrow(gs->s, &i);
        int col = getcol(gs->s, &i);
        ///verificam distanta
        int dtom = distance(row, col, np_row, np_col);
        if(dtom == 1 && gs->players[np].s >= 10)
        {
            moves[state].type = 'a';
            moves[state].tocol = col;
            moves[state].torow = row;
            ///incep modificarea stringului
            memmove(ngs[state].s, gs->s, strlen(gs->s) + 1);

            //sterg din string monstrul omorat de jucator
            if(delete_c(ngs[state].s, start, end-start)==-1)
            {
                return -1;
            }

            ngs[state].players[otherp].s = gs->players[otherp].s;
            ngs[state].players[otherp].H = gs->players[otherp].H;
            ngs[state].players[otherp].A = gs->players[otherp].A;
            ngs[state].players[otherp].D = gs->players[otherp].D;
            ngs[state].players[otherp].S = gs->players[otherp].S;

            ngs[state].players[np].s = gs->players[np].s - 10;
            ngs[state].players[np].H = gs->players[np].H + 10;
            ngs[state].players[np].A = gs->players[np].A;
            ngs[state].players[np].D = gs->players[np].D;
            ngs[state].players[np].S = gs->players[np].S;
            state++;


        }
        else
        {
            int di[] = {0, 0, 1, -1};
            int dj[] = {1, -1, 0, 0};
            char next_row, ex_row = '.';
            int next_col, ex_col = 0;
            for(int dir = 0 ; dir < 4 ; dir++)
            {
                next_row = row + di[dir];
                next_col = col + dj[dir];
                if(next_row == 'Z' + 1)
                {
                    next_row ='a';
                }
                if(next_row == 'a' - 1)
                {
                    next_row = 'Z';
                }
                if(valid_position(next_row, next_col, gs->s, H, W))
                {
                    int dist = distance(next_row, next_col, np_row, np_col);
                    if(dist <= gs->players[np].s && ngs)
                    {
                        move_player(ngs, gs, moves, state, next_row, next_col, np_row, np_col, next_player);
                        ngs[state].players[otherp].s = gs->players[otherp].s;
                        ngs[state].players[otherp].H = gs->players[otherp].H;
                        ngs[state].players[otherp].A = gs->players[otherp].A;
                        ngs[state].players[otherp].D = gs->players[otherp].D;
                        ngs[state].players[otherp].S = gs->players[otherp].S;

                        ngs[state].players[np].s = gs->players[np].s - dist;
                        ngs[state].players[np].H = gs->players[np].H;
                        ngs[state].players[np].A = gs->players[np].A;
                        ngs[state].players[np].D = gs->players[np].D;
                        ngs[state].players[np].S = gs->players[np].S;
                        dir = 4;
                        state++;
                    }
                    else if(dtom > gs->players[np].s)
                    {
                        point closest = closest_valid_point(next_row, next_col, np_row, np_col, gs->players[np].s, gs->s, H, W);
                        if(valid_position(closest.row, closest.col, gs->s, H, W))
                        {
                            ex_row = closest.row;
                            ex_col = closest.col;
                            move_player(ngs, gs, moves, state, closest.row, closest.col, np_row, np_col, next_player);

                            ngs[state].players[otherp].s = gs->players[otherp].s;
                            ngs[state].players[otherp].H = gs->players[otherp].H;
                            ngs[state].players[otherp].A = gs->players[otherp].A;
                            ngs[state].players[otherp].D = gs->players[otherp].D;
                            ngs[state].players[otherp].S = gs->players[otherp].S;

                            ngs[state].players[np].s = 0;
                            ngs[state].players[np].H = gs->players[np].H;
                            ngs[state].players[np].A = gs->players[np].A;
                            ngs[state].players[np].D = gs->players[np].D;
                            ngs[state].players[np].S = gs->players[np].S;

                            dir=4;
                        }///daca nu putem innainta
                        else
                        {
                            end_round(ngs, gs, moves, np, otherp, state);
                        }
                        state++;
                    }
                }

            }
        }
    }
    end_round(ngs, gs, moves, np, otherp, state);
    state++;
    return state;
}

void show_state(game_state gs)
{

    int aux=0;
    char c = 'A';
    aux=0;
    while(aux<2)
    {
        printf("%c: H=%hd A=%hd D=%hd s=%hd S=%hd\n", c, gs.players[aux].H, gs.players[aux].A, gs.players[aux].D, gs.players[aux].s, gs.players[aux].S);
        c++;
        aux++;
    }
}

int game_over(game_state gs)
{
    if(gs.players[0].H <= 0)
    {
        return 1;
    }
    if(gs.players[1].H <= 0)
    {
        return 1;
    }
    return 0;
}

int static_eval(game_state gs, char next_player)
{
    int player_index = (next_player == 'A') ? 0 : 1;
    int opp_index = (next_player == 'A') ? 1 : 0;
    if(game_over(gs) && gs.players[player_index].H > 0)
    {
        return 1e8;
    }
    if(game_over(gs) && gs.players[player_index].H <= 0)
    {
        return -1e8;
    }
    int move_score = 0;
    move_score = gs.players[player_index].H + gs.players[player_index].A + gs.players[player_index].D + gs.players[player_index].S;
    move_score = move_score - gs.players[opp_index].H - gs.players[opp_index].A - gs.players[opp_index].D - gs.players[opp_index].S;
    return move_score;
}


int minimax(game_state* gs, int depth, int is_maximizing, int H, int W, char next_player, char opp, item* items, int* alpha, int* beta)
{
    //printf("alpha %d beta %d\n", *alpha, *beta);
    if(depth == 0 || game_over(*gs))
    {
        return static_eval(*gs, next_player);
    }
    if(is_maximizing)
    {
        int max_eval = -1e8;
        game_state* ngs = malloc(30 * sizeof(game_state));
        if(ngs == NULL)
        {

            return -1;
        }
        move* moves = malloc(30 * sizeof(move));
        if(moves == NULL)
        {

            return -1;
        }
        int nos = next_states(H, W, gs, next_player, items, ngs, moves);
        //printf("am gasit %d stari\n",nos);
        for(int i = 0 ; i < nos ; i++)
        {
            int player_index = next_player - 'A';

            int eval;
            if(moves[i].type == 'p' || ngs[i].players[player_index].s == 0)
            {
                /*
                int a[H][W];
                decode(H,W,a,ngs[i].s);
                puts("");
                printf("%d\n", depth);
                print_map(H,W,a);
                show_state(ngs[i]);
                printf("%c %c %d %hd\n", moves[i].type, moves[i].torow, moves[i].tocol, ngs[i].players[player_index].s);
                printf("alpha %d beta %d\n", *alpha, *beta);*/
                eval = minimax(ngs + i, depth - 1, 0, H, W, next_player, opp, items, alpha, beta);
            }
            else
            {
                //printf("%c %c %d %hd\n", moves[i].type, moves[i].torow, moves[i].tocol, ngs[i].players[player_index].s);
                eval = minimax(ngs + i, depth, 1, H, W, next_player, opp, items, alpha, beta);
            }
            max_eval = (eval > max_eval) ? eval : max_eval;
            *alpha = (*alpha > eval) ? *alpha : eval;
            if (*beta <= *alpha) break;
        }
        free(ngs);
        free(moves);
        return max_eval;
    }
    else
    {
        int min_eval = 1e8;
        game_state* ngs = malloc(30 * sizeof(game_state));
        if(ngs == NULL)
        {
            return -1;
        }
        move* moves = malloc(30 * sizeof(move));
        if(moves == NULL)
        {
            return -1;
        }
        int nos = next_states(H, W, gs, opp, items, ngs, moves);
        //printf("am gasit %d stari\n",nos);
        for(int i = 0 ; i < nos ; i++)
        {
            int opp_index = opp - 'A';
            int eval;
            if(moves[i].type == 'p' || ngs[i].players[opp_index].s == 0)
            {
                /*
                int a[H][W];
                puts("");
                decode(H,W,a,ngs[i].s);
                printf("%d\n", depth);
                print_map(H,W,a);
                show_state(ngs[i]);
                printf("%c %c %d %hd\n", moves[i].type, moves[i].torow, moves[i].tocol, ngs[i].players[opp_index].s);
                printf("alpha %d beta %d\n", *alpha, *beta);*/
                eval = minimax(ngs + i, depth - 1, 1, H, W, next_player, opp, items, alpha, beta);
            }
            else
            {
                //printf("%c %c %d %hd\n", moves[i].type, moves[i].torow, moves[i].tocol, ngs[i].players[opp_index].s);
                eval = minimax(ngs + i, depth, 0, H, W, next_player, opp, items, alpha, beta);
            }
            min_eval = (eval < min_eval) ? eval : min_eval;
            *beta = (*beta < eval) ? *beta : eval;
            if (*beta <= *alpha) break;
        }
        free(ngs);
        free(moves);
        return min_eval;
    }
    return 1;
}

move best_move(const char* pf)
{
    move null= {'p','.',0};
    int H, W;
    char next_player;
    fscanf(pf, "%d %d %c", &H, &W, &next_player);
    char a[H][W], rows[H*W];
    int cols[H*W];
    player p1, p2;
    fscanf(pf, "%hd %hd %hd %hd %hd\n", &p1.H, &p1.A, &p1.D, &p1.s, &p1.S);
    fscanf(pf, "%hd %hd %hd %hd %hd\n", &p2.H, &p2.A, &p2.D, &p2.s, &p2.S);
    int n;
    fscanf(pf,"%d\n", &n);
    item* items = malloc(n*sizeof(item));
    for(int i = 0 ; i < n ; i++)
    {
        fscanf(pf, "%hd %hd %hd %hd\n", &items[i].dH, &items[i].dA, &items[i].dD, &items[i].dS);
    }
    game_state gs = {{p1, p2}};
    fgets(gs.s,199,pf);
    gs.s[strlen(gs.s)-1]='\0';/// stergem \n care se infiltreaza
    game_state* ngs = malloc(30 * sizeof(game_state));
    if(ngs == NULL)
    {
        return null;
    }
    move* moves = malloc(30 * sizeof(move));
    if(moves == NULL)
    {
        return null;
    }

    int max_score = 0, max_move;

    decode(H,W,a,gs.s);
    print_map(H,W,a);
    puts("");
    char opp = (next_player == 'A') ? 'B' : 'A';
    int nr_of_states = next_states(H,W,&gs,next_player,items,ngs,moves);
    int* scores = calloc(nr_of_states, sizeof(int));
    if(scores == NULL){
        return null;
    }
    int bsi = -1;
    //nr_of_states = -1;
    if(nr_of_states!=-1)
    {
        int alpha = -1e8;
        int beta = 1e8;
        int depth = 10;
        for(int i = 0 ; i < nr_of_states ; i++)
        {
            /*
            puts("!");
            decode(H,W,a,ngs[i].s);
            print_map(H,W,a);
            printf("%c %c %d\n", moves[i].type, moves[i].torow, moves[i].tocol);*/

            int is_maximizing = (moves[i].type == 'p') ? 0 : 1;
            scores[i] = minimax(ngs + i, depth, is_maximizing, H, W, next_player, opp, items, &alpha, &beta);

            if(scores[i] == -1)
            {
                return null;
            }
            if(bsi == -1 || scores[bsi] < scores[i])
            {
                bsi = i;
            }
        }
        //printf("alpha %d beta %d\n", alpha, beta);
    }
    /*
    decode(H,W,a,ngs[bsi].s);
    print_map(H,W,a);*/
    show_state(ngs[bsi]);
    puts(ngs[bsi].s);
    printf("%d %d\n", bsi, scores[bsi]);
    null.type = moves[bsi].type;
    null.tocol = moves[bsi].tocol;
    null.torow = moves[bsi].torow;
    free(ngs);
    free(moves);
    free(scores);
    return null;
}
