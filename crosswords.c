/* MAC0121 - 2018 - 2
 * EP 2 - 28 de agosto
 * Vitor Pereira da Silva
 * NUSP 10263531
 * Tab Size = 8
 */

/* TODO
 *
 * Finish Backtracking 
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

typedef struct {
	int field;
	Boold valid;
} Slot;

/* Prototipos */
void showMap(char * map, int m, int n);
void showFields(Field * fields, int n);
int max(int a, int b);
void * smalloc(size_t size);
int fillSolution(char * map, char * mapt, int m, int n, Field * fields, int n_fields, Word * words, int n_words);
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

			words[i].size = offset - tmp;

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
				fields[fieldtmp].trans = TRUE;
				fields[fieldtmp].available = TRUE;
				fields[fieldtmp].row = fieldrow; fieldrow = -1;
				fields[fieldtmp].column = fieldcolumn; fieldcolumn = -1;
				fields[fieldtmp].size = fieldsize;
				fieldtmp++;
				/*DEBUG*/ if(DEBUG) printf("f:%d at %d x %d [%d]\n", fieldtmp, fields[fieldtmp-1].row, fields[fieldtmp-1].column, fieldsize);
			}
		}

		/*DEBUG*/ if(DEBUG && fieldtmp != nfield) puts("Fatal Error");

		/* backtracking para resolver a cruzadinha */
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
		/*DEBUG*/ if(DEBUG) puts("free slots");
		free(slots); slots = NULL;

		r++;
	goto loop;

	end:
	return 0;
}

/* Implementações */
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
	Slot * slots = smalloc(sizeof(Slot) * n_words);
	int base = 0;
	for(int i = 0; i < n_words; i++) slots[i].valid = FALSE;
	/*Continue here with stack based*/
}

void printMap(char * map, char * mapt, int m, int n) {
	for(int i = 0; i < m; i++) {
		for(int j = 0; i < n; i++) {
			printf("%c ", (map[i*n+j] == ' ') ? mapt[j*m+i] : map[i*n+j]);
		}
		puts("");
	}
}
