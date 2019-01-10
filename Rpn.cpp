#include <stdio.h>
#include "help.h"
#include "structs_const.h"
#include "Robot2.0.h"

class RpnElem;

struct RpnItem
{
	RpnElem *p;
	RpnItem *next;
};

class Vars
{
	long var;
	int index,flag;
	char *name;
	Vars *next;
public:
	Vars(long x,char *s);
	Vars(long x,int i,char *s);
	static Vars *PushVar(Vars *elem,Vars *list);
	static int Search(Vars *list,char *s);
	static int Search(Vars *list,char *s,int i);
	static long GetVar(Vars *list,char *s);
	static long GetVar(Vars *list,char *s,int i);
	static void Change(Vars *list,char *s,long new_v);
	static void Change(Vars *list,char *s,int i,long new_v);
	~Vars(){}
};

Vars *Vars::PushVar(Vars *elem,Vars *list)
{
	elem->next=list;
	list=elem;
	return list;
}

Vars::Vars(long x,int i,char *s)
{
	var=x;
	flag=1;
	index=i;
	next=NULL;
	name=s;
}

Vars::Vars(long x,char *s)
{
	var=x;
	flag=0;
	next=NULL;
	name=s;
}

void Vars::Change(Vars *list,char *s,int i,long new_v)
{
	while (list!=NULL)
	{
		if (cmp_s(s,list->name) && list->flag==1 && list->index==i)
		{
			list->var=new_v;
			return ;
		}
		list=list->next;
	}
	return ;
}

void Vars::Change(Vars *list,char *s,long new_v)
{
	while (list!=NULL)
	{
		if (cmp_s(s,list->name) && list->flag==0)
		{
			list->var=new_v;
			return ;
		}
		list=list->next;
	}
	return ;
}

long Vars::GetVar(Vars *list,char *s,int i)
{
	while (list!=NULL)
	{
		if (cmp_s(s,list->name) && list->flag==1 && list->index==i)
			return list->var;
		list=list->next;
	}
	return 0;
}

long Vars::GetVar(Vars *list,char *s)
{
	while (list!=NULL)
	{
		if (cmp_s(s,list->name) && list->flag==0)
			return list->var;
		list=list->next;
	}
	return 0;
}

int Vars::Search(Vars *list,char *s)
{
	while (list!=NULL)
	{
		if (cmp_s(s,list->name) && list->flag==0)
			return 1;
		list=list->next;
	}
	return 0;
}

int Vars::Search(Vars *list,char *s,int i)
{
	while (list!=NULL)
	{
		if (cmp_s(s,list->name) && list->flag==1 && list->index==i)
			return 1;
		list=list->next;
	}
	return 0;
}

class Labels
{
	char *lab;
	RpnItem **p;
	Labels *next;
public:
	Labels(char *l,RpnItem *q);
	static Labels *PushLab(Labels *q,Labels *list);
	static Labels *Search(Labels *list,char *s);
	void Change(RpnItem *q){*p=q;}
	RpnItem **GetL(){return p;}
	static Labels *Delete(Labels *list,char *s);
	~Labels(){}
};

Labels *Labels::PushLab(Labels *q,Labels *list)
{
	q->next=list;
	return q;
}

Labels::Labels(char *l,RpnItem *q)
{
	lab=l;
	p=new RpnItem*;
	*p=q;
	next=NULL;
}

Labels *Labels::Delete(Labels *list,char *s)
{
	Labels *p;
	p=NULL;
	while (list!=NULL)
	{
		if (cmp_s(s,list->lab))
		{
			if (p!=NULL && list!=NULL)
				p->next=list->next;
			else if (p!=NULL)
				p->next=NULL;
		}
		p=list;
		list=list->next;
	}
	return list;
}

Labels *Labels::Search(Labels *list,char *s)
{
	while (list!=NULL)
	{
		if (cmp_s(s,list->lab))
			return list;
		list=list->next;
	}
	return NULL;
}

struct ItemsVars
{
	RpnItem *stack;
	Vars *list;
	Client *Robot;
};

class RpnElem
{
public:
	virtual ~RpnElem(){}
	virtual void Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const =0;
	virtual void Write() =0;
	static void Push(RpnItem **stack,RpnElem *elem);
	static RpnElem *Pop(RpnItem **stack);
};

RpnElem *RpnElem::Pop(RpnItem **stack)
{
	RpnItem *p=*stack;
	if (p!=NULL)
	{
		RpnElem *q=p->p;;
		*stack=p->next;
		delete p;
		return q;
	}
	else
		return NULL;
}

