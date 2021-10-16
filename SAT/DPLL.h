#ifndef SAT_DPLL_H
#define SAT_DPLL_H

#include <time.h>
#include "cnf.h"

status DPLL1(int num, int op);//���õ�һ�ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
status DPLL2(int num);   //���õڶ��ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
status DPLL3(int num);   //���õ����ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����

/*������CNF��ʽ��ȡһ������*/
status ChooseLiteral1(Root *r);  //ȡԭ��ʽ��Occur_Times��������
status ChooseLiteral2(Root *r);  //ȡÿ��DPLL�����ʽ�г��ִ������������֣����������֣�ȡ���ִ������ĸ�����
status ChooseLiteral3(Root *r);  //ȡÿ��DPLL�����ʽ������Ӿ��е�һ��δ����ֵ������

void AnswerComplete(void); //����SAT�����

boollist *BoolList;
Root *r;
int *occurtimes;
int *stack;
int top;
char filename[1000];

///*���ܣ����õ�һ�ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
// ������int num���ôν��뺯����ֵ��Ϊ1�����֣�
// ����ֵ�����ͣ�int
//        FALSE����ʽ�޽⣻
//       TRUE����ʽ�н�*/
//status DPLL1(int num) {
//    int l;
//    l = num;
//    while (l != 0) {
//        stack[++top] = l;    //�ô�ѡȡ������l��ջ
//        DeleteClause(r, l);   //��lΪ�棬��cnf������д���
//
//        //ɾ��-l����ʱ���ֿ��Ӿ䣬˵����ʽ�޽⣬��cnf����ͱ�Ԫ��ָ���δ��lΪ��ʱ��״̬������FALSE
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
//        //�޸ı�Ԫ���Ӧ��Ԫ�������Ϣ
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
//            return TRUE;   //�Ӿ伯Ϊ�գ�˵����ʽ�н⣬����TRUE
//        l = ifsingle(r);   //���Ӿ����
//    }
//    l = ChooseLiteral1(r);   //ѡȡ�µı�Ԫ�����Ѳ���
//    if (l == 0)   //�Ӿ伯Ϊ�գ�˵����ʽ�н⣬����TRUE
//        return TRUE;
//    if (DPLL1(l)) return TRUE;   //��lΪ�棬�ݹ����DPLL��⣬�ó���ʽ�н⣬����TRUE
//
//    //��lΪ�٣��ٴεݹ����DPLL���
//    l = -l;
//    if (DPLL1(l) == FALSE) {   //��ʽ�޽⣬��cnf����ͱ�Ԫ��ָ���δ��numΪ��ʱ��״̬������FALSE
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
//    else return TRUE;  //��ʽ�н⣬����TRUE
//}

/*���ܣ����õڶ��ֱ�Ԫѡȡ���Եĵݹ��㷨DPLL����
 ������int num���ôν��뺯����ֵ��Ϊ1�ı�Ԫ��ţ�
      int op����ͨ���cnf�ļ�Ϊ1��������������Ϊ2��
 ����ֵ��FALSE����ʽ�޽⣻
       TRUE����ʽ�н�*/
status DPLL1(int num, int op) {
    int l;
    literal *c;
    Clause *p = r->firstclause;
    list *line;
    l = num;
    while (l != 0) {
        stack[++top] = l;     //�ô�ѡȡ������l��ջ
        DeleteClause(r, l);   //��lΪ�棬��cnf������д���

        //ɾ��-l����ʱ���ֿ��Ӿ䣬˵����ʽ�޽⣬��cnf����ͱ�Ԫ��ָ���δ��numΪ��ʱ��״̬������FALSE
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

        //�޸ı�Ԫ���Ӧ��Ԫ�������Ϣ
        if (l > 0) {
            BoolList[l].Value = 1;
            BoolList[l].flag = 1;
        }
        else {
            BoolList[-l].Value = 0;
            BoolList[-l].flag = 1;
        }
        if (ChooseLiteral1(r) == 0)
            return TRUE;    //�Ӿ伯Ϊ�գ�˵����ʽ�н⣬����TRUE
        l = ifsingle(r);   //���Ӿ����
    }
    if (op == 1) l = ChooseLiteral1(r);   //ѡȡ�µı�Ԫ�����Ѳ���
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
    if (l == 0)     //�Ӿ伯Ϊ�գ�˵����ʽ�н⣬����TRUE
        return TRUE;
    if (DPLL1(l, 1))    //��lΪ�棬�ݹ����DPLL��⣬�ó���ʽ�н⣬����TRUE
        return TRUE;

    //��lΪ�٣��ٴεݹ����DPLL���
    l = - l;
    if (DPLL1(l, 1) == FALSE) {   //��ʽ�޽⣬��cnf����ͱ�Ԫ��ָ���δ��numΪ��ʱ��״̬������FALSE
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
    else    //��ʽ�н⣬����TRUE
        return TRUE;
}

status DPLL2(int num) {
    int l;
    /*���Ӿ����*/
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
    /*���Ӿ����*/
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

/*����:ȡԭ��ʽ��δ����ֵ�ı�Ԫ�г��ִ�����ࣨOccur_Times��󣩵ı�Ԫ����
 �������ͣ�int
 ����ֵ����ʽ�ǿգ�ѡ�����ֵ�ֵ
       ��ʽ��û��ʣ�����֣�0*/
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
    for (i = 0; i <= r->boolsize * 2; i++) count[i] = 0;   //��ʼ��
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
        //��û�г���������,�ҵ����ִ������ĸ�����
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

/*��δ����ֵ�ı�Ԫ���������л�����ȥ������ֵ1*/
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
