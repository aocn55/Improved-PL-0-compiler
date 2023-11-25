typedef enum {
    false,
    true
} bool;

#define norw 15  /* �ؼ��ָ��� ����һ��float�ؼ��ֺ�һ��else�ؼ��� */
#define txmax 100 /* ���ֱ����� */
#define nmax 14 /* number�����λ�� */
#define al 10 /* ���ŵ���󳤶� */
#define amax 2047 /* ��ַ�Ͻ�*/
#define levmax 3 /* ����������Ƕ���������� [0,  levmax]*/
#define cxmax 500 /* ��������������� */

/* ���� */
enum symbol {
    nul,
    ident,
    number,
    numberf, /* ���� */
    plus,
    minus,
    times,
    slash,
    oddsym,
    eql,
    neq,
    lss,
    leq,
    gtr,
    geq,
    lparen,
    rparen,
    comma,
    colon,  /* ���� ���������鶨�� */
    semicolon,
    period,
    becomes,
    beginsym,
    endsym,
    ifsym,
    thensym,
    elsesym,  /* ���� else�ؼ��� */
    whilesym,
    writesym,
    readsym,
    dosym,
    callsym,
    constsym,
    floatsym,    /* ����float */
    varsym,
    procsym,
    plusplus,  /* ���� ++ */
    pluseql,   /* ���� += */
    minusminus, /* ���� -- */
    minuseql,   /* ���� -= */
    timeseql,   /* ���� *= */
    slasheql,   /* ���� /= */
};
#define symnum 42

/* ���ֱ��е����� */
enum object {
    constant,
    variable,
    flo,
    procedur,
    array       //add
};

/* ��������� */
enum fct {
    lit,
    litf, // ���� ��������������ջ��
    opr,
    lod,
    lod2, // ���� ��������
    sto,
    sto2, // ���� ��������
    cal,
    inte,
    jmp,
    jpc
};
#define fctnum 11

/* ���������ṹ */
struct instruction {
    enum fct f; /* ���������ָ�� */
    int l; /* ���ò���������Ĳ�β� */
    int a; /* ����f�Ĳ�ͬ����ͬ */
};

struct instructionf {
    enum fct f; /* ���������ָ�� */
    int l; /* ���ò���������Ĳ�β� */
    float a; /* ����f�Ĳ�ͬ����ͬ */
};


FILE* fas; /* ������ֱ� */
FILE* fa; /* ������������ */
FILE* fa1; /* ���Դ�ļ�������ж�Ӧ���׵�ַ */
FILE* fa2; /* ������ */
bool listswitch; /* ��ʾ������������ */
bool tableswitch; /* ��ʾ���ֱ���� */
char ch; /* ��ȡ�ַ��Ļ�������getch ʹ�� */
enum symbol sym; /* ��ǰ�ķ��� */
char id[al + 1]; /* ��ǰident, �����һ���ֽ����ڴ��0 */
int num; /* ��ǰnumber */
float numf; /* �������洢��ǰfloat */
int cc, ll; /* getchʹ�õļ�������cc��ʾ��ǰ�ַ�(ch)��λ�� */
int cx; /* ���������ָ��, ȡֵ��Χ[0, cxmax-1]*/
int cx3; // ����
int cxf; // ���� ���������������ָ��
char line[81]; /* ��ȡ�л����� */
char a[al + 1]; /* ��ʱ����, �����һ���ֽ����ڴ��0 */
struct instruction code[cxmax]; /* ����������������� */
struct instructionf codef[cxmax]; /* ���������ڽ�������������� */
char word[norw][al]; /* ������ */
enum symbol wsym[norw]; /* �����ֶ�Ӧ�ķ���ֵ */
enum symbol ssym[256]; /* ���ַ��ķ���ֵ */
char mnemonic[fctnum][5]; /* ���������ָ������ */
bool declbegsys[symnum]; /* ��ʾ������ʼ�ķ��ż��� */
bool statbegsys[symnum]; /* ��ʾ��俪ʼ�ķ��ż��� */
bool facbegsys[symnum]; /* ��ʾ���ӿ�ʼ�ķ��ż��� */

/* ���ֱ�ṹ */
typedef struct {
    char name[al]; /* ���� */
    enum object kind; /* ���ͣ�const, var, float or procedure */
    enum object arrkind;
    int val; /* ��ֵ����constʹ�á��������飬�洢������������ var or float */
    int level; /* �����㣬��const��ʹ�� */
    int adr; /* ��ַ����const��ʹ�� */
    int size; /* ��Ҫ������������ռ�, procedure��arrayʹ�� */
    int low;  /* �½磺��������Ҫʹ��*/
    int end;  /* �Ͻ磺��������Ҫʹ��*/
}tablestruct;

tablestruct table[txmax]; /* ���ֱ� */

FILE* fin;
FILE* fout;
char fname[al];
int err; /* ��������� */

/* �������лᷢ��fatal errorʱ������-1��֪�������ĺ����������˳����� */
#define getsymdo                       if (-1 == getsym())               return -1
#define getchdo                        if (-1 == getch())                return -1
#define testdo(a, b, c)                if (-1 == test(a, b, c))          return -1
#define gendo(a, b, c)                 if (-1 == gen(a, b, c))           return -1
#define genfdo(a, b, c)                if (-1 == genf(a, b, c))          return -1   // ����
#define expressiondo(a, b, c, d, e)          if (-1 == expression(a, b, c, d, e))    return -1
#define factordo(a, b, c)              if (-1 == factor(a, b, c))        return -1
#define termdo(a, b, c)                if (-1 == term(a, b, c))          return -1
#define conditiondo(a, b, c)           if (-1 == condition(a, b, c))     return -1
#define statementdo(a, b, c)           if (-1 == statement(a, b, c))     return -1
#define constdeclarationdo(a, b, c)    if (-1 == constdeclaration(a, b, c))   return -1
#define vardeclarationdo(a, b, c)      if (-1 == vardeclaration(a, b, c))     return -1
#define floatdeclarationdo(a, b, c)    if (-1 == floatdeclaration(a, b, c))   return -1  // ����


void error(int n);
int getsym();
int getch();
void init();
int gen(enum fct x, int y, int z);
int genf(enum fct x, int y, float z); // ����
int test(bool* s1, bool* s2, int n);
int inset(int e, bool* s);
int addset(bool* sr, bool* s1, bool* s2, int n);
int subset(bool* sr, bool* s1, bool* s2, int n);
int mulset(bool* sr, bool* s1, bool* s2, int n);
int block(int lev, int tx, bool* fsys);
void interpret();
int factor(bool* fsys, int* ptx, int lev);
int term(bool* fsys, int* ptx, int lev);
int condition(bool* fsys, int* ptx, int lev);
int expression(bool* fsys, int* ptx, int lev);
int statement(bool* fsys, int* ptx, int lev);
void listcode(int cx0);
int vardeclaration(int* ptx, int lev, int* pdx);
int floatdeclaration(int* ptx, int lev, int* pdx); // ����
int constdeclaration(int* ptx, int lev, int* pdx);
int position(char* idt, int tx);
void enter(enum object k, int* ptx, int lev, int* pdx);
int base(int l, float* s, int b);
int isDigit(int index);  // ����
