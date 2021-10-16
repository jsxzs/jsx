#ifndef SAT_DPLL_H
#define SAT_DPLL_H

#include <time.h>
#include "cnf.h"

status DPLL1(int num, int op);//采用第一种变元选取策略的递归算法DPLL函数
status DPLL2(int num);   //采用第二种变元选取策略的递归算法DPLL函数
status DPLL3(int num);   //采用第三种变元选取策略的递归算法DPLL函数

/*在整个CNF公式中取一个文字*/
status ChooseLiteral1(Root *r);  //取原公式中Occur_Times最大的文字
status ChooseLiteral2(Root *r);  //取每次DPLL处理后公式中出现次数最大的正文字，若无正文字，取出现次数最大的负文字
status ChooseLiteral3(Root *r);  //取每次DPLL处理后公式中最短子句中第一个未被赋值的文字

void AnswerComplete(void); //完善SAT求解结果

boollist *BoolList;
Root *r;
int *occurtimes;
int *stack;
int top;
char filename[1000];

///*功能：采用第一种变元选取策略的递归算法DPLL函数
// 参数：int num：该次进入函数真值设为1的文字；
// 返回值：类型：int
//        FALSE：公式无解；
//       TRUE：公式有解*/
//status DPLL1(int num) {
//    int l;
//    l = num;
//    while (l != 0) {
//        stack[++top] = l;    //该次选取的文字l入栈
//        DeleteClause(r, l);   //设l为真，对cnf链表进行处理
//
//        //删除-l文字时出现空子句，说明公式无解，将cnf链表和变元表恢复至未设l为真时的状态，返回FALSE
//        if (DeleteLiteral(l) == FALSE) {
//            for (; stack[top] != num; top--) {
//                RecoverCNF(r, stack[top]);
//                BoolList[abs(stack[top])].flag = 0;
//            }
//            RecoverCNF(r, stack[top]);
//            BoolList[abs(stack[top])].flag = 0;
//            top--;
//            return FALSE;
//        }
//
//        //修改变元表对应变元的相关信息
//        if (l > 0) {
//            BoolList[l].Value = 1;
//            BoolList[l].flag = 1;
//        }
//        else {
//            BoolList[-l].Value = 0;
//            BoolList[-l].flag = 1;
//        }
//
//        if (ChooseLiteral1(r) == 0)
//            return TRUE;   //子句集为空，说明公式有解，返回TRUE
//        l = ifsingle(r);   //单子句策略
//    }
//    l = ChooseLiteral1(r);   //选取新的变元，分裂策略
//    if (l == 0)   //子句集为空，说明公式有解，返回TRUE
//        return TRUE;
//    if (DPLL1(l)) return TRUE;   //设l为真，递归调用DPLL求解，得出公式有解，返回TRUE
//
//    //设l为假，再次递归调用DPLL求解
//    l = -l;
//    if (DPLL1(l) == FALSE) {   //公式无解，将cnf链表和变元表恢复至未设num为真时的状态，返回FALSE
//        if (top == 0)
//            return FALSE;
//        for (; stack[top] != num; top--) {
//            RecoverCNF(r, stack[top]);
//            BoolList[abs(stack[top])].flag = 0;
//        }
//        RecoverCNF(r, stack[top]);
//        BoolList[abs(stack[top])].flag = 0;
//        top--;
//        return FALSE;
//    }
//    else return TRUE;  //公式有解，返回TRUE
//}

/*功能：采用第二种变元选取策略的递归算法DPLL函数
 参数：int num：该次进入函数真值设为1的变元序号；
      int op：普通求解cnf文件为1，生成数独终盘为2；
 返回值：FALSE：公式无解；
       TRUE：公式有解*/
status DPLL1(int num, int op) {
    int l;
    literal *c;
    Clause *p = r->firstclause;
    list *line;
    l = num;
    while (l != 0) {
        stack[++top] = l;     //该次选取的文字l入栈
        DeleteClause(r, l);   //设l为真，对cnf链表进行处理

        //删除-l文字时出现空子句，说明公式无解，将cnf链表和变元表恢复至未设num为真时的状态，返回FALSE
        if (DeleteLiteral(l) == FALSE) {
            for (; stack[top] != num; top--) {
                RecoverCNF(r, stack[top]);
                BoolList[abs(stack[top])].flag = 0;
            }
            RecoverCNF(r, stack[top]);
            BoolList[abs(stack[top])].flag = 0;
            top--;
            return FALSE;
        }

        //修改变元表对应变元的相关信息
        if (l > 0) {
            BoolList[l].Value = 1;
            BoolList[l].flag = 1;
        }
        else {
            BoolList[-l].Value = 0;
            BoolList[-l].flag = 1;
        }
        if (ChooseLiteral1(r) == 0)
            return TRUE;    //子句集为空，说明公式有解，返回TRUE
        l = ifsingle(r);   //单子句策略
    }
    if (op == 1) l = ChooseLiteral1(r);   //选取新的变元，分裂策略
    else {
        do {
            l = rand() % 729 + 1;
            for (line = BoolList[l].Neg.trac; line != NULL; line = line->next) {
                p = line->cla;
                if (p->number != 0)
                    break;
            }
        } while (BoolList[l].flag == 1);
        for (c = p->firstlit; c != NULL; c = c->next) {
            if (c->flag == 0) {
                l = c->data;
                break;
            }
        }
    }
    if (l == 0)     //子句集为空，说明公式有解，返回TRUE
        return TRUE;
    if (DPLL1(l, 1))    //设l为真，递归调用DPLL求解，得出公式有解，返回TRUE
        return TRUE;

    //设l为假，再次递归调用DPLL求解
    l = - l;
    if (DPLL1(l, 1) == FALSE) {   //公式无解，将cnf链表和变元表恢复至未设num为真时的状态，返回FALSE
        if (top == 0) return FALSE;
        for (; stack[top] != num; top--) {
            RecoverCNF(r, stack[top]);
            BoolList[abs(stack[top])].flag = 0;
        }
        RecoverCNF(r, stack[top]);
        BoolList[abs(stack[top])].flag = 0;
        top--;
        return FALSE;
    }
    else    //公式有解，返回TRUE
        return TRUE;
}

