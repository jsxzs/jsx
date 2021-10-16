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

/*�����Ӿ�������ṹ����*/
typedef struct literal {
    int data;//��¼�Ӿ��е�����
    int flag;//��Ǹ������Ƿ��ѱ�ɾ����δɾ��ʱֵΪ0������ֵΪʹ֮ɾ���ı�Ԫ���
    struct literal *next;//ָ����Ӿ�����һ���ֵ�ָ��
} literal;

/*����CNF��ʽ�����㣨���Ӿ�����ͷ��㣩�ṹ����*/
typedef struct Clause {
    int number;//�Ӿ�����ʾ��������
    int flag;//��Ǹ��Ӿ��Ƿ��ѱ�ɾ����δɾ��ʱֵΪ0������ֵΪʹ֮ɾ���ı�Ԫ���
    struct literal *firstlit;//�Ӿ�ͷָ��
    struct Clause *next;//ָ����һ�Ӿ��ͷ���
} Clause;

/*����CNF��ʽ����ͷ������ͣ��洢CNF��ʽ��Ϣ*/
typedef struct Root {
    int boolsize;//�洢��������
    int clausesize;//�洢�Ӿ�����
    Clause *firstclause;//ָ���һ���Ӿ�
} Root;

/*����ָ���Ӿ�����ͷ����������ṹ����*/
typedef struct list {
    Clause *cla;     //ָ���Ӿ�����ͷ���
    struct list *next;  //ָ��������һ���
} list;

/*�������������Ϣ����ṹ����*/
typedef struct LitTravel {
    list *trac;  //ָ����ĳ��Ԫ�����ֻ����ֵ��Ӿ�ͷ��������ͷ���
} LitTravel;

/*����洢��Ԫ��Ϣ�ı�Ԫ���Ա�Ԫ�ؽṹ����*/
typedef struct boollist {
    int Value;  //��Ԫ����ֵ
    int flag;   //��Ԫ�Ƿ��Ѹ�ֵ
    int times;  //��Ԫ�������Ӿ��г��ֵ��ܴ���
    LitTravel Pos;   //���иñ�Ԫ�����ֵ��Ӿ�ͷ��������ͷ���
    LitTravel Neg;   //���иñ�Ԫ�����ֵ��Ӿ�ͷ��������ͷ���
} boollist;


extern boollist *BoolList;  //��Ԫ���Ա�
extern Root *r;    //cnf��ʽ����ĸ�
extern int *occurtimes;     //ͳ��ԭcnf�ļ������б�Ԫ���ִ�����������
extern int *stack;   //��¼ѡȡ�ı�Ԫ����֮�����ĵ��Ӿ����֣�����dpll����
extern int top;     //ָ��ջ��
extern char filename[1000];

status Createcnf(FILE **fp);    //����CNF�ڽ�������Ԫ��
status CreateClause(FILE **fp, literal **head, Clause *listHead, int first);  //�����Ӿ�����
status Destroycnf(Root *r);  //���������������Ա�ṹ
status ifsingle(Root *r);   //�ж�CNF�������Ƿ��е��Ӿ�

status DeleteClause(Root *r, int l);     //ɾ������������l�����е��Ӿ�
status DeleteLiteral(int l);   //ɾ����������Ϊ-l���Ӿ�������
status RecoverCNF(Root *r, int l);     //�ָ���Ϊ����lΪ��ʱ��CNF�ڽ����������Ĳ���
void cnfTravel(Root *r);      //����CNF�ڽ�����
status SaveValue(boollist *ValueList, int solut, int time);//����CNF�Ľ⼰���ʱ����Ϣ
status AnswerCheck(int solut);

/*���ܣ�����CNF�ڽ�������Ԫ��
  ������FILE **fp���ļ���ָ���ַ
  ����ֵ�����ͣ�int
        OK�������ɹ�  */
