#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/in.h>
#include <math.h>
#include "structs_const.h"
#include "help_functions.h"
#include "table_strings.h"

void init_market(stock_market *market, int num)
{
	(*market).list_fact=NULL;
	(*market).num_products=2*num;
	(*market).num_materials=2*num;
	(*market).min_cost=start_min;
	(*market).max_cost=start_max;
	(*market).condition=start_cond;
	(*market).num_users=num;
	(*market).max_users=num;
	(*market).list_usr=NULL;
}

int count_death(gamers *p)
{
	int i=0;
	while (p!=NULL)
	{
		if ((*p).condition==death)
		{
			i++;
		}
		p=(*p).next;
	}
	return i;
}

void change_level(stock_market *market)
{
	int i=-1,r,cur,sum=0,num;
	float n;
	num=(*market).num_users-count_death((*market).list_usr);
	cur=(*market).condition;
	r=1+(int)(12.0*rand()/(RAND_MAX+1.0));
	while (sum<r && i<5)
	{
		i++;
		sum=sum+level_change[cur-1][i];
	}
	(*market).condition=i+1;
	cur=(*market).condition;
	if (cur>5)
	{
		exit(1);
	}
	(*market).min_cost=levels[cur-1][1];
	(*market).max_cost=levels[cur-1][3];
	n=levels[cur-1][0]*num;
	(*market).num_materials=(int)truncf(n);
	n=levels[cur-1][2]*num;
	(*market).num_products=(int)truncf(n);
}

char *word(char *s,int *i)
{
	int k,j;
	char *str;
	while (s[*i]==' ' && s[*i]!=0)
	{
		*i=*i+1;
	}
	k=*i;
	while (s[*i]!=' ' && s[*i]!=0)
	{
		*i=*i+1;
	}
	str=malloc((*i-k+1)*sizeof(*str));
	for (j=0;j<*i-k;j++)
	{
		str[j]=s[k+j];
	}
	str[*i-k]=0;
	return str;
}

void write_all(gamers *p,const char *s)
{
	while (p!=NULL)
	{
		write((*p).fd,s,length(s));
		p=(*p).next;
	}
}

gamers *push_usr(stock_market *market, int fd, int *check)
{
	gamers *q,*p;
	int num_users=(*market).num_users;
	p=(*market).list_usr;
	if ((count_users(p)+1)>num_users || *check>0)
	{
		write(fd,err_connect,length(err_connect));
		shutdown(fd,SHUT_RDWR);
		close(fd);
		return p;
	}
	if ((count_users(p)+1)==num_users)
	{
		*check=1;
	}
	q=malloc(sizeof(*q));
	(*q).next=p;
	(*q).fd=fd;
	(*q).buffer=0;
	(*q).num=count_users(p)+1;
	(*q).money=start_money;
	(*q).products=start_products;
	(*q).factory=start_factory;
	(*q).materials=start_materials;
	(*q).condition=active;
	(*q).call_f=0;
	(*q).call_m[0]=0;
	(*q).call_m[1]=0;
	(*q).call_p[0]=0;
	(*q).call_p[1]=0;
	printf("> Connect: new user %i.\n",(*q).num);
	write(fd,hello,length(hello));
	return q;
}

factories *push_fact(int num, factories *p)
{
	factories *q;
	q=malloc(sizeof(*q));
	(*q).user=num;
	(*q).month=0;
	(*q).next=p;
	return q;
}

gamers *search_usr(gamers *p,int num)
{
	while (p!=NULL)
	{
		if ((*p).num==num)
		{
			return p;
		}
		p=(*p).next;
	}
	return p;
}

factories *delete_factory(factories *q,int num)
{
	factories *p,*f;
	f=q;
	p=NULL;
	while (f!=NULL)
	{
		if ((*f).user==num && p==NULL)
		{
			p=f;
			f=(*f).next;
			free(p);
			q=f;
			p=NULL;
		}
		else if ((*f).user==num && p!=NULL)
		{
			(*p).next=(*f).next;
			free(f);
			f=p;
		}
		else
		{
			p=f;
			f=(*f).next;
		}
	}
	return q;
}