void RpnElem::Push(RpnItem **stack,RpnElem *elem)
{
	RpnItem *p=new RpnItem;
	p->next=*stack;
	p->p=elem;
	*stack=p;
}

class Exception
{
	RpnElem *elem;
	lexeme *lex;
	const char *s;
	const char *wait;
public:
	Exception(lexeme *l,const char *str,const char *w);
	Exception(RpnElem *e,const char *str,const char *w);
	Exception(const Exception &p);
	void GetElem() const {elem->Write();}
	int GetNum() const {return lex->string;}
	int GetSym() const {return lex->length;}
	const char *GetWait() const {return wait;}
	char *GetLex() const {return lex->lex;}
	const char *GetStr() const {return s;}
	~Exception(){s=NULL;}
};

Exception::Exception(lexeme *l,const char *str,const char *w)
{
	elem=NULL;
	lex=l;
	s=str;
	wait=w;
}

Exception::Exception(RpnElem *e,const char *str,const char *w)
{
	lex=NULL;
	elem=e;
	s=str;
	wait=w;
}

Exception::Exception(const Exception &p)
{
	elem=p.elem;
	lex=p.lex;
	s=p.s;
	wait=p.wait;
}

class RpnNoOp : public RpnElem
{
public:
	virtual ~RpnNoOp(){}
	virtual void Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const
	{*cur_cmd=(*cur_cmd)->next;}
	virtual void Write(){printf("NOP ");}
};

class RpnConst : public RpnElem
{
public:
	virtual ~RpnConst(){}
	virtual void Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const;
	virtual RpnElem *Clone() const =0;
	virtual void WriteL() =0;
	virtual void Write(){WriteL();}
};

void RpnConst::Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const
{
	Push(&(inf->stack),Clone());
	*cur_cmd=(*cur_cmd)->next;
}

class RpnInt : public RpnConst
{
	long val;
public:
	RpnInt(long a){val=a;}
	virtual ~RpnInt(){}
	virtual RpnElem *Clone() const {return new RpnInt(val);}
	long Get() const {return val;}
	virtual void WriteL(){printf("%ld ",val);}
};

class RpnStr : public RpnConst
{
	char *str;
public:
	RpnStr(char *s){str=s;}
	virtual ~RpnStr(){}
	virtual RpnElem *Clone() const {return new RpnStr(str);}
	char *Get() const {return str;}
	virtual void WriteL(){printf("%s ",str);}
};

class RpnLab : public RpnConst
{
	char *name;
	RpnItem **p;
public:
	RpnLab(RpnItem **a,char *s){p=a;name=s;}
	virtual ~RpnLab(){}
	virtual RpnElem *Clone() const {return new RpnLab(p,name);}
	RpnItem *Get() const {return *p;}
	virtual void WriteL(){printf("%s ",name);}
};

class RpnVar : public RpnConst
{
	char *var;
public:
	RpnVar(char *v){var=v;}
	virtual ~RpnVar(){}
	virtual RpnElem *Clone() const {return new RpnVar(var);}
	char *Get() const {return var;}
	virtual void WriteL(){printf("%s ",var);}
};

class RpnOpGo : public RpnElem
{
public:
	RpnOpGo(){}
	virtual ~RpnOpGo(){}
	virtual void Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const;
	virtual void Write(){printf("goto ");}
};

void RpnOpGo::Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnLab *lab=dynamic_cast<RpnLab*>(op);
	if (!lab)
		throw Exception(op,"Label","No");
	RpnItem *addr=lab->Get();
	*cur_cmd=addr;
	delete op;
}

class RpnOpGoFalse : public RpnElem
{
public:
	RpnOpGoFalse(){}
	virtual ~RpnOpGoFalse(){}
	virtual void Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const;
	virtual void Write(){printf("gotofalse ");}
};

void RpnOpGoFalse::Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnLab *lab=dynamic_cast<RpnLab*>(op1);
	if (!lab)
		throw Exception(op1,"Label","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op2);
	if (!i)
		throw Exception(op2,"Long","No");
	if (i->Get()==0)
	{
		RpnItem *addr=lab->Get();
		*cur_cmd=addr;
		delete op1;
		delete op2;
	}
	else
	{
		*cur_cmd=(*cur_cmd)->next;
		delete op1;
		delete op2;
	}
}

