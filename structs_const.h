#ifndef SENTRY
#define SENTRY

enum {SepStat=0,ErrStat=1,
	KeyStat=2,FunStat=3,
	VarStat=4,LabStat=5,
	IntStat=6,StrStat=7};
enum {duo_op=10};
enum {num_space_chars=3};
enum {num_arith_chars=19};
enum {death=-1,passive=0,active=1};
enum {start_money=10000};
enum {start_materials=4};
enum {start_products=2};
enum {start_factory=2};
enum {factory_cost=5000};
enum {product_cost=2000};
enum {product_cost_m=1};
enum {tax_material=300};
enum {tax_product=500};
enum {tax_factory=1000};
enum {buf_size=256};
enum {start_min=500};
enum {start_max=5500};
enum {start_cond=3};
enum {befor_build=4};
enum {end_build=5};

typedef struct file_des
{
        int fd;
        int buffer;
        int num;
        int money;
        int materials;
        int products;
        int factory;
	int call_m[2];
	int call_p[2];
	int call_f;
        int condition;
        char quest[buf_size];
        struct file_des *next;
} gamers;

typedef struct factories
{
	int user;
	int month;
	struct factories *next;
} factories;

typedef struct stock_market
{
	gamers *list_usr;
	factories *list_fact;
        int min_cost;
        int max_cost;
        int num_materials;
        int num_products;
	int condition;
	int num_users;
	int max_users;
} stock_market;

struct information
{
        int num_mat;
        int min_cost;
        int num_prod;
        int max_cost;
        int num_usr;
	int num_active_usr;
};

struct player
{
        int num;
        int mat;
        int money;
        int prod;
        int fact;
        int const_fact;
	int cost_mat;
	int cost_prod;
	int mat_auc;
	int prod_auc;
        struct player *next;
};

struct lexeme
{
        int type;
        int string;
        int length;
        char *lex;
        struct lexeme *next;
};


#endif
