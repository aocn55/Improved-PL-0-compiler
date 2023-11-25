/*
 * ʹ�÷�����
 * ���к�����PL/0Դ�����ļ�?
 * �ش��Ƿ�������������
 * �ش��Ƿ�������ֱ�
 * fa.tmp������������
 * fa1.tmp���Դ�ļ�������ж�Ӧ���׵�ַ
 * fa2.tmp������
 * fas.tmp������ֱ�
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
	scanf("%s", fname); /* �����ļ��� */

	fin = fopen(fname, "r");

	if (fin)
	{
		printf("List object code?(Y/N)");   /* �Ƿ������������� */
		scanf("%s", fname);
		listswitch = (fname[0] == 'y' || fname[0] == 'Y');

		printf("List symbol table?(Y/N)");  /* �Ƿ�������ֱ� */
		scanf("%s", fname);
		tableswitch = (fname[0] == 'y' || fname[0] == 'Y');

		fa1 = fopen("fa1.tmp", "w");
		fprintf(fa1, "Input pl/0 file?   ");
		fprintf(fa1, "%s\n", fname);

		init(); /* ��ʼ�� */

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

			if (-1 == block(0, 0, nxtlev)) /* ���ñ������ */
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
				interpret(); /* ���ý���ִ�г��� */
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
		printf("�޷��ҵ����PL/0�ļ�\n");
	}

	printf("\n");
	return 0;
}

/*
 * ��ʼ��
 */
void init()
{
	int i;

	/* ���õ��ַ����š�ɢ�д洢�����ڲ��� */
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

	/* ���ñ���������,������ĸ˳�򣬱����۰���� */
	strcpy(&(word[0][0]), "begin");
	strcpy(&(word[1][0]), "call");
	strcpy(&(word[2][0]), "const");
	strcpy(&(word[3][0]), "do");
	strcpy(&(word[4][0]), "else"); // ���� else
	strcpy(&(word[5][0]), "end");
	strcpy(&(word[6][0]), "float");//����float
	strcpy(&(word[7][0]), "if");
	strcpy(&(word[8][0]), "odd");
	strcpy(&(word[9][0]), "procedure");
	strcpy(&(word[10][0]), "read");
	strcpy(&(word[11][0]), "then");
	strcpy(&(word[12][0]), "var");
	strcpy(&(word[13][0]), "while");
	strcpy(&(word[14][0]), "write");

	/* ���ñ����ַ��� */
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


	/* ����ָ������ */
	strcpy(&(mnemonic[lit][0]), "lit");
	strcpy(&(mnemonic[litf][0]), "litf"); // ����
	strcpy(&(mnemonic[opr][0]), "opr");
	strcpy(&(mnemonic[lod][0]), "lod");
	strcpy(&(mnemonic[lod2][0]), "lod2"); // ����
	strcpy(&(mnemonic[sto][0]), "sto");
	strcpy(&(mnemonic[sto2][0]), "sto2");  // ����
	strcpy(&(mnemonic[cal][0]), "cal");
	strcpy(&(mnemonic[inte][0]), "int");
	strcpy(&(mnemonic[jmp][0]), "jmp");
	strcpy(&(mnemonic[jpc][0]), "jpc");

	/* ���÷��ż� */
	for (i = 0; i < symnum; i++)
	{
		declbegsys[i] = false;
		statbegsys[i] = false;
		facbegsys[i] = false;
	}

	/* ����������ʼ���ż� */
	declbegsys[constsym] = true;
	declbegsys[varsym] = true;
	declbegsys[floatsym] = true; // ����
	declbegsys[procsym] = true;

	/* ������俪ʼ���ż� */
	statbegsys[beginsym] = true;
	statbegsys[callsym] = true;
	statbegsys[ifsym] = true;
	statbegsys[whilesym] = true;
	statbegsys[readsym] = true;
	statbegsys[writesym] = true;


	/* �������ӿ�ʼ���ż� */
	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[numberf] = true;
	facbegsys[lparen] = true;
	facbegsys[plusplus] = true;//�������� 
	facbegsys[minusminus] = true;//�����Լ� 
}

/*
 * ������ʵ�ּ��ϵļ�������
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
 *   ��������ӡ����λ�úʹ������
 */
void error(int n)
{
	char space[81];
	memset(space, 32, 81);

	space[cc - 1] = 0; //����ʱ��ǰ�����Ѿ����꣬����cc-1

	printf("****%s!%d\n", space, n);
	fprintf(fa1, "****%s!%d\n", space, n);

	err++;
}

/*
 * ©���ո񣬶�ȡһ���ַ���
 *
 * ÿ�ζ�һ�У�����line��������line��getsymȡ�պ��ٶ�һ��
 *
 * ������getsym���á�
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
 * �ʷ���������ȡһ������
 */
