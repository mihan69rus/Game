help_functions.o: help_functions.c help_functions.h structs_const.h
	gcc -Wall -g -c help_functions.c -o help_functions.o

table_strings.o: table_strings.c table_strings.h structs_const.h
	g++ -Wall -g -c table_strings.c -o table_strings.o

Game: Game.c help_functions.o table_strings.o
	gcc -Wall -g Game.c help_functions.o table_strings.o -lm -o Game

help.o: help.cpp help.h structs_const.h
	g++ -Wall -g -c help.cpp -o help.o

help_rob.o: help_rob.cpp help_rob.h
	g++ -Wall -g -c help_rob.cpp -o help_rob.o

help_lex.o: help_lex.cpp help_lex.h table_strings.h
	g++ -Wall -g -c help_lex.cpp -o help_lex.o

Robot: Robot.cpp help.o help_rob.o table_strings.o structs_const.h
	g++ -Wall -g Robot.cpp help.o help_rob.o table_strings.o -o Robot

Robot2.0.o: Robot2.0.cpp
	g++ -Wall -g -c Robot2.0.cpp -o Robot2.0.o

lexis.o: lexis.cpp help_lex.o table_strings.o
	g++ -Wall -g -c lexis.cpp -o lexis.o

Rpn.o: Rpn.cpp Rpn.h
	g++ -Wall -g -c Rpn.cpp -o Rpn.o

Bot: syntax.cpp lexis.o help.o help_lex.o table_strings.o Rpn.o help_rob.o Robot2.0.o
	g++ -Wall -g syntax.cpp lexis.o help.o help_lex.o help_rob.o table_strings.o Rpn.o Robot2.0.o -o Bot

