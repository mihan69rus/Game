#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs_const.h"
#include "table_strings.h"
#include "lexis.h"
#include "help.h"
#include "Rpn.h"
#include "Robot2.0.h"

class Parser
{
	int num_lab;
	lexeme *cur;
	RpnItem **RpnList;
	RpnItem **Stack;
	Labels *LabList;
	Labels *LabNotMatch;
	RpnItem *RpnPush(RpnElem *p);
	RpnFun *CreateRpnElem();
	void HelpGoto();
	RpnItem **HelpIf();
	void AlgorithmD();
	void HelpFun(char *s);
	void HelpKey(char *s);
	void next();
	void S();
	void C();
	void F();
	void G();
	void H();
	void A();
	void O();
	void V();
	void K();
	void D();
	void P();
	void T();
	void semicolon();
	void colon();
	int check_unary_op();
	int check_double_op();
public:
	Parser();
	RpnItem **CheckList(lexeme *list);
};

void Parser::HelpKey(char *s)
{
	if (cmp_s(s,"sell"))
		RpnPush(new RpnFunSell());
	else if (cmp_s(s,"buy"))
		RpnPush(new RpnFunBuy());
	else if (cmp_s(s,"build"))
		RpnPush(new RpnFunBuild());
	else if (cmp_s(s,"prod"))
		RpnPush(new RpnFunProd());
	else if (cmp_s(s,"endturn"))
		RpnPush(new RpnFunEnd());
}

void Parser::HelpFun(char *s)
{
	if (cmp_s(s,"?my_id"))
		RpnPush(new RpnFunMyId());
	else if (cmp_s(s,"?turn"))
		RpnPush(new RpnFunTurn());
	else if (cmp_s(s,"?players"))
		RpnPush(new RpnFunPlayers());
	else if (cmp_s(s,"?active_players"))
		RpnPush(new RpnFunActivePlayers());
	else if (cmp_s(s,"?supply"))
		RpnPush(new RpnFunSupply());
	else if (cmp_s(s,"?raw_price"))
		RpnPush(new RpnFunRawPrice());
	else if (cmp_s(s,"?demand"))
		RpnPush(new RpnFunDemand());
	else if (cmp_s(s,"?prod_price"))
		RpnPush(new RpnFunProdPrice());
	else if (cmp_s(s,"?money"))
		RpnPush(new RpnFunMoney());
	else if (cmp_s(s,"?raw"))
		RpnPush(new RpnFunRaw());
	else if (cmp_s(s,"?production"))
		RpnPush(new RpnFunProduction());
	else if (cmp_s(s,"?factories"))
		RpnPush(new RpnFunFact());
	else if (cmp_s(s,"?const_factories"))
		RpnPush(new RpnFunCFact());
	else if (cmp_s(s,"?result_raw_sold"))
		RpnPush(new RpnFunRawSold());
	else if (cmp_s(s,"?result_raw_price"))
		RpnPush(new RpnFunResRawPrice());
	else if (cmp_s(s,"?result_prod_bought"))
		RpnPush(new RpnFunProdBought());
	else if (cmp_s(s,"?result_prod_price"))
		RpnPush(new RpnFunResProdPrice());
	else
		throw Exception(cur,"Don't find this function","function");
}

RpnItem **Parser::CheckList(lexeme *list)
{
	cur=list;
	S();
	if (LabNotMatch!=NULL)
		throw Exception(cur,"Didn't matched labels","lab");
	return RpnList;
}

RpnItem **Parser::HelpIf()
{
	next();
	RpnItem **label=new RpnItem*;
	RpnElem *p=new RpnNoOp();
	RpnItem *q=RpnPush(p);
	*label=q;
	A();
	RpnPush(new RpnLab(label,cur->lex));
	RpnPush(new RpnOpGoFalse());
	return label;
}

void Parser::HelpGoto()
{
	Labels *label;
	label=Labels::Search(LabList,cur->lex);
	if (label==NULL)
	{
		RpnElem *p=new RpnNoOp();
		RpnItem *q=RpnPush(p);
		Labels *l=new Labels(cur->lex,q);
		LabNotMatch=Labels::PushLab(l,LabNotMatch);
		LabList=Labels::PushLab(l,LabList);
		label=Labels::Search(LabList,cur->lex);
	}
	RpnPush(new RpnLab(label->GetL(),cur->lex));
	RpnPush(new RpnOpGo());
}

