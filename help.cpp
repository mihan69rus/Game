#include <stdio.h>
#include <stdlib.h>
#include "structs_const.h"
#include "help.h"

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
			return 0;
		s1++;
		s2++;
	}
	return (*s1==0 && *s2==0);
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

int num_from_str(const char *s,int *num)
{
	int i=0,res=0,flag=1,check=1;
	while (s[i]!=0)
	{
		if ((s[i]>='0' && s[i]<='9') || s[i]=='-')
		{
			*num=*num-check;
			check=0;
			if (*num==0)
			{
				if (s[i]=='-')
					flag=-1;
				else
					res=res*10+(s[i]-'0');
			}
		}
		else
		{
			if (*num==0)
				return res*flag;
			check=1;
			flag=1;
		}
		i++;
	}
	*num=-1;
	return res;
}

int search_substr(const char *s1,const char *s2)
{
	int i=0,j=0;
	while (s1[i]!=0)
	{
		if (s1[i]==s2[j])
		{
			i++;
			j++;
			if (s2[j]==0)
				return 1;
		}
		else
		{
			i++;
			j=0;
		}
	}
	return 0;
}

char *num_in_str(int num)
{
	char *s1=new char[buf_size/4];
	char *s2=new char[buf_size/4];
	int i=0,j=0;
	while (num>=0)
	{
		i=num%10;
		s1[j]=i+'0';
		num=(num-i)/10;
		j++;
		if (num==0)
			num=-1;
	}
	s1[j]=0;
	for (i=0;i<=(j-1);i++)
		s2[i]=s1[j-1-i];
	s2[j]=0;
	delete [] s1;
	return s2;
}

int min(const int a,const int b)
{
	if (a<b)
		return a;
	return b;
}