void check_factory(stock_market *market)
{
	factories *p;
	gamers *q;
	char *s;
	p=(*market).list_fact;
	while (p!=NULL)
	{
		(*p).month=(*p).month+1;
		if ((*p).month==end_build)
		{
			q=search_usr((*market).list_usr,(*p).user);
			(*q).factory++;
			(*p).user=-1;
			p=(*p).next;
			(*market).list_fact=
				delete_factory((*market).list_fact,-1);
		}
		else if ((*p).month==befor_build)
		{
			q=search_usr((*market).list_usr,(*p).user);
			(*q).money=(*q).money-factory_cost/2;
			write((*q).fd,my_cash,length(my_cash));
			s=num_in_str((*q).money);
			write((*q).fd,s,length(s));
			free(s);
			write((*q).fd,"\n",1);
			p=(*p).next;
		}
		else
		{
			p=(*p).next;
		}
	}
}

int count_factory(int num, factories *p)
{
	int i=0;
	while (p!=NULL)
	{
		if ((*p).user==num)
		{
			i++;
		}
		p=(*p).next;
	}
	return i;
}

int count_active(gamers *p)
{
	int i=0;
	while (p!=NULL)
	{
		if ((*p).condition==active)
		{
			i++;
		}
		p=(*p).next;
	}
	return i;
}

int count_passive(gamers *p)
{
	int i=0;
	while (p!=NULL)
	{
		if ((*p).condition==passive)
		{
			i++;
		}
		p=(*p).next;
	}
	return i;
}

gamers *delete_fd(gamers *p, int fd)
{
	gamers *q,*r;
	q=p;
	r=NULL;
	while (q!=NULL && (*q).next!=NULL && (*q).num!=fd)
	{
		r=q;
		q=(*q).next;
	}
	if (r!=NULL && q!=NULL)
	{
		(*r).next=(*q).next;
	}
	if (r==NULL && q!=NULL)
	{
		p=(*q).next;
	}
	shutdown((*q).fd,SHUT_RDWR);
	close((*q).fd);
	printf(">Disconnect user %i.\n",(*q).num);
	free(q);
	return p;
}

void show_market(int fd,stock_market *market)
{
	char *s;
	write(fd,about_market,length(about_market));
	write(fd,mat,length(mat));
	s=num_in_str((*market).num_materials);
	write(fd,s,length(s));
	free(s);
	write(fd,min_mat,length(min_mat));
	s=num_in_str((*market).min_cost);
	write(fd,s,length(s));
	free(s);
	write(fd,prods,length(prods));
	s=num_in_str((*market).num_products);
	write(fd,s,length(s));
	free(s);
	write(fd,max_prod,length(max_prod));
	s=num_in_str((*market).max_cost);
	write(fd,s,length(s));
	free(s);
	write(fd,about_players,length(about_players));
	s=num_in_str(count_users((*market).list_usr));
	write(fd,s,length(s));
	free(s);
	write(fd,active_players,length(active_players));
	s=num_in_str(count_active((*market).list_usr));
	write(fd,s,length(s));
	free(s);
	write(fd,passive_players,length(passive_players));
	s=num_in_str(count_passive((*market).list_usr));
	write(fd,s,length(s));
	free(s);
	write(fd,death_players,length(death_players));
	s=num_in_str(count_death((*market).list_usr));
	write(fd,s,length(s));
	free(s);
	write(fd,"\n",1);
}

void create_factory(gamers *p, stock_market *market)
{
	char *s;
	(*market).list_fact=push_fact((*p).num,(*market).list_fact);
	(*p).money=(*p).money-factory_cost/2;
	write((*p).fd,my_cash,length(my_cash));
	s=num_in_str((*p).money);
	write((*p).fd,s,length(s));
	free(s);
	write((*p).fd,"\n",1);
}

