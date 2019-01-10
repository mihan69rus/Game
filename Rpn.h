#ifndef SENTRY5
#define SENTRY5

#include <stdio.h>
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
	int GetNum() const {return lex->string;}
	int GetSym() const {return lex->length;}
	const char *GetWait() const {return wait;}
	void GetElem() const {elem->Write();}
	char *GetLex() const {return lex->lex;}
	const char *GetStr() const {return s;}
	~Exception(){s=NULL;}
};

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

class RpnOpGoFalse : public RpnElem
{
public:
	RpnOpGoFalse(){}
	virtual ~RpnOpGoFalse(){}
	virtual void Evaluate(ItemsVars *inf,RpnItem **cur_cmd) const;
	virtual void Write(){printf("gotofalse ");}
};

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

class RpnFunAddr : public RpnFun
{
public:
	RpnFunAddr(){prior=Ass;}
	virtual ~RpnFunAddr(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("$ ");}
};

class RpnFunPlus : public RpnFun
{
public:
	RpnFunPlus(){prior=PlusMinus;}
	virtual ~RpnFunPlus(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("+ ");}
};

class RpnFunMinus : public RpnFun
{
public:
	RpnFunMinus(){prior=PlusMinus;}
	virtual ~RpnFunMinus(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("- ");}
};

class RpnFunMul : public RpnFun
{
public:
	RpnFunMul(){prior=MulDiv;}
	virtual ~RpnFunMul(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("* ");}
};

class RpnFunDiv : public RpnFun
{
public:
	RpnFunDiv(){prior=MulDiv;}
	virtual ~RpnFunDiv(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("/ ");}
};

class RpnFunMod : public RpnFun
{
public:
	RpnFunMod(){prior=MulDiv;}
	virtual ~RpnFunMod(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("%% ");}
};

class RpnFunOr : public RpnFun
{
public:
	RpnFunOr(){prior=Dis;}
	virtual ~RpnFunOr(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("| ");}
};

class RpnFunAnd : public RpnFun
{
public:
	RpnFunAnd(){prior=Con;}
	virtual ~RpnFunAnd(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("& ");}
};

class RpnFunMore : public RpnFun
{
public:
	RpnFunMore(){prior=Cmp;}
	virtual ~RpnFunMore(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("> ");}
};

class RpnFunLess : public RpnFun
{
public:
	RpnFunLess(){prior=Cmp;}
	virtual ~RpnFunLess(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("< ");}
};

class RpnFunMoreEq : public RpnFun
{
public:
	RpnFunMoreEq(){prior=Cmp;}
	virtual ~RpnFunMoreEq(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf(">= ");}
};

class RpnFunLessEq : public RpnFun
{
public:
	RpnFunLessEq(){prior=Cmp;}
	virtual ~RpnFunLessEq(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("<= ");}
};

class RpnFunEq : public RpnFun
{
public:
	RpnFunEq(){prior=Eq;}
	virtual ~RpnFunEq(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("== ");}
};

class RpnFunNotEq : public RpnFun
{
public:
	RpnFunNotEq(){prior=Eq;}
	virtual ~RpnFunNotEq(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("<> ");}
};

class RpnFunUnMinus : public RpnFun
{
public:
	RpnFunUnMinus(){prior=NotMinus;}
	virtual ~RpnFunUnMinus(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("\\ ");}
};

class RpnFunNeg : public RpnFun
{
public:
	RpnFunNeg(){prior=NotMinus;}
	virtual ~RpnFunNeg(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("! ");}
};

class RpnFunPrint : public RpnFun
{
public:
	RpnFunPrint(){}
	virtual ~RpnFunPrint(){prior=Ass;}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("print ");}
};


class RpnFunAss : public RpnFun
{
public:
	RpnFunAss(){prior=Ass;}
	virtual ~RpnFunAss(){}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
	virtual void WriteC(){printf("= ");}
};

class RpnFunBrace : public RpnFun
{
public:
	RpnFunBrace(){}
	virtual ~RpnFunBrace(){prior=Ass;}
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

class RpnFunBuy : public RpnFun
{
public:
	RpnFunBuy(){prior=Ass;}
	virtual ~RpnFunBuy(){}
	virtual void WriteC(){printf("buy ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

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

class RpnFunRaw : public RpnFun
{
public:
	RpnFunRaw(){prior=Ass;}
	virtual ~RpnFunRaw(){}
	virtual void WriteC(){printf("?raw ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

class RpnFunProduction : public RpnFun
{
public:
	RpnFunProduction(){prior=Ass;}
	virtual ~RpnFunProduction(){}
	virtual void WriteC(){printf("?production ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

class RpnFunFact : public RpnFun
{
public:
	RpnFunFact(){prior=Ass;}
	virtual ~RpnFunFact(){}
	virtual void WriteC(){printf("?factories ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

class RpnFunCFact : public RpnFun
{
public:
	RpnFunCFact(){prior=Ass;}
	virtual ~RpnFunCFact(){}
	virtual void WriteC(){printf("?const_factories ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

class RpnFunRawSold : public RpnFun
{
public:
	RpnFunRawSold(){prior=Ass;}
	virtual ~RpnFunRawSold(){}
	virtual void WriteC(){printf("?result_raw_sold ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

class RpnFunResRawPrice : public RpnFun
{
public:
	RpnFunResRawPrice(){prior=Ass;}
	virtual ~RpnFunResRawPrice(){}
	virtual void WriteC(){printf("?result_raw_price ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

class RpnFunProdBought : public RpnFun
{
public:
	RpnFunProdBought(){prior=Ass;}
	virtual ~RpnFunProdBought(){}
	virtual void WriteC(){printf("?result_prod_bought ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

class RpnFunResProdPrice : public RpnFun
{
public:
	RpnFunResProdPrice(){prior=Ass;}
	virtual ~RpnFunResProdPrice(){}
	virtual void WriteC(){printf("?result_prod_price ");}
	virtual RpnElem *EvaluateFun(ItemsVars *inf) const;
};

#endif