int getsym()
{
	int i, j, k;
	while (ch == ' ' || ch == 10 || ch == 13 || ch == 9)
	{
		getchdo;
	}
	if (ch >= 'a' && ch <= 'z') { /* ���ֻ�������a..z��ͷ */
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
		do { /* ������ǰ�����Ƿ�Ϊ������ */
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
			sym = ident; /* ����ʧ���������ֻ����� */
		}
	}
	else
	{
		if (ch >= '0' && ch <= '9') { /* ����Ƿ�Ϊ���֣���0..9��ͷ */
			k = 0;
			num = 0;
			sym = number;
			do {
				num = 10 * num + ch - '0';
				k++;
				getchdo;
			} while (ch >= '0' && ch <= '9'); /* ��ȡ���ֵ�ֵ */

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
				} while (ch >= '0' && ch <= '9'); /* ��ȡ���ֵ�ֵ */
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
			if (ch == ':') /* ��⸳ֵ���� */
			{
				getchdo;
				if (ch == '=')
				{
					sym = becomes;
					getchdo;
				}
				else
				{
					sym = colon;   /* �������½���� */
				}
			}
			else if (ch == '+')
			{
				getchdo;
				if (ch == '=') {  /* '+'������'='�Ͱ�pluseql����sym */
					sym = pluseql;
					getchdo;
				}
				else if (ch == '+') {  /* '+'������'+'�Ͱ�plusplus����sym */
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
				if (ch == '=') {  /* '-'������'='�Ͱ�minuseql����sym */
					sym = minuseql;
					getchdo;
				}
				else if (ch == '-') {  /* '-'������'-'�Ͱ�minusminus����sym */
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
				if (ch == '=') {  /* '*'������'='�Ͱ�timeseql����sym */
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
				if (ch == '=') {  /* '/'������'='�Ͱ�slasheql����sym */
					sym = slasheql;
					getchdo;
				}
				else {
					sym = slash;
				}
			}
			else
			{
				if (ch == '<') /* ���С�ڻ�С�ڵ��ڷ��� */
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
					if (ch == '>') /* �����ڻ���ڵ��ڷ��� */
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
						sym = ssym[ch]; /* �����Ų�������������ʱ��ȫ�����յ��ַ����Ŵ��� */
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
 * �������������
 *
 * x: instruction.f;
 * y: instruction.l;
 * z: instruction.a;
 */
int gen(enum fct x, int y, int z)
{
	if (cx >= cxmax) {
		printf("Program too long"); /* ������� */
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
		printf("Program too long"); /* ������� */
		return -1;
	}
	codef[cxf].f = x;
	codef[cxf].l = y;
	codef[cxf].a = z;
	cxf++;
	return 0;
}

/*
 * ���Ե�ǰ�����Ƿ�Ϸ�
 *
 * ��ĳһ���֣���һ����䣬һ�����ʽ����Ҫ����ʱʱ����ϣ����һ����������ĳ��?
 * ���ò��ֵĺ�����ţ���test���������⣬���Ҹ��𵱼�ⲻͨ��ʱ�Ĳ��ȴ�ʩ��
 * ��������Ҫ���ʱָ����ǰ��Ҫ�ķ��ż��ϺͲ����õļ��ϣ���֮ǰδ��ɲ��ֵĺ��
 * ���ţ����Լ���ⲻͨ��ʱ�Ĵ���š�
 *
 * s1:   ������Ҫ�ķ���
 * s2:   �������������Ҫ�ģ�����Ҫһ�������õļ�?
 * n:    �����
 */
int test(bool* s1, bool* s2, int n)
{
	if (!inset(sym, s1)) {
		error(n);
		/* ����ⲻͨ��ʱ����ͣ��ȡ���ţ�ֱ����������Ҫ�ļ��ϻ򲹾ȵļ��� */
		while ((!inset(sym, s1)) && (!inset(sym, s2))) {
			getsymdo;
		}
	}
	return 0;
}

/*
 * ���������?
 *
 * lev:    ��ǰ�ֳ������ڲ�
 * tx:     ���ֱ�ǰβָ��
 * fsys:   ��ǰģ�������ż�?
 */
int block(int lev, int tx, bool* fsys)
{
	int i;

	int dx; /* ���ַ��䵽����Ե�ַ */
	int tx0; /* ������ʼtx */
	int cx0; /* ������ʼcx */
	bool nxtlev[symnum]; /* ���¼������Ĳ����У����ż��Ͼ�Ϊֵ�Σ�������ʹ������ʵ�֣�
												 ���ݽ�������ָ�룬Ϊ��ֹ�¼������ı��ϼ������ļ��ϣ������µĿ�?
												 ���ݸ��¼�����*/

	dx = 3;
	tx0 = tx; /* ��¼�������ֵĳ�ʼλ�� */
	table[tx].adr = cx;

	gendo(jmp, 0, 0);

	if (lev > levmax) {
		error(32);
	}

	do {
		if (sym == constsym) /* �յ������������ţ���ʼ���������� */
		{
			getsymdo;
			do {
				constdeclarationdo(&tx, lev, &dx); /* dx��ֵ�ᱻconstdeclaration�ı䣬ʹ��ָ�� */
				while (sym == comma) {
					getsymdo;
					constdeclarationdo(&tx, lev, &dx);
				}
				if (sym == semicolon) {
					getsymdo;
				}

				else
				{
					error(5); /*©���˶��Ż��߷ֺ�*/
				}

			} while (sym == ident);
		}

		if (sym == varsym) /* �յ������������ţ���ʼ����������� */
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

		if (sym == floatsym) /* �յ������������ţ���ʼ����������� */
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

		while (sym == procsym) /* �յ������������ţ���ʼ����������� */
		{
			getsymdo;
			if (sym == ident) {
				enter(procedur, &tx, lev, &dx); /* ��¼�������� */
				getsymdo;
			}
			else {
				error(4); /* procedure��ӦΪ��ʶ�� */
			}

			if (sym == semicolon) {
				getsymdo;
			}
			else {
				error(5); /* ©���˷ֺ� */
			}

			memcpy(nxtlev, fsys, sizeof(bool) * symnum);
			nxtlev[semicolon] = true;
			if (-1 == block(lev + 1, tx, nxtlev)) {
				return -1; /* �ݹ���� */
			}

			if (sym == semicolon) {
				getsymdo;
				memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
				nxtlev[ident] = true;
				nxtlev[procsym] = true;
				testdo(nxtlev, fsys, 6);
			}
			else {
				error(5); /* ©���˷ֺ� */
			}
		}
		memcpy(nxtlev, statbegsys, sizeof(bool) * symnum);
		memcpy(nxtlev, declbegsys, sizeof(bool) * symnum);  // ��ÿ���ӳ�����Ҳ���Խ�����������
		// nxtlev[ident] = true;
		nxtlev[beginsym] = true;
		testdo(nxtlev, declbegsys, 7);
	} while (inset(sym, declbegsys)); /* ֱ��û���������� */

	code[table[tx0].adr].a = cx; /* ��ʼ���ɵ�ǰ���̴��� */
	table[tx0].adr = cx; /* ��ǰ���̴����ַ */
	table[tx0].size = dx; /* ����������ÿ����һ�����������dx����1�����������Ѿ�������dx���ǵ�ǰ�������ݵ�size */
	cx0 = cx;
	gendo(inte, 0, dx); /* ���ɷ����ڴ���� */

	if (tableswitch) /* ������ֱ� */
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
			case array:   /* ���� */
				printf("    %d array  %s ", i, table[i].name);
				printf("lev=%d addr=%d\n", table[i].level, table[i].adr);
				fprintf(fas, "    %d proc  %s ", i, table[i].name);
				fprintf(fas, "lev=%d addr=%d\n", table[i].level, table[i].adr);
				break;
			}
		}
		printf("\n");
	}

	/* ���������Ϊ�ֺŻ�end */
	memcpy(nxtlev, fsys, sizeof(bool) * symnum); /* ÿ��������ż��Ͷ������ϲ������ż��ͣ��Ա㲹�� */
	nxtlev[semicolon] = true;
	nxtlev[endsym] = true;
	statementdo(nxtlev, &tx, lev);
	gendo(opr, 0, 0); /* ÿ�����̳��ڶ�Ҫʹ�õ��ͷ����ݶ�ָ�� */
	memset(nxtlev, 0, sizeof(bool) * symnum); /*�ֳ���û�в��ȼ��� */
	testdo(fsys, nxtlev, 8); /* �����������ȷ�� */
	listcode(cx0); /* ������� */
	return 0;
}

/*
 * �����ֱ��м���һ��
 *
 * k:      ��������const,var or procedure
 * ptx:    ���ֱ�βָ���ָ�룬Ϊ�˿��Ըı����ֱ�βָ���ֵ
 * lev:    �������ڵĲ��,���Ժ����е�lev��������
 * pdx:    dxΪ��ǰӦ����ı�������Ե�ַ�������Ҫ����1
 */
void enter(enum object k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name, id); /* ȫ�ֱ���id���Ѵ��е�ǰ���ֵ����� */
	table[(*ptx)].kind = k;
	switch (k) {
	case constant: /* �������� */
		if (num > amax) {
			error(31); /* ��Խ�� */
			num = 0;
		}
		table[(*ptx)].val = num;
		break;
	case variable: /* �������� */
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;
	case flo: /* ���������� */
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;
	case procedur: /*���������֡�*/
		table[(*ptx)].level = lev;
		break;
	}
}

/*
* start:   ���鿪ʼλ��
* end:     �������Ϊֹ
*
* ��������ͬenter����
*/
void enterArray(int* ptx, int lev, int* pdx, int start, int end, char* id, enum object k) {

	(*ptx)++;//�������ֱ�����
	strcpy(table[(*ptx)].name, id);
	table[(*ptx)].kind = array;//��������
	table[(*ptx)].arrkind = k; 
	table[(*ptx)].level = lev;//���
	table[(*ptx)].adr = (*pdx);//�����׵�ַ
	table[(*ptx)].low = start; //������ʼ�±���Բ�Ϊ0
	table[(*ptx)].end = end;   //�����Ͻ�
	(*pdx) += (end - start + 1);//�����ڴ��ַ�����������
}


/*
 * �������ֵ�λ��.
 * �ҵ��򷵻������ֱ��е�λ��,���򷵻�0.
 *
 * idt:    Ҫ���ҵ�����
 * tx:     ��ǰ���ֱ�βָ��
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
 * ������������
 */
int constdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident) {
		getsymdo;
		if (sym == eql || sym == becomes) {
			if (sym == becomes) {
				error(1); /* ��=д����:= */
			}
			getsymdo;
			if (sym == number) {
				enter(constant, ptx, lev, pdx);
				getsymdo;
			}
			else {
				error(2); /* ����˵��=��Ӧ������ */
			}
		}
		else {
			error(3); /* ����˵����ʶ��Ӧ��= */
		}
	}
	else {
		error(4); /* const��Ӧ�Ǳ�ʶ */
	}
	return 0;
}

