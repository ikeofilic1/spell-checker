#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "spell.h"

#define MAX_LEN 101

/*  Suggestions
- When you work with 2D arrays, be careful. Either manage the memory yourself, or
work with local 2D arrays. Note C99 allows parameters as array sizes as long as 
they are declared before the array in the parameter list. See: 
https://www.geeksforgeeks.org/pass-2d-array-parameter-c/

Worst case time complexity to compute the edit distance from T test words
 to D dictionary words where all words have length MAX_LEN:
Student answer:  Theta(T*M*MAX_LEN^2)


Worst case to do an unsuccessful binary search in a dictionary with D words, when 
all dictionary words and the searched word have length MAX_LEN 
Student answer:  Theta(log_2(D)) if you use your own compare: Theta(log_2(D)*MAX_LEN)
*/


/* You can write helper functions here */
void trim(char *word) {
	int idx = strlen(word) - 1;
	if (word[idx] == '\n') word[idx] = '\0';
}

int compare(const void *a, const void *b) {
  return strcmp((char *)a, (char *)b);
}

char* to_lower_string(char *string) {
    int sz = strlen(string) + 1;
    char *lowered = malloc(sz*sizeof(char));

	for (int i = 0; i < sz; ++i) {
		lowered[i] = tolower(string[i]);
	}
    return lowered;
}

void printc_repeat(char c, int times) {
	putchar('\n');
	for (int i = 0; i < times; ++i) putchar(c);
	putchar('\n');
}

void add_string(char ***arr, char *string, int *cap, int *size) {
	if (*cap == *size) {
		*cap *= 2;
		*arr = realloc(*arr, (*cap) *sizeof(char**));
	}
	(*arr)[*size] = string;
	*size = *size + 1;
}

void free_arr(char **arr, int size) {
	for (int i = 0; i < size; ++i) {
		free(arr[i]);
	}
	free(arr);
}

void set_word(char **similar, int size, char *word) {
	int choice = 0;

	do  {
		char c;
		printf("Enter your choice: ");
		scanf("%d%c", &choice, &c);
	} while (choice < -1 || choice > size);

	switch (choice) {
		case -1:
			printf("Enter correct word: ");
			fgets(word, MAX_LEN, stdin);
			trim(word);
		case 0:
			break;

		default:
			strcpy(word, similar[choice-1]);
	}
}

void word_menu(char dict[][MAX_LEN], int dict_size, char *lower, char *word) {
	int size = 0, cap = 5, count = 0;
	char **similar = malloc(cap*sizeof(char*));

	int min = edit_distance(lower, dict[dict_size/2], 0); //or any value you want to be max ED;
	for (int i = 0; i < dict_size; ++i) {
		int bias = strlen(dict[i]) - strlen(word);

		if (bias > min || bias < -min) continue;

		int dist = edit_distance(lower, dict[i], 0);

		if (dist < min) {
			size = 0;
			min = dist;
		}

		if (dist == min)
			add_string(&similar, dict[i], &cap, &size);			
		count++;
	}

	printf("\n-1 - Type correction\n");
	printf(" 0 - Leave word as is\n");
	printf("     Minimum distance: %d (compared edit distances %d)\n", min, count);
	printf("     Words that give minimum distance:\n");
	
	for(int i = 0; i < size; ++i) {
		printf(" %d - %s\n", i+1, similar[i]);
	}

	set_word(similar, size, word);
	free(similar);
}

void print_table(int rows, int cols, int table[rows][cols], char *vert, char *horiz) {
	printf("   |   |");
	for (int i = 0; i < strlen(horiz); ++i) 
		printf("%3c|", horiz[i]); 
	printc_repeat('-', 4*(cols+1));
	
	for (int i = 0; i < rows; ++i) {
		printf("%3c|", i ? vert[i-1] : ' ');
		for (int j = 0; j < cols; ++j) {
			printf("%3d|", table[i][j]);
		}
		printc_repeat('-', 4*(cols+1));
	}
}

int find_string(char array[][MAX_LEN], int end, char *val, int printOn) {
	int count = 0, ans = -1, start = 0;
	if (printOn == 1) printf("Binary search for: %s\n", val);
	while (start <= end) {
		int middle = (end + start)/2;
		count++;

		if (printOn == 1) printf("dict[%d] = %s\n", middle, array[middle]);
		int cmp = strcmp(array[middle], val);
		if (cmp == 0) {
			ans = middle;
			break;
		}
		if (cmp < 0) start = middle + 1;
		else end = middle - 1;
	}
	if (printOn == 1) printf("%s\n", ans == -1 ? "Not found" : "Found");
	printf("Search ran %d times\n", count);
    return ans;
}

