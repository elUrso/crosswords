/* MAC0121 - 2018 - 2
 * EP 2
 * Vitor Pereira da Silva
 * NºUSP 10263531
 */

#include <stdio.h>
#include <stdlib.h>

#define max(x, y) ((x > y) ? x : y)

struct Field {
    int i; /*row*/
    int j; /*column*/
    int v; /*is vertical?*/
    int s; /*size*/
};

/* prototypes */
void * smalloc(size_t size);
void showmaps(char ** map, char ** mapt, int m, int n);
int valid(char ** map, char ** mapt, int m, int n, char * word, struct Field field);
void write(char ** map, char ** mapt, int m, int n, char * word, struct Field field);
void clear(char ** map, char ** mapt, int m, int n, char * word, struct Field field);
int getnumber(void);

int main() {
    char ** map, ** mapt; /*map, transposed map*/
    char * rmap, * rmapt; /*raw map, raw transposed map*/
    int m, n;

    char ** word;
    char * rword; /*raw word*/
    int * sword; /*size of word*/
    int nword;

    int * stack;

    struct Field * field;
    int nfield;

    int base;

    int instance = 0;
    loop:
        instance++;

        /*Get board size*/

        m = getnumber();
        n = getnumber();

        if(m == 0 && n == 0) goto finish;

        rmap = smalloc(sizeof(char) * m * n);
        rmapt = smalloc(sizeof(char) * m * n);
        map = smalloc(sizeof(char *) * m);
        mapt = smalloc(sizeof(char *) * n);

        /*Initializate board*/

        for(int i = 0; i < m; i++) map[i] = rmap + n * i;
        for(int i = 0; i < n; i++) mapt[i] = rmapt + m * i;

        for(int i = 0; i < m; i++)
            for(int j = 0; j < n; j++) {
                map[i][j] = (getnumber() == 0) ? ' ' : '*';
                mapt[j][i] = map[i][j];
            }

        /*Get words*/
        nword = getnumber();

        /*Assuming max word size of max(m, n) + 1*/

        word = smalloc(sizeof(char *) * nword);
        rword = smalloc(sizeof(char) * (max(m, n) + 1) * nword);
        sword = smalloc(sizeof(int) * nword);

        for(int w = 0, base = 0, size = 0; w < nword; w++) {
            word[w] = rword + base;
            scanf("%s", rword + base);
            for(size = 0; rword[base + size] != '\0'; size++);
            sword[w] = size; /*number of chars*/
            base += size + 1;
        }

        /*Discover Fields*/

        nfield = 0;
        for(int i = 0; i < m; i++)
            for(int j = 0; j < n; j++) {
                if(j == 0 && map[i][j] != '*') nfield++;
                if(j != 0 && map[i][j-1] == '*' && map[i][j] == ' ') nfield++;
            }

        for(int j = 0; j < n; j++)
            for(int i = 0; i < m; i++) {
                if(i == 0 && map[i][j] != '*') nfield++;
                else if(i != 0 && map[i-1][j] == '*' && map[i][j] == ' ') nfield++;
            }

        field = smalloc(sizeof(struct Field) * nfield);

        base = 0;
        for(int i = 0, size = 0; i < m; i++) {
            for(int j = 0; j < n; j++) {
                if(j == 0) {
                    size = 0;
                    field[base].i = i;
                    field[base].j = j;
                }
                if(j != 0 && map[i][j-1] == '*' && map[i][j] == ' ') {
                    field[base].i = i;
                    field[base].j = j;  
                }
                if(map[i][j] == ' ') size++;
                else if(size > 0) {
                    field[base].s = size;
                    field[base].v = 0;
                    size = 0;
                    base++;
                }
            }
            if(size > 0) {
                field[base].s = size;
                field[base].v = 0;
                base++;
            }
        }

        for(int j = 0, size = 0; j < n; j++) {
            for(int i = 0; i < m; i++) {
                if(i == 0) {
                    size = 0;
                    field[base].i = i;
                    field[base].j = j;
                }
                if(i != 0 && map[i-1][j] == '*' && map[i][j] == ' ') {
                    field[base].i = i;
                    field[base].j = j;  
                }
                if(map[i][j] == ' ') size++;
                else if(size > 0) {
                    field[base].s = size;
                    field[base].v = 1;
                    size = 0;
                    base++;
                }
            }
            if(size > 0) {
                field[base].s = size;
                field[base].v = 1;
                base++;
            }
        }

        /*Initialize stack*/

        stack = smalloc(sizeof(int) * nword);
        for(int i = 0; i < nword; i++) stack[i] = -2;

        /*Begin Backtrack*/
        base = 0;

        backtrack:
            switch(stack[base]) {
                case -2: /*uninitialized*/
                    for(stack[base] = 0 /*pop, push 0*/; sword[base] != field[stack[base] /*top*/].s && stack[base] < nfield; stack[base]++ /*pop t, push t+1*/);
                    if(stack[base] < nfield && valid(map, mapt, m, n, word[base], field[stack[base]])) {
                        write(map, mapt, m, n, word[base], field[stack[base]]);
                        base++;
                        break;
                    }
                    if(stack[base] >= nfield) {
                        clear(map, mapt, m, n, word[base], field[stack[base]]);
                        stack[base] = -2; /*pop*/
                        base--;
                    }
                    break;
                default: /*initialized*/
                    recheck:
                    for(; sword[base] != field[stack[base] /*top*/].s && stack[base] < nfield; stack[base]++ /*pop t, push t+1*/);
                    if(stack[base] < nfield && valid(map, mapt, m, n, word[base], field[stack[base]])) {
                        write(map, mapt, m, n, word[base], field[stack[base]]);
                        base++;
                        break;
                    } else if(stack[base] < nfield){
                        stack[base]++;
                    }
                    if(stack[base] == nfield) {
                        clear(map, mapt, m, n, word[base], field[stack[base]]);
                        stack[base] = -2; /*pop*/
                        base--;
                        if(base < 0) break;
                        clear(map, mapt, m, n, word[base], field[stack[base]]);
                        stack[base]++;
                        goto recheck;
                    }
            }
            if(base >= nword || base < 0) goto solve;
        goto backtrack;

        solve:
        printf("Instancia %d\n", instance);

        if(base < 0) puts("Nao ha solucao");
        else showmaps(map, mapt, m, n);
        puts("");

        free(rmap); rmap = NULL;
        free(map); map = NULL;
        free(rmapt); rmapt = NULL;
        free(mapt); mapt = NULL;
        free(word); word = NULL;
        free(rword); rword = NULL;
        free(sword); sword = NULL;
        free(field); field = NULL;
        free(stack); stack = NULL;
    goto loop;

    finish: return 0;
}

