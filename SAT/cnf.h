#ifndef SAT_CNF_H
#define SAT_CNF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#define OK 1
#define ERROR 0
#define OVERFLOW -2

typedef int status;

/*定义子句链表结点结构类型*/
typedef struct literal {
    int data;//记录子句中的文字
    int flag;//标记该文字是否已被删除，未删除时值为0，否则值为使之删除的变元序号
    struct literal *next;//指向该子句中下一文字的指针
} literal;

/*定义CNF公式链表结点（即子句链表头结点）结构类型*/
typedef struct Clause {
    int number;//子句中显示的文字数
    int flag;//标记该子句是否已被删除，未删除时值为0，否则值为使之删除的变元序号
    struct literal *firstlit;//子句头指针
    struct Clause *next;//指向下一子句的头结点
} Clause;

/*定义CNF公式链表头结点类型，存储CNF范式信息*/
typedef struct Root {
    int boolsize;//存储文字数量
    int clausesize;//存储子句数量
    Clause *firstclause;//指向第一个子句
} Root;

/*定义指向子句链表头结点的链表结点结构类型*/
typedef struct list {
    Clause *cla;     //指向子句链表头结点
    struct list *next;  //指向链表下一结点
} list;

/*定义文字相关信息链表结构类型*/
typedef struct LitTravel {
    list *trac;  //指向含有某变元正文字或负文字的子句头结点链表的头结点
} LitTravel;

/*定义存储变元信息的变元线性表元素结构类型*/
typedef struct boollist {
    int Value;  //变元的真值
    int flag;   //变元是否已赋值
    int times;  //变元在所有子句中出现的总次数
    LitTravel Pos;   //含有该变元正文字的子句头结点链表的头结点
    LitTravel Neg;   //含有该变元负文字的子句头结点链表的头结点
} boollist;


extern boollist *BoolList;  //变元线性表
extern Root *r;    //cnf公式链表的根
extern int *occurtimes;     //统计原cnf文件中所有变元出现次数，并排序
extern int *stack;   //记录选取的变元及随之产生的单子句文字，用于dpll回溯
extern int top;     //指向栈顶
extern char filename[1000];

status Createcnf(FILE **fp);    //创建CNF邻接链表及变元表
status CreateClause(FILE **fp, literal **head, Clause *listHead, int first);  //创建子句链表
status Destroycnf(Root *r);  //销毁所有链表及线性表结构
status ifsingle(Root *r);   //判断CNF链表中是否含有单子句

status DeleteClause(Root *r, int l);     //删除出现了文字l的所有单子句
status DeleteLiteral(int l);   //删除所有文字为-l的子句链表结点
status RecoverCNF(Root *r, int l);     //恢复认为文字l为真时对CNF邻接链表所作的操作
void cnfTravel(Root *r);      //遍历CNF邻接链表
status SaveValue(boollist *ValueList, int solut, int time);//保存CNF的解及求解时间信息
status AnswerCheck(int solut);

/*功能：创建CNF邻接链表及变元表
  参数：FILE **fp：文件的指针地址
  返回值：类型：int
        OK：创建成功  */
