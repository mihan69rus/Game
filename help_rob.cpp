#include <stdio.h>
#include <stdlib.h>
#include "structs_const.h"
#include "table_strings.h"
#include "help_rob.h"

struct player *push(struct player *p)
{
	struct player *q;
	q=new struct player;
	q->next=p;
	q->num=0;
	q->mat=0;
	q->money=0;
	q->prod=0;
	return q;
}

struct player *delete_list(struct player *p)
{
	struct player *q;
	while (p!=NULL)
	{
		q=p->next;
		delete p;
		p=q;
	}
	return p;
}

void write_me(struct player *p)
{
	printf("ME:\n");
	printf("\tMaterials:%i\n",p->mat);
	printf("\tProducts:%i\n",p->prod);
	printf("\tMoney:%i\n",p->money);
	printf("\tFactories:%i\n",p->fact);
	printf("\tC_factories:%i\n",p->const_fact);
	printf("\tMaterial_auc:%i\n",p->mat_auc);
	printf("\tCost_mat_auc:%i\n",p->cost_mat);
	printf("\tProduct_auc:%i\n",p->prod_auc);
	printf("\tCost_prod_auc:%i\n",p->cost_prod);
}

void write_market(struct information *p)
{
	printf("Market:\n");
	printf("\tMaterials:%i\n",p->num_mat);
	printf("\tProducts:%i\n",p->num_prod);
	printf("\tMaterials cost:%i\n",p->min_cost);
	printf("\tProducts cost:%i\n",p->max_cost);
	printf("\tPlayers:%i\n",p->num_usr);
}

void write_list(struct player *p)
{
	while (p!=NULL)
	{
		printf("Player:%i\n",p->num);
		printf("\tMaterials:%i\n",p->mat);
		printf("\tProducts:%i\n",p->prod);
		printf("\tMoney:%i\n",p->money);
		printf("\tFactories:%i\n",p->fact);
		printf("\tC_factories:%i\n",p->const_fact);
		printf("\tMaterial_auc:%i\n",p->mat_auc);
		printf("\tCost_mat_auc:%i\n",p->cost_mat);
		printf("\tProduct_auc:%i\n",p->prod_auc);
		printf("\tCost_prod_auc:%i\n",p->cost_prod);
		p=p->next;
	}
}