void put_words(FILE *input, FILE *outf, char dict[][MAX_LEN], int dict_size, int printOn) {    
    char word[MAX_LEN];
    int c, cur_size = 0;
    const char *seperators = ", .!?"; //Any sep

    do {
        c = fgetc(input);
        if (c == '\0') continue; 

        if (strchr(seperators, c) == NULL && c != EOF) {
            word[cur_size++] = c;
            continue;
        }

        if (cur_size != 0) {
            word[cur_size] = '\0';
            char *lower = to_lower_string(word);

            printf("\n\n---> |%s|\n", word);
            int found = find_string(dict, dict_size - 1, lower, printOn);

            if (found == -1) 
                word_menu(dict, dict_size, lower, word);
            else printf("   - OK\n");

            fprintf(outf, "%s", word);

            free(lower);
            cur_size = 0;
        }

        if (c != EOF) fputc(c, outf);

    } while (!feof(input));
}


/*
Parameters:
first_string - pointer to the first string (displayed vertical in the table)
second_string - pointer to the second string (displayed horizontal in the table)
print_table - If 1, the table with the calculations for the edit distance will be printed.
              If 0 (or any value other than 1)it will not print the table
(See sample run)
Return:  the value of the edit distance (e.g. 3)
*/
int edit_distance(char * first_string, char * second_string, int printOn){
	int rows = strlen(first_string) + 1, cols = strlen(second_string) + 1;
	int table[rows][cols];

	for (int i = 0; i < cols; ++i) table[0][i] = i;
	for (int j = 0; j < rows; ++j) table[j][0] = j;	
	
	for (int i = 1; i < rows; ++i) {
		for (int j = 1; j < cols; ++j) {
			int min = table[i-1][j-1];

			if (first_string[i-1] != second_string[j-1]) {
				if (table[i][j-1] < min) min = table[i][j-1];
				if (table[i-1][j] < min) min = table[i-1][j];
				++min;
			}
			table[i][j] = min;
		}
	}	

	if (printOn == 1) print_table(rows, cols, table, first_string, second_string);
	return table[rows-1][cols-1];  // replace this line with your code
}
	  

/*
Parameters:
testname - string containing the name of the file with the paragraph to spell check, includes .txt e.g. "text1.txt" 
dictname - name of file with dictionary words. Includes .txt, e.g. "dsmall.txt"
printOn - If 1 it will print EXTRA debugging/tracing information (in addition to what it prints when 0):
			 dictionary as read from the file	
			 dictionary AFTER sorting in lexicographical order
			 for every word searched in the dictionary using BINARY SEARCH shows all the "probe" words and their indices indices
			 See sample run.
	      If 0 (or anything other than 1) does not print the above information, but still prints the number of probes.
		     See sample run.
*/

void spell_check(char * testname, char * dictname, int printOn){
	FILE *txtf = fopen(testname, "r"), *dictf = fopen(dictname, "r");

	if (!txtf || !dictf) {
		perror("Error opening file");
		if (txtf) fclose(txtf);
		if (dictf) fclose(dictf);
		return;
	}

	int N = 0;
	fscanf(dictf, "%d", &N);
	char dict[N][MAX_LEN];

	for (int i = 0; i < N && !feof(dictf); ++i) {
		fscanf(dictf, "%s", dict[i]);
	}
	if (printOn == 1) {
        printf("\nOriginal dictionary: \n");
		for (int i = 0; i < N; ++i) printf("%4d. %s\n", i, dict[i]);
	}
	qsort(dict, N, MAX_LEN*sizeof(char), compare);

	if (printOn == 1) {
        printf("\nSorted dictionary: \n");
		for (int i = 0; i < N; ++i) printf("%4d. %s\n", i, dict[i]);
	}

    char out[strlen(testname) + 5]; 
    sprintf(out, "out_%s", testname);
    FILE *outf = fopen(out, "w");

    put_words(txtf, outf, dict, N, printOn);
    
	fclose(txtf);
    //fclose(outf);
    fclose(dictf);
}

// ftell, seek to end, ftell again, use that to get size, allocate memory to contain entire file (static or malloc), strpbrk the string