void prod(gamers *p)
{
	char *s;
	if ((*p).call_f+1<=(*p).factory && (*p).call_f+1<=(*p).materials)
	{
		(*p).call_f++;
		write((*p).fd,future_money,length(future_money));
		s=num_in_str((*p).money-(*p).call_f*product_cost);
		write((*p).fd,s,length(s));
		free(s);
		write((*p).fd,"\n",1);
	}
	else
	{
		write((*p).fd,err_do_product,length(err_do_product));
	}
}

void show_usr(char *str, stock_market *market, int fd)
{
	int num;
	gamers *p;
	char *s;
	num=str_in_num(str);
	p=search_usr((*market).list_usr,num);
	if (p==NULL)
	{
		write(fd,err_search,length(err_search));
	}
	else
	{
		write(fd,about_usr,length(about_usr));
		write(fd,str,length(str));
		write(fd,":",1);
		write(fd,mat,length(mat));
		s=num_in_str((*p).materials);
		write(fd,s,length(s));
		free(s);
		write(fd,prods,length(prods));
		s=num_in_str((*p).products);
		write(fd,s,length(s));
		free(s);
		write(fd,usr_cash,length(usr_cash));
		s=num_in_str((*p).money);
		write(fd,s,length(s));
		free(s);
		write(fd,usr_fact,length(usr_fact));
		s=num_in_str((*p).factory);
		write(fd,s,length(s));
		free(s);
		write(fd,usr_build,length(usr_build));
		s=num_in_str(count_factory(num,(*market).list_fact));
		write(fd,s,length(s));
		free(s);
		write(fd,"\n",1);
	}
}

void show(char *s, stock_market *market, int fd)
{
	char *s1,*s2;
	int i=0;
	s1=word(s,&i);
	s2=word(s,&i);
	if (cmp_s(show_s,s1))
	{
		show_usr(s2,market,fd);
	}
	else
	{
		write(fd,err_words,length(err_words));
	}
	free(s1);
	free(s2);
}

void buy(char *s1,char *s2,gamers *p,stock_market *market)
{
	int cost,num;
	num=str_in_num(s1);
	cost=str_in_num(s2);
	if ((*p).call_m[0]!=0)
	{
		write((*p).fd,err_buy_auc,length(err_buy_auc));
	}
	else if (num>(*market).num_materials || num<0)
	{
		write((*p).fd,err_buy_num,length(err_buy_num));
	}
	else if (cost<(*market).min_cost)
	{
		write((*p).fd,err_buy_cost,length(err_buy_cost));
	}
	else
	{
		(*p).call_m[0]=num;
		(*p).call_m[1]=cost;
		write((*p).fd,buy_auc,length(buy_auc));
	}
}

void sale(char *s1,char *s2,gamers *p,stock_market *market)
{
	int cost,num;
	num=str_in_num(s1);
	cost=str_in_num(s2);
	if ((*p).call_p[0]!=0)
	{
		write((*p).fd,err_sale_auc,length(err_sale_auc));
	}
	else if (num>(*market).num_products || num<0)
	{
		write((*p).fd,err_sale_num,length(err_sale_num));
	}
	else if (cost>(*market).max_cost)
	{
		write((*p).fd,err_sale_cost,length(err_sale_cost));
	}
	else if (num>(*p).products)
	{
		write((*p).fd,err_you_sale,length(err_you_sale));
	}
	else
	{
		(*p).call_p[0]=num;
		(*p).call_p[1]=cost;
		write((*p).fd,sale_auc,length(sale_auc));
	}
}

void bid_auction(char *s,gamers *p,stock_market *market)
{
	char *s1,*s2,*s3;
	int i=0;
	s1=word(s,&i);
	s2=word(s,&i);
	s3=word(s,&i);
	if (cmp_s(buy_s,s1))
	{
		buy(s2,s3,p,market);
	}
	else if (cmp_s(sell_s,s1))
	{
		sale(s2,s3,p,market);
	}
	else
	{
		write((*p).fd,err_words,length(err_words));
	}
	free(s1);
	free(s2);
	free(s3);
}

