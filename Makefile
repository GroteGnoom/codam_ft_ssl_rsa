SRC = main.c miller.c

all:
	#`gcc -fsanitize=unsigned-integer-overflow -Wall -Werror -Wextra main.c -lm
	#https://stackoverflow.com/questions/23736507/is-there-a-reason-why-not-to-use-link-time-optimization-lto
	clang -fsanitize=integer -O3 -flto -Wall -Werror -Wextra $(SRC) -lm -o ft_ssl

test_miller:
	clang -DRSA_TEST -O3 -flto -Wall -Werror -Wextra miller.c -lm -o miller
	./miller

test_der:
	gcc -g -fsanitize=address -DDER_TEST -Wall -Wextra der.c -o der
	./der