/* implementations */
void * smalloc(size_t size) {
	void * t = malloc(size);
	if(t == NULL) exit(EXIT_FAILURE);
	return t;
}

void showmaps(char ** map, char ** mapt, int m, int n) {
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            printf("%c ", (map[i][j] == ' ') ? mapt[j][i] : map[i][j]);
        }
        puts("");
    }
}


int valid(char ** map, char ** mapt, int m, int n, char * word, struct Field field) {
    for(int i = 0; i < field.s; i++) {
        if(!(
            (word[i] == map[field.i + ((field.v) ? i : 0)][field.j + ((field.v) ? 0 : i)] ||
            map[field.i + ((field.v) ? i : 0)][field.j + ((field.v) ? 0 : i)] == ' ') &&
            (word[i] == mapt[field.j + ((field.v) ? 0 : i)][field.i + ((field.v) ? i : 0)] ||
            mapt[field.j + ((field.v) ? 0 : i)][field.i + ((field.v) ? i : 0)] == ' ')))
            return 0;
    }
    return 1;
}

void write(char ** map, char ** mapt, int m, int n, char * word, struct Field field) {
    for(int i = 0; i < field.s; i++)
        if(field.v)
            mapt[field.j][field.i + i] = word[i];
        else
            map[field.i][field.j + i] = word[i];
}

void clear(char ** map, char ** mapt, int m, int n, char * word, struct Field field) {
    for(int i = 0; i < field.s; i++)
        if(field.v)
            mapt[field.j][field.i + i] = ' ';
        else
            map[field.i][field.j + i] = ' ';
}

int getnumber() {
    int n;
    scanf("%d", &n);
    return n;
}