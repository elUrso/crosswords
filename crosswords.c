/* MAC0121 - 2018 - 2
 * EP 2 - 28 de agosto
 * Vitor Pereira da Silva
 * NUSP 10263531
 * Tab Size = 8
 */

/* TODO
 *
 * Fix valid
 *
 */

#include <stdio.h>
#include <stdlib.h>

/* Definições */
#define TRUE (1)
#define FALSE (0)
#define DEBUG (1)

typedef short int Bool;

typedef struct {
	int size;
	char * str;
} Word;

typedef struct {
	Bool trans;
	Bool available;
	int row;
	int column;
	int size;
} Field;

/* Prototipos */
void showMap(char * map, int m, int n);
void showFields(Field * fields, int n);
int max(int a, int b);
void * smalloc(size_t size);
int fillSolution(char * map, char * mapt, int m, int n, Field * fields, int n_fields, Word * words, int n_words);
void write(char * map, char * mapt, int m, int n, Word * word, Field * field);
void clear(char * map, char * mapt, int m, int n, Word * word, Field * field);
int is_valid(char * map, char * mapt, int m, int n, Word * word, Field * field);
void printMap(char * map, char * mapt, int m, int n);

/* Textos */
static char * instance = "Instancia %d\n";
static char * noSolution = "nao ha solucao\n";

