#ifndef SAT_SUDOKU_H
#define SAT_SUDOKU_H

#include "DPLL.h"
#include "cnf.h"

int sudoku_table[9][9];//定义全局变量int类型二维数组存储数独盘
int users_sudoku[9][9];//存储输出的含空格数独问题格局

FILE *CreateSudokuFile(void);//创建数独问题转化为SAT问题后的cnf文件
status CreateSudoku(void);//创建新的随机9*9数独终盘
status DigHole(void); //对已知数独终盘挖30个洞生成有唯一解的数独问题
status CNFSudokuTableTransform(void);//数独对应SAT变元表转化为二维数组
status SudokuTablePrint(void);//输出数独盘
status SudokuComplete(void);//完善数独终盘

/*创建数独问题转化为SAT问题后的cnf文件*/
FILE *CreateSudokuFile(void) {
    int x, y, z, i, j, k, l;//x代表数独的行，y代表数独的列，z取1～9分别代表该格中数独填入值为1～9中任一值
    FILE *fp;
    fp = fopen("SudokuTableBase.cnf", "wb");
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return NULL;
    }
    fprintf(fp, "p cnf 729 10287\n");//共有729个变元，9*9个数独空格每个格对应9个变元，填入1～9中某一值则对应变元为真，其他为假
    for (x = 0; x < 9; x++) {    //每格只能取1～9的一个值
        for (y = 0; y < 9; y++)
            for (z = 1; z < 9; z++)
                for (i = z + 1; i <= 9; i++)
                    fprintf(fp, "%d %d 0\n", -(81 * x + 9 * y + z), -(81 * x + 9 * y + i));
    }
    for (x = 0; x < 9; x++) {    //每行1～9只能各出现一次
        for (z = 1; z <= 9; z++)
            for (y = 0; y < 8; y++)
                for (i = y + 1; i <= 8; i++)
                    fprintf(fp, "%d %d 0\n", -(81 * x + 9 * y + z), -(81 * x + 9 * i + z));
    }
    for (y = 0; y < 9; y++) {    //每列1～9只能各出现一次
        for (z = 1; z <= 9; z++)
            for (x = 0; x < 8; x++)
                for (i = x + 1; i <= 8; i++)
                    fprintf(fp, "%d %d 0\n", -(81 * x + 9 * y + z), -(81 * i + 9 * y + z));
    }
    for (i = 0; i < 3; i++) {   //每个3×3盒子中1～9只能各出现一次
        for (j = 0; j < 3; j++)
            for (z = 1; z <= 9; z++)
                for (x = 0; x < 3; x++)
                    for (y = 0; y < 3; y++)
                        for (k = x + 1; k < 3; k++)
                            for (l = 0; l < 3; l++)
                                if (y != l)
                                    fprintf(fp, "%d %d 0\n", -(81 * (3 * i + x) + 9 * (3 * j + y) + z),
                                            -(81 * (3 * i + k) + 9 * (3 * j + l) + z));
    }
    for (x = 0; x < 9; x++) {    //每格必须取1～9中的一个值
        for (y = 0; y < 9; y++) {
            for (z = 1; z <= 9; z++)
                fprintf(fp, "%d ", 81 * x + 9 * y + z);
            fprintf(fp, "0\n");
        }
    }
    fclose(fp);
    fp = fopen("SudokuTableBase.cnf", "r");
    return fp;
}

/*创建随机9*9数独终盘*/
status CreateSudoku(void) {
    int x, y, z, i, d;
    int order[82], rank;
    FILE *fp;
    for (x = 0; x < 9; x++)
        for (y = 0; y < 9; y++)
            sudoku_table[x][y] = 0;
    do {
        fp = CreateSudokuFile();
        if (fp == NULL) {
            printf("文件打开失败！\n");
            return ERROR;
        }
        Createcnf(&fp);  //创建数独文件对应CNF邻接链表

        /*对1～81的数字进行随机排序*/
        for (i = 0; i <= 81; i++)
            order[i] = i;
        for (i = 81; i > 2; i--) {
            rank = rand() % i + 1;
            if (i != rank) {
                d = order[i];
                order[i] = order[rank];
                order[rank] = d;
            }
        }

        for (i = 1; i <= 10; i++) {    //在棋盘中随机选10个格子随机填入数字
            x = (order[i] - 1) / 9;   //x为顺序为i的格子在棋盘的行数
            y = (order[i] - 1) % 9;   //y为该格在棋盘的列数
            z = rand() % 9 + 1;       //z为填入的1～9的随机数

            /*修改变元表中该格对应变元的真值，并处理cnf链表*/
            for (d = 1; d <= 9; d++) {
                BoolList[81 * x + 9 * y + d].flag = 1;
                if (d == z) {
                    BoolList[81 * x + 9 * y + d].Value = 1;
                    DeleteClause(r, (81 * x + 9 * y + d));
                    DeleteLiteral((81 * x + 9 * y + d));
                }
                else {
                    BoolList[81 * x + 9 * y + d].Value = 0;
                    DeleteClause(r,-(81 * x + 9 * y + d));
                    DeleteLiteral( -(81 * x + 9 * y + d));
                }
            }

        }
    } while (DPLL1(0, 2) == FALSE);  //调用DPLL求解，直到得到数独终盘
    return OK;
}

