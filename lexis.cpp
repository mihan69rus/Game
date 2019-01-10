#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "help.h"
#include "help_lex.h"
#include "table_strings.h"
#include "structs_const.h"

class Automat
{
	enum {HomeCond=0,ErrorCond=1,
		StringCond=2,WordCond=3,
		SpecialCond=4,ArithCond=5,
		IntegerCond=6,SignCond=7};
	int num_inc,buf_ch,condition,num_string,ptr,num_sym;
	int flag_str,err;
	char *buf_str;
	void inc_buf();
	void put_in_buf();
	int check_buf(){return (ptr<(buf_size-1));}
	int create_state(lexeme *p);
	lexeme *new_lex();
	lexeme *Home(int ch);
	lexeme *Error(int ch);
	lexeme *Integer(int ch);
	lexeme *String(int ch);
	lexeme *Words(int ch);
	lexeme *Arith(int ch);
	lexeme *Special(int ch);
	lexeme *Sign(int ch);
public:
	Automat();
	lexeme *Run(int ch);
	~Automat(){delete [] buf_str;}
};

Automat::Automat()
{
	err=num_sym=buf_ch=condition=ptr=flag_str=0;
	buf_str=new char[buf_size];
	buf_str[0]=0;
	num_string=1;
	num_inc=1;
}

int Automat::create_state(lexeme *p)
{
	int c=condition;
	if (c==ArithCond || c==SignCond)
		return SepStat;
	else if (c==SpecialCond && p->lex[0]=='@')
		return VarStat;
	else if (c==SpecialCond && p->lex[0]=='$')
		return LabStat;
	else if (c==SpecialCond && p->lex[0]=='?')
		return FunStat;
	else if (c==WordCond)
		return KeyStat;
	else if (c==IntegerCond)
		return IntStat;
	else if (c==StringCond)
		return StrStat;
	else
		return ErrStat;
}

lexeme *Automat::new_lex()
{
	lexeme *p;
	if (buf_str[0]!=0 || condition==ErrorCond)
	{
		p=new lexeme;
		p->next=NULL;
		p->lex=buf_str;
		p->string=num_string;
		p->length=num_sym;
		p->type=create_state(p);
		num_inc=1;
		ptr=0;
		buf_str=new char[buf_size];
		buf_str[0]=0;
		return p;
	}
	else
		return NULL;
}

void Automat::inc_buf()
{
	char *s;
	int i=0;
	num_inc++;
	s=new char[buf_size*num_inc];
	while (buf_str[i]!=0)
	{
		s[i]=buf_str[i];
		i++;
	}
	s[i]=0;
	delete [] buf_str;
	buf_str=s;
}

void Automat::put_in_buf()
{
	if (check_buf())
		buf_str[ptr]=buf_ch;
	else
	{
		inc_buf();
		buf_str[ptr]=buf_ch;
	}
	ptr++;
	num_sym++;
	buf_str[ptr]=0;
}

lexeme *Automat::Error(int ch)
{
	lexeme *p;
	if (buf_ch==EOF && err==0 && flag_str==0)
		return NULL;
	else if (buf_ch==EOF && (err || flag_str))
		p=new_lex();
	else
	{
		err=1;
		return NULL;
	}
	return p;
}

lexeme *Automat::Arith(int ch)
{
	lexeme *p;
	put_in_buf();
	p=new_lex();
	if (ch=='>' || ch=='<' || ch=='=')
		condition=SignCond;
	else if (check_arith(ch))
		condition=ArithCond;
	else if (check_num(ch))
		condition=IntegerCond;
	else if (check_special_sym(ch))
		condition=SpecialCond;
	else if (ch=='"')
		condition=StringCond;
	else if (check_letter(ch))
		condition=WordCond;
	else if (check_space(ch))
		condition=HomeCond;
	else
		condition=ErrorCond;
	return p;
}

lexeme *Automat::Home(int ch)
{
	lexeme *p;
	p=new_lex();
	if (buf_ch=='\n')
	{
		num_string++;
		num_sym=1;
	}
	else if (buf_ch=='\t')
		num_sym=num_sym+8;
	else if (buf_ch!=0)
		num_sym++;
	if (check_space(ch))
		condition=HomeCond;
	else if (ch=='>' || ch=='<' || ch=='=')
		condition=SignCond;
	else if (check_arith(ch))
		condition=ArithCond;
	else if (check_letter(ch))
		condition=WordCond;
	else if (ch=='"')
		condition=StringCond;
	else if (check_num(ch))
		condition=IntegerCond;
	else if (check_special_sym(ch))
		condition=SpecialCond;
	else
		condition=ErrorCond;
	return p;
}