status Createcnf(FILE **fp) {
    char readfile[20];   //定义字符类型数组记录在文件中读到的内容
    int l, i, j;
    Clause *p, *q;
    r = (Root *) malloc(sizeof(Root));
    r->firstclause = NULL;

    while (fscanf(*fp, "%s", readfile) != EOF) {//循环读文件
        if (strcmp(readfile, "p") == 0)//当从文件中读到单个字符p时跳出循环
            break;
    }
    while (fscanf(*fp, "%s", readfile) != EOF) {
        if (strcmp(readfile, "cnf") == 0) {  //从文件中读到字符串‘cnf’
            fscanf(*fp, "%d", &l);
            r->boolsize = l;    //读取CNF文件变元数存入r->boolsize
            fscanf(*fp, "%d", &l);
            r->clausesize = l;   //读取CNF文件子句总数存入r->clausesize
            break;
        }
    }

    /*创建变元表*/
    BoolList = (boollist *) malloc((r->boolsize + 1) * sizeof(boollist));
    if (BoolList == NULL) return OVERFLOW;
    for (i = 0; i <= r->boolsize; i++) {     //初始化变元表
        BoolList[i].flag = 0;
        BoolList[i].times = 0;
        BoolList[i].Pos.trac = NULL;
        BoolList[i].Neg.trac = NULL;
    }

    occurtimes = (int *) malloc((r->boolsize) * sizeof(int));   //动态分配数组

    i = 2;
    if (r->clausesize != 0) {  //子句集不为空
        /*创建第一个子句链表*/
        fscanf(*fp, "%d", &l);
        p = (Clause *) malloc(sizeof(Clause));   //创建子句链表头结点
        if (p == NULL) return OVERFLOW;
        r->firstclause = p;
        q = p;
        p->number = CreateClause(fp, &p->firstlit, p, l);  //创建其对应子句链表
        p->flag = 0;
        p->next = NULL;

        /*创建CNF链表*/
        while (i <= r->clausesize) {
            i++;
            fscanf(*fp, "%d", &l);
            p = (Clause *) malloc(sizeof(Clause));   //创建子句链表头结点
            if (p == NULL) return OVERFLOW;
            p->number = CreateClause(fp, &p->firstlit, p, l);   //创建其对应子句链表
            p->flag = 0;
            p->next = NULL;
            q->next = p;
            q = q->next;
        }
    }

    //初始化栈stack
    stack = (int *) malloc(20000 * sizeof(int));
    stack[0] = 0;
    top = 0;

    /*将变元按出现次数由多至少在occurtimes数组中顺序排列*/
    for (i = 0; i < r->boolsize; i++)
        occurtimes[i] = i + 1;
    for (i = 0; i < r->boolsize; i++) {
        for (j = i + 1; j <= r->boolsize; j++) {
            if (BoolList[occurtimes[i]].times < BoolList[occurtimes[j - 1]].times) {
                l = occurtimes[i];
                occurtimes[i] = occurtimes[j - 1];
                occurtimes[j - 1] = l;
            }
        }
    }

    fclose(*fp);
    return OK;
}


/*功能：创建子句链表
  参数：FILE **fp：CNF文件指针地址
      literal **head：子句链表首个文字literal类型指针地址
      Clause *listHead：子句链表头结点Clause类型指针值
      int first：在Createcnf()函数内读到的子句第一个文字值
  返回值：类型：int
        i：子句内文字的数量
        ERROR：该子句为空子句
        OVERFLOW：空间不足溢出*/
status CreateClause(FILE **fp, literal **head, Clause *listHead, int first) {
    literal *p, *q;
    list *h;
    int l, l1, i = 0;    //i记录单个子句中文字数量
    if (first == 0) return ERROR;

    //创建子句链表第一个文字结点
    p = (literal *) malloc(sizeof(literal));
    if (p == NULL) return OVERFLOW;
    p->data = first;
    p->flag = 0;
    p->next = NULL;

    q = p;
    *head = p;
    i++;
    l1 = abs(first);

    //修改变元表中该变元的相关信息
    BoolList[l1].times++;
    h = (list *) malloc(sizeof(list));
    h->cla = listHead;
    if (first > 0) {
        h->next = BoolList[l1].Pos.trac;
        BoolList[l1].Pos.trac = h;
    }
    else {
        h->next = BoolList[l1].Neg.trac;
        BoolList[l1].Neg.trac = h;
    }

    fscanf(*fp, "%d", &l);
    while (l != 0) {
        // 创建文字链表结点
        p = (literal *) malloc(sizeof(literal));
        if (p == NULL) return OVERFLOW;
        p->data = l;
        p->flag = 0;
        p->next = NULL;
        q->next = p;
        q = q->next;
        i++;

        //修改变元表中该变元的相关信息
        l1 = abs(l);
        BoolList[l1].times++;
        h = (list *) malloc(sizeof(list));
        h->cla = listHead;
        if (l > 0) {
            h->next = BoolList[l1].Pos.trac;
            BoolList[l1].Pos.trac = h;
        }
        else {
            h->next = BoolList[l1].Neg.trac;
            BoolList[l1].Neg.trac = h;
        }
        fscanf(*fp, "%d", &l);  //读取下一文字
    }

    return i;
}

/*功能：销毁CNF邻接链表
  参数：Root *r:cnf链表的根结点指针值
  返回值：类型为int。销毁成功时返回OK  */
