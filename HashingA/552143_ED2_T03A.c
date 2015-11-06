/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
 *
 * RA: 552143
 * Aluno: Diego Tenorio de Araujo Siqueira
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 9
#define TAM_EQUIPE 40
#define TAM_DATA 11
#define TAM_DURACAO 6
#define TAM_PLACAR 3
#define TAM_MVP 40

#define TAM_REGISTRO 192
#define MAX_REGISTROS 5000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n\n"
#define REGISTRO_N_ENCONTRADO "Registro nao encontrado!\n\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ERRO_TABELA_CHEIA "ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"

#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE 0
#define OCUPADO 1
#define REMOVIDO 2

/* Registro da partida */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char equipe_azul[TAM_EQUIPE];
	char equipe_vermelha[TAM_EQUIPE];
	char data_partida[TAM_DATA];	// DD/MM/AAAA
	char duracao[TAM_DURACAO];			// MM:SS
	char vencedor[TAM_EQUIPE];
	char placar1[TAM_PLACAR];
	char placar2[TAM_PLACAR];
	char mvp[TAM_MVP];
} Partida;

/* Registro da Tabela Hash
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave *v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int rrnGlobal = 0;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Descarta o que estiver no buffer de entrada */
void ignore();

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o jogador */
void exibir_registro(int rrn);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
int prox_primo(int tam);
void criar_tabela(Hashtable *tabela, int tam);
void carregar_tabela(Hashtable *tabela);
void cadastrar(Hashtable *tabela);
void alterar(Hashtable tabela);
void buscar(Hashtable tabela);
void remover(Hashtable *tabela);
void imprimir_tabela(Hashtable tabela);
void liberar_tabela(Hashtable *tabela);
Partida recuperar_registro(int rrn);

void le_equipe(char equipe_chamada[]);
void le_data(char data_chamada[]);
void le_duracao(char duracao_chamada[]);
void le_vencedora(char vencedora_chamada[], char equipe_azul[], char equipe_vermelha[]);
void le_placar(char placar_chamada[]);
void le_apelido_mvp(char apelido_chamada[]); 

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() {

	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) carregar_arquivo();


	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) carregar_tabela(&tabela);



	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
		scanf("%d", &opcao);
		switch(opcao) {

		case 1:
			getchar();
			cadastrar(&tabela);
			break;
		case 2:
			getchar();
			alterar(tabela);
			break;
		case 3:
			getchar();
			buscar(tabela);
			break;
		case 4:
			getchar();
			remover(&tabela);
			break;
		case 5:
			imprimir_tabela(tabela);
			break;
		case 6:
			liberar_tabela(&tabela);
			break;

		case 10:
			printf("%s\n", ARQUIVO);
			break;

		default:
			ignore();
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}



/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

/* Descarta o que estiver no buffer de entrada */
void ignore() {
	char c;
	while ((c = getchar()) != '\n' && c != EOF);
}

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

/* Exibe a partida */
void exibir_registro(int rrn) {

	Partida j = recuperar_registro(rrn);

	printf("%s\n", j.pk);
	printf("%s\n", j.equipe_azul);
	printf("%s\n", j.equipe_vermelha);
	printf("%s\n", j.data_partida);
	printf("%s\n", j.duracao);
	printf("%s\n", j.vencedor);
	printf("%s\n", j.placar1);
	printf("%s\n", j.placar2);
	printf("%s\n", j.mvp);
	printf("\n");
}

/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

Partida recuperar_registro(int rrn){
	char *arq;
	Partida novo;

	arq = ARQUIVO;
	arq+=rrn;

	sscanf(arq, "%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@%[^@]@", novo.pk, novo.equipe_azul, novo.equipe_vermelha, novo.data_partida, novo.duracao, novo.vencedor, novo.placar1, novo.placar2, novo.mvp);

	return novo;
}

int prox_primo(int tam){
	
	int i, flag = 0, c=0; 
  
	if(tam <= 2){
		//printf("%d\n", 2);
		return 2;
	}
  	
	// the gaps between prime numbers are quite small, with the first gap over 100 is 370261
	while(c<100){
 	
 		flag = 0;
		for (i = 2 ; i < tam; i++) 
		   	if (tam % i == 0){ 
		   		flag = 1; 
		   	}

		if (!flag){
			//printf("%d\n", tam);
			return tam;
		}

		tam++;
		c++;
	}
}