void give_number(int fd, int num)
{
	char *s;
	s=num_in_str(num);
	write(fd,my_num,length(my_num));
	write(fd,s,length(s));
	write(fd,"\n",1);
	free(s);
}

int do_command(gamers *p, int *i, int count, stock_market *market)
{
	char *str;
	str=take_str((*p).quest,count);
	write((*p).fd,barrier,length(barrier));
	if (*i==0)
	{
		write((*p).fd,wait_game,length(wait_game));
	}
	else if (words_in_str(str)==2)
	{
		show(str,market,(*p).fd);
	}
	else if (cmp_s(my_number_s,str))
	{
		give_number((*p).fd,(*p).num);
	}
	else if (cmp_s(market_s,str))
	{
		show_market((*p).fd,market);
	}
	else if (cmp_s(help_s,str))
	{
		write((*p).fd,help_info,length(help_info));
	}
	else if ((*p).condition==passive)
	{
		write((*p).fd,wait_month_end,length(wait_month_end));
	}
	else if ((*p).condition==death)
	{
		write((*p).fd,already_death,length(already_death));
	}
	else if (words_in_str(str)>3)
	{
		write((*p).fd,err_words,length(err_words));
	}
	else if (cmp_s(end_s,str))
	{
		(*p).condition=passive;
		write((*p).fd,wait_month_end,length(wait_month_end));
	}
	else if (cmp_s(prod_s,str))
	{
		prod(p);
	}
	else if (cmp_s(build_s,str))
	{
		create_factory(p,market);
	}
	else if (words_in_str(str)==3)
	{
		bid_auction(str,p,market);
	}
	else
	{
		write((*p).fd,err_words,length(err_words));
	}
	free(str);
	write((*p).fd,barrier,length(barrier));
	return 0;
}

int ready_for_read(gamers *p,int *month,stock_market *market)
{
	int count,check=1;
	count=read((*p).fd,(*p).quest+(*p).buffer,buf_size-(*p).buffer-1);
	printf("> Read from %i.",(*p).num);
	printf(" Read %d char\n",count);
	if (count==-1)
	{
		printf("> Error: can't read.\n");
		check=-1;
	}
	else if (count==0)
	{
		check=-1;
	}
	else
	{
		count=count+(*p).buffer;
		while (check_for_work((*p).quest,&count) && check!=-1)
		{
			check=do_command(p,month,count,market);
			cut_str((*p).quest,&count);
		}
		if (count==buf_size)
		{
			check=-1;
		}
		else
		{
			(*p).buffer=count;
		}
	}
	if (check==-1)
	{
		count=0;
	}
	return check;
}

void def_call_m(gamers *p)
{
	while (p!=NULL)
	{
		(*p).call_m[0]=(*p).call_m[1]=0;
		p=(*p).next;
	}
}

void search_max(gamers *p, int *max_cost, int *num_max)
{
	*max_cost=0;
	*num_max=0;
	while (p!=NULL)
	{
		if ((*p).call_m[1]>*max_cost)
		{
			*max_cost=((*p).call_m[1]);
			*num_max=1;
		}
		else if ((*p).call_m[1]==*max_cost && *max_cost!=0)
		{
			*num_max=*num_max+1;
		}
		p=(*p).next;
	}
}

