/*
 * 使用方法：
 * 运行后输入PL/0源程序文件?
 * 回答是否输出虚拟机代码
 * 回答是否输出名字表
 * fa.tmp输出虚拟机代码
 * fa1.tmp输出源文件及其各行对应的首地址
 * fa2.tmp输出结果
 * fas.tmp输出名字表
 */
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include "pl0.h"
#include "string.h"

#define stacksize 500

int main()
{
	bool nxtlev[symnum];

	printf("Input pl/0 file?  ");
	scanf("%s", fname); /* 输入文件名 */

	fin = fopen(fname, "r");

	if (fin)
	{
		printf("List object code?(Y/N)");   /* 是否输出虚拟机代码 */
		scanf("%s", fname);
		listswitch = (fname[0] == 'y' || fname[0] == 'Y');

		printf("List symbol table?(Y/N)");  /* 是否输出名字表 */
		scanf("%s", fname);
		tableswitch = (fname[0] == 'y' || fname[0] == 'Y');

		fa1 = fopen("fa1.tmp", "w");
		fprintf(fa1, "Input pl/0 file?   ");
		fprintf(fa1, "%s\n", fname);

		init(); /* 初始化 */

		err = 0;
		cc = cx = ll = 0;
		cxf = 0;
		ch = ' ';
		if (-1 != getsym())
		{
			fa = fopen("fa.tmp", "w");
			fas = fopen("fas.tmp", "w");
			addset(nxtlev, declbegsys, statbegsys, symnum);
			nxtlev[period] = true;

			if (-1 == block(0, 0, nxtlev)) /* 调用编译程序 */
			{
				fclose(fa);
				fclose(fa1);
				fclose(fas);
				fclose(fin);
				printf("\n");
				return 0;
			}
			fclose(fa);
			fclose(fa1);
			fclose(fas);

			if (sym != period)
			{
				error(9);
			}

			if (err == 0)
			{
				fa2 = fopen("fa2.tmp", "w");
				interpret(); /* 调用解释执行程序 */
				fclose(fa2);
			}
			else
			{
				printf("%d errors in pl/0 program", err);
			}
		}

		fclose(fin);
	}
	else
	{
		printf("无法找到或打开PL/0文件\n");
	}

	printf("\n");
	return 0;
}

/*
 * 初始化
 */
void init()
{
	int i;

	/* 设置单字符符号。散列存储，便于查找 */
	for (i = 0; i <= 255; i++)
	{
		ssym[i] = nul;
	}
	ssym['+'] = plus;
	ssym['-'] = minus;
	ssym['*'] = times;
	ssym['/'] = slash;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	ssym['='] = eql;
	ssym[','] = comma;
	ssym['.'] = period;
	ssym['#'] = neq;
	ssym[';'] = semicolon;
	ssym[':'] = colon;

	/* 设置保留字名字,按照字母顺序，便于折半查找 */
	strcpy(&(word[0][0]), "begin");
	strcpy(&(word[1][0]), "call");
	strcpy(&(word[2][0]), "const");
	strcpy(&(word[3][0]), "do");
	strcpy(&(word[4][0]), "else"); // 新增 else
	strcpy(&(word[5][0]), "end");
	strcpy(&(word[6][0]), "float");//新增float
	strcpy(&(word[7][0]), "if");
	strcpy(&(word[8][0]), "odd");
	strcpy(&(word[9][0]), "procedure");
	strcpy(&(word[10][0]), "read");
	strcpy(&(word[11][0]), "then");
	strcpy(&(word[12][0]), "var");
	strcpy(&(word[13][0]), "while");
	strcpy(&(word[14][0]), "write");

	/* 设置保留字符号 */
	wsym[0] = beginsym;
	wsym[1] = callsym;
	wsym[2] = constsym;
	wsym[3] = dosym;
	wsym[4] = elsesym;
	wsym[5] = endsym;
	wsym[6] = floatsym;
	wsym[7] = ifsym;
	wsym[8] = oddsym;
	wsym[9] = procsym;
	wsym[10] = readsym;
	wsym[11] = thensym;
	wsym[12] = varsym;
	wsym[13] = whilesym;
	wsym[14] = writesym;


	/* 设置指令名称 */
	strcpy(&(mnemonic[lit][0]), "lit");
	strcpy(&(mnemonic[litf][0]), "litf"); // 新增
	strcpy(&(mnemonic[opr][0]), "opr");
	strcpy(&(mnemonic[lod][0]), "lod");
	strcpy(&(mnemonic[lod2][0]), "lod2"); // 新增
	strcpy(&(mnemonic[sto][0]), "sto");
	strcpy(&(mnemonic[sto2][0]), "sto2");  // 新增
	strcpy(&(mnemonic[cal][0]), "cal");
	strcpy(&(mnemonic[inte][0]), "int");
	strcpy(&(mnemonic[jmp][0]), "jmp");
	strcpy(&(mnemonic[jpc][0]), "jpc");

	/* 设置符号集 */
	for (i = 0; i < symnum; i++)
	{
		declbegsys[i] = false;
		statbegsys[i] = false;
		facbegsys[i] = false;
	}

	/* 设置声明开始符号集 */
	declbegsys[constsym] = true;
	declbegsys[varsym] = true;
	declbegsys[floatsym] = true; // 新增
	declbegsys[procsym] = true;

	/* 设置语句开始符号集 */
	statbegsys[beginsym] = true;
	statbegsys[callsym] = true;
	statbegsys[ifsym] = true;
	statbegsys[whilesym] = true;
	statbegsys[readsym] = true;
	statbegsys[writesym] = true;


	/* 设置因子开始符号集 */
	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[numberf] = true;
	facbegsys[lparen] = true;
	facbegsys[plusplus] = true;//新增自增 
	facbegsys[minusminus] = true;//新增自减 
}

/*
 * 用数组实现集合的集合运算
 */
int inset(int e, bool* s)
{
	return s[e];
}

int addset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] || s2[i];
	}
	return 0;
}

int subset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] && (!s2[i]);
	}
	return 0;
}

int mulset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		sr[i] = s1[i] && s2[i];
	}
	return 0;
}

/*
 *   出错处理，打印出错位置和错误编码
 */
void error(int n)
{
	char space[81];
	memset(space, 32, 81);

	space[cc - 1] = 0; //出错时当前符号已经读完，所以cc-1

	printf("****%s!%d\n", space, n);
	fprintf(fa1, "****%s!%d\n", space, n);

	err++;
}

