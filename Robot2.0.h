#ifndef SENTRY7
#define SENTRY7

#include <stdio.h>
#include "structs_const.h"
#include "table_strings.h"
#include "arpa/inet.h"

class Client
{
	int sockfd,mem_ptr,memory,my_num,month;
	struct information inf;
	struct sockaddr_in addr;
	struct player *list;
	char buf[buf_size];
	int recvstr(char *s);
	int sndstr(const char *s);
public:
	Client(int port,char *str_ip);
	void get_mynum();
	void get_market();
	void build(){sndstr(buildn);}
	void get_usr();
	void create_list();
	void buy(int num, int cost);
	void sell(int num, int cost);
	void prod();
	void get_me();
	void wait_end();
	void get_auction();
	void zero_auc();
	void end(){sndstr(endn);wait_end();get_auction();wait_end();
		get_market();get_usr();}
	struct information *get_inf(){return &inf;}
	struct player *get_list(){return list;}
	struct player *search_usr(int i);
	int get_month(){return month;}
	int max_cost(){return inf.max_cost;}
	int min_cost(){return inf.min_cost;}
	int num_prod(){return inf.num_prod;}
	int num_raw(){return inf.num_mat;}
	int num_player(){return inf.num_usr;}
	int num_active_player(){return inf.num_active_usr;}
	int mynum(){return my_num;}
	int money(int i){struct player *p=search_usr(i);return p->money;}
	int raw(int i){struct player *p=search_usr(i);return p->mat;}
	int production(int i){struct player *p=search_usr(i);return p->prod;}
	int factories(int i){struct player *p=search_usr(i);return p->fact;}
	int c_fact(int i){struct player *p=search_usr(i);
		return p->const_fact;}
	int raw_sold(int i){struct player *p=search_usr(i);
		return p->mat_auc;}
	int raw_price(int i){struct player *p=search_usr(i);
		return p->cost_mat;}
	int prod_bought(int i){struct player *p=search_usr(i);
		return p->prod_auc;}
	int prod_price(int i){struct player *p=search_usr(i);
		return p->cost_prod;}
	~Client();
};

#endif
