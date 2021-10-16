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
        printf("----------------------系统主菜单----------------------\n");
        printf("1.SAT                 2.Sudoku                 0.exit\n");
        printf("------------------------------------------------------\n");
        printf("请选择你的操作[0~2]:");
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
                printf("输入错误！\n");
                printf("----按任意键继续----\n");
                getchar();
                getchar();
                break;
        }
    }
    return 0;
}

status SAT(void) {
    int op = 1;//记录选择的操作序号
    int choose;//记录选择的算法序号
    int i = 0, solut = 0;   //solut记录CNF公式是否有解，有解为1，无解为0
    clock_t start = 0, finish = 0;//记录DPLL函数调用的起始和终止时间
    double duration = 0;//记录SAT求解时间
    FILE *open;
    while (op) {
        system("cls");
        printf("\n\n");
        printf("----------------------SAT菜单----------------------\n");
        printf("1.读取cnf文件    2.DPLL求解    3.公式解析与验证    0.exit\n");
        printf("------------------------------------------------------\n");
        printf("请选择你的操作[0~3]:");
        scanf("%d", &op);
        switch (op) {
            case 1:
                printf("输入要读取的cnf文件名:");
                scanf("%s", filename);
                open = fopen(filename, "r");
                if (open == NULL) printf("文件打开错误！\n");
                else Createcnf(&open);  //生成CNF公式数据存储结构
                printf("----按任意键继续----\n");
                getchar();
                getchar();
                break;
            case 2:
                if (r == NULL) printf("未导入文件\n");
                else {
                    printf(" 使用：1.优化2算法    2.优化1算法    3.优化前算法\n");
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
                            printf("输入错误！\n");
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
                    else printf("该公式无解！\n");
                    printf("计算时间为：%lfms\n", duration * 1000);
                    if (SaveValue(BoolList, solut, duration * 1000))
                        printf("成功存入同名文件！\n");
                    solut=0;
                    Destroycnf(r);//销毁CNF公式数据存储结构
                    r = NULL;
                    open = fopen(filename, "r");
                    Createcnf(&open);  //生成CNF公式数据存储结构
                }
                printf("----按任意键继续----\n");
                getchar();
                getchar();
                break;
            case 3:
                if(r==NULL) printf("未导入文件\n");
                else cnfTravel(r);
                printf("----按任意键继续----\n");
                getchar();
                getchar();
                break;
            case 0:
                break;
            default:
                printf("输入错误！\n");
                printf("----按任意键继续----\n");
                getchar();
                getchar();
                break;
        }
    }
    return OK;
}

status Sudoku(void) {
    int op = 1,  i, j, flag = 1, choose = 1;  //op和choose记录用户选择的操作
    int solut = 0;
    char c;
    int x[3];
    srand((unsigned) time(NULL));
    while (op) {
        system("cls");
        printf("\n\n");
        printf("----------------------Sudoku菜单----------------------\n");
        printf("           1.数独游戏             0.退出\n");
        printf("------------------------------------------------------\n");
        printf("请选择你的操作[0~1]:");
        scanf("%d", &op);
        switch (op) {
            case 1:
                printf("加载中.........");
                solut = CreateSudoku();   //生成基础数独终盘对应变元真值表
                if (solut) {
                    SudokuComplete();
                    CNFSudokuTableTransform(); //将求解的变元真值表转换为二维数组数独终盘
                    DigHole();  //对数独终盘挖洞
                    choose = 1;
                    while (choose) {
                        flag = 1;  //flag标记用户求解答案正确与否，正确为1，错误为0
                        printf("按“行列值”的顺序，依次输入你的答案（例：“001”代表第0行第0列的空格内填入1），每输入一个答案用回车键断开，输入完成后请输入空格并按回车继续：\n");
                        getchar();
                        c = getchar();
                        while (c != '\n') {
                            i = 0;
                            while (c != ' ') {
                                x[i++] = c - '0';
                                c = getchar();
                            }
                            users_sudoku[x[0]][x[1]] = x[2];//记录用户输入答案
                            c = getchar();
                        }
                        for (i = 0; i < 9; i++) {
                            for (j = 0; j < 9; j++)
                                //对比用户解答和数独答案，判断其求解正确性
                                if (sudoku_table[i][j] != users_sudoku[i][j]) {
                                    printf("答案错误！\n");
                                    flag = 0;
                                    break;
                                }
                            if (flag == 0) {
                                break;
                            }
                        }
                        if (flag) {
                            printf("答案正确！\n");
                            SudokuTablePrint();//输出数独终盘
                            choose = 0;
                            break;
                        }
                        else {
                            printf("1.查看答案    2.继续解答    0.exit\n");
                            printf("请选择你的操作[0~2]:");
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
                                    printf("输入错误！\n");
                                    break;
                            }
                        }
                    }
                }
                else printf("生成失败！\n");
                printf("----按任意键继续----\n");
                getchar();
                getchar();
                break;
            case 0:
                break;
            default:
                printf("输入错误！");
                printf("----按任意键继续----\n");
                getchar();
                getchar();
                break;
        }
    }
    return OK;
}