status DPLL2(int num) {
    int l;
    /*单子句策略*/
    l = num;
    while (l != 0) {
        stack[++top] = l;
        DeleteClause(r, l);
        if (DeleteLiteral(l) == FALSE) {
            for (; stack[top] != num; top--) {
                RecoverCNF(r, stack[top]);
                BoolList[abs(stack[top])].flag = 0;
            }
            RecoverCNF(r, stack[top]);
            BoolList[abs(stack[top])].flag = 0;
            top--;
            return FALSE;
        }
        if (l > 0) {
            BoolList[l].Value = 1;
            BoolList[l].flag = 1;
        }
        else {
            BoolList[0 - l].Value = 0;
            BoolList[0 - l].flag = 1;
        }
        if (ChooseLiteral2(r) == 0) return TRUE;
        l = ifsingle(r);
    }
    l = ChooseLiteral2(r);
    if (l == 0) return TRUE;
    if (DPLL2(l)) return TRUE;
    l = -l;
    if (DPLL2(l) == FALSE) {
        if (top == 0) return FALSE;
        for (; stack[top] != num; top--) {
            RecoverCNF(r, stack[top]);
            BoolList[abs(stack[top])].flag = 0;
        }
        RecoverCNF(r, stack[top]);
        BoolList[abs(stack[top])].flag = 0;
        top--;
        return FALSE;
    }
    else return TRUE;
}

status DPLL3(int num) {
    int l;
    /*单子句策略*/
    l = num;
    while (l != 0) {
        stack[++top] = l;
        DeleteClause(r, l);
        if (DeleteLiteral(l) == FALSE) {
            for (; stack[top] != num; top--) {
                RecoverCNF(r, stack[top]);
                BoolList[abs(stack[top])].flag = 0;
            }
            RecoverCNF(r, stack[top]);
            BoolList[abs(stack[top])].flag = 0;
            top--;
            return FALSE;
        }
        if (l > 0) {
            BoolList[l].Value = 1;
            BoolList[l].flag = 1;
        }
        else {
            BoolList[0 - l].Value = 0;
            BoolList[0 - l].flag = 1;
        }
        if (ChooseLiteral3(r) == 0) return TRUE;
        l = ifsingle(r);
    }
    l = ChooseLiteral3(r);
    if (l == 0) return TRUE;
    if (DPLL3(l)) return TRUE;
    l = 0 - l;
    if (DPLL3(l) == FALSE) {
        if (top == 0) return FALSE;
        for (; stack[top] != num; top--) {
            RecoverCNF(r, stack[top]);
            BoolList[abs(stack[top])].flag = 0;
        }
        RecoverCNF(r, stack[top]);
        BoolList[abs(stack[top])].flag = 0;
        top--;
        return FALSE;
    }
    else return TRUE;
}

/*功能:取原公式中未赋真值的变元中出现次数最多（Occur_Times最大）的变元文字
 返回类型：int
 返回值：公式非空：选中文字的值
       公式中没有剩余文字：0*/
status ChooseLiteral1(Root *r) {
    int i ;
    for (i = 0; i < r->boolsize; i++) {
        if (BoolList[occurtimes[i]].flag == 0) {
            return occurtimes[i];
        }
    }
    return 0;
}

status ChooseLiteral2(Root *r) {
    Clause *p = r->firstclause;
    literal *q;
    int *count, maxword = 0, max = 0, i;
    count = (int *) malloc(sizeof(int) * (r->boolsize * 2 + 1));
    for (i = 0; i <= r->boolsize * 2; i++) count[i] = 0;   //初始化
    for (; p != NULL; p = p->next) {
        if (!p->flag) {
            for (q = p->firstlit; q != NULL; q = q->next) {
                if (!q->flag) {
                    if (q->data > 0) count[q->data]++;
                    else count[r->boolsize - q->data]++;
                }
            }
        }
    }
    for (i = 1; i <= r->boolsize; i++) {
        if (max < count[i]) {
            max = count[i];
            maxword = i;
        }
    }
    if (max == 0) {
        //若没有出现正文字,找到出现次数最多的负文字
        for (i = r->boolsize + 1; i <= r->boolsize * 2; i++) {
            if (max < count[i]) {
                max = count[i];
                maxword = r->boolsize - i;
            }
        }
    }
    free(count);
    return maxword;
}

status ChooseLiteral3(Root *r) {
    Clause *p = r->firstclause, *q;
    literal *h;
    while (p != NULL && p->flag) p = p->next;
    if (p == NULL) return 0;
    int min = p->number;
    q = p;
    for (p = p->next; p != NULL; p = p->next) {
        if (p->flag == 0) {
            if (p->number < min) {
                min = p->number;
                q = p;
            }
        }
    }
    for (h = q->firstlit; h != NULL; h = h->next) {
        if (h->flag == 0) {
            if (BoolList[abs(h->data)].flag == 0) return h->data;
        }
    }
    return 0;
}

/*将未赋真值的变元（求解过程中化简舍去）赋真值1*/
void AnswerComplete(void) {
    int i;
    for (i = 1; i <= r->boolsize; i++) {
        if (BoolList[i].flag == 0) {
            BoolList[i].flag = 1;
            BoolList[i].Value = 1;
        }
    }
}


#endif //SAT_DPLL_H