void Parser::AlgorithmD()
{
	RpnFun *p=CreateRpnElem();
	RpnElem *elem=RpnElem::Pop(Stack);
	RpnFun *op=dynamic_cast<RpnFun*>(elem);
	RpnFun *op2=dynamic_cast<RpnFunBrace*>(elem);
	while (op!=NULL && (op->GetPrior() >= p->GetPrior()) &&	op2==NULL)
	{
		RpnPush(elem);
		elem=RpnElem::Pop(Stack);
		op=dynamic_cast<RpnFun*>(elem);
		op2=dynamic_cast<RpnFunBrace*>(elem);
	}
	RpnElem::Push(Stack,elem);
	RpnElem::Push(Stack,p);
}

RpnFun *Parser::CreateRpnElem()
{
	if (cmp_s(cur->lex,"+"))
		return new RpnFunPlus();
	else if (cmp_s(cur->lex,"-"))
		return new RpnFunMinus();
	else if (cmp_s(cur->lex,"*"))
		return new RpnFunMul();
	else if (cmp_s(cur->lex,"/"))
		return new RpnFunDiv();
	else if (cmp_s(cur->lex,"%"))
		return new RpnFunMod();
	else if (cmp_s(cur->lex,"|"))
		return new RpnFunOr();
	else if (cmp_s(cur->lex,"&"))
		return new RpnFunAnd();
	else if (cmp_s(cur->lex,">"))
		return new RpnFunMore();
	else if (cmp_s(cur->lex,"<"))
		return new RpnFunLess();
	else if (cmp_s(cur->lex,">=") || cmp_s(cur->lex,"=>"))
		return new RpnFunMoreEq();
	else if (cmp_s(cur->lex,"<=") || cmp_s(cur->lex,"<="))
		return new RpnFunLessEq();
	else if (cmp_s(cur->lex,"=="))
		return new RpnFunEq();
	else if (cmp_s(cur->lex,"<>"))
		return new RpnFunNotEq();
	else if (cmp_s(cur->lex,"+"))
		return new RpnFunPlus();
	else if (cmp_s(cur->lex,"\\"))
		return new RpnFunUnMinus();
	else if (cmp_s(cur->lex,"!"))
		return new RpnFunNeg();
	else if (cmp_s(cur->lex,"="))
		return new RpnFunAss();
	return NULL;
}

Parser::Parser()
{
	cur=NULL;
	RpnList=new RpnItem*;
	*RpnList=new RpnItem;
	(*RpnList)->next=NULL;
	(*RpnList)->p=new RpnNoOp();
	LabList=NULL;
	Stack=new RpnItem*;
	*Stack=NULL;
	LabNotMatch=NULL;
	RpnElem::Push(Stack,new RpnFunBrace());
}

RpnItem *Parser::RpnPush(RpnElem *p)
{
	RpnItem *q;
	RpnItem *ptr;
	ptr=*RpnList;
	q=new RpnItem;
	q->p=p;
	q->next=NULL;
	while (ptr!=NULL && ptr->next!=NULL)
		ptr=ptr->next;
	if (*RpnList==NULL)
		*RpnList=q;
	else
		ptr->next=q;
	return q;
}

void Parser::semicolon()
{
	if (cmp_s(";",cur->lex))
		next();
	else
		throw Exception(cur,"Bad command",";");
}

void Parser::colon()
{
	if (cmp_s(":",cur->lex))
		next();
	else
		throw Exception(cur,"Bad label",":");
}

int Parser::check_unary_op()
{
	return (cmp_s("!",cur->lex) || cmp_s(cur->lex,"\\"));
}

int Parser::check_double_op()
{
	int i=0;
	char *s=cur->lex;
	for (i=0;i<duo_op;i++)
	{
		if (ops[i]==s[0] && s[1]==0)
			return 1;
	}
	if (cmp_s(s,">=") || cmp_s(s,"<=") || cmp_s(s,"=="))
		return 1;
	else if (cmp_s(s,"=>") || cmp_s(s,"=<") || cmp_s(s,"<>"))
		return 1;
	return 0;
}

void Parser::next()
{
	if (cur==NULL)
		throw Exception(cur,"Don't expect end of lexemes",
			"end");
	else if (cur->next==NULL)
		throw Exception(cur,"Don't expect end of lexemes",
			"end");
	cur=cur->next;
}