/*
 * 漏掉空格，读取一个字符。
 *
 * 每次读一行，存入line缓冲区，line被getsym取空后再读一行
 *
 * 被函数getsym调用。
 */
int getch()
{
	if (cc == ll)
	{
		if (feof(fin))
		{
			printf("program incomplete");
			return -1;
		}
		ll = 0;
		cc = 0;
		printf("%d ", cx);
		fprintf(fa1, "%d ", cx);
		ch = ' ';
		while (ch != 10)
		{
			// fscanf(fin,"%c", &ch)
			// richard
			if (EOF == fscanf(fin, "%c", &ch))
			{
				line[ll] = 0;
				break;
			}
			// end richard
			printf("%c", ch);
			fprintf(fa1, "%c", ch);
			line[ll] = ch;
			ll++;
		}
		printf("\n");
		fprintf(fa1, "\n");
	}
	ch = line[cc];
	cc++;
	return 0;
}

/*
 * 词法分析，获取一个符号
 */
int getsym()
{
	int i, j, k;
	while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
	{
		getchdo;
	}
	if (ch >= 'a' && ch <= 'z') { /* 名字或保留字以a..z开头 */
		k = 0;
		do {
			if (k < al)
			{
				a[k] = ch;
				k++;
			}
			getchdo;
		} while (ch >= 'a' && ch <= 'z' || ch >= '0' && ch <= '9');
		a[k] = 0;
		strcpy(id, a);
		i = 0;
		j = norw - 1;
		do { /* 搜索当前符号是否为保留字 */
			k = (i + j) / 2;
			if (strcmp(id, word[k]) <= 0)
			{
				j = k - 1;
			}
			if (strcmp(id, word[k]) >= 0)
			{
				i = k + 1;
			}
		} while (i <= j);
		if (i - 1 > j) {
			sym = wsym[k];
		}
		else
		{
			sym = ident; /* 搜索失败则，是名字或数字 */
		}
	}
	else
	{
		if (ch >= '0' && ch <= '9') { /* 检测是否为数字：以0..9开头 */
			k = 0;
			num = 0;
			sym = number;
			do {
				num = 10 * num + ch - '0';
				k++;
				getchdo;
			} while (ch >= '0' && ch <= '9'); /* 获取数字的值 */

			if (ch == '.')
			{
				sym = numberf;
				getchdo;
				numf = num;
				float time = 0.1;
				do {
					numf += (ch - '0') * time;
					time *= 0.1;
					getchdo;
				} while (ch >= '0' && ch <= '9'); /* 获取数字的值 */
			}
			else
			{
				k--;
				if (k > nmax) {
					error(30);
				}
			}

		}
		else
		{
			if (ch == ':') /* 检测赋值符号 */
			{
				getchdo;
				if (ch == '=')
				{
					sym = becomes;
					getchdo;
				}
				else
				{
					sym = colon;   /* 数组上下界符号 */
				}
			}
			else if (ch == '+')
			{
				getchdo;
				if (ch == '=') {  /* '+'后面是'='就把pluseql赋给sym */
					sym = pluseql;
					getchdo;
				}
				else if (ch == '+') {  /* '+'后面是'+'就把plusplus赋给sym */
					sym = plusplus;
					getchdo;
				}
				else
				{
					sym = plus;
				}
			}
			else if (ch == '-')
			{
				getchdo;
				if (ch == '=') {  /* '-'后面是'='就把minuseql赋给sym */
					sym = minuseql;
					getchdo;
				}
				else if (ch == '-') {  /* '-'后面是'-'就把minusminus赋给sym */
					sym = minusminus;
					getchdo;
				}
				else {
					sym = minus;
				}
			}
			else if (ch == '*')
			{
				getchdo;
				if (ch == '=') {  /* '*'后面是'='就把timeseql赋给sym */
					sym = timeseql;
					getchdo;
				}
				else {
					sym = times;
				}
			}
			else if (ch == '/')
			{
				getchdo;
				if (ch == '=') {  /* '/'后面是'='就把slasheql赋给sym */
					sym = slasheql;
					getchdo;
				}
				else {
					sym = slash;
				}
			}
			else
			{
				if (ch == '<') /* 检测小于或小于等于符号 */
				{
					getchdo;
					if (ch == '=')
					{
						sym = leq;
						getchdo;
					}
					else
					{
						sym = lss;
					}
				}
				else
				{
					if (ch == '>') /* 检测大于或大于等于符号 */
					{
						getchdo;
						if (ch == '=')
						{
							sym = geq;
							getchdo;
						}
						else
						{
							sym = gtr;
						}
					}
					else
					{
						sym = ssym[ch]; /* 当符号不满足上述条件时，全部按照单字符符号处理 */
						// getchdo;
						// richard
						if (sym != period) {
							getchdo;
						}
						// end richard
					}
				}
			}
		}
	}
	return 0;
}

/*
 * 生成虚拟机代码
 *
 * x: instruction.f;
 * y: instruction.l;
 * z: instruction.a;
 */
int gen(enum fct x, int y, int z)
{
	if (cx >= cxmax) {
		printf("Program too long"); /* 程序过长 */
		return -1;
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx].a = z;
	cx++;
	return 0;
}

int genf(enum fct x, int y, float z)
{
	if (cxf >= cxmax) {
		printf("Program too long"); /* 程序过长 */
		return -1;
	}
	codef[cxf].f = x;
	codef[cxf].l = y;
	codef[cxf].a = z;
	cxf++;
	return 0;
}

/*
 * 测试当前符号是否合法
 *
 * 在某一部分（如一条语句，一个表达式）将要结束时时我们希望下一个符号属于某集?
 * （该部分的后跟符号），test负责这项检测，并且负责当检测不通过时的补救措施，
 * 程序在需要检测时指定当前需要的符号集合和补救用的集合（如之前未完成部分的后跟
 * 符号），以及检测不通过时的错误号。
 *
 * s1:   我们需要的符号
 * s2:   如果不是我们需要的，则需要一个补救用的集?
 * n:    错误号
 */
int test(bool* s1, bool* s2, int n)
{
	if (!inset(sym, s1)) {
		error(n);
		/* 当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合 */
		while ((!inset(sym, s1)) && (!inset(sym, s2))) {
			getsymdo;
		}
	}
	return 0;
}