/*对已知数独终盘挖30个洞生成有唯一解的数独问题*/
status DigHole(void) {
    int x, y, z, i, j, d;
    int rank, origin, result = 0, circle;
    int table_diged[9][9];
    int diged_cells[82];   //记录挖洞位置
    FILE *fp;
    for (i = 0; i <= 81; i++)
        diged_cells[i] = 0;
    fp = CreateSudokuFile();
    if (fp == NULL) {
        printf("文件打开失败！\n");
        return ERROR;
    }
    Createcnf(&fp);

    /*将数独信息填入对应变元真值表中，并对CNF邻接链表作相应处理*/
    for (i = 0; i < 9; i++)
        for (j = 0; j < 9; j++) {
            d = sudoku_table[i][j];
            if (d != 0)
                for (z = 1; z <= 9; z++) {
                    BoolList[81 * i + 9 * j + z].flag = 1;
                    if (d == z)
                        BoolList[81 * i + 9 * j + z].Value = 1;
                    else
                        BoolList[81 * i + 9 * j + z].Value = 0;
                }
        }
    for (i = 1; i <= r->boolsize; i++) {
        if (BoolList[i].Value == 1) {
            DeleteClause(r, i);
            DeleteLiteral( i);
        }
        else {
            DeleteClause(r, -i);
            DeleteLiteral( -i);
        }
    }

    /*复制sudoku_table的值至数组table_diged*/
    for (i = 0; i < 9; i++)
        for (j = 0; j < 9; j++)
            table_diged[i][j] = sudoku_table[i][j];

    /*挖第一个洞*/
    rank = rand() % 81 + 1;
    diged_cells[rank] = 1;
    x = (rank - 1) / 9;//顺序为rank的cell在棋盘的行数
    y = (rank - 1) % 9;//该cell在棋盘的列数
    origin = sudoku_table[x][y];
    table_diged[x][y] = 0;//记录挖洞的位置

    /*将CNF公式邻接链表恢复至未知第一个洞位置的填入值，即其对应变元真值时的状态*/
    for (z = 1; z <= 9; z++) {
        if (z == origin)
            RecoverCNF(r, 81 * x + 9 * y + z);
        else RecoverCNF(r, -(81 * x + 9 * y + z));
        BoolList[81 * x + 9 * y + z].flag = 0;
        BoolList[81 * x + 9 * y + z].Value = 0;
    }
    Destroycnf(r);

    for (circle = 1; circle < 30; circle++) {
        /*每次循环重新建立当前数独信息对应CNF邻接链表及变元表，并对变元表填入相应信息*/
        fp = CreateSudokuFile();
        if (fp == NULL) {
            printf("文件打开失败！\n");
            return ERROR;
        }
        Createcnf(&fp);
        for (i = 0; i < 9; i++)
            for (j = 0; j < 9; j++) {
                d = table_diged[i][j];
                if (d != 0)
                    for (z = 1; z <= 9; z++) {
                        BoolList[81 * i + 9 * j + z].flag = 1;
                        if (d == z)
                            BoolList[81 * i + 9 * j + z].Value = 1;
                        else
                            BoolList[81 * i + 9 * j + z].Value = 0;
                    }
            }
        for (i = 1; i <= r->boolsize; i++) {
            if (BoolList[i].Value == 1) {
                DeleteClause(r, i);
                DeleteLiteral( i);
            }
            else {
                DeleteClause(r, -i);
                DeleteLiteral( -i);
            }
        }

        /*挖下一个洞*/
        rank = rand() % 81 + 1;
        x = (rank - 1) / 9;   //x为该洞在棋盘的行数
        y = (rank - 1) % 9;   //y为该洞在棋盘的列数
        if (diged_cells[rank] == 1) {    //该洞已经被挖过，继续循环挖洞
            circle--;
            continue;
        }
        diged_cells[rank] = 1;     //记录挖洞的位置
        origin = sudoku_table[x][y];   //记录该洞原数值
        table_diged[x][y] = 0;

        /*依次对挖洞位置填入除原数值外的8个数，并调用DPLL求解，如有解则说明挖洞后解不唯一，该洞不可挖，若无解则该洞可挖*/
        for (z = 1; z <= 9; z++) {
            result = 0;
            if (z != origin) {
                for (d = 1; d <= 9; d++) {   //根据填入的数，处理CNF链表
                    BoolList[81 * x + 9 * y + d].flag = 1;
                    if (d == z) {
                        BoolList[81 * x + 9 * y + d].Value = 1;
                        DeleteClause(r, 81 * x + 9 * y + d);
                        DeleteLiteral( 81 * x + 9 * y + d);
                    }
                    else {
                        BoolList[81 * x + 9 * y + d].Value = 0;
                        DeleteClause(r, -(81 * x + 9 * y + d));
                        DeleteLiteral( -(81 * x + 9 * y + d));
                    }
                }
                DPLL1(0, 1);    //调用DPLL求解
                SudokuComplete();
                result =AnswerCheck(1);
                if (result == 1) {      //有解则说明该洞不可挖,回复该洞，继续挖下一个洞
                    table_diged[x][y] = origin;
                    break;
                }
                else {        //无解则该洞可挖
                    for (d = 1; d <= 9; d++) {    //恢复链表在该洞填入z之前的状态
                        if (d == z)
                            RecoverCNF(r, 81 * x + 9 * y + d);
                        else RecoverCNF(r, -(81 * x + 9 * y + d));
                        BoolList[81 * x + 9 * y + d].flag = 0;
                        BoolList[81 * x + 9 * y + d].Value = 0;
                    }
                }
            }
        }
        if (result == 1) {
            circle--;    //本次挖洞不成功，继续挖洞
            continue;
        }
        Destroycnf(r);
    }
    printf("\n数独生成！\n");
    /*输出生成的数独题*/
    printf("   0   1   2   3   4   5   6   7   8  \n");
    printf(" +---+---+---+---+---+---+---+---+---+\n");
    for (i = 0; i < 9; i++) {
        printf("%d", i);
        for (j = 0; j < 9; j++) {
            if (j == 0 || j == 3 || j == 6) printf("|");
            else printf(" ");
            if (table_diged[i][j] != 0) printf(" %d ", table_diged[i][j]);
            else printf("   ");
        }
        printf("|\n");
        if (i == 2 || i == 5 || i == 8) printf(" +---+---+---+---+---+---+---+---+---+\n");
        else printf(" |           |           |           |\n");
    }
    for (i = 0; i < 9; i++)
        for (j = 0; j < 9; j++)
            users_sudoku[i][j] = table_diged[i][j];  /*复制table_diged的值至数组users_sudoku*/
    return OK;
}