int main() {
	Bool gameIsValid;
	int m /*linhas*/, n /*colunas*/, f /*campos*/, w /*quantidade de palavras*/, r /*rodada*/;
	int tmp /*número temporário*/, offset /*ponteiro do heap de palavras*/, size /*tamanho do heap de palavras*/;
	int nfield /*número de campos*/, hfield /*número de campos horizontais*/, vfield /*numero de campos verticais (transposta)*/;
	Field * fields = NULL;
	char * map = NULL /*tabuleiro*/, * mapt = NULL /*transposta*/, * str = NULL/*heap de palavras*/;
	Word * words = NULL /*lista de palavras*/;
	int slotp = 0 /*ponteiro da pilha*/;
	/*Game loop*/

	r = 1;
	loop:
		/*Define tamanho da cruzadinha*/
		scanf("%d %d", &m, &n);

		/*Confere se é um tamanho válido*/
		gameIsValid = m || n;
		if(!gameIsValid) goto end;

		/*Carrega o tabuleiro*/
		f = m * n;
		map = smalloc(f * sizeof(char));
		mapt = smalloc(f * sizeof(char));

		for(int i = 0; i < f; i++) {
			scanf("%d", &tmp);
			map[i] = (tmp == -1)? '*' : ' ';
		}

		/*Gera a transposta do tabuleiro*/
		for(int i = 0; i < m; i++)
			for(int j = 0; j < n; j++)
				mapt[j*m+i] = map[i*n+j];

		/*DEBUG*/ if(DEBUG) {showMap(map, m, n); showMap(mapt, n, m);}

		/*Obtem lista de palavras*/
		scanf("%d", &w);

		/*Inicializa o heap, a lista e a pilha de slots*/
		words = smalloc(sizeof(Word) * w);
		size = (m+1) * (n+1) * w;
		str = smalloc(sizeof(char) * size);
		offset = 0;

		for(int i = 0; i < w; i++) {
			scanf("%s", &str[offset]);

			/*DEBUG/ if(DEBUG) puts(&str[offset]);*/

			words[i].str = &str[offset];

			tmp = offset + 1;
			while(str[offset++] != '\0');
			offset++;

			words[i].size = offset - tmp - 1;

			if(offset > size) {
				while(++i < w) scanf("%s", str);
				goto fail;
			}
		}

		/*DEBUG*/ if(DEBUG) for(int i = 0; i < w; i++) puts(words[i].str);

		/* Obtem os campos do mapa*/

		nfield = 0;
		hfield = 0;
		vfield = 0;

		for(int i = 0; i < m; i++) {
			int k = FALSE;
			for(int j = 0; j < n; j++) {
				switch(map[j+i*n]) {
					case ' ':
						k = TRUE;
						break;
					case '*':
						hfield += (k)? 1 : 0;
						k = FALSE;
				}
			}
			hfield += (k)? 1 : 0;
		}

		/*DEBUG*/ if(DEBUG) printf("%d fields in horizontal\n", hfield);

		for(int i = 0; i < n; i++) {
			int k = FALSE;
			for(int j = 0; j < m; j++) {
				switch(mapt[j+i*m]) {
					case ' ':
						k = TRUE;
						break;
					case '*':
						vfield += (k)? 1 : 0;
						k = FALSE;
				}
			}
			vfield += (k)? 1 : 0;
		}

		/*DEBUG*/ if(DEBUG) printf("%d fields in vertical\n", vfield);

		nfield = hfield + vfield;
		fields = smalloc(sizeof(Field) * nfield);

		int fieldtmp = 0;
		int fieldsize, fieldrow, fieldcolumn;

		for(int i = 0; i < m; i++) {
			int k = FALSE;
			fieldsize = 0;
			fieldrow = -1;
			fieldcolumn = -1;
			for(int j = 0; j < n; j++) {
				switch(map[j+i*n]) {
					case ' ':
						if(fieldrow == -1 && fieldcolumn == -1) {
							fieldrow = i;
							fieldcolumn = j;
						}
						k = TRUE;
						fieldsize++;
						break;
					case '*':
						if(k) {
							fields[fieldtmp].trans = FALSE;
							fields[fieldtmp].available = TRUE;
							fields[fieldtmp].row = fieldrow; fieldrow = -1;
							fields[fieldtmp].column = fieldcolumn; fieldcolumn = -1;
							fields[fieldtmp].size = fieldsize;
							fieldtmp++;
							/*DEBUG*/ if(DEBUG) printf("m:%d at %d x %d [%d]\n", fieldtmp, fields[fieldtmp-1].row, fields[fieldtmp-1].column, fieldsize);
						}
						fieldsize = 0;
						k = FALSE;
				}
			}
			if(k) {
				fields[fieldtmp].trans = FALSE;
				fields[fieldtmp].available = TRUE;
				fields[fieldtmp].row = fieldrow; fieldrow = -1;
				fields[fieldtmp].column = fieldcolumn; fieldcolumn = -1;
				fields[fieldtmp].size = fieldsize;
				fieldtmp++;
				/*DEBUG*/ if(DEBUG) printf("f:%d at %d x %d [%d]\n", fieldtmp, fields[fieldtmp-1].row, fields[fieldtmp-1].column, fieldsize);
			}
		}

		/*DEBUG*/ if(DEBUG) puts("TT");

		for(int i = 0; i < n; i++) {
			int k = FALSE;
			fieldsize = 0;
			fieldrow = -1;
			fieldcolumn = -1;
			for(int j = 0; j < m; j++) {
				switch(mapt[j+i*m]) {
					case ' ':
						if(fieldrow == -1 && fieldcolumn == -1) {
							fieldrow = i;
							fieldcolumn = j;
						}
						k = TRUE;
						fieldsize++;
						break;
					case '*':
						if(k) {
							fields[fieldtmp].trans = TRUE;
							fields[fieldtmp].available = TRUE;
							fields[fieldtmp].column = fieldrow; fieldrow = -1;
							fields[fieldtmp].row = fieldcolumn; fieldcolumn = -1;
							fields[fieldtmp].size = fieldsize;
							fieldtmp++;
							/*DEBUG*/ if(DEBUG) printf("m:%d at %d x %d [%d]\n", fieldtmp, fields[fieldtmp-1].row, fields[fieldtmp-1].column, fieldsize);
						}
						fieldsize = 0;
						k = FALSE;
				}
			}
			if(k) {
				fields[fieldtmp].trans = TRUE;
				fields[fieldtmp].available = TRUE;
				fields[fieldtmp].column = fieldrow; fieldrow = -1;
				fields[fieldtmp].row = fieldcolumn; fieldcolumn = -1;
				fields[fieldtmp].size = fieldsize;
				fieldtmp++;
				/*DEBUG*/ if(DEBUG) printf("f:%d at %d x %d [%d]\n", fieldtmp, fields[fieldtmp-1].row, fields[fieldtmp-1].column, fieldsize);
			}
		}

		/*DEBUG*/ if(DEBUG && fieldtmp != nfield) puts("Fatal Error");

		/* backtracking to solve the crossword*/
		
		/*DEBUG*/ if(DEBUG) puts("Begin backtracking");
		
		if(!fillSolution(map, mapt, m, n, fields, nfield, words, w)) goto fail;

		printf(instance, r);
		printMap(map, mapt, m, n);

		goto defer;
		fail:
			printf(instance, r);
			printf("%s", noSolution);
		defer:

		/*DEBUG*/ if(DEBUG) puts("free map");
		free(map); map = NULL;
		/*DEBUG*/ if(DEBUG) puts("free mapt");
		free(mapt); mapt = NULL;
		/*DEBUG*/ if(DEBUG) puts("free words");
		free(words); words = NULL;
		/*DEBUG*/ if(DEBUG) puts("free str");
		free(str); str = NULL;
		/*DEBUG*/ if(DEBUG) puts("free fields");
		free(fields); fields = NULL;

		r++;
	goto loop;

	end:
	return 0;
}