/*
 * ������������  ����������������
 */
int vardeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		int n1 = 0, n2 = 0;
		bool e = false;
		getsymdo;
		// ���˱�������Ҫ�ж��Ƿ��� ( �����ж��Ƿ�Ϊ����
		if (sym == lparen)
		{
			char mid[11]; //�洢�������֣�������д���ֱ�
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
						error(31);//�����Ǳ���
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
									error(31);//�����Ǳ���
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
										error(31);//ȱ��������
									}
								}
								else
								{
									error(31);//�½�����Ͻ�
								}
							}
						}
					}
					else
					{
						error(31);//ȱ��ð��
					}
				}
			}
			else
			{
				error(31);//ֻ�������ֻ��߱�ʶ��
			}
			getsymdo;
		}
		else
		{
			//int��
			enter(variable, ptx, lev, pdx);
			// ��д���ֱ�
		}

	}
	else
	{
		error(4);   /* var��Ӧ�Ǳ�ʶ */
	}
	return 0;
}

/*
 * �����ͱ������� ��չ����������
 */
int floatdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		int n1 = 0, n2 = 0;
		bool e = false;
		getsymdo;
		// ���˱�������Ҫ�ж��Ƿ��� ( �����ж��Ƿ�Ϊ����
		if (sym == lparen)
		{
			char mid[11]; //�洢�������֣�������д���ֱ�
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
						error(31);//�����Ǳ���
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
									error(31);//�����Ǳ���
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
										error(31);//ȱ��������
									}
								}
								else
								{
									error(31);//�½�����Ͻ�
								}
							}
						}
					}
					else
					{
						error(31);//ȱ��ð��
					}
				}
			}
			else
			{
				error(31);//ֻ�������ֻ��߱�ʶ��
			}
			getsymdo;
		}
		else
		{
			//������
			enter(flo, ptx, lev, pdx);
			// ��д���ֱ�
		}

	}
	else
	{
		error(4);   /* var��Ӧ�Ǳ�ʶ */
	}
	return 0;
}