status Createcnf(FILE **fp) {
    char readfile[20];   //�����ַ����������¼���ļ��ж���������
    int l, i, j;
    Clause *p, *q;
    r = (Root *) malloc(sizeof(Root));
    r->firstclause = NULL;

    while (fscanf(*fp, "%s", readfile) != EOF) {//ѭ�����ļ�
        if (strcmp(readfile, "p") == 0)//�����ļ��ж��������ַ�pʱ����ѭ��
            break;
    }
    while (fscanf(*fp, "%s", readfile) != EOF) {
        if (strcmp(readfile, "cnf") == 0) {  //���ļ��ж����ַ�����cnf��
            fscanf(*fp, "%d", &l);
            r->boolsize = l;    //��ȡCNF�ļ���Ԫ������r->boolsize
            fscanf(*fp, "%d", &l);
            r->clausesize = l;   //��ȡCNF�ļ��Ӿ���������r->clausesize
            break;
        }
    }

    /*������Ԫ��*/
    BoolList = (boollist *) malloc((r->boolsize + 1) * sizeof(boollist));
    if (BoolList == NULL) return OVERFLOW;
    for (i = 0; i <= r->boolsize; i++) {     //��ʼ����Ԫ��
        BoolList[i].flag = 0;
        BoolList[i].times = 0;
        BoolList[i].Pos.trac = NULL;
        BoolList[i].Neg.trac = NULL;
    }

    occurtimes = (int *) malloc((r->boolsize) * sizeof(int));   //��̬��������

    i = 2;
    if (r->clausesize != 0) {  //�Ӿ伯��Ϊ��
        /*������һ���Ӿ�����*/
        fscanf(*fp, "%d", &l);
        p = (Clause *) malloc(sizeof(Clause));   //�����Ӿ�����ͷ���
        if (p == NULL) return OVERFLOW;
        r->firstclause = p;
        q = p;
        p->number = CreateClause(fp, &p->firstlit, p, l);  //�������Ӧ�Ӿ�����
        p->flag = 0;
        p->next = NULL;

        /*����CNF����*/
        while (i <= r->clausesize) {
            i++;
            fscanf(*fp, "%d", &l);
            p = (Clause *) malloc(sizeof(Clause));   //�����Ӿ�����ͷ���
            if (p == NULL) return OVERFLOW;
            p->number = CreateClause(fp, &p->firstlit, p, l);   //�������Ӧ�Ӿ�����
            p->flag = 0;
            p->next = NULL;
            q->next = p;
            q = q->next;
        }
    }

    //��ʼ��ջstack
    stack = (int *) malloc(20000 * sizeof(int));
    stack[0] = 0;
    top = 0;

    /*����Ԫ�����ִ����ɶ�������occurtimes������˳������*/
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


/*���ܣ������Ӿ�����
  ������FILE **fp��CNF�ļ�ָ���ַ
      literal **head���Ӿ������׸�����literal����ָ���ַ
      Clause *listHead���Ӿ�����ͷ���Clause����ָ��ֵ
      int first����Createcnf()�����ڶ������Ӿ��һ������ֵ
  ����ֵ�����ͣ�int
        i���Ӿ������ֵ�����
        ERROR�����Ӿ�Ϊ���Ӿ�
        OVERFLOW���ռ䲻�����*/
status CreateClause(FILE **fp, literal **head, Clause *listHead, int first) {
    literal *p, *q;
    list *h;
    int l, l1, i = 0;    //i��¼�����Ӿ�����������
    if (first == 0) return ERROR;

    //�����Ӿ������һ�����ֽ��
    p = (literal *) malloc(sizeof(literal));
    if (p == NULL) return OVERFLOW;
    p->data = first;
    p->flag = 0;
    p->next = NULL;

    q = p;
    *head = p;
    i++;
    l1 = abs(first);

    //�޸ı�Ԫ���иñ�Ԫ�������Ϣ
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
        // ��������������
        p = (literal *) malloc(sizeof(literal));
        if (p == NULL) return OVERFLOW;
        p->data = l;
        p->flag = 0;
        p->next = NULL;
        q->next = p;
        q = q->next;
        i++;

        //�޸ı�Ԫ���иñ�Ԫ�������Ϣ
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
        fscanf(*fp, "%d", &l);  //��ȡ��һ����
    }

    return i;
}

/*���ܣ�����CNF�ڽ�����
  ������Root *r:cnf����ĸ����ָ��ֵ
  ����ֵ������Ϊint�����ٳɹ�ʱ����OK  */
