#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "help.h"
#include "help_rob.h"
#include "structs_const.h"
#include "table_strings.h"

enum {errrd=-1};

class Client
{
	int sockfd,mem_ptr,memory,my_num,month;
	struct information inf;
	struct sockaddr_in addr;
	struct player *list,me;
	char buf[buf_size];
	int recvstr(char *s);
	int sndstr(const char *s);
public:
	Client(int port,char *str_ip);
	void get_mynum();
	void get_market();
	void end(){sndstr(endn);};
	void get_usr();
	void create_list();
	void buy(int num, int cost);
	void sell(int num, int cost);
	void prod();
	void get_me();
	void wait_end();
	void get_auction();
	void zero_auc();
	struct information *get_inf(){return &inf;}
	struct player *get_list(){return list;}
	struct player *get_field_me(){return &me;}
	int get_month(){return month;}
	int max_cost(){return inf.max_cost;}
	int min_cost(){return inf.min_cost;}
	int get_myprod(){return me.prod;}
	int get_money(){return me.money;}
	int num_prod(){return inf.num_prod;}
	~Client();
};

Client::~Client()
{
	list=delete_list(list);
	shutdown(sockfd,SHUT_RDWR);
	close(sockfd);
}

Client::Client(int port,char *str_ip)
{
	mem_ptr=memory=my_num=month=0;
	list=NULL;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if (sockfd==-1)
	{
		printf("Error client:Can't create socket\n");
		exit(1);
	}
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	if (!inet_aton(str_ip,&(addr.sin_addr)))
	{
		printf("Error client:Can't find IP\n");
		exit(1);
	}
	if (0!=connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)))
	{
		printf("Error client:Can't connect \n");
		exit(1);
	}
}

void Client::wait_end()
{
	int i=1,num=1;
	char string[buf_size];
	while (i)
	{
		recvstr(string);
		if (string[0]=='%')
			i=0;
	}
	month=num_from_str(string,&num);
}

void Client::get_me()
{
	int j=2,k=1;
	char *s;
	char str[buf_size];
	sndstr(show_s);
	sndstr(" ");
	s=num_in_str(my_num);
	me.num=my_num;
	sndstr(s);
	delete [] s;
	sndstr("\n");
	while (j)
	{
		k=1;
		recvstr(str);
		if (cmp_s(str,barrier))
			j--;
		else if (search_substr(str,mat_b))
			me.mat=num_from_str(str,&k);
		else if (search_substr(str,prods_b))
			me.prod=num_from_str(str,&k);
		else if (search_substr(str,usr_cash_b))
			me.money=num_from_str(str,&k);
		else if (search_substr(str,usr_fact_b))
			me.fact=num_from_str(str,&k);
		else if (search_substr(str,usr_build_b))
			me.const_fact=num_from_str(str,&k);
	}
	me.next=NULL;
}

void Client::create_list()
{
	int i=1;
	for (i=1;i<=inf.num_usr;i++)
	{
		if (i!=my_num)
		{
			list=push(list);
			list->num=i;
		}
	}
}

void Client::get_usr()
{
	int j=0,k=0;
	char str[buf_size];
	struct player *p;
	char *s;
	p=list;
	while (p!=NULL)
	{
		j=2;
		sndstr(show_s);
		sndstr(" ");
		s=num_in_str(p->num);
		sndstr(s);
		delete [] s;
		sndstr("\n");
		while (j)
		{
			k=1;
			recvstr(str);
			if (cmp_s(str,barrier))
				j--;
			else if (search_substr(str,mat_b))
				p->mat=num_from_str(str,&k);
			else if (search_substr(str,prods_b))
				p->prod=num_from_str(str,&k);
			else if (search_substr(str,usr_cash_b))
				p->money=num_from_str(str,&k);
			else if (search_substr(str,usr_fact_b))
				p->fact=num_from_str(str,&k);
			else if (search_substr(str,usr_build_b))
				p->const_fact=num_from_str(str,&k);
		}
		p=p->next;
	}
}

void Client::get_market()
{
	int i=2,k=1;
	char str[buf_size];
	sndstr(marketn);
	while (i)
	{
		recvstr(str);
		k=1;
		if (cmp_s(str,barrier))
			i--;
		else if (search_substr(str,mat_b))
			inf.num_mat=num_from_str(str,&k);
		else if (search_substr(str,min_mat_b))
			inf.min_cost=num_from_str(str,&k);
		else if (search_substr(str,prods_b))
			inf.num_prod=num_from_str(str,&k);
		else if (search_substr(str,max_prod_b))
			inf.max_cost=num_from_str(str,&k);
		else if (search_substr(str,about_players_b))
			inf.num_usr=num_from_str(str,&k);
	}
}

void Client::get_mynum()
{
	char str[buf_size];
	int i=2,num=1;
	sndstr(my_numbern);
	while (i)
	{
		recvstr(str);
		if (cmp_s(str,barrier))
			i--;
		else if (str[0]=='#')
			my_num=num_from_str(str,&num);
	}
}