void Parser::S()
{
	RpnElem::Push(Stack,new RpnFunBrace());
	if (cur->type==LabStat)
	{
		RpnElem *p=new RpnNoOp();
		RpnItem *q=RpnPush(p);
		LabList=Labels::PushLab(new Labels(cur->lex,q),LabList);
		next();
		colon();
		C();
	}
	else
		throw Exception(cur,"Didn't find label","label");
}

void Parser::C()
{
	if (cur->type==FunStat)
	{
		char *s=cur->lex;
		next();
		F();
		HelpFun(s);
		semicolon();
		C();
	}
	else if (cur->type==LabStat)
	{
		RpnElem *p=new RpnNoOp();
		RpnItem *q=RpnPush(p);
		Labels *lab=Labels::Search(LabNotMatch,cur->lex);
		if (lab!=NULL)
		{
			lab->Change(q);
			LabNotMatch=Labels::Delete(LabNotMatch,cur->lex);
		}
		else
			LabList=Labels::PushLab(new Labels(cur->lex,q),
				LabList);
		next();
		colon();
		C();
	}
	else if (cur->type==VarStat)
	{
		RpnPush(new RpnVar(cur->lex));
		next();
		V();
		if (cmp_s("=",cur->lex))
		{
			next();
			A();
			RpnPush(new RpnFunAss());
			semicolon();
			C();
		}
		else
			throw Exception(cur,"Expected assignment","=");
	}
	else if (cmp_s(";",cur->lex))
	{
		next();
		C();
	}
	else if (cmp_s("end",cur->lex))
	{
		if (cur->next!=NULL)
			throw Exception(cur,"Lexeme after 'end'","EOF");
	}
	else
	{
		K();
		C();
	}
}

void Parser::F()
{
	if (cmp_s("(",cur->lex))
	{
		next();
		G();
	}
	else
		throw Exception(cur,"Function without parenthesis","(");
}

void Parser::G()
{
	if (cmp_s(")",cur->lex))
		next();
	else
	{
		RpnElem::Push(Stack,new RpnFunBrace());
		A();
		RpnElem::Pop(Stack);
		H();
	}
}

void Parser::H()
{
	if (cmp_s(")",cur->lex))
		next();
	else if (cmp_s(",",cur->lex))
	{
		next();
		RpnElem::Push(Stack,new RpnFunBrace());
		A();
		RpnElem::Pop(Stack);
		H();
	}
	else
		throw Exception(cur,"Bad parameters in function",",'or')");
}

void Parser::A()
{
	if (cur->type==IntStat)
	{
		RpnPush(new RpnInt(str_in_num(cur->lex)));
		next();
		O();
	}
	else if (cur->type==VarStat)
	{
		RpnPush(new RpnVar(cur->lex));
		next();
		V();
		RpnPush(new RpnFunAddr());
		O();
	}
	else if (cur->type==FunStat)
	{
		char *s=cur->lex;
		next();
		F();
		HelpFun(s);
		O();
	}
	else if (check_unary_op())
	{
		AlgorithmD();
		next();
		A();
		O();
	}
	else if (cmp_s("(",cur->lex))
	{
		RpnElem::Push(Stack,new RpnFunBrace());
		next();
		A();
		if (cmp_s(")",cur->lex))
		{
			RpnElem *op=RpnElem::Pop(Stack);
			RpnFunBrace *ch=dynamic_cast<RpnFunBrace*>(op);
			while (!ch)
			{
				RpnPush(op);
				op=RpnElem::Pop(Stack);
				ch=dynamic_cast<RpnFunBrace*>(op);
			}
			delete op;
			next();
		}
		else
			throw Exception(cur,"Didn't find bracket",")");
		O();
	}
	else
		throw Exception(cur,"Bad expression","expression");
	RpnElem *op=RpnElem::Pop(Stack);
	RpnFunBrace *ch=dynamic_cast<RpnFunBrace*>(op);
	while (!ch)
	{
		RpnPush(op);
		op=RpnElem::Pop(Stack);
		ch=dynamic_cast<RpnFunBrace*>(op);
	}
	RpnElem::Push(Stack,new RpnFunBrace());
}

void Parser::O()
{
	if (check_double_op())
	{
		AlgorithmD();
		next();
		A();
	}
}

void Parser::V()
{
	if (cmp_s("[",cur->lex))
	{
		next();
		RpnElem::Push(Stack,new RpnFunBrace());
		A();
		RpnElem::Pop(Stack);
		if (cmp_s("]",cur->lex))
			next();
		else
			throw Exception(cur,"Didn't find brace","]");
	}
}

