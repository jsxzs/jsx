#ifndef SAT_SUDOKU_H
#define SAT_SUDOKU_H

#include "DPLL.h"
#include "cnf.h"

int sudoku_table[9][9];//����ȫ�ֱ���int���Ͷ�ά����洢������
int users_sudoku[9][9];//�洢����ĺ��ո�����������

FILE *CreateSudokuFile(void);//������������ת��ΪSAT������cnf�ļ�
status CreateSudoku(void);//�����µ����9*9��������
status DigHole(void); //����֪����������30����������Ψһ�����������
status CNFSudokuTableTransform(void);//������ӦSAT��Ԫ��ת��Ϊ��ά����
status SudokuTablePrint(void);//���������
status SudokuComplete(void);//������������

/*������������ת��ΪSAT������cnf�ļ�*/
FILE *CreateSudokuFile(void) {
    int x, y, z, i, j, k, l;//x�����������У�y�����������У�zȡ1��9�ֱ����ø�����������ֵΪ1��9����һֵ
    FILE *fp;
    fp = fopen("SudokuTableBase.cnf", "wb");
    if (fp == NULL) {
        printf("�ļ���ʧ�ܣ�\n");
        return NULL;
    }
    fprintf(fp, "p cnf 729 10287\n");//����729����Ԫ��9*9�������ո�ÿ�����Ӧ9����Ԫ������1��9��ĳһֵ���Ӧ��ԪΪ�棬����Ϊ��
    for (x = 0; x < 9; x++) {    //ÿ��ֻ��ȡ1��9��һ��ֵ
        for (y = 0; y < 9; y++)
            for (z = 1; z < 9; z++)
                for (i = z + 1; i <= 9; i++)
                    fprintf(fp, "%d %d 0\n", -(81 * x + 9 * y + z), -(81 * x + 9 * y + i));
    }
    for (x = 0; x < 9; x++) {    //ÿ��1��9ֻ�ܸ�����һ��
        for (z = 1; z <= 9; z++)
            for (y = 0; y < 8; y++)
                for (i = y + 1; i <= 8; i++)
                    fprintf(fp, "%d %d 0\n", -(81 * x + 9 * y + z), -(81 * x + 9 * i + z));
    }
    for (y = 0; y < 9; y++) {    //ÿ��1��9ֻ�ܸ�����һ��
        for (z = 1; z <= 9; z++)
            for (x = 0; x < 8; x++)
                for (i = x + 1; i <= 8; i++)
                    fprintf(fp, "%d %d 0\n", -(81 * x + 9 * y + z), -(81 * i + 9 * y + z));
    }
    for (i = 0; i < 3; i++) {   //ÿ��3��3������1��9ֻ�ܸ�����һ��
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
    for (x = 0; x < 9; x++) {    //ÿ�����ȡ1��9�е�һ��ֵ
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

/*�������9*9��������*/
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
            printf("�ļ���ʧ�ܣ�\n");
            return ERROR;
        }
        Createcnf(&fp);  //���������ļ���ӦCNF�ڽ�����

        /*��1��81�����ֽ����������*/
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

        for (i = 1; i <= 10; i++) {    //�����������ѡ10�����������������
            x = (order[i] - 1) / 9;   //xΪ˳��Ϊi�ĸ��������̵�����
            y = (order[i] - 1) % 9;   //yΪ�ø������̵�����
            z = rand() % 9 + 1;       //zΪ�����1��9�������

            /*�޸ı�Ԫ���иø��Ӧ��Ԫ����ֵ��������cnf����*/
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
    } while (DPLL1(0, 2) == FALSE);  //����DPLL��⣬ֱ���õ���������
    return OK;
}

/*����֪����������30����������Ψһ�����������*/
status DigHole(void) {
    int x, y, z, i, j, d;
    int rank, origin, result = 0, circle;
    int table_diged[9][9];
    int diged_cells[82];   //��¼�ڶ�λ��
    FILE *fp;
    for (i = 0; i <= 81; i++)
        diged_cells[i] = 0;
    fp = CreateSudokuFile();
    if (fp == NULL) {
        printf("�ļ���ʧ�ܣ�\n");
        return ERROR;
    }
    Createcnf(&fp);

    /*��������Ϣ�����Ӧ��Ԫ��ֵ���У�����CNF�ڽ���������Ӧ����*/
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

    /*����sudoku_table��ֵ������table_diged*/
    for (i = 0; i < 9; i++)
        for (j = 0; j < 9; j++)
            table_diged[i][j] = sudoku_table[i][j];

    /*�ڵ�һ����*/
    rank = rand() % 81 + 1;
    diged_cells[rank] = 1;
    x = (rank - 1) / 9;//˳��Ϊrank��cell�����̵�����
    y = (rank - 1) % 9;//��cell�����̵�����
    origin = sudoku_table[x][y];
    table_diged[x][y] = 0;//��¼�ڶ���λ��

    /*��CNF��ʽ�ڽ�����ָ���δ֪��һ����λ�õ�����ֵ�������Ӧ��Ԫ��ֵʱ��״̬*/
    for (z = 1; z <= 9; z++) {
        if (z == origin)
            RecoverCNF(r, 81 * x + 9 * y + z);
        else RecoverCNF(r, -(81 * x + 9 * y + z));
        BoolList[81 * x + 9 * y + z].flag = 0;
        BoolList[81 * x + 9 * y + z].Value = 0;
    }
    Destroycnf(r);

    for (circle = 1; circle < 30; circle++) {
        /*ÿ��ѭ�����½�����ǰ������Ϣ��ӦCNF�ڽ�������Ԫ�����Ա�Ԫ��������Ӧ��Ϣ*/
        fp = CreateSudokuFile();
        if (fp == NULL) {
            printf("�ļ���ʧ�ܣ�\n");
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

        /*����һ����*/
        rank = rand() % 81 + 1;
        x = (rank - 1) / 9;   //xΪ�ö������̵�����
        y = (rank - 1) % 9;   //yΪ�ö������̵�����
        if (diged_cells[rank] == 1) {    //�ö��Ѿ����ڹ�������ѭ���ڶ�
            circle--;
            continue;
        }
        diged_cells[rank] = 1;     //��¼�ڶ���λ��
        origin = sudoku_table[x][y];   //��¼�ö�ԭ��ֵ
        table_diged[x][y] = 0;

        /*���ζ��ڶ�λ�������ԭ��ֵ���8������������DPLL��⣬���н���˵���ڶ���ⲻΨһ���ö������ڣ����޽���ö�����*/
        for (z = 1; z <= 9; z++) {
            result = 0;
            if (z != origin) {
                for (d = 1; d <= 9; d++) {   //�����������������CNF����
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
                DPLL1(0, 1);    //����DPLL���
                SudokuComplete();
                result =AnswerCheck(1);
                if (result == 1) {      //�н���˵���ö�������,�ظ��ö�����������һ����
                    table_diged[x][y] = origin;
                    break;
                }
                else {        //�޽���ö�����
                    for (d = 1; d <= 9; d++) {    //�ָ������ڸö�����z֮ǰ��״̬
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
            circle--;    //�����ڶ����ɹ��������ڶ�
            continue;
        }
        Destroycnf(r);
    }
    printf("\n�������ɣ�\n");
    /*������ɵ�������*/
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
            users_sudoku[i][j] = table_diged[i][j];  /*����table_diged��ֵ������users_sudoku*/
    return OK;
}

/*������ӦSAT��Ԫ��ת��Ϊ��ά����*/
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

/*���������*/
status SudokuTablePrint(void) {
    int i, j;
    printf("   0   1   2   3   4   5   6   7   8  \n");//�б��
    printf(" +---+---+---+---+---+---+---+---+---+\n");
    for (i = 0; i < 9; i++) {
        printf("%d", i);     //�б��
        for (j = 0; j < 9; j++) {
            if (j == 0 || j == 3 || j == 6) {
                printf("|");     //3*3�����������
            }
            else printf(" ");
            printf(" %d ", sudoku_table[i][j]);
        }
        printf("|\n");
        if (i == 2 || i == 5 || i == 8) {
            printf(" +---+---+---+---+---+---+---+---+---+\n");  //3*3���������
        }
        else printf(" |           |           |           |\n"); //3*3�����������
    }
    return OK;
}

/*������������*/
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