int Client::recvstr(char *s)
{
	int j=0;
	while (1)
	{
		if (memory<=0)
		{
			memory=read(sockfd,buf,buf_size);
			mem_ptr=0;
		}
		if (memory==-1)
		{
			memory=0;
			mem_ptr=0;
			return errrd;
		}
		else if (memory==0)
		{
			int i=0;
			while (search_substr(&(buf[i]),"Win"))
				i++;
			if (i!=0)
			{
				int num=1;
				int usr=num_from_str(&(buf[i]),&num);
				if (usr==my_num)
					printf("I win\n");
				else
					printf("I loose\n");
			}
			printf("Disconnect from server\n");
			exit(1);
		}
		else
		{
			int i=0,first=mem_ptr,end=mem_ptr+memory-1;
			for (i=first;i<=end;i++)
			{
				s[j]=buf[i];
				memory--;
				mem_ptr++;
				if (buf[i]=='\n')
				{
					s[j]='\n';
					s[j+1]=0;
					return memory;
				}
				j++;
			}
		}
	}
}

int Client::sndstr(const char *s)
{
	return write(sockfd,s,length(s));
}

void Client::prod()
{
	if (me.money-product_cost>0)
		sndstr(prodn);
}

void Client::buy(int num, int cost)
{
	char *s1,*s2;
	char str[buf_size];
	int j=2;
	s1=num_in_str(num);
	s2=num_in_str(cost);
	sndstr(buy_s);
	sndstr(" ");
	sndstr(s1);
	sndstr(" ");
	sndstr(s2);
	sndstr("\n");
	delete [] s1;
	delete [] s2;
	while (j)
	{
		recvstr(str);
		if (cmp_s(str,barrier))
			j--;
	}
}

void Client::sell(int num, int cost)
{
	char *s1,*s2;
	char str[buf_size];
	int j=2;
	s1=num_in_str(num);
	s2=num_in_str(cost);
	sndstr(sell_s);
	sndstr(" ");
	sndstr(s1);
	sndstr(" ");
	sndstr(s2);
	sndstr("\n");
	delete [] s1;
	delete [] s2;
	while (j)
	{
		recvstr(str);
		if (cmp_s(str,barrier))
			j--;
	}
}

void Client::zero_auc()
{
	struct player *p;
	p=list;
	while (p!=NULL)
	{
		p->prod_auc=0;
		p->mat_auc=0;
		p->cost_prod=0;
		p->cost_mat=0;
		p=p->next;
	}
	me.prod_auc=0;
	me.mat_auc=0;
	me.cost_prod=0;
	me.cost_mat=0;
}

void Client::get_auction()
{
	struct player *p;
	int j=2,num=1,i=0;;
	char str[buf_size];
	zero_auc();
	while (j)
	{
		num=1;
		recvstr(str);
		if (cmp_s(str,s_auc) || cmp_s(str,e_aucn))
		{
			j--;
		}
		else
		{
			i=num_from_str(str,&num);
			p=list;
			while (p!=NULL && p->num!=i && my_num!=i)
				p=p->next;
			num=2;
			if (my_num==i && search_substr(str,"bought"))
			{
				i=num_from_str(str,&num);
				me.mat_auc=i;
				num=3;
				i=num_from_str(str,&num);
				me.cost_mat=i;
			}
			else if (my_num==i && search_substr(str,"sold"))
			{
				i=num_from_str(str,&num);
				me.prod_auc=i;
				num=3;
				i=num_from_str(str,&num);
				me.cost_prod=i;
			}
			else if (search_substr(str,"bought"))
			{
				i=num_from_str(str,&num);
				p->mat_auc=i;
				num=3;
				i=num_from_str(str,&num);
				p->cost_mat=i;
			}
			else if (search_substr(str,"sold"))
			{
				i=num_from_str(str,&num);
				p->prod_auc=i;
				num=3;
				i=num_from_str(str,&num);
				p->cost_prod=i;
			}
		}
	}
}

int main(int argc, char **argv)
{
	if (argc!=3)
	{
		printf (">Write IP and port\n");
		exit(1);
	}
	int port=str_in_num((const char *)argv[2]);
	int num=0;
	Client Me(port,argv[1]);
	Me.wait_end();
	Me.get_mynum();
	Me.get_market();
	Me.create_list();
	for (;;)
	{
		Me.get_me();
		if (Me.get_money()>0)
		{
			Me.get_market();
			Me.get_usr();
			Me.prod();
			Me.prod();
			Me.buy(2,Me.min_cost());
			num=min(Me.get_myprod(),Me.num_prod());
			Me.sell(num,Me.max_cost());
			Me.end();
		}
		Me.wait_end();
		printf("Month:%i\n",Me.get_month());
		Me.get_auction();
		write_market(Me.get_inf());
		write_list(Me.get_list());
		write_me(Me.get_field_me());
		Me.wait_end();
	}
	return 0;
}
