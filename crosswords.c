/* MAC0121 - 2018 - 2
 * EP 2 - 28 de agosto
 * Vitor Pereira da Silva
 * NUSP 10263531
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

/* Prototipos */
void showMap(char * map, int m, int n);
int max(int a, int b);
void * smalloc(size_t size);

/* Textos */
static char * instance = "Instancia %d\n";
static char * noSolution = "nao ha solucao\n";

int main() {
	Bool gameIsValid;
	int m /*linhas*/, n /*colunas*/, f /*campos*/, w /*quantidade de palavras*/, r /*rodada*/;
	int tmp /*número temporário*/, offset /*ponteiro do heap de palavras*/, size /*tamanho do heap de palavras*/;
	char * map /*tabuleiro*/, * mapt /*transposta*/, * str /*heap de palavras*/;
	Word * words /*lista de palavras*/;
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

		/*Inicializa o heap e a lista*/
		words = smalloc(sizeof(Word) * w);
		size = (m+1) * (n+1) * w;
		str = smalloc(sizeof(char) * size);
		offset = 0;

		for(int i = 0; i < w; i++) {
			scanf("%s", &str[offset]);

			/*DEBUG*/ if(DEBUG) puts(&str[offset]);

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

		goto defer;
		fail:
			printf(instance, r);
			printf("%s", noSolution);
		defer:

		free(map);
		free(mapt);
		free(words);
		free(str);

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