void buy_player(stock_market *m, int num, int max)
{
	int j=0,num_buy;
	char *s;
	gamers *p;
	p=(*m).list_usr;
	while (p!=NULL && j!=num)
	{
		if ((*p).call_m[1]==max)
		{
			j++;
		}
		if ((*p).next==NULL || j==num)
		{
			break;
		}
		p=(*p).next;
	}
	if ((*m).num_materials>=(*p).call_m[0] && j!=0)
	{
		num_buy=(*p).call_m[0];
		(*m).num_materials=(*m).num_materials-(*p).call_m[0];
	}
	else if (j!=0)
	{
		num_buy=(*m).num_materials;
		(*m).num_materials=0;
	}
	(*p).money=(*p).money-num_buy*(*p).call_m[1];
	(*p).materials=(*p).materials+num_buy;
	if (num_buy)
	{
		write_all((*m).list_usr,"#");
		s=num_in_str((*p).num);
		write_all((*m).list_usr,s);
		free(s);
		write_all((*m).list_usr,buy_player1);
		s=num_in_str(num_buy);
		write_all((*m).list_usr,s);
		free(s);
		write_all((*m).list_usr,buy_player2);
		s=num_in_str((*p).call_m[1]);
		write_all((*m).list_usr,s);
		free(s);
		write_all((*m).list_usr,".\n");
	}
	(*p).call_m[0]=(*p).call_m[1]=0;
}

void buy_auction(stock_market *m)
{
	int max_cost=-1,num_max=-1;
	while (max_cost!=0 && (*m).num_materials!=0)
	{
		search_max((*m).list_usr,&max_cost,&num_max);
		while (num_max!=0)
		{
			buy_player(m,rand()%num_max+1,max_cost);
			num_max--;
		}
	}
	(*m).num_materials=0;
	def_call_m((*m).list_usr);
}

void def_call_p(gamers *p)
{
	while (p!=NULL)
	{
		(*p).call_p[0]=(*p).call_p[1]=0;
		p=(*p).next;
	}
}

void search_min(gamers *p, int *min_cost, int *num_min)
{
	*min_cost=0;
	*num_min=0;
	while (p!=NULL)
	{
		if (((*p).call_p[1]<*min_cost && (*p).call_p[1]!=0) ||
			(*min_cost==0 && (*p).call_p[1]!=0))
		{
			*min_cost=((*p).call_p[1]);
			*num_min=1;
		}
		else if ((*p).call_p[1]==*min_cost && *min_cost!=0)
		{
			*num_min=*num_min+1;
		}
		p=(*p).next;
	}
}

void sale_player(stock_market *m, int num, int min)
{
	int j=0,num_sale;
	char *s;
	gamers *p;
	p=(*m).list_usr;
	while (p!=NULL && j!=num)
	{
		if ((*p).call_p[1]==min)
		{
			j++;
		}
		if ((*p).next==NULL || j==num)
		{
			break;
		}
		p=(*p).next;
	}
	if ((*m).num_products>=(*p).call_p[0] && j!=0)
	{
		num_sale=(*p).call_p[0];
		(*m).num_products=(*m).num_products-(*p).call_p[0];
	}
	else if (j!=0)
	{
		num_sale=(*m).num_products;
		(*m).num_products=0;
	}
	(*p).money=(*p).money+num_sale*(*p).call_p[1];
	(*p).products=(*p).products-num_sale;
	if (num_sale)
	{
		write_all((*m).list_usr,"#");
		s=num_in_str((*p).num);
		write_all((*m).list_usr,s);
		free(s);
		write_all((*m).list_usr,sale_player1);
		s=num_in_str(num_sale);
		write_all((*m).list_usr,s);
		free(s);
		write_all((*m).list_usr,sale_player2);
		s=num_in_str((*p).call_p[1]);
		write_all((*m).list_usr,s);
		free(s);
		write_all((*m).list_usr,".\n");
	}
	(*p).call_p[0]=(*p).call_p[1]=0;
}

void sale_auction(stock_market *m)
{
	int min_cost=-1,num_min=-1;
	while (min_cost!=0 && (*m).num_products!=0)
	{
		search_min((*m).list_usr,&min_cost,&num_min);
		while (num_min!=0)
		{
			sale_player(m,rand()%num_min+1,min_cost);
			num_min--;
		}
	}
	(*m).num_products=0;
	def_call_p((*m).list_usr);
}

void product_auction(stock_market *market)
{
	gamers *p;
	p=(*market).list_usr;
	while (p!=NULL)
	{
		(*p).materials=(*p).materials-(*p).call_f;
		(*p).products=(*p).products+(*p).call_f;
		(*p).money=(*p).money-product_cost*(*p).call_f;
		(*p).call_f=0;
		p=(*p).next;
	}
}