/*
 * 编译程序主?
 *
 * lev:    当前分程序所在层
 * tx:     名字表当前尾指针
 * fsys:   当前模块后跟符号集?
 */
int block(int lev, int tx, bool* fsys)
{
	int i;

	int dx; /* 名字分配到的相对地址 */
	int tx0; /* 保留初始tx */
	int cx0; /* 保留初始cx */
	bool nxtlev[symnum]; /* 在下级函数的参数中，符号集合均为值参，但由于使用数组实现，
												 传递进来的是指针，为防止下级函数改变上级函数的集合，开辟新的空?
												 传递给下级函数*/

	dx = 3;
	tx0 = tx; /* 记录本层名字的初始位置 */
	table[tx].adr = cx;

	gendo(jmp, 0, 0);

	if (lev > levmax) {
		error(32);
	}

	do {
		if (sym == constsym) /* 收到常量声明符号，开始处理常量声明 */
		{
			getsymdo;
			do {
				constdeclarationdo(&tx, lev, &dx); /* dx的值会被constdeclaration改变，使用指针 */
				while (sym == comma) {
					getsymdo;
					constdeclarationdo(&tx, lev, &dx);
				}
				if (sym == semicolon) {
					getsymdo;
				}

				else
				{
					error(5); /*漏掉了逗号或者分号*/
				}

			} while (sym == ident);
		}

		if (sym == varsym) /* 收到变量声明符号，开始处理变量声明 */
		{
			getsymdo;
			do {
				vardeclarationdo(&tx, lev, &dx);
				while (sym == comma) {
					getsymdo;
					vardeclarationdo(&tx, lev, &dx);
				}
				if (sym == semicolon) {
					getsymdo;
				}
				else {
					error(5);
				}
			} while (sym == ident);
		}

		if (sym == floatsym) /* 收到变量声明符号，开始处理变量声明 */
		{
			getsymdo;
			floatdeclarationdo(&tx, lev, &dx);
			while (sym == comma) {
				getsymdo;
				floatdeclarationdo(&tx, lev, &dx);
			}
			if (sym == semicolon) {
				getsymdo;
			}
			else {
				error(5);
			}
		}

		while (sym == procsym) /* 收到过程声明符号，开始处理过程声明 */
		{
			getsymdo;
			if (sym == ident) {
				enter(procedur, &tx, lev, &dx); /* 记录过程名字 */
				getsymdo;
			}
			else {
				error(4); /* procedure后应为标识符 */
			}

			if (sym == semicolon) {
				getsymdo;
			}
			else {
				error(5); /* 漏掉了分号 */
			}

			memcpy(nxtlev, fsys, sizeof(bool) * symnum);
			nxtlev[semicolon] = true;
			if (-1 == block(lev + 1, tx, nxtlev)) {
				return -1; /* 递归调用 */
			}

			if (sym == semicolon) {
				getsymdo;
				memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
				nxtlev[ident] = true;
				nxtlev[procsym] = true;
				testdo(nxtlev, fsys, 6);
			}
			else {
				error(5); /* 漏掉了分号 */
			}
		}
		memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
		memcpy(nxtlev, declbegsys, sizeof(bool) * symnum);  // 在每个子程序里也可以进行声明变量
		// nxtlev[ident] = true;
		nxtlev[beginsym] = true;
		testdo(nxtlev, declbegsys, 7);
	} while (inset(sym, declbegsys)); /* 直到没有声明符号 */

	code[table[tx0].adr].a = cx; /* 开始生成当前过程代码 */
	table[tx0].adr = cx; /* 当前过程代码地址 */
	table[tx0].size = dx; /* 声明部分中每增加一条声明都会给dx增加1，声明部分已经结束，dx就是当前过程数据的size */
	cx0 = cx;
	gendo(inte, 0, dx); /* 生成分配内存代码 */

	if (tableswitch) /* 输出名字表 */
	{
		printf("TABLE:\n");
		if (tx0 + 1 > tx) {
			printf("    NULL\n");
		}
		for (i = tx0 + 1; i <= tx; i++) {
			switch (table[i].kind) {
			case constant:
				printf("    %d const %s ", i, table[i].name);
				printf("val = %d\n", table[i].val);
				fprintf(fas, "    %d const %s ", i, table[i].name);
				fprintf(fas, "val = %d\n", table[i].val);
				break;
			case variable:
				printf("    %d var %s ", i, table[i].name);
				printf("lev = %d  addr = %d\n", table[i].level, table[i].adr);
				fprintf(fas, "    %d var %s ", i, table[i].name);
				fprintf(fas, "lev = %d  addr = %d\n", table[i].level, table[i].adr);
				break;
			case flo:
				printf("    %d float %s ", i, table[i].name);
				printf("lev = %d  addr = %d\n", table[i].level, table[i].adr);
				fprintf(fas, "    %d float %s ", i, table[i].name);
				fprintf(fas, "lev = %d  addr = %d\n", table[i].level, table[i].adr);
				break;
			case procedur:
				printf("    %d proc %s ", i, table[i].name);
				printf("lev = %d  addr = %d  size = %d\n", table[i].level, table[i].adr, table[i].size);
				fprintf(fas, "    %d proc %s ", i, table[i].name);
				fprintf(fas, "lev = %d  addr = %d  size = %d\n", table[i].level, table[i].adr, table[i].size);
				break;
			case array:   /* 数组 */
				printf("    %d array  %s ", i, table[i].name);
				printf("lev=%d addr=%d\n", table[i].level, table[i].adr);
				fprintf(fas, "    %d proc  %s ", i, table[i].name);
				fprintf(fas, "lev=%d addr=%d\n", table[i].level, table[i].adr);
				break;
			}
		}
		printf("\n");
	}

	/* 语句后跟符号为分号或end */
	memcpy(nxtlev, fsys, sizeof(bool) * symnum); /* 每个后跟符号集和都包含上层后跟符号集和，以便补救 */
	nxtlev[semicolon] = true;
	nxtlev[endsym] = true;
	statementdo(nxtlev, &tx, lev);
	gendo(opr, 0, 0); /* 每个过程出口都要使用的释放数据段指令 */
	memset(nxtlev, 0, sizeof(bool) * symnum); /*分程序没有补救集合 */
	testdo(fsys, nxtlev, 8); /* 检测后跟符号正确性 */
	listcode(cx0); /* 输出代码 */
	return 0;
}