/*数独对应SAT变元表转化为二维数组*/
status CNFSudokuTableTransform(void) {
    int i, j, z;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            for (z = 1; z <= 9; z++) {
                if (BoolList[81 * i + 9 * j + z].Value == 1) {
                    sudoku_table[i][j] = z;
                }
            }
        }
    }
    return OK;
}

/*输出数独盘*/
status SudokuTablePrint(void) {
    int i, j;
    printf("   0   1   2   3   4   5   6   7   8  \n");//列标号
    printf(" +---+---+---+---+---+---+---+---+---+\n");
    for (i = 0; i < 9; i++) {
        printf("%d", i);     //行标号
        for (j = 0; j < 9; j++) {
            if (j == 0 || j == 3 || j == 6) {
                printf("|");     //3*3区域竖间隔线
            }
            else printf(" ");
            printf(" %d ", sudoku_table[i][j]);
        }
        printf("|\n");
        if (i == 2 || i == 5 || i == 8) {
            printf(" +---+---+---+---+---+---+---+---+---+\n");  //3*3区域横间隔线
        }
        else printf(" |           |           |           |\n"); //3*3区域竖间隔线
    }
    return OK;
}

/*完善数独终盘*/
status SudokuComplete(void){
    int i,j,z,y,d;
    int complete_table[9][9];
    for (i=0; i<9; i++)
        for (j=0; j<9; j++)
            complete_table[i][j]=0;
    for (i=0; i<9; i++)
        for (j=0; j<9; j++)
            for (z=1; z<=9; z++)
                if (BoolList[81 * i + 9 * j + z].Value == 1)
                    complete_table[i][j]=z;

    for (i=0; i<9; i++) {
        for (j=0; j<9; j++) {
            if (complete_table[i][j]==0) {
                complete_table[i][j]=1;
                for (d=0; d<9; d++)
                    for (y=0; y<9; y++)
                        if (y!=j)
                            if (complete_table[i][j]==complete_table[i][y])
                                complete_table[i][j]++;
            }
        }
    }
    for (i=0; i<9; i++)
        for (j=0; j<9; j++) {
            d=complete_table[i][j];
            if (d!=0)
                for (z=1; z<=9; z++) {
                    BoolList[81 * i + 9 * j + z].flag=1;
                    if (d==z)
                        BoolList[81 * i + 9 * j + z].Value=1;
                    else
                        BoolList[81 * i + 9 * j + z].Value=0;
                }
        }
    return OK;
}

#endif //SAT_SUDOKU_H