class RpnFun : public RpnElem
{
public:
	virtual ~RpnFun(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const=0;
	virtual int GetPrior(){return prior;}
	virtual void Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const;
	virtual void WriteC() =0;
	virtual void Write(){WriteC();}
protected:
	int prior;
	enum {Ass,Dis,Con,Eq,Cmp,PlusMinus,MulDiv,NotMinus};
};

void RpnFun::Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const
{
	RpnElem *res=EvaluateFun(inf);
	if (res)
		Push(&(inf->stack),res);
	(*cur_cmd)=(*cur_cmd)->next;
}

class RpnFunAddr : public RpnFun
{
public:
	RpnFunAddr(){prior=Ass;}
	virtual ~RpnFunAddr(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("$ ");}
};

RpnElem *RpnFunAddr::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	long res=0;
	RpnVar *i1=dynamic_cast<RpnVar*>(op1);
	RpnInt *j1=dynamic_cast<RpnInt*>(op1);
	if (i1)
	{
		char *s=i1->Get();
		if (Vars::Search(inf->list,s))
			res=Vars::GetVar(inf->list,s);
		else
			throw Exception(op1,"Variable","No");
	}
	else if (j1)
	{
		RpnElem *op2=Pop(&(inf->stack));
		RpnVar *i2=dynamic_cast<RpnVar*>(op2);
		if (i2)
		{
			char *s=i2->Get();
			long ind=j1->Get();
			if (Vars::Search(inf->list,s,ind))
				res=Vars::GetVar(inf->list,s,ind);
			else
				throw Exception(op2,"Variable","No");
		}
		else
			throw Exception(op2,"Variable","No");
		delete op2;
	}
	else
		throw Exception(op1,"Long","No");
	delete op1;
	return new RpnInt(res);
}

class RpnFunPlus : public RpnFun
{
public:
	RpnFunPlus(){prior=PlusMinus;}
	virtual ~RpnFunPlus(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("+ ");}
};

RpnElem *RpnFunPlus::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i1->Get()+i2->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunMinus : public RpnFun
{
public:
	RpnFunMinus(){prior=PlusMinus;}
	virtual ~RpnFunMinus(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("- ");}
};

RpnElem *RpnFunMinus::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i2->Get()-i1->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunMul : public RpnFun
{
public:
	RpnFunMul(){prior=MulDiv;}
	virtual ~RpnFunMul(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("* ");}
};

RpnElem *RpnFunMul::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i1->Get()*i2->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunDiv : public RpnFun
{
public:
	RpnFunDiv(){prior=MulDiv;}
	virtual ~RpnFunDiv(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("/ ");}
};

RpnElem *RpnFunDiv::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i2->Get()/i1->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunMod : public RpnFun
{
public:
	RpnFunMod(){prior=MulDiv;}
	virtual ~RpnFunMod(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("%% ");}
};

RpnElem *RpnFunMod::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i2->Get()%i1->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunOr : public RpnFun
{
public:
	RpnFunOr(){prior=Dis;}
	virtual ~RpnFunOr(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("| ");}
};

RpnElem *RpnFunOr::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i1->Get() || i2->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunAnd : public RpnFun
{
public:
	RpnFunAnd(){prior=Con;}
	virtual ~RpnFunAnd(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("& ");}
};

RpnElem *RpnFunAnd::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i1->Get() && i2->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunMore : public RpnFun
{
public:
	RpnFunMore(){prior=Cmp;}
	virtual ~RpnFunMore(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("> ");}
};

RpnElem *RpnFunMore::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i2->Get()>i1->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunLess : public RpnFun
{
public:
	RpnFunLess(){prior=Cmp;}
	virtual ~RpnFunLess(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("< ");}
};

RpnElem *RpnFunLess::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i2->Get() < i1->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunMoreEq : public RpnFun
{
public:
	RpnFunMoreEq(){prior=Cmp;}
	virtual ~RpnFunMoreEq(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf(">= ");}
};

RpnElem *RpnFunMoreEq::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i2->Get()>=i1->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunLessEq : public RpnFun
{
public:
	RpnFunLessEq(){prior=Cmp;}
	virtual ~RpnFunLessEq(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("<= ");}
};

RpnElem *RpnFunLessEq::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i2->Get()<=i1->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunEq : public RpnFun
{
public:
	RpnFunEq(){prior=Eq;}
	virtual ~RpnFunEq(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("== ");}
};

RpnElem *RpnFunEq::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i1->Get()==i2->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunNotEq : public RpnFun
{
public:
	RpnFunNotEq(){prior=Eq;}
	virtual ~RpnFunNotEq(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("<> ");}
};

RpnElem *RpnFunNotEq::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	int res=i1->Get()!=i2->Get();
	delete op1;
	delete op2;
	return new RpnInt(res);
}