status Destroycnf(Root *r) {
    Clause *p;
    literal *c;
    list *h;
    int i;
    if (r->firstclause != NULL) {
        p = r->firstclause;
        /*����CNF����*/
        while (p != NULL) {
            if (p->firstlit != NULL) {
                c = p->firstlit;
                /*���ٵ����Ӿ�����*/
                while (c != NULL) {
                    p->firstlit = c->next;
                    free(c);//�ͷŴ洢���ֵĵ����Ӿ�������ռ�
                    c = p->firstlit;
                }
            }
            r->firstclause = p->next;
            free(p);  //�ͷ��Ӿ�����ͷ���洢�ռ�
            p = r->firstclause;
        }

        /*�ͷű�Ԫ����ÿ����Ԫ����������Ϣ����洢�ռ�*/
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

/*���ܣ��ж�CNF��ʽ���Ƿ���ڵ��Ӿ�
  ������Root *r:cnf����ĸ����ָ��ֵ
  ����ֵ�����ͣ�int
          ���ڵ��Ӿ�ʱ�����ظõ��Ӿ���Ψһ������ֵ
         �����ڵ��Ӿ�ʱ������0  */
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

/*���ܣ�ɾ��������l���Ӿ�
  ������int l����ֵΪ1������ֵ
       Root *r:cnf����ĸ����ָ��ֵ
  ����ֵ�����ͣ�int
         ɾ���ɹ�ʱ����OK*/
status DeleteClause(Root *r, int l) {
    int l1;
    Clause *p;
    literal *c;
    list *line;
    l1 = abs(l);
    if (l > 0) line = BoolList[l1].Pos.trac;  //lΪ�����֣�����l��Ӧ��Ԫ����������Ϣ����ɾ��ÿ���Ӿ�
    else line = BoolList[l1].Neg.trac;   //lΪ�����֣�����l��Ӧ��Ԫ�ĸ�������Ϣ����
    for (; line != NULL; line = line->next) {
        p = line->cla;
        if (p->flag == 0) {  //pָ����Ӿ�ͷ����Ӧ�Ӿ�δ��ɾ��
            p->flag = l1;   //��pָ�����flagֵ���Ϊl1����ʾ���Ӿ����Ԫl1����ɾ��
            r->clausesize--;   //�Ӿ������1
            for (c = p->firstlit; c != NULL; c = c->next)   //�����޸�pָ���Ӿ���ÿ�����ֵ�flag���ֵ
                if (c->flag == 0) {
                    c->flag = l1;
                    BoolList[abs(c->data)].times--;   //�ñ�Ԫ���ִ�����1
                    p->number--;   //�Ӿ䳤�ȼ�1
                }
        }
    }
    return OK;
}

/*���ܣ�ɾ����������Ϊ-l���Ӿ�������
  ����l�� int l����ֵΪ1������ֵ
  ����ֵ�����ͣ�int
         OK���ɹ�ɾ������
         FALSE����ʽ�޽�   */
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
        if (p->flag == 0) {    //pָ����Ӿ�����δ��ɾ��
            if (p->number == 1) return FALSE;  //Ҫɾ��������Ϊ��ǰ�Ӿ��ڵ�Ψһ���֣����Ӿ���ֵΪ0����ʽ�޽�
            for (c = p->firstlit; c != NULL; c = c->next) {
                if (c->data == l2 && c->flag == 0) {  //cָ���Ӿ�������������Ϊl2��δ��ɾ��
                    c->flag = l1;       //��Ǹý����flagֵΪl1����ʾ���Ԫl1����ɾ��
                    BoolList[abs(c->data)].times--;   //��Ԫ���ڸñ�Ԫ���ֵĴ�����һ
                    p->number--;    //�Ӿ䳤�ȼ�1
                }
            }
        }
    }
    return OK;
}


/*���ܣ��ָ���Ϊ����lΪ��ǰ��CNF�ڽ�����
  ������int l����ֵΪ1ʱ�����������l
  ����ֵ�����ͣ�int
         �ڽ�����ָ��ɹ�ʱ����OK*/
