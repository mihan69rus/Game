#include <stdio.h>
#include <stdlib.h>
#include "structs_const.h"
#include "help_functions.h"

int length(const char *s)
{
	int i=0;
	if (s!=NULL)
	{
		while (*s!=0)
		{
			i++;
			s++;
		}
	}
	return i;
}

int cmp_s(const char *s1,const char *s2)
{
	while (*s1 && *s2)
	{
		if (*s1!=*s2)
		{
			return 0;
		}
		s1++;
		s2++;
	}
	return (*s1==0 && *s2==0);
}

int count_users(gamers *p)
{
	int i=0;
	while (p!=NULL)
	{
		p=(*p).next;
		i++;
	}
	return i;
}

char *num_in_str(int num)
{
	char *s;
	int i=0,t=num,c=0;
	if (num<0)
	{
		c=1;
		i=1;
		num=num*(-1);
	}
	if (num==0)
	{
		i=1;
	}
	while (t!=0)
	{
		t=t/10;
		i++;
	}
	s=malloc((i+1)*sizeof(*s));
	s[i]=0;
	i--;
	while (i>=0)
	{
		t=num%10;
		s[i]=t+'0';
		i--;
		num=num/10;
	}
	if (c)
	{
		*s='-';
	}
	return s;
}

int str_in_num(const char *s)
{
	int i=0;
	while (*s!=0)
	{
		i=i*10+((*s)-'0');
		s++;
	}
	return i;
}

char *take_str(char *s,int count)
{
	char *t;
	char *m;
	int i=0;
	while (s[i]!='\n' && i<count)
	{
		i++;
	}
	m=malloc((i+1)*sizeof(*m));
	i=0;
	t=s;
	while (s[i]!='\n' && i<count)
	{
		m[i]=*t;
		i++;
		t++;
	}
	m[i]=0;
	return m;
}

int check_for_work(char *s,int *count)
{
	int i=0,j=0;
	while (j<*count)
	{
		if (s[j]=='\r' && s[j+1]=='\n')
		{
			i=j;
			while (i<*count-1)
			{
				s[i]=s[i+1];
				i++;
			}
			*count=*count-1;
		}
		j++;
	}
	j=i=0;
	while (j<*count)
	{
		if (s[j]=='\n')
		{
			i++;
		}
		j++;
	}
	return i;
}

char *cut_str(char *s, int *count)
{
	int i=0,j,k;
	while (s[i]!='\n' && i<*count)
	{
		i++;
	}
	if (i==*count)
	{
		return s;
	}
	for (j=0;j<=i;j++)
	{
		k=1;
		while (k<*count)
		{
			s[k-1]=s[k];
			k++;
		}
		*count=*count-1;
	}
	if (*count==1 && s[0]=='\n')
	{
		s[0]=0;
	}
	return s;
}


int words_in_str(char *s)
{
	int i=0,j=0;
	while (*s!=0)
	{
		if ((*s==' ' || *s=='\t' || *s=='\n')  && i)
		{
			j++;
			i=0;
		}
		else if (*s!=' ' && *s!='\t' && *s!='\n')
		{
			i=1;
		}
		s++;
	}
	if (i)
	{
		j++;
	}
	return j;
}