class RpnFunUnMinus : public RpnFun
{
public:
	RpnFunUnMinus(){prior=NotMinus;}
	virtual ~RpnFunUnMinus(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("\\ ");}
};

RpnElem *RpnFunUnMinus::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	int res=(-1)*(i1->Get());
	delete op1;
	return new RpnInt(res);
}

class RpnFunNeg : public RpnFun
{
public:
	RpnFunNeg(){prior=NotMinus;}
	virtual ~RpnFunNeg(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("! ");}
};

RpnElem *RpnFunNeg::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	int res=!(i1->Get());
	delete op1;
	return new RpnInt(res);
}

class RpnFunPrint : public RpnFun
{
public:
	RpnFunPrint(){prior=Ass;}
	virtual ~RpnFunPrint(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("print ");}
};

RpnElem *RpnFunPrint::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	RpnStr *s=dynamic_cast<RpnStr*>(op);
	if (i)
		printf("%ld\n",i->Get());
	else if (s)
		printf("%s\n",s->Get());
	else
		throw Exception(op,"Long","No");
	delete op;
	return NULL;
}

class RpnFunAss : public RpnFun
{
public:
	RpnFunAss(){prior=Ass;}
	virtual ~RpnFunAss(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("= ");}
};

RpnElem *RpnFunAss::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	RpnElem *op1=Pop(&(inf->stack));
	RpnVar *i1=dynamic_cast<RpnVar*>(op1);
	RpnInt *j1=dynamic_cast<RpnInt*>(op1);
	if (i1)
	{
		char *s=i1->Get();
		if (Vars::Search(inf->list,s))
			Vars::Change(inf->list,s,res);
		else
			inf->list=Vars::PushVar(new Vars(res,s),inf->list);
	}
	else if (j1)
	{
		RpnElem *op2=Pop(&(inf->stack));
		RpnVar *i2=dynamic_cast<RpnVar*>(op2);
		if (i2)
		{
			char *s=i2->Get();
			long ind=j1->Get();
			if (Vars::Search(inf->list,s,ind))
				Vars::Change(inf->list,s,ind,res);
			else
				inf->list=Vars::PushVar(new Vars(res,ind,s),
					inf->list);
		}
		else
			throw Exception(op2,"Variable","No");
		delete op2;
	}
	else
		throw Exception(op1,"Long","No");
	delete op1;
	delete op;
	return NULL;
}

