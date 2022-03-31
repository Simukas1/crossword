/*
* Author: Simonas Jarukaitis
* email: simasjarukaitis@gmail.com
* 
* input: first file contains crossword labyrinth, second file contains words, available for the crossword.
* 
* output: if program can solve crossword, it will output solved crossword.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void get_dimensions(int *i, int* j);
void get_word_dimensions(int* i, int* j);
void get_words(char** words, int* words_count);
void get_labyrinth(char** labyrinth, int lab_i, int lab_j);
void to_lower_all_words(char** words, int words_count, int words_j);
void remove_duplicates(char** words, int* words_count, int sz);
char get_second_char(char** labyrinth, int lab_i, int lab_j);
int* get_word_sizes(char** words, int words_count, int words_j);
int get_cell_size_from_top(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c);
int check_if_i_can_fill_from_top(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, int size, char* word);
int check_if_i_can_fill_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, int size, char* word);
void fill_cell_from_top(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, char* word, int size);
int get_cell_size_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c);
void fill_cell_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, char* word, int size);
void return_to_prev_version_from_top(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, int size);
void return_to_prev_version_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, int size);
void construt_crossword(char** labyrinth, char** filled, int lab_i, int lab_j, int prev_i, int prev_j, char c, char** words, int word_count, int* words_char_count, int* used, int* found);
void construt_crossword_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int prev_i, int prev_j, char c, char** words, int word_count, int* words_char_count, int* used, int* found);
void output_crossword(char** labyrinth, int lab_i, int lab_j);
int check_if_good(char** labyrinth, char** filled, int lab_i, int lab_j, char c);

int main()
{
	int lab_i=0, lab_j=0, words_i, words_j, words_count=0;
	get_dimensions(&lab_i, &lab_j);
	char** labyrinth;
	labyrinth = (char**)malloc(sizeof(char*) * lab_i);
	if (labyrinth == NULL) {
		printf("Could not allocate memory \n");
		return 0;
	}
	for (int i = 0; i < lab_i; ++i) {
		labyrinth[i] = (char*)malloc(sizeof(char) * lab_j);
	}
	char** words;
	get_word_dimensions(&words_i, &words_j);
	words = (char**)malloc(sizeof(char*) * words_i);
	if (words == NULL) {
		printf("Could not allocate memory \n");
		return 0;
	}
	for (int i = 0; i < words_i; i++) {
		words[i] = (char*)malloc(sizeof(char) * words_j);
	}
	get_words(words, &words_count);
	get_labyrinth(labyrinth, lab_i, lab_j);
	to_lower_all_words(words, words_count, words_j);
	remove_duplicates(words, &words_count, words_count);
	char first_char = labyrinth[0][0], second_char=get_second_char(labyrinth, lab_i, lab_j);
	//now lets check all "boxes", who are first_char type
	int* word_char_count = (int*)malloc(sizeof(int) * (words_j+1));
	word_char_count = get_word_sizes(words, words_count, words_j);
	char** filled;
	filled = (char**)malloc(sizeof(char*) * lab_i);
	if (filled == NULL) {
		printf("Could not allocate memory \n");
		return 0;
	}
	for (int i = 0; i < lab_i; ++i) {
		filled[i] = (char*)malloc(sizeof(char) * words_j);
	}
	for (int i = 0; i < lab_i; ++i) {
		for (int j = 0; j < lab_j; ++j) {
			*(*(filled+i)+j) = 0;
		}
	}
	int* used = (int*)malloc(sizeof(int) * words_count);
	if (used == NULL) {
		printf("Couldn't allocate memory");
		return 0;
	}
	for (int i = 0; i < words_count; i++) {
		used[i] = 0;
	}
	int status = 0;
	construt_crossword(labyrinth, filled, lab_i, lab_j, 0, 0, second_char, words, words_count, word_char_count, used, &status);
	if (!status) {
		construt_crossword(labyrinth, filled, lab_i, lab_j, 0, 0, first_char, words, words_count, word_char_count, used, &status);
	}
	if (!status) {
		printf("Sorry, I could not create crossword :(\n");
	}
	return 0;
}

void get_dimensions(int *i, int* j) {
	FILE* fin = fopen("labirintas.txt", "r");
	int i_size = 0, j_size=0;
	char c = 1;
	while (c != '\n') {
		c = getc(fin);
		i_size++;
	}
	i_size--;
	fclose(fin);
	FILE* fin_1 = fopen("labirintas.txt", "r");
	for (j_size = 0; c != EOF; ++j_size) {
		for (int k = 0; k < i_size; ++k) {
			c = getc(fin_1);
		}
		c = getc(fin_1);
	}
	*i = j_size;
	*j = i_size;
	fclose(fin_1);
}

void get_word_dimensions(int* i, int* j) {
	FILE* fin = fopen("zodziai.txt", "r");
	int j_size = 0, ii;
	char c = 1;
	for (ii = 0; c != EOF; ii++) {
		int jj;
		c = getc(fin);
		for (jj = 1; c != '\n'; jj++) {
			c = getc(fin);
			if (c == EOF) {
				if (j_size < jj) {
					j_size = jj;
				}
				*i = ii+1;
				*j = j_size-1;
				fclose(fin);
				return;
			}
		}
		if (j_size < jj) {
			j_size = jj;
		}
	}
	*i = ii;
	*j = j_size;
	fclose(fin);
}

void get_words(char** words, int* words_count) {
	FILE* fin = fopen("zodziai.txt", "r");
	*words_count = 0;
	char c = 1;
	while (c != EOF) {
		int size = fscanf(fin, "%[^\n]", words[*words_count]);
		c = getc(fin);
		(*words_count)++;	
	}
	fclose(fin);
}

void get_labyrinth(char** labyrinth, int lab_i, int lab_j) {
	FILE* fin = fopen("labirintas.txt", "r");
	for (int i = 0; i < lab_i; i++) {
		for (int j = 0; j < lab_j; j++) {
			labyrinth[i][j] = getc(fin);
		}
		char c = getc(fin);
	}
	fclose(fin);
}

void to_lower_all_words(char** words, int words_count, int words_j) {
	for (int i = 0; i < words_count; i++) {
		for (int j = 0; j < words_j; j++) {
			if (words[i][j] >= 65 && words[i][j] <= 90) {
				words[i][j] += 32;
			}
		}
	}
}

void remove_duplicates(char** words, int *words_count, int sz) {
	int *words_cnt = words_count, ind=0;
	for (int i = 0; i < sz; ++i) {
		for (int j = i + 1; j < sz-ind; ++j) {
			if (memcmp(words[i], words[j], sizeof(words[i]))==0) {
				for (int k = j; k < sz-1-ind; k++) {
					words[k] = words[k + 1];
				}
				(*words_cnt)--;
				ind++;
			}
		}
	}
	words_cnt = words_count;
}

char get_second_char(char** labyrinth, int lab_i, int lab_j) {
	for (int i = 0; i < lab_i; i++) {
		for (int j = 0; j < lab_j; j++) {
			if (i != 0 || j != 0) {
				if (labyrinth[i][j] != labyrinth[0][0]) {
					return labyrinth[i][j];
				}
			}
		}
	}
	return -1;
}

int* get_word_sizes(char** words, int words_count, int words_j) {
	int* word_char_count = (int*)malloc(sizeof(int) * (words_j+1));
	if (word_char_count == NULL) {
		printf("Could not allocate memory\n");
		return NULL;
	}
	for (int i = 0; i <= words_j; i++) {
		word_char_count[i] = 0;
	}
	for (int i = 0; i < words_count; ++i) {
		word_char_count[strlen(words[i])]++;
	}
	return word_char_count;
}

int get_cell_size_from_top(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c) {
	int sz = 0, curr_i=i_cell;
	while ((labyrinth[curr_i][j_cell] == c || filled[curr_i][j_cell]) &&  curr_i < lab_i) {
		sz++;
		curr_i++;
		if (curr_i == lab_i) {
			return sz;
		}
	}
	return sz;
}

int check_if_i_can_fill_from_top(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, int size, char* word) {
	int check1 = 0, ii_cell=i_cell;
	for (int i = 0; i < size; i++) {
		if (labyrinth[ii_cell][j_cell] == c || filled[ii_cell][j_cell]==0) {
			check1 = 1;
			break;
		}
		ii_cell++;
		if (ii_cell == lab_i) {
			break;
		}
	}
	if (!check1) {
		return 0;
	}
	for (int i = 0; i < size; i++) {
		if (labyrinth[i_cell][j_cell] != c && labyrinth[i_cell][j_cell] != word[i] && filled[i_cell][j_cell]<2) {
			return 0;
		}
		i_cell++;
		if (i_cell == lab_i) {
			return 1;
		}
	}
	return 1;
}

void fill_cell_from_top(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, char* word, int size) {
	for (int i = 0; i < size; i++) {
		labyrinth[i_cell][j_cell] = word[i];
		filled[i_cell][j_cell]++;
		i_cell++;
	}
}

int get_cell_size_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c) {
	int sz = 0, curr_j = j_cell;
	while ((labyrinth[i_cell][curr_j] == c || filled[i_cell][curr_j]!=0) && curr_j < lab_j) {
		sz++;
		curr_j++;
		if (curr_j == lab_j) {
			return sz;
		}
	}
	return sz;
} 

int check_if_i_can_fill_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, int size, char* word) {
	int check1 = 0, jj_cell = j_cell, size1=0;
	for (int i = 0; i < size; i++) {
		if (labyrinth[i_cell][jj_cell] == c || filled[i_cell][jj_cell]==0) {
			check1 = 1;
			break;
		}
		jj_cell++;
	}
	if (!check1) {
		return 0;
	}
	for (int i = 0; i < size; i++) {
		if (filled[i_cell][j_cell] != 0 && word[i] != labyrinth[i_cell][j_cell]) {
			return 0;
		}
		j_cell++;
		if (j_cell == lab_j) {
			return 1;
		}
	}
	return 1;
}

void fill_cell_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, char* word, int size) {
	for (int i = 0; i < size; i++) {
		labyrinth[i_cell][j_cell] = word[i];
		filled[i_cell][j_cell]++;
		j_cell++;
	}
}

void return_to_prev_version_from_top(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, int size) {
	for (int i = 0; i < size; i++) {
		if (filled[i_cell][j_cell] == 1) {
			labyrinth[i_cell][j_cell] = c;
			filled[i_cell][j_cell]--;
		}
		i_cell++;
	}
}

void return_to_prev_version_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int i_cell, int j_cell, char c, int size) {
	for (int i = 0; i < size; i++) {
		if (filled[i_cell][j_cell] == 1) {
			labyrinth[i_cell][j_cell] = c;
			filled[i_cell][j_cell]--;
		}
		j_cell++;
	}
}

void construt_crossword(char** labyrinth, char** filled, int lab_i, int lab_j, int prev_i, int prev_j, char c, char** words, int word_count, int* words_char_count, int* used, int* found) {
	if (*found) {
		return;
	}
	if (check_if_good(labyrinth, filled, lab_i, lab_j, c)) {
		output_crossword(labyrinth, lab_i, lab_j);
		*found = 1;
		return;
	}
	for (int ii = 0; ii < lab_i; ii++) {
		for (int jj = 0; jj < lab_j; jj++) {
			int size = get_cell_size_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c);
			if (size >= 2) {
				for (int words_cnt_i = 0; words_cnt_i < word_count; words_cnt_i++) {
					if (ii == 0) {
						int status = check_if_i_can_fill_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i]);
						if (size == strlen(words[words_cnt_i]) && !used[words_cnt_i] && check_if_i_can_fill_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i])) {
							fill_cell_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, words[words_cnt_i], size);
							used[words_cnt_i] = 1;
							construt_crossword(labyrinth, filled, lab_i, lab_j, ii, jj + 1, c, words, word_count, words_char_count, used, found);
							used[words_cnt_i] = 0;
							return_to_prev_version_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, size);
						}
					}
					else if (ii!=lab_i-1){
						if (labyrinth[ii - 1][jj] != c && filled[ii - 1][jj] == 0) {
							int status = check_if_i_can_fill_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i]);
							if (size == strlen(words[words_cnt_i]) && !used[words_cnt_i] && check_if_i_can_fill_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i])) {
								fill_cell_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, words[words_cnt_i], size);
								used[words_cnt_i] = 1;
								construt_crossword(labyrinth, filled, lab_i, lab_j, ii, jj + 1, c, words, word_count, words_char_count, used, found);
								used[words_cnt_i] = 0;
								return_to_prev_version_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, size);
							}
						}
					}
					else {
						if (labyrinth[ii - 1][jj] != c && filled[ii - 1][jj] == 0) {
							if (size == strlen(words[words_cnt_i]) && !used[words_cnt_i] && check_if_i_can_fill_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i])) {
								fill_cell_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, words[words_cnt_i], size);
								used[words_cnt_i] = 1;
								construt_crossword(labyrinth, filled, lab_i, lab_j, ii+1, 0, c, words, word_count, words_char_count, used, found);
								used[words_cnt_i] = 0;
								return_to_prev_version_from_top(labyrinth, filled, lab_i, lab_j, ii, jj, c, size);
							}
						}
					}
				}
			}
			if (ii == lab_i - 1 && jj == lab_j - 1) {
				construt_crossword_from_left(labyrinth, filled, lab_i, lab_j, 0, 0, c, words, word_count, words_char_count, used, found);
			}
		}
	}
}

void construt_crossword_from_left(char** labyrinth, char** filled, int lab_i, int lab_j, int prev_i, int prev_j, char c, char** words, int word_count, int* words_char_count, int* used, int* found){
	if (*found) {
		return;
	}
	if (check_if_good(labyrinth, filled, lab_i, lab_j, c)) {
		output_crossword(labyrinth, lab_i, lab_j);
		*found = 1;
		return;
	}
	for (int ii = 0; ii < lab_i; ii++) {
		for (int jj = 0; jj < lab_j; jj++) {
			int size = get_cell_size_from_left(labyrinth, filled,  lab_i, lab_j, ii, jj, c);
			if (size >= 2) {
				for (int words_cnt_i = 0; words_cnt_i < word_count; words_cnt_i++) {
					if (jj == 0) {
						if (size == strlen(words[words_cnt_i]) && !used[words_cnt_i] && check_if_i_can_fill_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i])) {
							fill_cell_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, words[words_cnt_i], size);
							used[words_cnt_i] = 1;
							construt_crossword_from_left(labyrinth, filled, lab_i, lab_j, ii, jj + 1, c, words, word_count, words_char_count, used, found);
							used[words_cnt_i] = 0;
							return_to_prev_version_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, size);
						}
					}
					else if (jj!=lab_j-1){
						if (labyrinth[ii][jj - 1] != c && filled[ii][jj - 1] == 0) {
							int status = check_if_i_can_fill_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i]);
							if (size == strlen(words[words_cnt_i]) && !used[words_cnt_i] && check_if_i_can_fill_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i])) {
								fill_cell_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, words[words_cnt_i], size);
								used[words_cnt_i] = 1;
								construt_crossword_from_left(labyrinth, filled, lab_i, lab_j, ii+1, 0, c, words, word_count, words_char_count, used, found);
								used[words_cnt_i] = 0;
								return_to_prev_version_from_left(labyrinth, filled, lab_i, lab_j, ii, jj+1, c, size);
							}
						}
					}
					else {
						if (labyrinth[ii][jj - 1] != c && filled[ii][jj - 1] == 0) {
							if (size == strlen(words[words_cnt_i]) && !used[words_cnt_i] && check_if_i_can_fill_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, size, words[words_cnt_i])) {
								fill_cell_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, words[words_cnt_i], size);
								used[words_cnt_i] = 1;
								construt_crossword_from_left(labyrinth, filled, lab_i, lab_j, ii, jj + 1, c, words, word_count, words_char_count, used, found);
								used[words_cnt_i] = 0;
								return_to_prev_version_from_left(labyrinth, filled, lab_i, lab_j, ii, jj, c, size);
							}
						}
					}
				}
			}
		}
	}
}

int check_if_good(char** labyrinth, char** filled, int lab_i, int lab_j, char c) {
	for (int i = 0; i < lab_i; i++) {
		for (int j = 0; j < lab_j; j++) {
			if (labyrinth[i][j] == c && filled[i][j]==0) {
				return 0;
			}
		}
	}
	return 1;
}

void output_crossword(char** labyrinth, int lab_i, int lab_j) {
	for (int i = 0; i < lab_i; i++) {
		for (int j = 0; j < lab_j; j++) {
			printf("%c", labyrinth[i][j]);
		}
		putchar('\n');
	}
	putchar('\n');
}