#include "table_strings.h"
#include "structs_const.h"

const int level_change[5][5]=
{
        { 4, 4, 2, 1, 1},
        { 3, 4, 4, 1, 1},
        { 1, 3, 4, 3, 1},
        { 1, 1, 3, 4, 3},
        { 1, 1, 2, 4, 4}
};

const float levels[5][4]=
{
        { 1.0, 800, 3.0, 6500},
        { 1.5, 650, 2.5, 6000},
        { 2.0, 500, 2.0, 5500},
        { 2.5, 400, 1.5, 5000},
        { 3.0, 300, 1.0, 4500}
};

const int ops[duo_op]={'+','-','*','/','%','&','|','#','>','<'};
const int space_chars[num_space_chars]={' ','\t','\n'};
const int arith_chars[num_arith_chars]={
	'+','-','*','/','%','!','#','&','|','{','}',
	'(',')','[',']','\\',',',';',':'};

const char hello[]=">Hello. Game will be started, when all players connect.\n";
const char err_connect[]=">Sorry, but game has already started\n";
const char wait_start[]=">Game didn't start. Please, wait all players\n";
const char about_market[]=">Condition of market:";
const char mat[]="\n#  Number of materials: ";
const char min_mat[]="\n#  Minimum cost of materials: ";
const char prods[]="\n#  Number of products: ";
const char max_prod[]="\n#  Maximum cost of products: ";
const char about_players[]="\n#Count of players: ";
const char active_players[]="\n#Count of active players: ";
const char death_players[]="\n#Count of dead players: ";
const char passive_players[]="\n#Count of passive players: ";
const char mat_b[]="#  Number of materials: ";
const char min_mat_b[]="#  Minimum cost of materials: ";
const char prods_b[]="#  Number of products: ";
const char max_prod_b[]="#  Maximum cost of products: ";
const char about_players_b[]="#Count of players: ";
const char active_players_b[]="#Count of active players: ";
const char death_players_b[]="#Count of dead players: ";
const char passive_players_b[]="#Count of passive players: ";
const char help_info[]=">List of commands:\n>  market\n> "
                        " show <player>\n>  my_number\n>"
                        "  prod\n>  build\n>  buy "
                        "<number> <money>\n>  sell <number> <mone"
                        "y>\n>  end\n>  help\n";
const char start_month[]="\n%Month: ";
const char my_cash[]=">Take money for factory. Your money: ";
const char my_num[]="#Your number: ";
const char future_money[]=">You will have money: ";
const char err_do_product[]=">You can't do more products.\n";
const char err_words[]=">Write 'help' to view commands list.\n";
const char err_search[]=">No player with this number.\n";
const char about_usr[]=">Player ";
const char usr_cash[]="\n#  Player's money: ";
const char usr_fact[]="\n#  Working factories: ";
const char usr_build[]="\n#  Constructing factories: ";
const char usr_cash_b[]="#  Player's money: ";
const char usr_fact_b[]="#  Working factories: ";
const char usr_build_b[]="#  Constructing factories: ";
const char err_buy_num[]=">Market haven't this number of materials.\n";
const char err_buy_cost[]=">Minimum cost more than you suggested.\n";
const char buy_auc[]=">You will take part in auction materials.\n";
const char err_buy_auc[]=">You have already take part in auction material\n";
const char err_sale_num[]=">Market don't buy this number of products.\n";
const char err_sale_cost[]=">Maximum cost less than you suggested.\n";
const char sale_auc[]=">You will take part in auction products.\n";
const char err_sale_auc[]=">You have already take part in auction product\n";
const char wait_month_end[]=">Please, wait for the end of this month.\n";
const char wait_game[]=">Game don't start. Please wait, other players.\n";
const char end_month[]="%End of month ";
const char buy_player1[]=" player bought ";
const char buy_player2[]=" materials, each for ";
const char sale_player1[]=" player sold ";
const char sale_player2[]=" products, each for ";
const char err_you_sale[]=">You haven't so many products.\n";
const char die[]="#Death of player ";
const char game_start[]=">Starting the game...\n%Month: 1\n";
const char win[]=">Win player number ";
const char no_win[]=">No winner today:)\n";
const char barrier[]="~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
const char s_auc[]="s~~~~~~~~~~~~~~start auction~~~~~~~~~~~~~~~~~~s\n";
const char e_auc[]="e~~~~~~~~~~~~~~~end auction~~~~~~~~~~~~~~~~~~~e\n\n";
const char e_aucn[]="e~~~~~~~~~~~~~~~end auction~~~~~~~~~~~~~~~~~~~e\n";
const char tax_month[]="#Taxes for month: ";
const char already_death[]=">You have already dead.\n";
const char tax_f[]=">Taxes for factory: ";
const char tax_m[]=">Taxes for material: ";
const char tax_p[]=">Taxes for product: ";
const char prod_s[]="prod";
const char prodn[]="prod\n";
const char market_s[]="market";
const char marketn[]="market\n";
const char my_number_s[]="my_number";
const char my_numbern[]="my_number\n";
const char help_s[]="help";
const char helpn[]="help\n";
const char sell_s[]="sell";
const char buy_s[]="buy";
const char endn[]="end\n";
const char end_s[]="end";
const char build_s[]="build";
const char buildn[]="build\n";
const char show_s[]="show";
