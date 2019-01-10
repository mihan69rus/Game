#include <stdio.h>
#include <stdlib.h>
#include "structs_const.h"
#include "table_strings.h"
#include "help_lex.h"

int check_space(int ch)
{
	return (ch==' ' || ch=='\t' || ch=='\n');
}

int check_arith(int ch)
{
	int j;
	for (j=0;j<num_arith_chars;j++)
	{
		if (arith_chars[j]==ch)
			return 1;
	}
	return 0;
}

int check_letter(int ch)
{
	return ((ch>='a' && ch<='z') || (ch>='A' && ch<='Z'));
}

int check_num(int ch)
{
	return (ch>='0' && ch<='9');
}

int check_special_sym(int ch)
{
	return (ch=='@' || ch=='$' || ch=='?');
}

int check_list(lexeme *p)
{
	int i=1,str=0;
	while (p!=NULL)
	{
		if (p->type==ErrStat)
		{
			printf ("%i, %i: Error: ",p->string,p->length);
			printf ("Couldn't get lexeme\n");
			i=0;
		}
		if (p->length>=80 && str!=p->string)
		{
			printf("%i: Warning: string more 80\n",p->string);
			str=p->string;
		}
		p=p->next;
	}
	return i;
}

void write_lexemes(lexeme *p)
{
	while (p!=NULL)
	{
		printf("[%s] num_str: %i type: ",p->lex,p->string);
		if (p->type==SepStat)
			printf("separator\n\n");
		else if (p->type==KeyStat)
			printf("command\n\n");
		else if (p->type==IntStat)
			printf("integer\n\n");
		else if (p->type==VarStat)
			printf("variable\n\n");
		else if (p->type==FunStat)
			printf("function\n\n");
		else if (p->type==LabStat)
			printf("label\n\n");
		else if (p->type==StrStat)
			printf("string\n\n");
		else if (p->type==ErrStat)
			printf("error\n\n");
		p=p->next;
	}
}

void delete_lexemes(lexeme *p)
{
	lexeme *q;
	while (p!=NULL)
	{
		delete [] p->lex;
		q=p;
		p=p->next;
		delete q;
	}
}