status Destroycnf(Root *r) {
    Clause *p;
    literal *c;
    list *h;
    int i;
    if (r->firstclause != NULL) {
        p = r->firstclause;
        /*销毁CNF链表*/
        while (p != NULL) {
            if (p->firstlit != NULL) {
                c = p->firstlit;
                /*销毁单个子句链表*/
                while (c != NULL) {
                    p->firstlit = c->next;
                    free(c);//释放存储文字的单个子句链表结点空间
                    c = p->firstlit;
                }
            }
            r->firstclause = p->next;
            free(p);  //释放子句链表头结点存储空间
            p = r->firstclause;
        }

        /*释放变元表中每个变元正负文字信息链表存储空间*/
        for (i = 0; i <= r->boolsize; i++) {
            for (h = BoolList[i].Pos.trac; h != NULL; h = BoolList[i].Pos.trac) {
                BoolList[i].Pos.trac = h->next;
                free(h);
            }
            for (h = BoolList[i].Neg.trac; h != NULL; h = BoolList[i].Neg.trac) {
                BoolList[i].Neg.trac = h->next;
                free(h);
            }
        }
    }
    free(occurtimes);
    free(r);
    return OK;
}

/*功能：判断CNF范式中是否存在单子句
  参数：Root *r:cnf链表的根结点指针值
  返回值：类型：int
          存在单子句时，返回该单子句中唯一的文字值
         不存在单子句时，返回0  */
status ifsingle(Root *r) {
    Clause *p;
    literal *c;
    for (p = r->firstclause; p != NULL; p = p->next) {
        if (p->flag == 0)
            if (p->number == 1) {
                for (c = p->firstlit; c != NULL; c = c->next) {
                    if (c->flag == 0)
                        return c->data;
                }
            }
    }
    return 0;
}

/*功能：删除含文字l的子句
  参数：int l：真值为1的文字值
       Root *r:cnf链表的根结点指针值
  返回值：类型：int
         删除成功时返回OK*/
status DeleteClause(Root *r, int l) {
    int l1;
    Clause *p;
    literal *c;
    list *line;
    l1 = abs(l);
    if (l > 0) line = BoolList[l1].Pos.trac;  //l为正文字，遍历l对应变元的正文字信息链表，删除每个子句
    else line = BoolList[l1].Neg.trac;   //l为负文字，搜索l对应变元的负文字信息链表
    for (; line != NULL; line = line->next) {
        p = line->cla;
        if (p->flag == 0) {  //p指向的子句头结点对应子句未被删除
            p->flag = l1;   //将p指向结点的flag值标记为l1，表示该子句因变元l1而被删除
            r->clausesize--;   //子句个数减1
            for (c = p->firstlit; c != NULL; c = c->next)   //依次修改p指向子句中每个文字的flag标记值
                if (c->flag == 0) {
                    c->flag = l1;
                    BoolList[abs(c->data)].times--;   //该变元出现次数减1
                    p->number--;   //子句长度减1
                }
        }
    }
    return OK;
}

/*功能：删除所有文字为-l的子句链表结点
  参数l： int l：真值为1的文字值
  返回值：类型：int
         OK：成功删除文字
         FALSE：公式无解   */
status DeleteLiteral(int l) {
    Clause *p;
    literal *c;
    list *line;
    int l1, l2;
    l1 = abs(l);
    l2 = -l;
    if (l > 0) line = BoolList[l1].Neg.trac;
    else line = BoolList[l1].Pos.trac;
    for (; line != NULL; line = line->next) {
        p = line->cla;
        if (p->flag == 0) {    //p指向的子句链表未被删除
            if (p->number == 1) return FALSE;  //要删除的文字为当前子句内的唯一文字，该子句真值为0，公式无解
            for (c = p->firstlit; c != NULL; c = c->next) {
                if (c->data == l2 && c->flag == 0) {  //c指向子句链表结点内文字为l2且未被删除
                    c->flag = l1;       //标记该结点内flag值为l1，表示因变元l1而被删除
                    BoolList[abs(c->data)].times--;   //变元表内该变元出现的次数减一
                    p->number--;    //子句长度减1
                }
            }
        }
    }
    return OK;
}


/*功能：恢复认为文字l为真前的CNF邻接链表
  参数：int l：真值为1时求解出错的文字l
  返回值：类型：int
         邻接链表恢复成功时返回OK*/