void criar_tabela(Hashtable *tabela, int tam){
	int i;

	tabela->tam = tam;
	tabela->v = (Chave *) malloc(sizeof(Chave) * tam);

	for(i=0; i< tabela->tam; i++)
		tabela->v[i].estado = LIVRE; 
}

void carregar_tabela(Hashtable *tabela){
	int i, j, c, col, pos;
	char *aux = ARQUIVO;
	int busca = 0, cheio = 0;
	Chave novo;
	int tamArq = strlen(ARQUIVO);

	while(tamArq > 0){
		sscanf(aux, "%[^@]", novo.pk);
		aux+= strlen(novo.pk)+1;

		//printf("%s\n", novo.pk);

		//busca chave repetida
		busca = 0;
		for(j=0; !busca && j< tabela->tam; j++){
			if(strcmp(tabela->v[j].pk, novo.pk) == 0 && tabela->v[j].estado == OCUPADO){
				//printf(ERRO_PK_REPETIDA, novo.pk);
				busca = 1;
			}
		}

		if(busca){
			printf(ERRO_PK_REPETIDA, novo.pk);
		}
		else{
			pos = (1 * novo.pk[0] + 2 * novo.pk[1] + 3 * novo.pk[2] + 4 * novo.pk[3] + 5 * novo.pk[4] + 6 * novo.pk[5] + 7 * novo.pk[6] + 8 * novo.pk[7]) % tabela->tam;
			//insere no lugar certo
			c=0;
			col=0;
			cheio =0;
			for(i = pos; !cheio && i < tabela->tam; i = (i+1) % tabela->tam){
				// se tem estado livre
				if(tabela->v[i].estado == LIVRE || tabela->v[i].estado == REMOVIDO){
					tabela->v[i].estado = OCUPADO;
					strcpy(tabela->v[i].pk, novo.pk);
					tabela->v[i].rrn = rrnGlobal;

					novo.rrn = rrnGlobal;
					rrnGlobal+=192;

					//printf(REGISTRO_INSERIDO, novo.pk, col);				
					//flag pra inserir
					cheio = 1;
				}

				// se ja rodou toda a tabela
				if(c == tabela->tam){
					//printf(ERRO_TABELA_CHEIA);	
					cheio = 1;
				}
				c++;
				col++;
				//i = (i+1) % tabela->tam;
			}

		//posiciono o ponteiro aux no proximo registro
		j=0;
		while(j<8){
			if(*aux == '@')
				j++;
				aux++;
		}
		while(*aux == '#')
			aux++;
		}
		tamArq -=192;
	}
}

void cadastrar(Hashtable *tabela){
	char *p;
	Partida novo;
	int col=0;
	int i, pos, c=0, tamR, j;

	p = ARQUIVO;
	p+= rrnGlobal;

	le_equipe(novo.equipe_azul);
	le_equipe(novo.equipe_vermelha);
	le_data(novo.data_partida);
	le_duracao(novo.duracao);
	le_vencedora(novo.vencedor, novo.equipe_azul, novo.equipe_vermelha);
	le_placar(novo.placar1);
	le_placar(novo.placar2);
	le_apelido_mvp(novo.mvp);

	novo.pk[0] = toupper(novo.equipe_azul[0]);
	novo.pk[1] = toupper(novo.equipe_vermelha[0]);
	novo.pk[2] = toupper(novo.mvp[0]);
	novo.pk[3] = toupper(novo.mvp[1]);
	novo.pk[4] = novo.data_partida[0];
	novo.pk[5] = novo.data_partida[1];
	novo.pk[6] = novo.data_partida[3];
	novo.pk[7] = novo.data_partida[4];
	novo.pk[8] = '\0';

	//verificar se existe uma pk repetida	
	pos = (1 * novo.pk[0] + 2 * novo.pk[1] + 3 * novo.pk[2] + 4 * novo.pk[3] + 5 * novo.pk[4] + 6 * novo.pk[5] + 7 * novo.pk[6] + 8 * novo.pk[7]) % tabela->tam;


	//printf("Posicao: %d Chave: %s \n", pos, novo.pk);
	//verificar se nao tem chave igual antes de inserir
	for(j=0; j< tabela->tam; j++){
		if(strcmp(tabela->v[j].pk, novo.pk) == 0 && tabela->v[j].estado == OCUPADO){
			printf(ERRO_PK_REPETIDA, novo.pk);
			return;
		}
	}

	c=0;
	col=0;

	for(i = pos; i < tabela->tam; i = (i+1) % tabela->tam){
		// se tem estado livre
		if(tabela->v[i].estado == LIVRE || tabela->v[i].estado == REMOVIDO){
			tabela->v[i].estado = OCUPADO;
			strcpy(tabela->v[i].pk, novo.pk);
			tabela->v[i].rrn = rrnGlobal;

			sprintf(p, "%s@%s@%s@%s@%s@%s@%s@%s@%s@", novo.pk, novo.equipe_azul, novo.equipe_vermelha, novo.data_partida, novo.duracao, novo.vencedor, novo.placar1, novo.placar2, novo.mvp);
			tamR = 28 + strlen(novo.pk) + strlen(novo.equipe_azul) + strlen(novo.equipe_vermelha) + strlen(novo.vencedor) + strlen(novo.mvp);
			p+=tamR;

			while(tamR < 192){
				sprintf(p, "#");
				p++;
				tamR++;
			}

			rrnGlobal+=192;	

			printf(REGISTRO_INSERIDO, novo.pk, col);				
			return;
		}

		// se ja rodou toda a tabela
		if(c == tabela->tam){
			printf(ERRO_TABELA_CHEIA);	
			return;
		}
		c++;
		col++;
		//i = (i+1) % tabela->tam;
	}
}

