#include "cnf.h"
#include "DPLL.h"
#include "sudoku.h"

status SAT(void);
status Sudoku(void);

int main()
{
    int op = 1;
    r=NULL;
    while (op) {
        system("cls");
        printf("\n\n");
        printf("-----------------------WELCOME!-----------------------\n");
        printf("----------------------ϵͳ���˵�----------------------\n");
        printf("1.SAT                 2.Sudoku                 0.exit\n");
        printf("------------------------------------------------------\n");
        printf("��ѡ����Ĳ���[0~2]:");
        scanf("%d", &op);
        switch (op) {
            case 1:
                SAT();
                break;
            case 2:
                Sudoku();
                break;
            case 0:
                break;
            default:
                printf("�������\n");
                printf("----�����������----\n");
                getchar();
                getchar();
                break;
        }
    }
    return 0;
}

status SAT(void) {
    int op = 1;//��¼ѡ��Ĳ������
    int choose;//��¼ѡ����㷨���
    int i = 0, solut = 0;   //solut��¼CNF��ʽ�Ƿ��н⣬�н�Ϊ1���޽�Ϊ0
    clock_t start = 0, finish = 0;//��¼DPLL�������õ���ʼ����ֹʱ��
    double duration = 0;//��¼SAT���ʱ��
    FILE *open;
    while (op) {
        system("cls");
        printf("\n\n");
        printf("----------------------SAT�˵�----------------------\n");
        printf("1.��ȡcnf�ļ�    2.DPLL���    3.��ʽ��������֤    0.exit\n");
        printf("------------------------------------------------------\n");
        printf("��ѡ����Ĳ���[0~3]:");
        scanf("%d", &op);
        switch (op) {
            case 1:
                printf("����Ҫ��ȡ��cnf�ļ���:");
                scanf("%s", filename);
                open = fopen(filename, "r");
                if (open == NULL) printf("�ļ��򿪴���\n");
                else Createcnf(&open);  //����CNF��ʽ���ݴ洢�ṹ
                printf("----�����������----\n");
                getchar();
                getchar();
                break;
            case 2:
                if (r == NULL) printf("δ�����ļ�\n");
                else {
                    printf(" ʹ�ã�1.�Ż�2�㷨    2.�Ż�1�㷨    3.�Ż�ǰ�㷨\n");
                    scanf("%d", &choose);
                    switch (choose) {
                        case 1:
                            start = clock();
                            solut = DPLL3(0);
                            finish = clock();
                            duration = (double) (finish - start) / CLOCKS_PER_SEC;
                            break;
                        case 2:
                            start = clock();
                            solut = DPLL2(0);
                            finish = clock();
                            duration = (double) (finish - start) / CLOCKS_PER_SEC;
                            break;
                        case 3:
                            start = clock();
                            solut = DPLL1(0, 1);
                            finish = clock();
                            duration = (double) (finish - start) / CLOCKS_PER_SEC;
                            break;
                        default:
                            printf("�������\n");
                            break;
                    }
                    if (solut) {
                        AnswerComplete();
                        for (i = 1; i <= r->boolsize; i++) {
                            if (BoolList[i].Value == 0) printf("-%d ", i);
                            else printf("%d ", i);
                        }
                        printf("\n");
                    }
                    else printf("�ù�ʽ�޽⣡\n");
                    printf("����ʱ��Ϊ��%lfms\n", duration * 1000);
                    if (SaveValue(BoolList, solut, duration * 1000))
                        printf("�ɹ�����ͬ���ļ���\n");
                    solut=0;
                    Destroycnf(r);//����CNF��ʽ���ݴ洢�ṹ
                    r = NULL;
                    open = fopen(filename, "r");
                    Createcnf(&open);  //����CNF��ʽ���ݴ洢�ṹ
                }
                printf("----�����������----\n");
                getchar();
                getchar();
                break;
            case 3:
                if(r==NULL) printf("δ�����ļ�\n");
                else cnfTravel(r);
                printf("----�����������----\n");
                getchar();
                getchar();
                break;
            case 0:
                break;
            default:
                printf("�������\n");
                printf("----�����������----\n");
                getchar();
                getchar();
                break;
        }
    }
    return OK;
}

status Sudoku(void) {
    int op = 1,  i, j, flag = 1, choose = 1;  //op��choose��¼�û�ѡ��Ĳ���
    int solut = 0;
    char c;
    int x[3];
    srand((unsigned) time(NULL));
    while (op) {
        system("cls");
        printf("\n\n");
        printf("----------------------Sudoku�˵�----------------------\n");
        printf("           1.������Ϸ             0.�˳�\n");
        printf("------------------------------------------------------\n");
        printf("��ѡ����Ĳ���[0~1]:");
        scanf("%d", &op);
        switch (op) {
            case 1:
                printf("������.........");
                solut = CreateSudoku();   //���ɻ����������̶�Ӧ��Ԫ��ֵ��
                if (solut) {
                    SudokuComplete();
                    CNFSudokuTableTransform(); //�����ı�Ԫ��ֵ��ת��Ϊ��ά������������
                    DigHole();  //�����������ڶ�
                    choose = 1;
                    while (choose) {
                        flag = 1;  //flag����û�������ȷ�����ȷΪ1������Ϊ0
                        printf("��������ֵ����˳������������Ĵ𰸣�������001�������0�е�0�еĿո�������1����ÿ����һ�����ûس����Ͽ���������ɺ�������ո񲢰��س�������\n");
                        getchar();
                        c = getchar();
                        while (c != '\n') {
                            i = 0;
                            while (c != ' ') {
                                x[i++] = c - '0';
                                c = getchar();
                            }
                            users_sudoku[x[0]][x[1]] = x[2];//��¼�û������
                            c = getchar();
                        }
                        for (i = 0; i < 9; i++) {
                            for (j = 0; j < 9; j++)
                                //�Ա��û����������𰸣��ж��������ȷ��
                                if (sudoku_table[i][j] != users_sudoku[i][j]) {
                                    printf("�𰸴���\n");
                                    flag = 0;
                                    break;
                                }
                            if (flag == 0) {
                                break;
                            }
                        }
                        if (flag) {
                            printf("����ȷ��\n");
                            SudokuTablePrint();//�����������
                            choose = 0;
                            break;
                        }
                        else {
                            printf("1.�鿴��    2.�������    0.exit\n");
                            printf("��ѡ����Ĳ���[0~2]:");
                            scanf("%d", &choose);
                            switch (choose) {
                                case 1:
                                    SudokuTablePrint();
                                    choose=0;
                                    break;
                                case 2:
                                    break;
                                case 0:
                                    break;
                                default:
                                    printf("�������\n");
                                    break;
                            }
                        }
                    }
                }
                else printf("����ʧ�ܣ�\n");
                printf("----�����������----\n");
                getchar();
                getchar();
                break;
            case 0:
                break;
            default:
                printf("�������");
                printf("----�����������----\n");
                getchar();
                getchar();
                break;
        }
    }
    return OK;
}