all:
	#`gcc -fsanitize=unsigned-integer-overflow -Wall -Werror -Wextra main.c -lm
	#https://stackoverflow.com/questions/23736507/is-there-a-reason-why-not-to-use-link-time-optimization-lto
	gcc -O3 -flto -Wall -Werror -Wextra main.c -lm
