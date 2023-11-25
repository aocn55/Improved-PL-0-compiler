typedef enum {
    false,
    true
} bool;

#define norw 15  /* 关键字个数 新增一个float关键字和一个else关键字 */
#define txmax 100 /* 名字表容量 */
#define nmax 14 /* number的最大位数 */
#define al 10 /* 符号的最大长度 */
#define amax 2047 /* 地址上界*/
#define levmax 3 /* 最大允许过程嵌套声明层数 [0,  levmax]*/
#define cxmax 500 /* 最多的虚拟机代码数 */

/* 符号 */
enum symbol {
    nul,
    ident,
    number,
    numberf, /* 新增 */
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
    colon,  /* 新增 ：用于数组定义 */
    semicolon,
    period,
    becomes,
    beginsym,
    endsym,
    ifsym,
    thensym,
    elsesym,  /* 新增 else关键字 */
    whilesym,
    writesym,
    readsym,
    dosym,
    callsym,
    constsym,
    floatsym,    /* 新增float */
    varsym,
    procsym,
    plusplus,  /* 新增 ++ */
    pluseql,   /* 新增 += */
    minusminus, /* 新增 -- */
    minuseql,   /* 新增 -= */
    timeseql,   /* 新增 *= */
    slasheql,   /* 新增 /= */
};
#define symnum 42

/* 名字表中的类型 */
enum object {
    constant,
    variable,
    flo,
    procedur,
    array       //add
};

/* 虚拟机代码 */
enum fct {
    lit,
    litf, // 新增 操作浮点数进入栈顶
    opr,
    lod,
    lod2, // 新增 用于数组
    sto,
    sto2, // 新增 用于数组
    cal,
    inte,
    jmp,
    jpc
};
#define fctnum 11

/* 虚拟机代码结构 */
struct instruction {
    enum fct f; /* 虚拟机代码指令 */
    int l; /* 引用层与声明层的层次差 */
    int a; /* 根据f的不同而不同 */
};

struct instructionf {
    enum fct f; /* 虚拟机代码指令 */
    int l; /* 引用层与声明层的层次差 */
    float a; /* 根据f的不同而不同 */
};


FILE* fas; /* 输出名字表 */
FILE* fa; /* 输出虚拟机代码 */
FILE* fa1; /* 输出源文件及其各行对应的首地址 */
FILE* fa2; /* 输出结果 */
bool listswitch; /* 显示虚拟机代码与否 */
bool tableswitch; /* 显示名字表与否 */
char ch; /* 获取字符的缓冲区，getch 使用 */
enum symbol sym; /* 当前的符号 */
char id[al + 1]; /* 当前ident, 多出的一个字节用于存放0 */
int num; /* 当前number */
float numf; /* 新增，存储当前float */
int cc, ll; /* getch使用的计数器，cc表示当前字符(ch)的位置 */
int cx; /* 虚拟机代码指针, 取值范围[0, cxmax-1]*/
int cx3; // 新增
int cxf; // 新增 浮点型虚拟机代码指针
char line[81]; /* 读取行缓冲区 */
char a[al + 1]; /* 临时符号, 多出的一个字节用于存放0 */
struct instruction code[cxmax]; /* 存放虚拟机代码的数组 */
struct instructionf codef[cxmax]; /* 新增，用于解决浮点型立即数 */
char word[norw][al]; /* 保留字 */
enum symbol wsym[norw]; /* 保留字对应的符号值 */
enum symbol ssym[256]; /* 单字符的符号值 */
char mnemonic[fctnum][5]; /* 虚拟机代码指令名称 */
bool declbegsys[symnum]; /* 表示声明开始的符号集合 */
bool statbegsys[symnum]; /* 表示语句开始的符号集合 */
bool facbegsys[symnum]; /* 表示因子开始的符号集合 */

/* 名字表结构 */
typedef struct {
    char name[al]; /* 名字 */
    enum object kind; /* 类型：const, var, float or procedure */
    enum object arrkind;
    int val; /* 数值，仅const使用。对于数组，存储它的数据类型 var or float */
    int level; /* 所处层，仅const不使用 */
    int adr; /* 地址，仅const不使用 */
    int size; /* 需要分配的数据区空间, procedure和array使用 */
    int low;  /* 下界：仅数组需要使用*/
    int end;  /* 上界：仅数组需要使用*/
}tablestruct;

tablestruct table[txmax]; /* 名字表 */

FILE* fin;
FILE* fout;
char fname[al];
int err; /* 错误计数器 */

/* 当函数中会发生fatal error时，返回-1告知调用它的函数，最终退出程序 */
#define getsymdo                       if (-1 == getsym())               return -1
#define getchdo                        if (-1 == getch())                return -1
#define testdo(a, b, c)                if (-1 == test(a, b, c))          return -1
#define gendo(a, b, c)                 if (-1 == gen(a, b, c))           return -1
#define genfdo(a, b, c)                if (-1 == genf(a, b, c))          return -1   // 新增
#define expressiondo(a, b, c, d, e)          if (-1 == expression(a, b, c, d, e))    return -1
#define factordo(a, b, c)              if (-1 == factor(a, b, c))        return -1
#define termdo(a, b, c)                if (-1 == term(a, b, c))          return -1
#define conditiondo(a, b, c)           if (-1 == condition(a, b, c))     return -1
#define statementdo(a, b, c)           if (-1 == statement(a, b, c))     return -1
#define constdeclarationdo(a, b, c)    if (-1 == constdeclaration(a, b, c))   return -1
#define vardeclarationdo(a, b, c)      if (-1 == vardeclaration(a, b, c))     return -1
#define floatdeclarationdo(a, b, c)    if (-1 == floatdeclaration(a, b, c))   return -1  // 新增


void error(int n);
int getsym();
int getch();
void init();
int gen(enum fct x, int y, int z);
int genf(enum fct x, int y, float z); // 新增
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
int floatdeclaration(int* ptx, int lev, int* pdx); // 新增
int constdeclaration(int* ptx, int lev, int* pdx);
int position(char* idt, int tx);
void enter(enum object k, int* ptx, int lev, int* pdx);
int base(int l, float* s, int b);
int isDigit(int index);  // 新增
