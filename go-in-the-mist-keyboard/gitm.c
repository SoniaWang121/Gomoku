    // code here
    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <ctype.h>

    #define N 19

    // 保存棋盘的类
    typedef struct box
    {
        int value;         // 黑：1，白：0，空位：-1
    } box;

    box BOX[N][N];      
    int win = -1;         // 谁赢了（0：白棋，1：黑棋，2：平局）
    int whoplay = 1;      // 轮到谁下棋了
    int playercolor = 1;  // 0 : 白   1：黑
    int dx[4] = { 1,0,1,1 }; // - | \ / 四个方向
    int dy[4] = { 0,1,1,-1 };
    bool g_bisterm = false;
    char g_szHistory[1024] = {0};
    int x_mist = (N/2)+1;
    int y_mist = (N/2)+1;
    int place_count = 0;

    void init()
    {
        win = -1;
        whoplay = 1;
        playercolor = 1;
        g_bisterm = false;
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                BOX[i][j].value = -1;
            }
        }
    }

    // 0 无
    // 1 赢
    // 2 平
    int isWIN()
    {    
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (BOX[i][j].value != -1)
                {
                    int nowcolor = BOX[i][j].value; 
                    int length[4] = { 0,0,0,0 };    
                    for (int k = 0; k < 4; k++)
                    {
                        int nowi = i;
                        int nowj = j;
                        while (nowi <= 18 && nowj <= 18 && nowi >= 0 && nowj >= 0 && BOX[nowi][nowj].value == nowcolor)
                        {
                            length[k]++;
                            nowj += dx[k];
                            nowi += dy[k];
                        }
                        nowi = i;
                        nowj = j;
                        while (nowi <= 18 && nowj <= 18 && nowi >= 0 && nowj >= 0 && BOX[nowi][nowj].value == 1 - nowcolor)
                        {
                            length[k]++;
                            nowj -= dx[k];
                            nowi -= dy[k];
                        }
                    }
                    for (int k = 0; k < 4; k++)
                    {
                        if (length[k] >= 5) {
                            return 1;
                        }
                    }
                }
            }
        }

        if (place_count == 361)
            return 2;

        return 0;
    }

    char* checkSpace(char* sz1)
    {
        for (int i = 0; i < strlen(sz1); ++i)
        {
            if (sz1[i] == ' ')
            {
                sz1[i] = 0;
                return &(sz1[i + 1]);
            }
        }
        
        return NULL;
    }

    void cmd_who()
    {
        if (whoplay == 1)
            printf("B\n");
        else
            printf("W\n");
    }

    void cmd_place(char* sz, int* finish)
    {    
        int x = 0;
        int y = 0;
        int i = 0;

        if ((!isalpha(sz[0])) || (sz[strlen(sz)-1] == ' ')){
            printf("Invalid!\n");
            return;
        }

        for(i = 0; i<strlen(sz); i++){
            if (sz[i] == ' '){
                printf("Invalid!\n");
                return;
            }
        }
        
        if (strlen(sz) < 2 || strlen(sz) > 3 || sz[1] == '0')
        {
            printf("Invalid coordinate\n");
            return;
        }
        
        if (strlen(sz) == 2)
        {
            x = sz[0] - 'A';
            y = sz[1] - '1';
        }
        
        if (strlen(sz) == 3)
        {
            x = sz[0] - 'A';
            y = (sz[1] - '0') * 10 + (sz[2] - '1');
        }
        
        if ((x > 18 || x < 0) || (y > 18 || y < 0))
        {
            printf("Invalid coordinate\n");
            return;
        }
        
        if (BOX[x][y].value != -1)
        {
            printf("Occupied coordinate\n");
            return;
        }
        
        BOX[x][y].value = whoplay;
        //  计算雾的中心
        x_mist = 1 + ((5*(x+1)*(x+1) + 3*(x+1) + 4)%N);
        y_mist = 1 + ((4*(y+1)*(y+1) + 2*(y+1) - 4)%N);
        // printf("The hole %d, %d\n", x_mist, y_mist);    
        strcat(g_szHistory, sz);
        place_count += 1;
        
        int ret = isWIN();
        if (ret == 1)
        {
            if (whoplay == 1)
            {
                printf("Black wins!\n");
            }
            else
            {
                printf("White wins!\n");
            }
            
            if (!g_bisterm)
            {
                printf("%s\n", g_szHistory);
            }
            *finish = 1;
        }
        else if (ret == 2)
        {
            printf("Wow, a tie!\n");
            printf("%s\n", g_szHistory);
            *finish = 1;
        }
        
        if (whoplay == 1)
            whoplay = 0;
        else if (whoplay == 0)
            whoplay = 1;
    }

    void cmd_view()
    {
        int i, j;
        printf("%c%d,", x_mist + 64, y_mist);
        x_mist -= 1;
        y_mist -= 1;
        for(j = y_mist + 3; j >= y_mist - 3; j--)
        {
            for(i = x_mist - 3; i <= x_mist + 3; i++)
            {
                if(i<0||i>=N||j<0||j>=N)
                {
                    printf("x");
                    continue;
                }
                else{
                    if (BOX[i][j].value == -1)
                    {
                        printf(".");
                    }
                    else if (BOX[i][j].value == 0)
                    {
                        printf("o");
                    }
                    else if (BOX[i][j].value == 1)
                    {
                        printf("#");
                    } 
                }

            }
        }
        printf("\n");
    }

    int flowController()
    {
        init();
        while (1)
        {
            char szGetLine[1024] = {0};
            fgets(szGetLine, 1024, stdin);
            
            if (szGetLine[strlen(szGetLine) - 1] == '\n')
            {
                szGetLine[strlen(szGetLine) - 1] = '\0';
            }
            
            char szCurLine[1024] = {0};
            strcpy(szCurLine, szGetLine);
            char* cmd2 = NULL;
            if ((cmd2 = checkSpace(szCurLine)) == NULL)
            {
                if (!strcmp(szCurLine, "who"))
                {
                    //printf("who \n");
                    cmd_who();
                }
                else if (!strcmp(szCurLine, "term"))
                {
                    // printf("term \n");
                    g_bisterm = true;
                    return 1;
                }
                else if (!strcmp(szCurLine, "resign"))
                {
                    if (whoplay == 0)
                    {
                        printf("Black wins!\n");
                        
                        if (!g_bisterm)
                        {
                            printf("%s\n", g_szHistory);
                        }
                    }
                    else
                    {
                        printf("White wins!\n");
                        if (!g_bisterm)
                        {
                            printf("%s\n", g_szHistory);
                        }
                    }
                    return 0;
                }
                else if (!strcmp(szCurLine, "view"))
                {
                    cmd_view();
                }
                else if (!strcmp(szCurLine, "history"))
                {
                    // printf("history \n");
                    if (!g_bisterm)
                    {
                        printf("%s\n", g_szHistory);
                    }
                }
                else if (!strcmp(szCurLine, "place"))
                {
                    printf("Invalid!\n");
                }
                else{
                    printf("Invalid!\n");
                }
            }
            else if (!strcmp(szCurLine, "place"))
            {
                int finish = 0;
                cmd_place(cmd2, &finish);
                
                if (finish == 1)
                    return 0;
            }
            else{
                printf("Invalid!\n");
            }
        }
    }

    int main()
    {
        if (!flowController())
        {
            printf("Thank you for playing!\n");
        }
        // getchar();
    }