status RecoverCNF(Root *r, int l) {
    Clause *p;
    literal *c;
    list *line1, *line2;
    int l1;
    l1 = abs(l);
    if (l > 0) {  //l为正文字
        line1 = BoolList[l1].Pos.trac;
        line2 = BoolList[l1].Neg.trac;
    }
    else {   //l为负文字
        line1 = BoolList[l1].Neg.trac;
        line2 = BoolList[l1].Pos.trac;
    }
    //若l为正，对变元l1的正文字信息链表进行搜索，寻找被删除的子句；若l为负，对变元l1的负文字信息链表进行搜索，寻找被删除的子句
    for (; line1 != NULL; line1 = line1->next) {
        p = line1->cla;
        if (p->flag == l1) {  //p指向的子句因设l为真而删除
            p->flag = 0;    //恢复子句的存在
            r->clausesize++;   //子句数加1
            for (c = p->firstlit; c != NULL; c = c->next) {  //遍历子句中的文字
                if (c->flag == l1) {   //c指向的文字结点因变元l1而被删除
                    c->flag = 0;    //恢复该文字的存在
                    p->number++;    //子句长度加1
                }
            }
        }
    }
    //若l为正，对变元l1的负文字信息链表进行搜索，寻找被删除的文字；若l为负，对变元l1的正文字信息链表进行搜索，寻找被删除的文字
    for (; line2 != NULL; line2 = line2->next) {
        p = line2->cla;
        for (c = p->firstlit; c != NULL; c = c->next) {   //遍历子句中的文字
            if (c->flag == l1) {    //c指向的文字结点因变元l1而被删除
                c->flag = 0;  //恢复该文字的存在
                p->number++;   //子句长度加1
            }
        }
    }
    return OK;
}

/*功能：遍历CNF邻接链表，并输出文字
  参数：Root *r:cnf链表的根结点指针值
  返回值：无*/
void cnfTravel(Root *r) {
    Clause *p;
    literal *c;
    for (p = r->firstclause; p != NULL; p = p->next) {
        for (c = p->firstlit; c != NULL; c = c->next) printf("%d ", c->data);
        printf("\n");
    }
    printf("遍历完毕！\n");
}

/*功能：在同名res文件中保存CNF范式的解及求解时间信息
 参数：int solut：CNF公式的解，有解为1，无解为0；
      int time:DPLL求解时间；
 返回值：类型：int
       ERROR：文件打开失败；
        OK：解存储成功*/
status SaveValue(boollist *ValueList, int solut, int time) {
    int i = 0,j=strlen(filename);
    FILE *save;
    char filenamesave[1000];
    strcpy(filenamesave,filename);
    filenamesave[j-1]='s';
    filenamesave[j-2]='e';
    filenamesave[j-3]='r';
    save = fopen(filenamesave, "wb");
    if (save == NULL) {
        printf("文件打开失败！\n");
        return ERROR;
    }
    fprintf(save, "s %d\nv ", solut);
    for (i = 1; i <= r->boolsize; i++) {
        if (ValueList[i].flag == 1) {
            if (ValueList[i].Value == 1) {
                fprintf(save, "%d ", i);
            }
            else fprintf(save, "%d ", -i);
        }
    }
    fprintf(save, "\nt %dms", time);
    fclose(save);
    return OK;
}

/*功能：检查SAT求解结果正确性
 参数：int solut：公式求解结果，有解为1，无解为0；
 返回值：类型：int
        TRUE：求解正确；
        FALSE：求解错误*/
status AnswerCheck(int solut) {
    Clause *p;
    literal *c;
    int flag, l, value, i = 0;
    if (solut == 1) {//公式有解
        for (p = r->firstclause; p != NULL; p = p->next) {
            i++;
            flag = 0;
            for (c = p->firstlit; c != NULL; c = c->next) {
                l = abs(c->data);
                if (c->data > 0) value = BoolList[l].Value;
                else value = 1 - BoolList[l].Value;
                if (value == 1) {
                    flag = 1;//子句中有文字真值为1，子句真值为1
                    break;
                }
            }
            if (flag == 0) {
                //
                //printf("第%d行ERROR\n",i);
                //
                return FALSE;//子句中无真值为1的文字，子句真值为0，求解错误
            }
        }
        return TRUE;
    }
    else {//公式无解
        for (p = r->firstclause; p != NULL; p = p->next) {
            flag = 0;
            for (c = p->firstlit; c != NULL; c = c->next) {
                l = abs(c->data);
                if (c->data > 0) {
                    value = BoolList[l].Value;
                }
                else value = 1 - BoolList[l].Value;
                if (value == 1) flag = 1;//子句中有文字真值为1，子句真值为1
            }
            if (flag == 0) return TRUE;//子句真值为0，求解正确
        }
        return FALSE;
    }
}


#endif //SAT_CNF_H