status RecoverCNF(Root *r, int l) {
    Clause *p;
    literal *c;
    list *line1, *line2;
    int l1;
    l1 = abs(l);
    if (l > 0) {  //lΪ������
        line1 = BoolList[l1].Pos.trac;
        line2 = BoolList[l1].Neg.trac;
    }
    else {   //lΪ������
        line1 = BoolList[l1].Neg.trac;
        line2 = BoolList[l1].Pos.trac;
    }
    //��lΪ�����Ա�Ԫl1����������Ϣ�������������Ѱ�ұ�ɾ�����Ӿ䣻��lΪ�����Ա�Ԫl1�ĸ�������Ϣ�������������Ѱ�ұ�ɾ�����Ӿ�
    for (; line1 != NULL; line1 = line1->next) {
        p = line1->cla;
        if (p->flag == l1) {  //pָ����Ӿ�����lΪ���ɾ��
            p->flag = 0;    //�ָ��Ӿ�Ĵ���
            r->clausesize++;   //�Ӿ�����1
            for (c = p->firstlit; c != NULL; c = c->next) {  //�����Ӿ��е�����
                if (c->flag == l1) {   //cָ������ֽ�����Ԫl1����ɾ��
                    c->flag = 0;    //�ָ������ֵĴ���
                    p->number++;    //�Ӿ䳤�ȼ�1
                }
            }
        }
    }
    //��lΪ�����Ա�Ԫl1�ĸ�������Ϣ�������������Ѱ�ұ�ɾ�������֣���lΪ�����Ա�Ԫl1����������Ϣ�������������Ѱ�ұ�ɾ��������
    for (; line2 != NULL; line2 = line2->next) {
        p = line2->cla;
        for (c = p->firstlit; c != NULL; c = c->next) {   //�����Ӿ��е�����
            if (c->flag == l1) {    //cָ������ֽ�����Ԫl1����ɾ��
                c->flag = 0;  //�ָ������ֵĴ���
                p->number++;   //�Ӿ䳤�ȼ�1
            }
        }
    }
    return OK;
}

/*���ܣ�����CNF�ڽ��������������
  ������Root *r:cnf����ĸ����ָ��ֵ
  ����ֵ����*/
void cnfTravel(Root *r) {
    Clause *p;
    literal *c;
    for (p = r->firstclause; p != NULL; p = p->next) {
        for (c = p->firstlit; c != NULL; c = c->next) printf("%d ", c->data);
        printf("\n");
    }
    printf("������ϣ�\n");
}

/*���ܣ���ͬ��res�ļ��б���CNF��ʽ�Ľ⼰���ʱ����Ϣ
 ������int solut��CNF��ʽ�Ľ⣬�н�Ϊ1���޽�Ϊ0��
      int time:DPLL���ʱ�䣻
 ����ֵ�����ͣ�int
       ERROR���ļ���ʧ�ܣ�
        OK����洢�ɹ�*/
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
        printf("�ļ���ʧ�ܣ�\n");
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

/*���ܣ����SAT�������ȷ��
 ������int solut����ʽ��������н�Ϊ1���޽�Ϊ0��
 ����ֵ�����ͣ�int
        TRUE�������ȷ��
        FALSE��������*/
status AnswerCheck(int solut) {
    Clause *p;
    literal *c;
    int flag, l, value, i = 0;
    if (solut == 1) {//��ʽ�н�
        for (p = r->firstclause; p != NULL; p = p->next) {
            i++;
            flag = 0;
            for (c = p->firstlit; c != NULL; c = c->next) {
                l = abs(c->data);
                if (c->data > 0) value = BoolList[l].Value;
                else value = 1 - BoolList[l].Value;
                if (value == 1) {
                    flag = 1;//�Ӿ�����������ֵΪ1���Ӿ���ֵΪ1
                    break;
                }
            }
            if (flag == 0) {
                //
                //printf("��%d��ERROR\n",i);
                //
                return FALSE;//�Ӿ�������ֵΪ1�����֣��Ӿ���ֵΪ0��������
            }
        }
        return TRUE;
    }
    else {//��ʽ�޽�
        for (p = r->firstclause; p != NULL; p = p->next) {
            flag = 0;
            for (c = p->firstlit; c != NULL; c = c->next) {
                l = abs(c->data);
                if (c->data > 0) {
                    value = BoolList[l].Value;
                }
                else value = 1 - BoolList[l].Value;
                if (value == 1) flag = 1;//�Ӿ�����������ֵΪ1���Ӿ���ֵΪ1
            }
            if (flag == 0) return TRUE;//�Ӿ���ֵΪ0�������ȷ
        }
        return FALSE;
    }
}


#endif //SAT_CNF_H