void buscar(Hashtable tabela){
	char buscaPk[9];
	int j;

	scanf("%8[^\n]", buscaPk);
    getchar();
    buscaPk[9] = '\0';

	for(j=0; j< tabela.tam; j++){
		if(strcmp(tabela.v[j].pk, buscaPk) == 0 && tabela.v[j].estado != REMOVIDO){
			exibir_registro(tabela.v[j].rrn);
			return;
		}
	}    

	printf(REGISTRO_N_ENCONTRADO);
}

void alterar(Hashtable tabela){
	char buscaPk[9];
	char duracao[6];
	int j, c=0;
	char *p;

	p = ARQUIVO;


	scanf("%8[^\n]", buscaPk);
    getchar();
    buscaPk[9] = '\0';

	for(j=0; j< tabela.tam; j++){
		if(strcmp(tabela.v[j].pk, buscaPk) == 0 && tabela.v[j].estado != REMOVIDO){
			le_duracao(duracao);

			p+=tabela.v[j].rrn;
			while(c<4){
				if(*p == '@')
					c++;

				p++;
			}

			*p = duracao[0];
			p++;
			*p = duracao[1];
			p++;
			*p = duracao[2];
			p++;
			*p = duracao[3];
			p++;
			*p = duracao[4];
			p++;

			return;
		}
	}

	printf(REGISTRO_N_ENCONTRADO);
}

void remover(Hashtable *tabela){
	char buscaPk[9];
	int j, c=0;
	char *p;

	p = ARQUIVO;

	scanf("%8[^\n]", buscaPk);
    getchar();
    buscaPk[9] = '\0';

    for(j=0; j< tabela->tam; j++){
		if(strcmp(tabela->v[j].pk, buscaPk) == 0  && tabela->v[j].estado != REMOVIDO){
			//marcar no arquivo de dados com *| 
			//printf("VOU REMOVER %s\n", tabela->v[j].pk);
			tabela->v[j].estado = REMOVIDO;
			
			p+=tabela->v[j].rrn;
			while(c<192){
				*p = '*';
				p++;
				*p = '|';
				p++;
				c++;
			}
			
			return;
		}
	}

	printf(REGISTRO_N_ENCONTRADO);
}

void imprimir_tabela(Hashtable tabela){
	int i;

	for(i=0; i< tabela.tam; i++){
		if(tabela.v[i].estado == LIVRE)
			printf("[%d] Livre\n", i);

		if(tabela.v[i].estado == OCUPADO)
			printf("[%d] Ocupado: %s\n", i, tabela.v[i].pk);

		if(tabela.v[i].estado == REMOVIDO)
			printf("[%d] Removido: %s\n", i, tabela.v[i].pk);
	}
}

void liberar_tabela(Hashtable *tabela){
}