void Parser::K()
{
	if (cmp_s("endturn",cur->lex) || cmp_s("prod",cur->lex)
		|| cmp_s("build",cur->lex))
	{
		char *s=cur->lex;
		next();
		HelpKey(s);
		semicolon();
	}
	else if (cmp_s("sell",cur->lex) || cmp_s("buy",cur->lex))
	{
		char *s=cur->lex;
		next();
		A();
		A();
		HelpKey(s);
		semicolon();
	}
	else if (cmp_s("goto",cur->lex))
	{
		next();
		if (cur->type==LabStat)
		{
			HelpGoto();
			next();
			semicolon();
		}
		else
			throw Exception(cur,"Must be label","label");
	}
	else if (cmp_s("print",cur->lex))
	{
		next();
		P();
		semicolon();
	}
	else if (cmp_s("if",cur->lex))
	{
		RpnItem **label=HelpIf();
		if (cmp_s("{",cur->lex))
		{
			next();
			D();
			RpnElem *p=new RpnNoOp();
			RpnItem *q=RpnPush(p);
			*label=q;
		}
		else
			throw Exception(cur,"Didn't find brace","{");
	}
	else
		throw Exception(cur,"Didn't find this key","key");
}

void Parser::D()
{
	if (cur->type==FunStat)
	{
		char *s=cur->lex;
		next();
		F();
		HelpFun(s);
		semicolon();
		D();
	}
	else if (cur->type==LabStat)
	{
		RpnElem *p=new RpnNoOp();
		RpnItem *q=RpnPush(p);
		Labels *lab=Labels::Search(LabNotMatch,cur->lex);
		if (lab!=NULL)
		{
			lab->Change(q);
			LabNotMatch=Labels::Delete(LabNotMatch,cur->lex);
		}
		else
			LabList=Labels::PushLab(new Labels(cur->lex,q),
				LabList);
		next();
		colon();
		D();
	}
	else if (cur->type==VarStat)
	{
		RpnPush(new RpnVar(cur->lex));
		next();
		V();
		if (cmp_s("=",cur->lex))
		{
			next();
			A();
			RpnPush(new RpnFunAss());
			semicolon();
			D();
		}
		else
			throw Exception(cur,"Expected assignment","=");
	}
	else if (cmp_s(";",cur->lex))
	{
		next();
		D();
	}
	else if (cmp_s(cur->lex,"}"))
		next();
	else
	{
		K();
		D();
	}
}

void Parser::P()
{
	if (cur->type==StrStat)
	{
		RpnPush(new RpnStr(cur->lex));
		next();
		RpnPush(new RpnFunPrint);
		T();
	}
	else
	{
		A();
		RpnPush(new RpnFunPrint);
		T();
	}
}

void Parser::T()
{
	if (cmp_s(",",cur->lex))
	{
		next();
		P();
	}
}

int main(int argc,char **argv)
{
	lexeme *list;
	list=GetList(argc,argv);
	Parser checker;
	RpnItem **Cur;
	RpnItem *Cur2;
	ItemsVars *Inf=new ItemsVars;
	Inf->stack=NULL;
	Inf->list=NULL;
	try
	{
		Cur=checker.CheckList(list);
		Cur2=*Cur;
	}
	catch (const Exception &e)
	{
		printf("%i, %i: Error: ",e.GetNum(),e.GetSym());
		printf("%s: ",e.GetStr());
		printf("Expected '%s', but '%s' found\n",
			e.GetWait(),e.GetLex());
		return 1;
	}
	while (Cur2!=NULL)
	{
		Cur2->p->Write();
		Cur2=Cur2->next;
	}
	printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	int port=str_in_num((const char *)argv[3]);
	Client Me(port,argv[2]);
	Me.wait_end();
	Me.get_market();
	Me.get_mynum();
	Me.create_list();
	Me.zero_auc();
	Me.get_usr();
	Inf->Robot=&Me;
	try
	{
		while (*Cur!=NULL)
			(*Cur)->p->Evaluate(Inf,Cur);
	}
	catch (const Exception &e)
	{
		printf("ERROR: in progress. Expected %s.\n",e.GetStr());
		printf("Error: don't expected RpnElem=");
		e.GetElem();
		printf(".\n");
		return 1;
	}
	printf("\nOk\n");
	return 0;
}