/* Implementations */
void showMap(char * map, int m, int n) {
	for(int i = 0; i < m; i++) {
		for(int j = 0; j < n; j++) printf("%s", (map[i*n+j] == '*')? " ■" : " ▢");
		puts("");
	}
}

int max(int a, int b) {
	return (a > b)? a : b;
}

void * smalloc(size_t size) {
	void * t = malloc(size);
	if(t == NULL) exit(EXIT_FAILURE);
	return t;
}

void showFields(Field * fields, int n) {
	for(int i = 0; i < n; i++)
		printf(	"Field at %d %d of size %d\n",
			(fields[i].trans)? fields[i].column : fields[i].row,
			(fields[i].trans)? fields[i].row : fields[i].column,
			fields[i].size);
}

int fillSolution(char * map, char * mapt, int m, int n, Field * fields, int n_fields, Word * words, int n_words) {
	/*DEBUG*/ if(DEBUG) puts("Alloc stack");
	int * stack = smalloc(sizeof(int) * n_words);
	int base = 0;

	for(int i = 0; i < n_words; i++) {
		stack[i] = -1; /*Clear stack*/
	}

	while(base != -1 || base < n_words) {
		/*DEBUG*/ if(DEBUG) printf("Loop >> base %d, stack[base] %d, word = %s\n", base, stack[base], words[base].str);
		switch(stack[base]) { /*Read top value*/
			case -1:
				/*find a valid field*/
				next: stack[base]++;
				/*DEBUG*/ if(DEBUG) printf("Word (%d) %s:%d at field(%d) %d\n", base, words[base].str, words[base].size, stack[base], fields[stack[base]].size);
				if(words[base].size != fields[stack[base]].size || !fields[stack[base]].available) {
					if(stack[base] == n_fields) break; /*check if it is the last field*/
					goto next;
				}
				if(is_valid(map, mapt, m, n, &words[base], &fields[stack[base]])) {
					/*DEBUG*/ puts("Valid");
					write(map, mapt, m, n, &words[base], &fields[stack[base]]);
					/*DEBUG*/ printMap(map, mapt, m, n);
					stack[++base] = -1; /*push -1 to stack*/
				} else
					goto next;
				break;
			default:

				/*back a step*/
				if(stack[base] == n_fields) {
					clear(map, mapt, m, n, &words[base], &fields[stack[base]]);
					stack[base--] = -1; /*pop value from stack*/
					break;
				}

				/*Try the next valid spot*/
				clear(map, mapt, m, n, &words[base], &fields[stack[base]]);
				goto next;
		}
	}

	if(base == -1) return FALSE;
	return TRUE;
}

void printMap(char * map, char * mapt, int m, int n) {
	for(int i = 0; i < m; i++) {
		for(int j = 0; j < n; j++) {
			printf("%c ", (map[j+i*n] != ' ') ? map[j + i*n] : mapt[i + j*m]);
		}
		puts("");
	}

}

void write(char * map, char * mapt, int m, int n, Word * word, Field * field) {
	for(int i = 0; i < word->size; i++)
		if(field->trans)
			mapt[field->row * m + i + field->column] = word->str[i];
		else
			map[field->row + i + field->column * n] = word->str[i];
	field->available = FALSE;
}

void clear(char * map, char * mapt, int m, int n, Word * word, Field * field) {
	for(int i = 0; i < word->size; i++)
		if(field->trans)
			mapt[field->row * m + i + field->column] = ' ';
		else
			map[field->row + i + field->column * n] = ' ';
	field->available = TRUE;
}

int is_valid(char * map, char * mapt, int m, int n, Word * word, Field * field) {
	/*BEBUG*/ if(DEBUG) printf("Field: %d x %d %d\n", field->row, field->column, field->trans);
	for(int i = 0; i < word->size; i++) {
		if(field->trans) {
			/*DEBUG*/ if(DEBUG) printf("w: %c m: %c t: %c\n", word->str[i], map[field->row + (field->column + i) * n], mapt[field->row * m + i + field->column]);
			if(!((mapt[field->row * m + i + field->column] == ' ' || mapt[field->row * m + i + field->column] == word->str[i]) &&
				(map[field->row + (field->column + i) * n] == ' ' || map[field->row + (field->column + i) * n] == word->str[i])))
				return FALSE;
		} else {
			/*DEBUG*/ if(DEBUG) printf("w: %c m: %c t: %c\n", word->str[i], map[field->row + i + field->column * n], mapt[(field->row + i) * m + field->column]);
			if(!((mapt[(field->row + i) * m + field->column] == ' ' || mapt[(field->row + i) * m + field->column] == word->str[i]) &&
				(map[field->row + i + field->column * n] == ' ' || map[field->row + i + field->column * n] == word->str[i])))
				return FALSE;
		}
	}
	return TRUE;
}