/*
 * 在名字表中加入一项
 *
 * k:      名字种类const,var or procedure
 * ptx:    名字表尾指针的指针，为了可以改变名字表尾指针的值
 * lev:    名字所在的层次,，以后所有的lev都是这样
 * pdx:    dx为当前应分配的变量的相对地址，分配后要增加1
 */
void enter(enum object k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name, id); /* 全局变量id中已存有当前名字的名字 */
	table[(*ptx)].kind = k;
	switch (k) {
	case constant: /* 常量名字 */
		if (num > amax) {
			error(31); /* 数越界 */
			num = 0;
		}
		table[(*ptx)].val = num;
		break;
	case variable: /* 变量名字 */
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;
	case flo: /* 浮点数名字 */
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;
	case procedur: /*　过程名字　*/
		table[(*ptx)].level = lev;
		break;
	}
}

/*
* start:   数组开始位置
* end:     数组结束为止
*
* 其他变量同enter方法
*/
void enterArray(int* ptx, int lev, int* pdx, int start, int end, char* id, enum object k) {

	(*ptx)++;//增加名字表内容
	strcpy(table[(*ptx)].name, id);
	table[(*ptx)].kind = array;//数组类型
	table[(*ptx)].arrkind = k; 
	table[(*ptx)].level = lev;//层次
	table[(*ptx)].adr = (*pdx);//数组首地址
	table[(*ptx)].low = start; //数组起始下标可以不为0
	table[(*ptx)].end = end;   //数组上界
	(*pdx) += (end - start + 1);//连续内存地址分配给该数组
}


/*
 * 查找名字的位置.
 * 找到则返回在名字表中的位置,否则返回0.
 *
 * idt:    要查找的名字
 * tx:     当前名字表尾指针
 */
int position(char* idt, int tx)
{
	int i;
	strcpy(table[0].name, idt);
	i = tx;
	while (strcmp(table[i].name, idt) != 0) {
		i--;
	}
	return i;
}

/*
 * 常量声明处理
 */
int constdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident) {
		getsymdo;
		if (sym == eql || sym == becomes) {
			if (sym == becomes) {
				error(1); /* 把=写成了:= */
			}
			getsymdo;
			if (sym == number) {
				enter(constant, ptx, lev, pdx);
				getsymdo;
			}
			else {
				error(2); /* 常量说明=后应是数字 */
			}
		}
		else {
			error(3); /* 常量说明标识后应是= */
		}
	}
	else {
		error(4); /* const后应是标识 */
	}
	return 0;
}

/*
 * 变量声明处理  并扩充了整型数组
 */
int vardeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		int n1 = 0, n2 = 0;
		bool e = false;
		getsymdo;
		// 有了变量名后要判断是否后跟 ( 用于判断是否为数组
		if (sym == lparen)
		{
			char mid[11]; //存储变量名字，用于填写名字表
			memcpy(mid, id, 11);
			//printf("find (");
			getsymdo;
			if (sym == number || sym == ident)
			{

				if (sym == number)
				{
					n1 = num;
				}
				else
				{
					n1 = isDigit(position(id, *ptx));
					if (n1 == -1)
					{
						e = true;
						error(31);//不能是变量
					}
				}
				if (!e)
				{
					getsymdo;
					if (sym == colon)
					{
						getsymdo;
						if (sym == number || sym == ident)
						{
							if (sym == number)
							{
								n2 = num;
							}
							else
							{
								n2 = isDigit(position(id, *ptx));
								if (n2 == -1)
								{
									e = true;
									error(31);//不能是变量
								}
							}
							if (!e)
							{
								if (n1 <= n2)
								{
									getsymdo;
									if (sym == rparen)
									{
										enterArray(ptx, lev, pdx, n1, n2, mid, varsym);
									}
									else
									{
										error(31);//缺少右括号
									}
								}
								else
								{
									error(31);//下界大于上界
								}
							}
						}
					}
					else
					{
						error(31);//缺少冒号
					}
				}
			}
			else
			{
				error(31);//只能是数字或者标识符
			}
			getsymdo;
		}
		else
		{
			//int型
			enter(variable, ptx, lev, pdx);
			// 填写名字表
		}

	}
	else
	{
		error(4);   /* var后应是标识 */
	}
	return 0;
}

/*
 * 浮点型变量声明 扩展浮点型数组
 */
int floatdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		int n1 = 0, n2 = 0;
		bool e = false;
		getsymdo;
		// 有了变量名后要判断是否后跟 ( 用于判断是否为数组
		if (sym == lparen)
		{
			char mid[11]; //存储变量名字，用于填写名字表
			memcpy(mid, id, 11);
			//printf("find (");
			getsymdo;
			if (sym == number || sym == ident)
			{

				if (sym == number)
				{
					n1 = num;
				}
				else
				{
					n1 = isDigit(position(id, *ptx));
					if (n1 == -1)
					{
						e = true;
						error(31);//不能是变量
					}
				}
				if (!e)
				{
					getsymdo;
					if (sym == colon)
					{
						getsymdo;
						if (sym == number || sym == ident)
						{
							if (sym == number)
							{
								n2 = num;
							}
							else
							{
								n2 = isDigit(position(id, *ptx));
								if (n2 == -1)
								{
									e = true;
									error(31);//不能是变量
								}
							}
							if (!e)
							{
								if (n1 <= n2)
								{
									getsymdo;
									if (sym == rparen)
									{
										enterArray(ptx, lev, pdx, n1, n2, mid, floatsym);
									}
									else
									{
										error(31);//缺少右括号
									}
								}
								else
								{
									error(31);//下界大于上界
								}
							}
						}
					}
					else
					{
						error(31);//缺少冒号
					}
				}
			}
			else
			{
				error(31);//只能是数字或者标识符
			}
			getsymdo;
		}
		else
		{
			//浮点型
			enter(flo, ptx, lev, pdx);
			// 填写名字表
		}

	}
	else
	{
		error(4);   /* var后应是标识 */
	}
	return 0;
}

///*
// * 浮点型变量声明
// */
//int floatdeclaration(int* ptx, int lev, int* pdx)
//{
//	if (sym == ident)
//	{
//		enter(flo, ptx, lev, pdx); // 填写名字表
//		getsymdo;
//	}
//	else
//	{
//		error(4);   /* var后应是标识 */
//	}
//	return 0;
//}