/* Leituras usadas no Trabalho 1, com algumas correções de erros. Feita em conjunto com Gabriel Alves*/
void le_equipe(char equipe_chamada[]) {
        char equipe[1000];
        int flag=0;
 
        do {
                flag = 0;
                scanf("%[^\n]", equipe);
                getchar();
                if(strlen(equipe) > 39) {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
        } while(flag);
        strcpy(equipe_chamada, equipe);
}
void le_data(char data_chamada[]) {
        char data[11];
        int flag=0;
 
        do {
                flag = 0;
                scanf("%10[^\n]", data);
                getchar();
                if(data[2] != '/' || data[5] != '/') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(data[0] == '0' && data[1] == '0') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;      
                }
                else if(data[3] == '0' && data[4] == '0') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(data[6] != '2' || data[7] != '0' || data[8] != '1') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(data[9] != '1' && data[9] != '2' && data[9] != '3' && data[9] != '4' && data[9] != '5') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(data[0] != '0' && data[0] != '1' && data[0] != '2' && data[0] != '3') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(data[0] == '3' && data[1] != '0' && data[1] != '1') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(data[3] != '0' && data[3] != '1') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(data[3] == '1' && data[4] != '0' && data[4] != '1' && data[4] != '2') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
        } while(flag);
 
        strcpy(data_chamada, data);
}
void le_duracao(char duracao_chamada[]) {
        char duracao[6];
        int flag=0;
 
        do {
                flag = 0;
                scanf("%5[^\n]", duracao);
                ignore();
 
                if(duracao[2] != ':') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(strlen(duracao) != 5) {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(duracao[0] == '0' && duracao[1] == '0' && duracao[3] == '0' && duracao[4] == '0') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(duracao[0] != '0' && duracao[0] != '1' && duracao[0] != '2' && duracao[0] != '3' && duracao[0] != '4' && duracao[0] != '5' && duracao[0] != '6' && duracao[0] != '7' && duracao[0] != '8' && duracao[0] != '9') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(duracao[1] != '0' && duracao[1] != '1' && duracao[1] != '2' && duracao[1] != '3' && duracao[1] != '4' && duracao[1] != '5' && duracao[1] != '6' && duracao[1] != '7' && duracao[1] != '8' && duracao[1] != '9') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(duracao[3] != '0' && duracao[3] != '1' && duracao[3] != '2' && duracao[3] != '3' && duracao[3] != '4' && duracao[3] != '5' && duracao[3] != '6' && duracao[3] != '7' && duracao[3] != '8' && duracao[3] != '9') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(duracao[3] == '7' || duracao[3] == '8' || duracao[3] == '9'){
                		printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(duracao[3] == '6' && duracao[4] != '0'){
                		printf(CAMPO_INVALIDO);
                        flag = 1;
                }
                else if(duracao[4] != '0' && duracao[4] != '1' && duracao[4] != '2' && duracao[4] != '3' && duracao[4] != '4' && duracao[4] != '5' && duracao[4] != '6' && duracao[4] != '7' && duracao[4] != '8' && duracao[4] != '9') {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
 
        } while(flag);
 
        strcpy(duracao_chamada, duracao);      
}
void le_vencedora(char vencedora_chamada[], char equipe_azul[], char equipe_vermelha[]) {
        int flag;
        char vencedora[40];
 
        do{
                flag = 0;
                scanf("%39[^\n]", vencedora);
                getchar();
 
                if(strcmp(equipe_azul, vencedora) != 0 && strcmp(equipe_vermelha, vencedora) != 0) {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
        }while(flag);
 
        strcpy(vencedora_chamada, vencedora);
}
void le_placar(char placar_chamada[]) {
        char placar[3];
        int tamanho_placar, flag=1;
 
        do {
                tamanho_placar = 0;
                flag = 0;
                scanf("%s[^\n]", placar);
                getchar();
                tamanho_placar = strlen(placar);
                if(tamanho_placar != 2 || (placar[0] != '0' && placar[0] != '1' && placar[0] != '2' && placar[0] != '3' && placar[0] != '4' && placar[0] != '5' && placar[0] != '6' && placar[0] != '7' && placar[0] != '8' && placar[0] != '9') || (placar[1] != '0' && placar[1] != '1' && placar[1] != '2' && placar[1] != '3' && placar[1] != '4' && placar[1] != '5' && placar[1] != '6' && placar[1] != '7' && placar[1] != '8' && placar[1] != '9') ) {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
        } while (flag);
 
        strcpy(placar_chamada, placar);
}
void le_apelido_mvp(char apelido_chamada[]) {
        char apelido_mvp[1000];
        int flag=0;
 
        do {
                flag = 0;
                scanf("%[^\n]", apelido_mvp);
                getchar();
                if(strlen(apelido_mvp) > 39) {
                        printf(CAMPO_INVALIDO);
                        flag = 1;
                }
        } while(flag);
 
        strcpy(apelido_chamada, apelido_mvp);
}