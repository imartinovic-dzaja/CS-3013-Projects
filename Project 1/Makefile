prolific: prolific.c read_seed.c get_random_number.c 
	gcc -o prolific prolific.c read_seed.c get_random_number.c  -I. 

generation: generation.c read_seed.c get_random_number.c
	gcc -o generation  generation.c read_seed.c get_random_number.c -I.

explorer: explorer.c read_seed.c get_random_number.c
	gcc -o explorer  explorer.c read_seed.c get_random_number.c -I.

slug: slug.c  read_seed.c get_random_number.c
	gcc -o slug  slug.c read_seed.c get_random_number.c -I.

slugrace: slugrace.c  
	gcc -o slugrace slugrace.c -I.

all: prolific.c generation.c explorer.c slug.c read_seed.c get_random_number.c
	gcc -o prolific prolific.c read_seed.c get_random_number.c -I.
	gcc -o generation  generation.c read_seed.c get_random_number.c -I.
	gcc -o explorer  explorer.c read_seed.c get_random_number.c -I.
	gcc -o slug  slug.c read_seed.c get_random_number.c -I.
	gcc -o slugrace slugrace.c -I.
	
	
.PHONY: clean

clean:
	rm -f prolific generation explorer slug slugrace