//判断是否为常量，是返回常量值，否返回-1
// 返回值为-1时，说明不是常量，是变量
int isDigit(int index)
{
	if (index == 0 || table[index].kind != constant)
	{
		error(31);//常量未找到
		return -1;
	}
	else
	{
		return table[index].val;
	}
}

/*
 * 输出目标代码清单
 */
void listcode(int cx0)
{
	int i;
	if (listswitch) {
		for (i = cx0; i < cx; i++) {
			printf("%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
			fprintf(fa, "%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		}
	}
}

/*
 * 语句处理
 */
int statement(bool* fsys, int* ptx, int lev)
{
	int i, cx1, cx2;
	bool nxtlev[symnum];

	if (sym == ident) /* 准备按照赋值语句处理 */
	{
		i = position(id, *ptx);
		if (i == 0)
		{
			error(11); /* 变量未找到 */
		}
		else
		{
			if (table[i].kind != variable && table[i].kind != flo && table[i].kind != array)
			{
				error(12); /* 赋值语句格式错误 */
				i = 0;
			}

			if (table[i].kind == array)
			{
				getsymdo;

				expressiondo(nxtlev, ptx, lev, true, i);//下标的表达式，将偏移量（已经减掉下界值）放到栈顶

				//gendo(lit, 0, table[i].low);
				//gendo(opr, 0, 3);
				gendo(lit, 0, table[i].adr);//将基地址放到栈顶
				gendo(opr, 0, 2);//当前栈顶是真实地址
				if (sym == becomes) {
					getsymdo;
				}
				else {
					error(13);
				}
				memcpy(nxtlev, fsys, sizeof(bool) * symnum);
				expressiondo(nxtlev, ptx, lev, false, i); /* 处理赋值符号右侧表达式 */
				if (i != 0)
				{
					/* expression将执行一系列指令，但最终结果将会保存在栈顶，执行sto命令完成赋值 */
					gendo(sto2, lev - table[i].level, 0);
				}
			}
			else
			{
				getsymdo;
				if (sym == becomes) {
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					expressiondo(nxtlev, ptx, lev, false, 0); /* 处理赋值符号右侧表达式 */
					if (i != 0) {
						/* expression将执行一系列指令，但最终结果将会保存在栈顶，执行sto命令完成赋值 */
						gendo(sto, lev - table[i].level, table[i].adr);
					}
				}
				else if (sym == pluseql) {  /* 新增+= */
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					gendo(lod, lev - table[i].level, table[i].adr);
					expressiondo(nxtlev, ptx, lev, false, 0);
					if (i != 0)
					{
						gendo(opr, 0, 2);  // 栈顶和次栈顶相加 
						gendo(sto, lev - table[i].level, table[i].adr);
					}

				}
				else if (sym == plusplus) {  /* 新增后置++语句 */
					getsymdo;
					gendo(lod, lev - table[i].level, table[i].adr);
					if (i != 0)
					{
						gendo(lit, 0, 1);
						gendo(opr, 0, 2);
						gendo(sto, lev - table[i].level, table[i].adr);
					}
				}
				else if (sym == minuseql) {  /* 新增-= */
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					gendo(lod, lev - table[i].level, table[i].adr);
					expressiondo(nxtlev, ptx, lev, false, 0);
					if (i != 0)
					{
						gendo(opr, 0, 3);
						gendo(sto, lev - table[i].level, table[i].adr);
					}

				}
				else if (sym == minusminus) {  /* 新增后置--语句 */
					getsymdo;
					gendo(lod, lev - table[i].level, table[i].adr);
					if (i != 0)
					{
						gendo(lit, 0, 1);
						gendo(opr, 0, 3);
						gendo(sto, lev - table[i].level, table[i].adr);
					}

				}
				else if (sym == timeseql) {  /* 新增*= */
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					gendo(lod, lev - table[i].level, table[i].adr);
					expressiondo(nxtlev, ptx, lev, false, 0);
					if (i != 0)
					{
						gendo(opr, 0, 4);
						gendo(sto, lev - table[i].level, table[i].adr);
					}

				}
				else if (sym == slasheql) {  /* 新增 /=  */
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					gendo(lod, lev - table[i].level, table[i].adr);
					expressiondo(nxtlev, ptx, lev, false, 0);
					if (i != 0)
					{
						gendo(opr, 0, 5);
						gendo(sto, lev - table[i].level, table[i].adr);
					}

				}
				else
				{
					error(13);
				}
			}
		}
	}
	else {
		if (sym == readsym) /* 准备按照read语句处理 */
		{
			getsymdo;
			if (sym != lparen) {
				error(34); /* 格式错误，应是左括号 */
			}
			else {
				//int kind;
				do {
					getsymdo;		
					//kind = sym;
					//if (kind != floatsym && kind != varsym) { // 必须显示说明输入格式
					//	error(35);  /* 格式错误，应有读入数据的类型 */
					//}
					//else {
					//	getsymdo;
					//}
					if (sym == ident) {
						i = position(id, *ptx); /* 查找要读的变量 */
					}
					else {
						i = 0;
					}
					if (i == 0) {
						error(35); /* read()中应是声明过的变量名 */
					}
					else {
						if (table[i].kind != variable && table[i].kind != flo && table[i].kind != array)
						{
							error(41); /* 读取的值应放入整型变量，浮点型变量或者整型数组中 */
						}
						else
						{
							if (table[i].kind == variable) { // 整型变量
								gendo(opr, 0, 16); /* 生成输入指令，读取值到栈顶 */
								gendo(sto, lev - table[i].level, table[i].adr);
								getsymdo;
							}
							else if (table[i].kind == flo) { // 浮点型变量
								gendo(opr, 0, 18); /* 生成输入指令，读取值到栈顶 */
								gendo(sto, lev - table[i].level, table[i].adr);
								getsymdo;
							}
							else // 整型数组
							{
								getsymdo;
								expressiondo(nxtlev, ptx, lev, true, i);//括号内的表达式，将偏移量放到栈顶
								gendo(lit, 0, table[i].adr);//基地址
								gendo(opr, 0, 2);//当前栈顶是真实地址
								gendo(opr, 0, 16);  /* 生成输入指令，读取值到栈顶 */
								gendo(sto2, lev - table[i].level, 0);
							}
						}
					}

				} while (sym == comma); /* 一条read语句可读多个变量 */
			}
			if (sym != rparen)
			{
				error(33); /* 格式错误，应是右括号 */
				while (!inset(sym, fsys)) /* 出错补救，直到收到上层函数的后跟符号 */
				{
					getsymdo;
				}
			}
			else {
				getsymdo;
			}
		}
		else {
			if (sym == writesym) /* 准备按照write语句处理，与read类似 */
			{
				getsymdo;
				if (sym == lparen) {
					int kind;
					do {
						getsymdo;
						kind = sym;
						if (kind != floatsym && kind != varsym) {
							error(36);  /* 格式错误，应有写出数据的类型 */
						}
						else {
							getsymdo;
						}
						memcpy(nxtlev, fsys, sizeof(bool) * symnum);
						nxtlev[rparen] = true;
						// 可以跟输出类型的声明
						nxtlev[floatsym] = true;
						nxtlev[varsym] = true;
						nxtlev[constsym] = true;
						nxtlev[comma] = true; /* write的后跟符号为) or , */
						expressiondo(nxtlev, ptx, lev, false, 0); /* 调用表达式处理，此处与read不同，read为给变量赋值 */
						if (kind == varsym) { /* 生成输出指令，输出栈顶的值 */
							gendo(opr, 0, 14);
						}
						else {
							gendo(opr, 0, 17); /* 新增，输出float */
						}
					} while (sym == comma);

					if (sym != rparen) {
						error(33); /* write()中应为完整表达式 */
					}
					else {
						getsymdo;
					}
				}
				// gendo(opr, 0, 15); /* 输出换行 */
			}
			else
			{
				if (sym == callsym) /* 准备按照call语句处理 */
				{
					getsymdo;
					if (sym != ident)
					{
						error(14);  /* call后应为标识符 */
					}
					else
					{
						i = position(id, *ptx);
						if (i == 0)
						{
							error(11);  /* 过程未找到 */
						}
						else
						{
							if (table[i].kind == procedur)
							{
								gendo(cal, lev - table[i].level, table[i].adr);   /* 生成call指令 */
							}
							else
							{
								error(15);  /* call后标识符应为过程 */
							}
						}
						getsymdo;
					}
				}
				else
				{
					if (sym == ifsym)   /* 准备按照if语句处理 */
					{
						getsymdo;
						memcpy(nxtlev, fsys, sizeof(bool)* symnum);
						nxtlev[thensym] = true;
						nxtlev[dosym] = true;   /* 后跟符号为then或do */
						conditiondo(nxtlev, ptx, lev); /* 调用条件处理（逻辑运算）函数 */
						if (sym == thensym)
						{
							getsymdo;
						}
						else
						{
							error(16);  /* 缺少then */
						}
						cx1 = cx;   /* 保存当前指令地址 */
						gendo(jpc, 0, 0);   /* 生成条件跳转指令，跳转地址未知，暂时写0 */
						statementdo(fsys, ptx, lev);    /* 处理then后的语句 */
						if (sym == elsesym)//添加的处理else的语句 
						{
							getsymdo;//词法分析子程序 
							cx2 = cx;/*记录jmp指令位置*/
							gendo(jmp, 0, 0);//无条件跳转指令 ,将来直接跳转到else语句后面 
							code[cx1].a = cx;  //地址回填
							statementdo(fsys, ptx, lev);/*处理else后面的语句*/
							code[cx2].a = cx;//else后面的语句结束位置，if语句执行后跳转到该位置 ，为jmp指令最后一个赋值 
						}
						else
						{
							code[cx1].a = cx; /* 经statement处理后，cx为then后语句执行完的位置，它正是前面未定的跳转地址 */
						}
					}
					else
					{
						if (sym == beginsym)    /* 准备按照复合语句处理 */
						{
							getsymdo;
							memcpy(nxtlev, fsys, sizeof(bool) * symnum);
							nxtlev[semicolon] = true;
							nxtlev[endsym] = true;  /* 后跟符号为分号或end */
							/* 循环调用语句处理函数，直到下一个符号不是语句开始符号或收到end */
							statementdo(nxtlev, ptx, lev);

							while (inset(sym, statbegsys) || sym == semicolon)
							{
								if (sym == semicolon)
								{
									getsymdo;
								}
								else
								{
									error(10);  /* 缺少分号 */
								}
								statementdo(nxtlev, ptx, lev);
							}
							if (sym == endsym)
							{
								getsymdo;
							}
							else
							{
								error(17);  /* 缺少end或分号 */
							}
						}
						else
						{
							if (sym == whilesym)    /* 准备按照while语句处理 */
							{
								cx1 = cx;   /* 保存判断条件操作的位置 */
								getsymdo;
								memcpy(nxtlev, fsys, sizeof(bool) * symnum);
								nxtlev[dosym] = true;   /* 后跟符号为do */
								conditiondo(nxtlev, ptx, lev);  /* 调用条件处理 */
								cx2 = cx;   /* 保存循环体的结束的下一个位置 */
								gendo(jpc, 0, 0);   /* 生成条件跳转，但跳出循环的地址未知 */
								if (sym == dosym)
								{
									getsymdo;
								}
								else
								{
									error(18);  /* 缺少do */
								}
								statementdo(fsys, ptx, lev);    /* 循环体 */
								gendo(jmp, 0, cx1); /* 回头重新判断条件 */
								code[cx2].a = cx;   /* 反填跳出循环的地址，与if类似 */
							}
							else
							{
								memset(nxtlev, 0, sizeof(bool) * symnum); /* 语句结束无补救集合 */
								testdo(fsys, nxtlev, 19);   /* 检测语句结束的正确性 */
							}
						}
					}
				}
			}
		}
	}
	return 0;
}

/*
* 表达式处理 五个参数分别为：
 * fsys：当前模块的后跟符号集合
 * ptx：名字表当前尾指针
 * lev：当前分程序所在层
 * nowArray：当前处理的是否为数组
 * index：当前处理的数组在名字表中的位置
*/
int expression(bool* fsys, int* ptx, int lev, bool nowArray, int index)
{
	enum symbol addop;  /* 用于保存正负号 */
	bool nxtlev[symnum];

	if (sym == plus || sym == minus) /* 开头的正负号，此时当前表达式被看作一个正的或负的项 */
	{
		addop = sym;    /* 保存开头的正负号 */
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* 处理项 */
		if (addop == minus)
		{
			gendo(opr, 0, 1); /* 如果开头为负号生成取负指令 */
		}
	}
	else    /* 此时表达式被看作项的加减 */
	{
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* 处理项 */
	}
	while (sym == plus || sym == minus)
	{
		addop = sym;
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* 处理项 */
		if (addop == plus)
		{
			gendo(opr, 0, 2);   /* 生成加法指令 */
		}
		else
		{
			gendo(opr, 0, 3);   /* 生成减法指令 */
		}
	}
	//如果是数组，下标应该减去下界值才能得到真实的地址值
	if (nowArray == true) {
		// 判断越下界
		gendo(lit, 0, table[index].low);
		gendo(opr, index, 19);

		// 判断越上界
		gendo(lit, 0, table[index].end);
		gendo(opr, index, 20);

		gendo(lit, 0, table[index].low);
		gendo(opr, 0, 3);
	}
	return 0;
}

/*
 * 项处理
 */
int term(bool* fsys, int* ptx, int lev)
{
	enum symbol mulop; /* 用于保存乘除法符号 */
	bool nxtlev[symnum];

	memcpy(nxtlev, fsys, sizeof(bool) * symnum);
	nxtlev[times] = true;
	nxtlev[slash] = true;
	factordo(nxtlev, ptx, lev); /* 处理因子 */
	while (sym == times || sym == slash) {
		mulop = sym;
		getsymdo;
		factordo(nxtlev, ptx, lev);
		if (mulop == times) {
			gendo(opr, 0, 4); /* 生成乘法指令 */
		}
		else {
			gendo(opr, 0, 5); /* 生成除法指令 */
		}
	}
	return 0;
}

/*
 * 因子处理
 */
int factor(bool* fsys, int* ptx, int lev)
{

	int i;
	bool nxtlev[symnum];
	testdo(facbegsys, fsys, 24);  /* 检测因子的开始符号 */

	/* while(inset(sym, facbegsys)) */ /* 循环直到不是因子开始符号 */
	if (inset(sym, facbegsys)) /* BUG: 原来的方法var1(var2+var3)会被错误识别为因子 */
	{
		if (sym == ident) /* 因子为常量或变量 */
		{
			bool ar = false;
			i = position(id, *ptx); /* 查找名字 */
			printf("%d %s", *ptx, id);
			if (i == 0) {
				error(11); /* 标识符未声明 */
			}
			getsymdo;
			if (sym == plusplus) /* 新增 后置++ */
			{
				gendo(lod, lev - table[i].level, table[i].adr);
				gendo(lit, 0, 1);
				gendo(opr, 0, 2);
				gendo(sto, lev - table[i].level, table[i].adr);
				// 将相加前的数返回栈顶
				gendo(lod, lev - table[i].level, table[i].adr);
				gendo(lit, 0, 1);
				gendo(opr, 0, 3);

				getsymdo;
			}
			else if (sym == minusminus)  /* 新增 后置-- */
			{
				gendo(lod, lev - table[i].level, table[i].adr);
				gendo(lit, 0, 1);
				gendo(opr, 0, 3);
				gendo(sto, lev - table[i].level, table[i].adr);
				// 将相加减的数返回栈顶
				gendo(lod, lev - table[i].level, table[i].adr);
				gendo(lit, 0, 1);
				gendo(opr, 0, 2);

				getsymdo;
			}
			else
			{
				switch (table[i].kind)
				{
				case constant: /* 名字为常量 */
					ar = false;
					gendo(lit, 0, table[i].val); /* 直接把常量的值入栈 */
					break;
				case variable: /* 名字为变量 */
					ar = false;
					gendo(lod, lev - table[i].level, table[i].adr); /* 找到变量地址并将其值入栈 */
					break;
				case flo:
					ar = false;
					gendo(lod, lev - table[i].level, table[i].adr); /* 找到变量地址并将其值入栈 */
					break;
				case procedur: /* 名字为过程 */
					error(21); /* 不能为过程 */
					break;
				case array: /* 名字为数组 */
					ar = true;
					getsymdo;
					expressiondo(nxtlev, ptx, lev, true, i); //下标的值在栈顶
					getsymdo;
					gendo(lit, 0, table[i].adr);
					gendo(opr, 0, 2);//当前栈顶是真实地址
					gendo(lod2, lev - table[i].level, 0);
					break;
				}
			}

		}
		else {
			if (sym == number) /* 因子为数 */
			{
				if (num > amax) {
					error(31);
					num = 0;
				}
				gendo(lit, 0, num);
				getsymdo;
			}
			else if (sym == plusplus) /* 新增 前置++ */
			{
				getsymdo;
				if (sym == ident)
				{
					getsymdo;
					i = position(id, *ptx);
					if (i == 0)
					{
						error(11);
					}
					else
					{
						if (table[i].kind == variable) // 只有整形使用合法
						{
							gendo(lod, lev - table[i].level, table[i].adr);
							gendo(lit, 0, 1);
							gendo(opr, 0, 2);
							gendo(sto, lev - table[i].level, table[i].adr);
							// 将相加后的数返回栈顶
							gendo(lod, lev - table[i].level, table[i].adr);
						}
					}
				}

			}
			else if (sym == minusminus)
			{
				getsymdo;
				if (sym == ident)
				{
					getsymdo;
					i = position(id, *ptx);
					if (i == 0)
					{
						error(11);
					}
					else
					{
						if (table[i].kind == variable) // 只有整形使用合法
						{
							gendo(lod, lev - table[i].level, table[i].adr);
							gendo(lit, 0, 1);
							gendo(opr, 0, 3);
							gendo(sto, lev - table[i].level, table[i].adr);
							// 将相减后的数返回栈顶
							gendo(lod, lev - table[i].level, table[i].adr);
						}
					}

				}

			}
			else if (sym == numberf) { /* 新增，因子为浮点数 */
				gendo(litf, 0, cxf);
				genfdo(litf, 0, numf);
				getsymdo;
			}
			else {
				if (sym == lparen) /* 因子为表达式 */
				{
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					nxtlev[rparen] = true;
					expressiondo(nxtlev, ptx, lev, false, 0);
					if (sym == rparen)
					{
						getsymdo;
					}
					else
					{
						error(22); /* 缺少右括号 */
					}
				}
				testdo(fsys, facbegsys, 23); /* 因子后有非法符号 */
			}
		}
	}
	return 0;
}

/*
 * 条件处理
 */
int condition(bool* fsys, int* ptx, int lev)
{
	enum symbol relop;
	bool nxtlev[symnum];

	if (sym == oddsym) /* 准备按照odd运算处理 */
	{
		getsymdo;
		expressiondo(fsys, ptx, lev, false, 0);
		gendo(opr, 0, 6); /* 生成odd指令 */
	}
	else {
		/* 逻辑表达式处理 */
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[eql] = true;
		nxtlev[neq] = true;
		nxtlev[lss] = true;
		nxtlev[leq] = true;
		nxtlev[gtr] = true;
		nxtlev[geq] = true;
		expressiondo(nxtlev, ptx, lev, false, 0);
		if (sym != eql && sym != neq && sym != lss && sym != leq && sym != gtr && sym != geq) {
			error(20);
		}
		else
		{
			relop = sym;
			getsymdo;
			expressiondo(fsys, ptx, lev, false, 0);
			switch (relop)
			{
			case eql:
				gendo(opr, 0, 8);
				break;
			case neq:
				gendo(opr, 0, 9);
				break;
			case lss:
				gendo(opr, 0, 10);
				break;
			case geq:
				gendo(opr, 0, 11);
				break;
			case gtr:
				gendo(opr, 0, 12);
				break;
			case leq:
				gendo(opr, 0, 13);
				break;
			}
		}
	}
	return 0;
}

/*
 * 解释程序
 */
void interpret()
{
	int p, b, t; /* 指令指针，指令基址，栈顶指针 */
	struct instruction i; /* 存放当前指令 */
	float s[stacksize]; /* 栈 */
	printf("\n");
	printf("--start pl0--\n");
	t = 0;
	b = 0;
	p = 0;
	s[0] = s[1] = s[2] = 0;
	do {
		i = code[p]; /* 读当前指令 */
		p++;
		switch (i.f) {
		case lit: /* 将a的值取到栈顶 */
			s[t] = i.a;
			t++;
			break;
		case litf:
			s[t] = codef[i.a].a;
			t++;
			break;
		case opr: /* 数学、逻辑运算 */
			switch (i.a) {
			case 0:
				t = b;
				p = s[t + 2];
				b = s[t + 1];
				break;
			case 1:
				s[t - 1] = -s[t - 1];
				break;
			case 2:
				t--;
				s[t - 1] = s[t - 1] + s[t];
				break;
			case 3:
				t--;
				s[t - 1] = s[t - 1] - s[t];
				break;
			case 4:
				t--;
				s[t - 1] = s[t - 1] * s[t];
				break;
			case 5:
				t--;
				s[t - 1] = s[t - 1] / s[t];
				break;
			case 6:
				s[t - 1] = (int)s[t - 1] % 2;
				break;
			case 8:
				t--;
				s[t - 1] = (s[t - 1] == s[t]);
				break;
			case 9:
				t--;
				s[t - 1] = (s[t - 1] != s[t]);
				break;
			case 10:
				t--;
				s[t - 1] = (s[t - 1] < s[t]);
				break;
			case 11:
				t--;
				s[t - 1] = (s[t - 1] >= s[t]);
				break;
			case 12:
				t--;
				s[t - 1] = (s[t - 1] > s[t]);
				break;
			case 13:
				t--;
				s[t - 1] = (s[t - 1] <= s[t]);
				break;
			case 14: // 输出var
				printf("? %d\n", (int)s[t - 1]);
				fprintf(fa2, "? %d\n", (int)s[t - 1]);
				t--;
				break;
			case 15:
				printf("\n");
				fprintf(fa2, "\n");
				break;
			case 16: // 输入var
				printf("?->> ");
				fprintf(fa2, "?->> ");
				scanf("%f", &(s[t]));
				fprintf(fa2, "%d\n", (int)s[t]);
				t++;
				break;
			case 17: // 新增，输出float
				printf("? %f\n", s[t - 1]);
				fprintf(fa2, "? %f\n", s[t - 1]);
				t--;
				break;
			case 18: // 新增，输入float
				printf("?->> ");
				fprintf(fa2, "?->> ");
				scanf("%f", &(s[t]));
				fprintf(fa2, "%f\n", s[t]);
				t++;
				break;
			case 19: // 新增 判断数组越下界
				t--;
				if (s[t] > s[t - 1])
				{
					printf("error:%s数组越下界\n", table[i.l].name); // 此时的l表示变量在变量表中的标号
					exit(-1);
				}
				break;
			case 20: // 新增 判断数组越上界
				t--;
				if (s[t] < s[t - 1])
				{
					printf("error:%s数组越上界\n", table[i.l].name); // 此时的l表示变量在变量表中的标号
					exit(-1);
				}
				break;
			}
			break;
		case lod: /* 取相对当前过程的数据基地址为a的内存的值到栈顶 */
			s[t] = s[base(i.l, s, b) + i.a];
			t++;
			break;
		case lod2: /* 取相对当前过程的数据基地址为a的内存的值到栈顶 */
			s[t - 1] = s[base(i.l, s, b) + (int)s[t - 1]];//应当覆盖基址和偏移量所在位置，以保证可以进行条件判断
			//t++;
			break;
		case sto: /* 栈顶的值存到相对当前过程的数据基地址为a的内存 */
			t--;
			s[base(i.l, s, b) + i.a] = s[t];
			break;
		case sto2: /* 栈顶的值存到相对当前过程的数据基地址为a的内存 */
			t--;	/* 真实地址在次栈顶 赋值量在栈顶 */
			s[base(i.l, s, b) + (int)s[t - 1]] = s[t]; // 根据变量的赋值sto来更改
			break;
		case cal: /* 调用子过程 */
			s[t] = base(i.l, s, b); /* 将父过程基地址入栈 */
			s[t + 1] = b; /* 将本过程基地址入栈，此两项用于base函数 */
			s[t + 2] = p; /* 将当前指令指针入栈 */
			b = t; /* 改变基地址指针值为新过程的基地址 */
			p = i.a; /* 跳转 */
			break;
		case inte: /* 分配内存 */
			t += i.a;
			break;
		case jmp: /* 直接跳转 */
			p = i.a;
			break;
		case jpc: /* 条件跳转 */
			t--;
			if (s[t] == 0) {
				p = i.a;
			}
			break;
		}
	} while (p != 0);
}

/* 通过过程基址求上l层过程的基址 */
int base(int l, float* s, int b)
{
	int b1;
	b1 = b;
	while (l > 0) {
		b1 = s[b1];
		l--;
	}
	return b1;
}