void taxes(stock_market *market)
{
	gamers *p;
	int sum;
	char *s;
	p=(*market).list_usr;
	while (p!=NULL)
	{
		sum=0;
		if ((*p).condition==death)
		{
			p=(*p).next;
			continue;
		}
		(*p).money=(*p).money-(*p).factory*tax_factory;
		sum=sum+(*p).factory*tax_factory;
		(*p).money=(*p).money-(*p).materials*tax_material;
		sum=sum+(*p).materials*tax_material;
		(*p).money=(*p).money-(*p).products*tax_product;
		sum=sum+(*p).products*tax_product;
		write((*p).fd,tax_month,length(tax_month));
		s=num_in_str(sum);
		write((*p).fd,s,length(s));
		write((*p).fd,"\n",1);
		free(s);
		p=(*p).next;
	}
}

void change_condition(stock_market *market)
{
	gamers *p;
	p=(*market).list_usr;
	while (p!=NULL)
	{
		if ((*p).condition==passive)
		{
			(*p).condition=active;
		}
		p=(*p).next;
	}
}

void check_usr(stock_market *market)
{
	gamers *p;
	char *s;
	p=(*market).list_usr;
	while (p!=NULL)
	{
		if ((*p).money<=0 && (*p).condition!=death)
		{
			(*p).condition=death;
			write_all((*market).list_usr,die);
			s=num_in_str((*p).num);
			write_all((*market).list_usr,s);
			free(s);
			write_all((*market).list_usr,"\n");
			(*market).list_fact=
				delete_factory((*market).list_fact,(*p).num);
		}
		p=(*p).next;
	}
}

void game_over(stock_market *market)
{
	int i,num=(*market).num_users;
	gamers *p;
	char *s;
	p=(*market).list_usr;
	if (count_death((*market).list_usr)==num)
	{
		write_all((*market).list_usr,no_win);
	}
	else
	{
		write_all((*market).list_usr,win);
		while (p!=NULL && (*p).condition==death)
		{
			p=(*p).next;
		}
		s=num_in_str((*p).num);
		write_all((*market).list_usr,s);
		free(s);
		write_all((*market).list_usr,"\n");
	}
	for (i=1;i<=num;i++)
	{
		(*market).list_fact=delete_factory((*market).list_fact,i);
		(*market).list_usr=delete_fd((*market).list_usr,i);
	}
}

void auction(stock_market *market, int *month)
{
	char *str;
	int deaths;
	write_all((*market).list_usr,end_month);
	str=num_in_str(*month);
	write_all((*market).list_usr,str);
	free(str);
	write_all((*market).list_usr,"\n");
	write_all((*market).list_usr,s_auc);
	buy_auction(market);
	sale_auction(market);
	write_all((*market).list_usr,e_auc);
	product_auction(market);
	taxes(market);
	change_condition(market);
	check_factory(market);
	check_usr(market);
	change_level(market);
	deaths=count_death((*market).list_usr);
	if (((*market).num_users-deaths)<=1)
	{
		game_over(market);
		init_market(market,(*market).max_users);
		*month=0;
	}
	else
	{
		*month=*month+1;
		write_all((*market).list_usr,start_month);
		str=num_in_str(*month);
		write_all((*market).list_usr,str);
		free(str);
		write_all((*market).list_usr,"\n");
		write_all((*market).list_usr,tax_m);
		str=num_in_str(tax_material);
		write_all((*market).list_usr,str);
		free(str);
		write_all((*market).list_usr,"\n");
		write_all((*market).list_usr,tax_p);
		str=num_in_str(tax_product);
		write_all((*market).list_usr,str);
		free(str);
		write_all((*market).list_usr,"\n");
		write_all((*market).list_usr,tax_f);
		str=num_in_str(tax_factory);
		write_all((*market).list_usr,str);
		free(str);
		write_all((*market).list_usr,"\n\n");
	}
}