lexeme *Automat::Integer(int ch)
{
	lexeme *p;
	put_in_buf();
	if (check_num(ch))
		return NULL;
	else
		p=new_lex();
	if (check_space(ch))
		condition=HomeCond;
	else if (ch=='>' || ch=='<' || ch=='=')
		condition=SignCond;
	else if (check_arith(ch))
		condition=ArithCond;
	else
		condition=ErrorCond;
	return p;
}

lexeme *Automat::String(int ch)
{
	lexeme *p;
	if (ch==EOF)
		condition=ErrorCond;
	if (buf_ch!='"')
	{
		put_in_buf();
		if (buf_ch=='\n')
		{
			num_string++;
			num_sym=1;
		}
		return NULL;
	}
	else if (flag_str==0)
	{
		flag_str=1;
		return NULL;
	}
	else
	{
		flag_str=0;
		p=new_lex();
		if (check_space(ch))
			condition=HomeCond;
		else if (ch=='>' || ch=='<' || ch=='=')
			condition=SignCond;
		else if (check_arith(ch))
			condition=ArithCond;
		else
			condition=ErrorCond;
		return p;
	}
}

lexeme *Automat::Words(int ch)
{
	lexeme *p;
	put_in_buf();
	if (check_letter(ch) || ch=='_')
		return NULL;
	else
	{
		p=new_lex();
		if (check_space(ch))
			condition=HomeCond;
		else if (ch=='>' || ch=='<' || ch=='=')
			condition=SignCond;
		else if (check_arith(ch))
			condition=ArithCond;
		else
			condition=ErrorCond;
		return p;
	}
}

lexeme *Automat::Special(int ch)
{
	lexeme *p;
	put_in_buf();
	if (check_letter(ch) || ch=='_')
		return NULL;
	else
	{
		p=new_lex();
		if (check_space(ch))
			condition=HomeCond;
		else if (ch=='>' || ch=='<' || ch=='=')
			condition=SignCond;
		else if (check_arith(ch))
			condition=ArithCond;
		else
			condition=ErrorCond;
		return p;
	}
}

lexeme *Automat::Sign(int ch)
{
	lexeme *p;
	put_in_buf();
	if (ch=='>' || ch=='<' || ch=='=')
		return NULL;
	else
	{
		p=new_lex();
		if (check_num(ch))
			condition=IntegerCond;
		else if (check_space(ch))
			condition=HomeCond;
		else if (check_arith(ch))
			condition=ArithCond;
		else if (check_special_sym(ch))
			condition=SpecialCond;
		else if (ch=='"')
			condition=StringCond;
		else if (check_letter(ch))
			condition=WordCond;
		else
			condition=ErrorCond;
		return p;
	}
}

lexeme *Automat::Run(int ch)
{
	lexeme *p;
	if (condition==ArithCond)
		p=Arith(ch);
	else if (condition==HomeCond)
		p=Home(ch);
	else if (condition==WordCond)
		p=Words(ch);
	else if (condition==IntegerCond)
		p=Integer(ch);
	else if (condition==SpecialCond)
		p=Special(ch);
	else if (condition==StringCond)
		p=String(ch);
	else if (condition==SignCond)
		p=Sign(ch);
	else if (condition==ErrorCond)
		p=Error(ch);
	if (ch==EOF && buf_ch!=EOF)
	{
		buf_ch=ch;
		if (p!=NULL)
			p->next=Run(ch);
		else
			p=Run(ch);
	}
	buf_ch=ch;
	return p;
}

lexeme *GetList(int argc,char **argv)
{
	lexeme *list,*p,*q;
	FILE *fd;
	int c;
	list=q=p=NULL;
	Automat analyzer;
	if (argc!=4)
	{
		printf("Select only one file, IP and Port\n");
		exit(1);
	}
	else
	{
		fd=fopen(argv[1],"r");
		if (!fd)
		{
			perror(argv[1]);
			exit(1);
		}
	}
	while ((c=fgetc(fd))!=EOF)
	{
		p=analyzer.Run(c);
		if (list==NULL)
			list=p;
		if (p!=NULL && q!=NULL)
		{
			q->next=p;
			q=q->next;
		}
		else if (p!=NULL)
			q=p;
	}
	if (q!=NULL)
		q->next=analyzer.Run(c);
	else
		q=analyzer.Run(c);
	if (list==NULL)
		list=q;
//	write_lexemes(list);
	if (!check_list(list))
	{
		write_lexemes(list);
		delete_lexemes(list);
		exit(1);
	}
	else
		return list;
}