///*
// * �����ͱ�������
// */
//int floatdeclaration(int* ptx, int lev, int* pdx)
//{
//	if (sym == ident)
//	{
//		enter(flo, ptx, lev, pdx); // ��д���ֱ�
//		getsymdo;
//	}
//	else
//	{
//		error(4);   /* var��Ӧ�Ǳ�ʶ */
//	}
//	return 0;
//}

//�ж��Ƿ�Ϊ�������Ƿ��س���ֵ���񷵻�-1
// ����ֵΪ-1ʱ��˵�����ǳ������Ǳ���
int isDigit(int index)
{
	if (index == 0 || table[index].kind != constant)
	{
		error(31);//����δ�ҵ�
		return -1;
	}
	else
	{
		return table[index].val;
	}
}

/*
 * ���Ŀ������嵥
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
 * ��䴦��
 */
int statement(bool* fsys, int* ptx, int lev)
{
	int i, cx1, cx2;
	bool nxtlev[symnum];

	if (sym == ident) /* ׼�����ո�ֵ��䴦�� */
	{
		i = position(id, *ptx);
		if (i == 0)
		{
			error(11); /* ����δ�ҵ� */
		}
		else
		{
			if (table[i].kind != variable && table[i].kind != flo && table[i].kind != array)
			{
				error(12); /* ��ֵ����ʽ���� */
				i = 0;
			}

			if (table[i].kind == array)
			{
				getsymdo;

				expressiondo(nxtlev, ptx, lev, true, i);//�±�ı��ʽ����ƫ�������Ѿ������½�ֵ���ŵ�ջ��

				//gendo(lit, 0, table[i].low);
				//gendo(opr, 0, 3);
				gendo(lit, 0, table[i].adr);//������ַ�ŵ�ջ��
				gendo(opr, 0, 2);//��ǰջ������ʵ��ַ
				if (sym == becomes) {
					getsymdo;
				}
				else {
					error(13);
				}
				memcpy(nxtlev, fsys, sizeof(bool) * symnum);
				expressiondo(nxtlev, ptx, lev, false, i); /* ����ֵ�����Ҳ���ʽ */
				if (i != 0)
				{
					/* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��sto������ɸ�ֵ */
					gendo(sto2, lev - table[i].level, 0);
				}
			}
			else
			{
				getsymdo;
				if (sym == becomes) {
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					expressiondo(nxtlev, ptx, lev, false, 0); /* ����ֵ�����Ҳ���ʽ */
					if (i != 0) {
						/* expression��ִ��һϵ��ָ������ս�����ᱣ����ջ����ִ��sto������ɸ�ֵ */
						gendo(sto, lev - table[i].level, table[i].adr);
					}
				}
				else if (sym == pluseql) {  /* ����+= */
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool) * symnum);
					gendo(lod, lev - table[i].level, table[i].adr);
					expressiondo(nxtlev, ptx, lev, false, 0);
					if (i != 0)
					{
						gendo(opr, 0, 2);  // ջ���ʹ�ջ����� 
						gendo(sto, lev - table[i].level, table[i].adr);
					}

				}
				else if (sym == plusplus) {  /* ��������++��� */
					getsymdo;
					gendo(lod, lev - table[i].level, table[i].adr);
					if (i != 0)
					{
						gendo(lit, 0, 1);
						gendo(opr, 0, 2);
						gendo(sto, lev - table[i].level, table[i].adr);
					}
				}
				else if (sym == minuseql) {  /* ����-= */
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
				else if (sym == minusminus) {  /* ��������--��� */
					getsymdo;
					gendo(lod, lev - table[i].level, table[i].adr);
					if (i != 0)
					{
						gendo(lit, 0, 1);
						gendo(opr, 0, 3);
						gendo(sto, lev - table[i].level, table[i].adr);
					}

				}
				else if (sym == timeseql) {  /* ����*= */
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
				else if (sym == slasheql) {  /* ���� /=  */
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
		if (sym == readsym) /* ׼������read��䴦�� */
		{
			getsymdo;
			if (sym != lparen) {
				error(34); /* ��ʽ����Ӧ�������� */
			}
			else {
				//int kind;
				do {
					getsymdo;		
					//kind = sym;
					//if (kind != floatsym && kind != varsym) { // ������ʾ˵�������ʽ
					//	error(35);  /* ��ʽ����Ӧ�ж������ݵ����� */
					//}
					//else {
					//	getsymdo;
					//}
					if (sym == ident) {
						i = position(id, *ptx); /* ����Ҫ���ı��� */
					}
					else {
						i = 0;
					}
					if (i == 0) {
						error(35); /* read()��Ӧ���������ı����� */
					}
					else {
						if (table[i].kind != variable && table[i].kind != flo && table[i].kind != array)
						{
							error(41); /* ��ȡ��ֵӦ�������ͱ����������ͱ����������������� */
						}
						else
						{
							if (table[i].kind == variable) { // ���ͱ���
								gendo(opr, 0, 16); /* ��������ָ���ȡֵ��ջ�� */
								gendo(sto, lev - table[i].level, table[i].adr);
								getsymdo;
							}
							else if (table[i].kind == flo) { // �����ͱ���
								gendo(opr, 0, 18); /* ��������ָ���ȡֵ��ջ�� */
								gendo(sto, lev - table[i].level, table[i].adr);
								getsymdo;
							}
							else // ��������
							{
								getsymdo;
								expressiondo(nxtlev, ptx, lev, true, i);//�����ڵı��ʽ����ƫ�����ŵ�ջ��
								gendo(lit, 0, table[i].adr);//����ַ
								gendo(opr, 0, 2);//��ǰջ������ʵ��ַ
								gendo(opr, 0, 16);  /* ��������ָ���ȡֵ��ջ�� */
								gendo(sto2, lev - table[i].level, 0);
							}
						}
					}

				} while (sym == comma); /* һ��read���ɶ�������� */
			}
			if (sym != rparen)
			{
				error(33); /* ��ʽ����Ӧ�������� */
				while (!inset(sym, fsys)) /* �����ȣ�ֱ���յ��ϲ㺯���ĺ������ */
				{
					getsymdo;
				}
			}
			else {
				getsymdo;
			}
		}
		else {
			if (sym == writesym) /* ׼������write��䴦����read���� */
			{
				getsymdo;
				if (sym == lparen) {
					int kind;
					do {
						getsymdo;
						kind = sym;
						if (kind != floatsym && kind != varsym) {
							error(36);  /* ��ʽ����Ӧ��д�����ݵ����� */
						}
						else {
							getsymdo;
						}
						memcpy(nxtlev, fsys, sizeof(bool) * symnum);
						nxtlev[rparen] = true;
						// ���Ը�������͵�����
						nxtlev[floatsym] = true;
						nxtlev[varsym] = true;
						nxtlev[constsym] = true;
						nxtlev[comma] = true; /* write�ĺ������Ϊ) or , */
						expressiondo(nxtlev, ptx, lev, false, 0); /* ���ñ��ʽ�����˴���read��ͬ��readΪ��������ֵ */
						if (kind == varsym) { /* �������ָ����ջ����ֵ */
							gendo(opr, 0, 14);
						}
						else {
							gendo(opr, 0, 17); /* ���������float */
						}
					} while (sym == comma);

					if (sym != rparen) {
						error(33); /* write()��ӦΪ�������ʽ */
					}
					else {
						getsymdo;
					}
				}
				// gendo(opr, 0, 15); /* ������� */
			}
			else
			{
				if (sym == callsym) /* ׼������call��䴦�� */
				{
					getsymdo;
					if (sym != ident)
					{
						error(14);  /* call��ӦΪ��ʶ�� */
					}
					else
					{
						i = position(id, *ptx);
						if (i == 0)
						{
							error(11);  /* ����δ�ҵ� */
						}
						else
						{
							if (table[i].kind == procedur)
							{
								gendo(cal, lev - table[i].level, table[i].adr);   /* ����callָ�� */
							}
							else
							{
								error(15);  /* call���ʶ��ӦΪ���� */
							}
						}
						getsymdo;
					}
				}
				else
				{
					if (sym == ifsym)   /* ׼������if��䴦�� */
					{
						getsymdo;
						memcpy(nxtlev, fsys, sizeof(bool)* symnum);
						nxtlev[thensym] = true;
						nxtlev[dosym] = true;   /* �������Ϊthen��do */
						conditiondo(nxtlev, ptx, lev); /* �������������߼����㣩���� */
						if (sym == thensym)
						{
							getsymdo;
						}
						else
						{
							error(16);  /* ȱ��then */
						}
						cx1 = cx;   /* ���浱ǰָ���ַ */
						gendo(jpc, 0, 0);   /* ����������תָ���ת��ַδ֪����ʱд0 */
						statementdo(fsys, ptx, lev);    /* ����then������ */
						if (sym == elsesym)//��ӵĴ���else����� 
						{
							getsymdo;//�ʷ������ӳ��� 
							cx2 = cx;/*��¼jmpָ��λ��*/
							gendo(jmp, 0, 0);//��������תָ�� ,����ֱ����ת��else������ 
							code[cx1].a = cx;  //��ַ����
							statementdo(fsys, ptx, lev);/*����else��������*/
							code[cx2].a = cx;//else�����������λ�ã�if���ִ�к���ת����λ�� ��Ϊjmpָ�����һ����ֵ 
						}
						else
						{
							code[cx1].a = cx; /* ��statement�����cxΪthen�����ִ�����λ�ã�������ǰ��δ������ת��ַ */
						}
					}
					else
					{
						if (sym == beginsym)    /* ׼�����ո�����䴦�� */
						{
							getsymdo;
							memcpy(nxtlev, fsys, sizeof(bool) * symnum);
							nxtlev[semicolon] = true;
							nxtlev[endsym] = true;  /* �������Ϊ�ֺŻ�end */
							/* ѭ��������䴦������ֱ����һ�����Ų�����俪ʼ���Ż��յ�end */
							statementdo(nxtlev, ptx, lev);

							while (inset(sym, statbegsys) || sym == semicolon)
							{
								if (sym == semicolon)
								{
									getsymdo;
								}
								else
								{
									error(10);  /* ȱ�ٷֺ� */
								}
								statementdo(nxtlev, ptx, lev);
							}
							if (sym == endsym)
							{
								getsymdo;
							}
							else
							{
								error(17);  /* ȱ��end��ֺ� */
							}
						}
						else
						{
							if (sym == whilesym)    /* ׼������while��䴦�� */
							{
								cx1 = cx;   /* �����ж�����������λ�� */
								getsymdo;
								memcpy(nxtlev, fsys, sizeof(bool) * symnum);
								nxtlev[dosym] = true;   /* �������Ϊdo */
								conditiondo(nxtlev, ptx, lev);  /* ������������ */
								cx2 = cx;   /* ����ѭ����Ľ�������һ��λ�� */
								gendo(jpc, 0, 0);   /* ����������ת��������ѭ���ĵ�ַδ֪ */
								if (sym == dosym)
								{
									getsymdo;
								}
								else
								{
									error(18);  /* ȱ��do */
								}
								statementdo(fsys, ptx, lev);    /* ѭ���� */
								gendo(jmp, 0, cx1); /* ��ͷ�����ж����� */
								code[cx2].a = cx;   /* ��������ѭ���ĵ�ַ����if���� */
							}
							else
							{
								memset(nxtlev, 0, sizeof(bool) * symnum); /* �������޲��ȼ��� */
								testdo(fsys, nxtlev, 19);   /* �������������ȷ�� */
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
* ���ʽ���� ��������ֱ�Ϊ��
 * fsys����ǰģ��ĺ�����ż���
 * ptx�����ֱ�ǰβָ��
 * lev����ǰ�ֳ������ڲ�
 * nowArray����ǰ������Ƿ�Ϊ����
 * index����ǰ��������������ֱ��е�λ��
*/
int expression(bool* fsys, int* ptx, int lev, bool nowArray, int index)
{
	enum symbol addop;  /* ���ڱ��������� */
	bool nxtlev[symnum];

	if (sym == plus || sym == minus) /* ��ͷ�������ţ���ʱ��ǰ���ʽ������һ�����Ļ򸺵��� */
	{
		addop = sym;    /* ���濪ͷ�������� */
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* ������ */
		if (addop == minus)
		{
			gendo(opr, 0, 1); /* �����ͷΪ��������ȡ��ָ�� */
		}
	}
	else    /* ��ʱ���ʽ��������ļӼ� */
	{
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* ������ */
	}
	while (sym == plus || sym == minus)
	{
		addop = sym;
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool) * symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);   /* ������ */
		if (addop == plus)
		{
			gendo(opr, 0, 2);   /* ���ɼӷ�ָ�� */
		}
		else
		{
			gendo(opr, 0, 3);   /* ���ɼ���ָ�� */
		}
	}
	//��������飬�±�Ӧ�ü�ȥ�½�ֵ���ܵõ���ʵ�ĵ�ֵַ
	if (nowArray == true) {
		// �ж�Խ�½�
		gendo(lit, 0, table[index].low);
		gendo(opr, index, 19);

		// �ж�Խ�Ͻ�
		gendo(lit, 0, table[index].end);
		gendo(opr, index, 20);

		gendo(lit, 0, table[index].low);
		gendo(opr, 0, 3);
	}
	return 0;
}

/*
 * ���
 */
int term(bool* fsys, int* ptx, int lev)
{
	enum symbol mulop; /* ���ڱ���˳������� */
	bool nxtlev[symnum];

	memcpy(nxtlev, fsys, sizeof(bool) * symnum);
	nxtlev[times] = true;
	nxtlev[slash] = true;
	factordo(nxtlev, ptx, lev); /* �������� */
	while (sym == times || sym == slash) {
		mulop = sym;
		getsymdo;
		factordo(nxtlev, ptx, lev);
		if (mulop == times) {
			gendo(opr, 0, 4); /* ���ɳ˷�ָ�� */
		}
		else {
			gendo(opr, 0, 5); /* ���ɳ���ָ�� */
		}
	}
	return 0;
}

/*
 * ���Ӵ���
 */
int factor(bool* fsys, int* ptx, int lev)
{

	int i;
	bool nxtlev[symnum];
	testdo(facbegsys, fsys, 24);  /* ������ӵĿ�ʼ���� */

	/* while(inset(sym, facbegsys)) */ /* ѭ��ֱ���������ӿ�ʼ���� */
	if (inset(sym, facbegsys)) /* BUG: ԭ���ķ���var1(var2+var3)�ᱻ����ʶ��Ϊ���� */
	{
		if (sym == ident) /* ����Ϊ��������� */
		{
			bool ar = false;
			i = position(id, *ptx); /* �������� */
			printf("%d %s", *ptx, id);
			if (i == 0) {
				error(11); /* ��ʶ��δ���� */
			}
			getsymdo;
			if (sym == plusplus) /* ���� ����++ */
			{
				gendo(lod, lev - table[i].level, table[i].adr);
				gendo(lit, 0, 1);
				gendo(opr, 0, 2);
				gendo(sto, lev - table[i].level, table[i].adr);
				// �����ǰ��������ջ��
				gendo(lod, lev - table[i].level, table[i].adr);
				gendo(lit, 0, 1);
				gendo(opr, 0, 3);

				getsymdo;
			}
			else if (sym == minusminus)  /* ���� ����-- */
			{
				gendo(lod, lev - table[i].level, table[i].adr);
				gendo(lit, 0, 1);
				gendo(opr, 0, 3);
				gendo(sto, lev - table[i].level, table[i].adr);
				// ����Ӽ���������ջ��
				gendo(lod, lev - table[i].level, table[i].adr);
				gendo(lit, 0, 1);
				gendo(opr, 0, 2);

				getsymdo;
			}
			else
			{
				switch (table[i].kind)
				{
				case constant: /* ����Ϊ���� */
					ar = false;
					gendo(lit, 0, table[i].val); /* ֱ�Ӱѳ�����ֵ��ջ */
					break;
				case variable: /* ����Ϊ���� */
					ar = false;
					gendo(lod, lev - table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
					break;
				case flo:
					ar = false;
					gendo(lod, lev - table[i].level, table[i].adr); /* �ҵ�������ַ������ֵ��ջ */
					break;
				case procedur: /* ����Ϊ���� */
					error(21); /* ����Ϊ���� */
					break;
				case array: /* ����Ϊ���� */
					ar = true;
					getsymdo;
					expressiondo(nxtlev, ptx, lev, true, i); //�±��ֵ��ջ��
					getsymdo;
					gendo(lit, 0, table[i].adr);
					gendo(opr, 0, 2);//��ǰջ������ʵ��ַ
					gendo(lod2, lev - table[i].level, 0);
					break;
				}
			}

		}
		else {
			if (sym == number) /* ����Ϊ�� */
			{
				if (num > amax) {
					error(31);
					num = 0;
				}
				gendo(lit, 0, num);
				getsymdo;
			}
			else if (sym == plusplus) /* ���� ǰ��++ */
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
						if (table[i].kind == variable) // ֻ������ʹ�úϷ�
						{
							gendo(lod, lev - table[i].level, table[i].adr);
							gendo(lit, 0, 1);
							gendo(opr, 0, 2);
							gendo(sto, lev - table[i].level, table[i].adr);
							// ����Ӻ��������ջ��
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
						if (table[i].kind == variable) // ֻ������ʹ�úϷ�
						{
							gendo(lod, lev - table[i].level, table[i].adr);
							gendo(lit, 0, 1);
							gendo(opr, 0, 3);
							gendo(sto, lev - table[i].level, table[i].adr);
							// ��������������ջ��
							gendo(lod, lev - table[i].level, table[i].adr);
						}
					}

				}

			}
			else if (sym == numberf) { /* ����������Ϊ������ */
				gendo(litf, 0, cxf);
				genfdo(litf, 0, numf);
				getsymdo;
			}
			else {
				if (sym == lparen) /* ����Ϊ���ʽ */
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
						error(22); /* ȱ�������� */
					}
				}
				testdo(fsys, facbegsys, 23); /* ���Ӻ��зǷ����� */
			}
		}
	}
	return 0;
}

/*
 * ��������
 */
int condition(bool* fsys, int* ptx, int lev)
{
	enum symbol relop;
	bool nxtlev[symnum];

	if (sym == oddsym) /* ׼������odd���㴦�� */
	{
		getsymdo;
		expressiondo(fsys, ptx, lev, false, 0);
		gendo(opr, 0, 6); /* ����oddָ�� */
	}
	else {
		/* �߼����ʽ���� */
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
 * ���ͳ���
 */
void interpret()
{
	int p, b, t; /* ָ��ָ�룬ָ���ַ��ջ��ָ�� */
	struct instruction i; /* ��ŵ�ǰָ�� */
	float s[stacksize]; /* ջ */
	printf("\n");
	printf("--start pl0--\n");
	t = 0;
	b = 0;
	p = 0;
	s[0] = s[1] = s[2] = 0;
	do {
		i = code[p]; /* ����ǰָ�� */
		p++;
		switch (i.f) {
		case lit: /* ��a��ֵȡ��ջ�� */
			s[t] = i.a;
			t++;
			break;
		case litf:
			s[t] = codef[i.a].a;
			t++;
			break;
		case opr: /* ��ѧ���߼����� */
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
			case 14: // ���var
				printf("? %d\n", (int)s[t - 1]);
				fprintf(fa2, "? %d\n", (int)s[t - 1]);
				t--;
				break;
			case 15:
				printf("\n");
				fprintf(fa2, "\n");
				break;
			case 16: // ����var
				printf("?->> ");
				fprintf(fa2, "?->> ");
				scanf("%f", &(s[t]));
				fprintf(fa2, "%d\n", (int)s[t]);
				t++;
				break;
			case 17: // ���������float
				printf("? %f\n", s[t - 1]);
				fprintf(fa2, "? %f\n", s[t - 1]);
				t--;
				break;
			case 18: // ����������float
				printf("?->> ");
				fprintf(fa2, "?->> ");
				scanf("%f", &(s[t]));
				fprintf(fa2, "%f\n", s[t]);
				t++;
				break;
			case 19: // ���� �ж�����Խ�½�
				t--;
				if (s[t] > s[t - 1])
				{
					printf("error:%s����Խ�½�\n", table[i.l].name); // ��ʱ��l��ʾ�����ڱ������еı��
					exit(-1);
				}
				break;
			case 20: // ���� �ж�����Խ�Ͻ�
				t--;
				if (s[t] < s[t - 1])
				{
					printf("error:%s����Խ�Ͻ�\n", table[i.l].name); // ��ʱ��l��ʾ�����ڱ������еı��
					exit(-1);
				}
				break;
			}
			break;
		case lod: /* ȡ��Ե�ǰ���̵����ݻ���ַΪa���ڴ��ֵ��ջ�� */
			s[t] = s[base(i.l, s, b) + i.a];
			t++;
			break;
		case lod2: /* ȡ��Ե�ǰ���̵����ݻ���ַΪa���ڴ��ֵ��ջ�� */
			s[t - 1] = s[base(i.l, s, b) + (int)s[t - 1]];//Ӧ�����ǻ�ַ��ƫ��������λ�ã��Ա�֤���Խ��������ж�
			//t++;
			break;
		case sto: /* ջ����ֵ�浽��Ե�ǰ���̵����ݻ���ַΪa���ڴ� */
			t--;
			s[base(i.l, s, b) + i.a] = s[t];
			break;
		case sto2: /* ջ����ֵ�浽��Ե�ǰ���̵����ݻ���ַΪa���ڴ� */
			t--;	/* ��ʵ��ַ�ڴ�ջ�� ��ֵ����ջ�� */
			s[base(i.l, s, b) + (int)s[t - 1]] = s[t]; // ���ݱ����ĸ�ֵsto������
			break;
		case cal: /* �����ӹ��� */
			s[t] = base(i.l, s, b); /* �������̻���ַ��ջ */
			s[t + 1] = b; /* �������̻���ַ��ջ������������base���� */
			s[t + 2] = p; /* ����ǰָ��ָ����ջ */
			b = t; /* �ı����ַָ��ֵΪ�¹��̵Ļ���ַ */
			p = i.a; /* ��ת */
			break;
		case inte: /* �����ڴ� */
			t += i.a;
			break;
		case jmp: /* ֱ����ת */
			p = i.a;
			break;
		case jpc: /* ������ת */
			t--;
			if (s[t] == 0) {
				p = i.a;
			}
			break;
		}
	} while (p != 0);
}

/* ͨ�����̻�ַ����l����̵Ļ�ַ */
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