class RpnFunBrace : public RpnFun
{
public:
	RpnFunBrace(){prior=Ass;}
	virtual ~RpnFunBrace(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const{return NULL;}
	virtual void WriteC(){printf("( ");}
};

class RpnFunSell : public RpnFun
{
public:
	RpnFunSell(){prior=Ass;}
	virtual ~RpnFunSell(){}
	virtual void WriteC(){printf("sell ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunSell::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	inf->Robot->sell(i2->Get(),i1->Get());
	delete op1;
	delete op2;
	return NULL;
}

class RpnFunBuy : public RpnFun
{
public:
	RpnFunBuy(){prior=Ass;}
	virtual ~RpnFunBuy(){}
	virtual void WriteC(){printf("buy ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunBuy::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op1=Pop(&(inf->stack));
	RpnInt *i1=dynamic_cast<RpnInt*>(op1);
	if (!i1)
		throw Exception(op1,"Long","No");
	RpnElem *op2=Pop(&(inf->stack));
	RpnInt *i2=dynamic_cast<RpnInt*>(op2);
	if (!i2)
		throw Exception(op2,"Long","No");
	inf->Robot->buy(i2->Get(),i1->Get());
	delete op1;
	delete op2;
	return NULL;
}


class RpnFunEnd : public RpnFun
{
public:
	RpnFunEnd(){prior=Ass;}
	virtual ~RpnFunEnd(){}
	virtual void WriteC(){printf("endturn ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{inf->Robot->end();return NULL;}
};

class RpnFunProd : public RpnFun
{
public:
	RpnFunProd(){prior=Ass;}
	virtual ~RpnFunProd(){}
	virtual void WriteC(){printf("prod ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{inf->Robot->prod();return NULL;}
};

class RpnFunBuild : public RpnFun
{
public:
	RpnFunBuild(){prior=Ass;}
	virtual ~RpnFunBuild(){}
	virtual void WriteC(){printf("build ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{inf->Robot->build();return NULL;}
};

class RpnFunTurn : public RpnFun
{
public:
	RpnFunTurn(){prior=Ass;}
	virtual ~RpnFunTurn(){}
	virtual void WriteC(){printf("?turn ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{return new RpnInt(inf->Robot->get_month());}
};

class RpnFunRawPrice : public RpnFun
{
public:
	RpnFunRawPrice(){prior=Ass;}
	virtual ~RpnFunRawPrice(){}
	virtual void WriteC(){printf("?raw_price ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{return new RpnInt(inf->Robot->min_cost());}
};

class RpnFunProdPrice : public RpnFun
{
public:
	RpnFunProdPrice(){prior=Ass;}
	virtual ~RpnFunProdPrice(){}
	virtual void WriteC(){printf("?prod_price ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{return new RpnInt(inf->Robot->max_cost());}
};

class RpnFunDemand : public RpnFun
{
public:
	RpnFunDemand(){prior=Ass;}
	virtual ~RpnFunDemand(){}
	virtual void WriteC(){printf("?demand ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{return new RpnInt(inf->Robot->num_prod());}
};

class RpnFunSupply : public RpnFun
{
public:
	RpnFunSupply(){prior=Ass;}
	virtual ~RpnFunSupply(){}
	virtual void WriteC(){printf("?supply ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{return new RpnInt(inf->Robot->num_raw());}
};

class RpnFunPlayers : public RpnFun
{
public:
	RpnFunPlayers(){prior=Ass;}
	virtual ~RpnFunPlayers(){}
	virtual void WriteC(){printf("?players ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{return new RpnInt(inf->Robot->num_player());}
};

class RpnFunMyId : public RpnFun
{
public:
	RpnFunMyId(){prior=Ass;}
	virtual ~RpnFunMyId(){}
	virtual void WriteC(){printf("?my_id ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{return new RpnInt(inf->Robot->mynum());}
};

class RpnFunActivePlayers : public RpnFun
{
public:
	RpnFunActivePlayers(){prior=Ass;}
	virtual ~RpnFunActivePlayers(){}
	virtual void WriteC(){printf("?active_players ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const
		{return new RpnInt(inf->Robot->num_active_player());}
};

class RpnFunMoney : public RpnFun
{
public:
	RpnFunMoney(){prior=Ass;}
	virtual ~RpnFunMoney(){}
	virtual void WriteC(){printf("?money ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunMoney::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->money(res));
}

class RpnFunRaw : public RpnFun
{
public:
	RpnFunRaw(){prior=Ass;}
	virtual ~RpnFunRaw(){}
	virtual void WriteC(){printf("?raw ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunRaw::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->raw(res));
}

class RpnFunProduction : public RpnFun
{
public:
	RpnFunProduction(){prior=Ass;}
	virtual ~RpnFunProduction(){}
	virtual void WriteC(){printf("?production ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunProduction::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->production(res));
}

class RpnFunFact : public RpnFun
{
public:
	RpnFunFact(){prior=Ass;}
	virtual ~RpnFunFact(){}
	virtual void WriteC(){printf("?factories ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunFact::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->factories(res));
}

class RpnFunCFact : public RpnFun
{
public:
	RpnFunCFact(){prior=Ass;}
	virtual ~RpnFunCFact(){}
	virtual void WriteC(){printf("?const_factories ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunCFact::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->c_fact(res));
}

class RpnFunRawSold : public RpnFun
{
public:
	RpnFunRawSold(){prior=Ass;}
	virtual ~RpnFunRawSold(){}
	virtual void WriteC(){printf("?result_raw_sold ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunRawSold::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->raw_sold(res));
}

class RpnFunResRawPrice : public RpnFun
{
public:
	RpnFunResRawPrice(){prior=Ass;}
	virtual ~RpnFunResRawPrice(){}
	virtual void WriteC(){printf("?result_raw_price ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunResRawPrice::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->raw_price(res));
}

class RpnFunProdBought : public RpnFun
{
public:
	RpnFunProdBought(){prior=Ass;}
	virtual ~RpnFunProdBought(){}
	virtual void WriteC(){printf("?result_prod_bought ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunProdBought::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->prod_bought(res));
}

class RpnFunResProdPrice : public RpnFun
{
public:
	RpnFunResProdPrice(){prior=Ass;}
	virtual ~RpnFunResProdPrice(){}
	virtual void WriteC(){printf("?result_prod_price ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

RpnElem *RpnFunResProdPrice::EvaluateFun(ItemsVars *inf) const
{
	RpnElem *op=Pop(&(inf->stack));
	RpnInt *i=dynamic_cast<RpnInt*>(op);
	if (!i)
		throw Exception(op,"Long","No");
	long res=i->Get();
	delete op;
	return new RpnInt(inf->Robot->prod_price(res));
}