int analyze_param(int argc,char **argv,int *port,int *num)
{
	if (argc!=3)
	{
		printf("> Write port.\n> Write number of users.\n");
		exit(1);
	}
	*port=str_in_num(argv[1]);
	*num=str_in_num(argv[2]);
	return 0;
}

void error_after_select()
{
	if (errno!=EINTR)
	{
		printf("> Error: in select");
	}
	else
	{
		printf("> Got a signal.\n");
	}
}

void check_err_in_socket(int ls)
{
	if (ls==-1)
	{
		perror("socket");
		printf("Error: can't create socket.\n");
		exit(1);
	}
}

void check_err_in_bind()
{
	perror("bind");
	printf("Error: error in bind.\n");
	exit(1);
}

gamers *check_fd(stock_market *market, int *month,fd_set *readfds)
{
	gamers *p;
	int count,fd;
	p=(*market).list_usr;
	while ((*market).list_usr!=NULL && p!=NULL)
	{
		if (FD_ISSET((*p).fd,readfds))
		{
			count=ready_for_read(p,month,market);
			if (count==-1)
			{
				fd=(*p).num;
				p=(*p).next;
				(*market).list_usr=
					delete_fd((*market).list_usr,fd);
				(*market).num_users=
					count_users((*market).list_usr);
			}
		}
		if (p!=NULL && count!=-1)
		{
			p=(*p).next;
		}
		count=0;
	}
	if (count_active((*market).list_usr)==0)
	{
		auction(market,month);
	}
	return (*market).list_usr;
}

gamers *check_new(stock_market *market,
	int *month,
	int ls,
	struct sockaddr_in *addr)
{
	int i=*month,fd;
	socklen_t addrlen=sizeof(addr);
	fd=accept(ls,(struct sockaddr*)addr,&addrlen);
	(*market).list_usr=push_usr(market,fd,month);
	if (*month==1 && i==0)
	{
		write_all((*market).list_usr,game_start);
		write_all((*market).list_usr,err_words);
	}
	return (*market).list_usr;
}

int check_max_fd(gamers *p, int max_fd)
{
	if ((*p).fd>max_fd)
	{
		max_fd=(*p).fd;
	}
	return max_fd;
}

int main(int argc, char **argv)
{
	int num_users=0,ls,month=0,max_fd,res,port=0;
	struct sockaddr_in addr;
	gamers *p;
	stock_market market;
	struct timeval time;
	fd_set readfds;
	printf("Start working...\n");
	analyze_param(argc,argv,&port,&num_users);
	init_market(&market,num_users);
	ls=socket(AF_INET,SOCK_STREAM,0);
	check_err_in_socket(ls);
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	if (-1 == bind(ls,(struct sockaddr*)&addr,sizeof(addr)))
	{
		check_err_in_bind();
	}
	listen(ls,num_users);
	for (;;)
	{
		FD_ZERO(&readfds);
		FD_SET(ls,&readfds);
		max_fd=ls;
		p=market.list_usr;
		while (market.list_usr!=NULL && p!=NULL)
		{
			FD_SET((*p).fd,&readfds);
			max_fd=check_max_fd(p,max_fd);
			p=(*p).next;
		}
		FD_SET((int)0,&readfds);
		time.tv_sec=6;
		time.tv_usec=0;
		res=select(max_fd+1,&readfds,NULL,NULL,&time);
		if (res<0)
		{
			error_after_select();
			continue;
		}
		if (res==0)
		{
			printf(">Time\n");
			continue;
		}
		if (FD_ISSET(ls,&readfds))
		{
			market.list_usr=check_new(&market,&month,ls,&addr);
			continue;
		}
		if (FD_ISSET((int)0,&readfds))
		{
			int s;
			s=getchar();
			printf("Da %i\n",s);
			continue;
		}
		market.list_usr=check_fd(&market,&month,&readfds);
	}
	return 0;
}
