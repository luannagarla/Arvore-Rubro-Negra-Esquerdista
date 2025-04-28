/*
 * Implementação de Árvore Rubro-Negra Esquerdista (LLRB) em C
 * - Inserção e remoção
 * - Comentários explicativos linha a linha
 */

#include <stdio.h>
#include <stdlib.h>

// Enumeração para representar a cor do nó
enum cor
{
    VERMELHO,
    PRETO
};

// Ponteiro para nó
typedef struct no *p_no;

// Estrutura de um nó da árvore
struct no
{
    int chave;     // Valor armazenado no nó
    enum cor cor;  // Cor do nó (VERMELHO ou PRETO)
    p_no esq, dir; // Ponteiros para os filhos esquerdo e direito
};

// Verifica se um nó é vermelho
int ehVermelho(p_no x)
{
    if (x == NULL)
        return 0;              // NULL é considerado preto
    return x->cor == VERMELHO; // Retorna 1 se o nó é vermelho
}

// Verifica se um nó é preto
int ehPreto(p_no x)
{
    if (x == NULL)
        return 1;           // NULL é considerado preto
    return x->cor == PRETO; // Retorna 1 se o nó é preto
}

// Rotação para esquerda: corrige quando há um link vermelho à direita
p_no rotaciona_para_esquerda(p_no raiz)
{
    p_no x = raiz->dir;   // x será promovido
    raiz->dir = x->esq;   // filho esquerdo de x se torna filho direito de raiz
    x->esq = raiz;        // raiz se torna filho esquerdo de x
    x->cor = raiz->cor;   // x assume a cor original da raiz
    raiz->cor = VERMELHO; // raiz vira vermelho
    return x;             // retorna nova subárvore
}

// Rotação para direita: corrige dois links vermelhos consecutivos à esquerda
p_no rotaciona_para_direita(p_no raiz)
{
    p_no x = raiz->esq;   // x será promovido
    raiz->esq = x->dir;   // filho direito de x se torna filho esquerdo de raiz
    x->dir = raiz;        // raiz se torna filho direito de x
    x->cor = raiz->cor;   // x assume a cor da raiz
    raiz->cor = VERMELHO; // raiz vira vermelho
    return x;             // retorna nova subárvore
}

// Sobe a cor do nó: simula divisão de nó 4 em árvore 2-3-4
void sobe_vermelho(p_no raiz)
{
    raiz->cor = VERMELHO;   // raiz vira vermelha
    raiz->esq->cor = PRETO; // filhos viram pretos
    raiz->dir->cor = PRETO;
}

// Troca cores entre pai e filhos (utilizado em diversas operações)
void troca_cores(p_no h)
{
    h->cor = !h->cor; // inverte cor do nó
    if (h->esq)
        h->esq->cor = !h->esq->cor; // inverte esquerda
    if (h->dir)
        h->dir->cor = !h->dir->cor; // inverte direita
}

// Move vermelho para a esquerda (durante remoção)
p_no move_vermelho_para_esquerda(p_no h)
{
    troca_cores(h); // troca cor de h e seus filhos
    if (ehVermelho(h->dir->esq))
    { // caso especial: neto esquerdo do filho direito é vermelho
        h->dir = rotaciona_para_direita(h->dir);
        h = rotaciona_para_esquerda(h);
        troca_cores(h);
    }
    return h;
}

// Move vermelho para a direita (durante remoção)
p_no move_vermelho_para_direita(p_no h)
{
    troca_cores(h); // troca cores de h e filhos
    if (ehVermelho(h->esq->esq))
    { // neto esquerdo do filho esquerdo é vermelho
        h = rotaciona_para_direita(h);
        troca_cores(h);
    }
    return h;
}

// Retorna o nó com menor chave da subárvore
p_no minimo(p_no h)
{
    while (h->esq != NULL)
        h = h->esq;
    return h;
}

// Balanceia uma subárvore (após inserção ou remoção)
p_no balancear(p_no h)
{
    if (ehVermelho(h->dir) && ehPreto(h->esq))
        h = rotaciona_para_esquerda(h);
    if (ehVermelho(h->esq) && ehVermelho(h->esq->esq))
        h = rotaciona_para_direita(h);
    if (ehVermelho(h->esq) && ehVermelho(h->dir))
        troca_cores(h);
    return h;
}

// Inserção recursiva
p_no inserir_rec(p_no raiz, int chave)
{
    if (raiz == NULL)
    {
        // Caso base: insere novo nó vermelho
        p_no novo = malloc(sizeof(struct no));
        novo->chave = chave;
        novo->cor = VERMELHO;
        novo->esq = novo->dir = NULL;
        return novo;
    }
    if (chave < raiz->chave)
        raiz->esq = inserir_rec(raiz->esq, chave); // insere à esquerda
    else
        raiz->dir = inserir_rec(raiz->dir, chave); // insere à direita

    raiz = balancear(raiz); // corrige a estrutura após inserção
    return raiz;
}

// Função principal de inserção
p_no inserir(p_no raiz, int chave)
{
    raiz = inserir_rec(raiz, chave); // insere recursivamente
    raiz->cor = PRETO;               // raiz sempre deve ser preta
    return raiz;
}

// Remoção recursiva de um nó com chave específica
p_no remover_rec(p_no h, int chave)
{
    if (chave < h->chave)
    {
        if (ehPreto(h->esq) && ehPreto(h->esq->esq))
            h = move_vermelho_para_esquerda(h); // garante estrutura para remoção
        h->esq = remover_rec(h->esq, chave);
    }
    else
    {
        if (ehVermelho(h->esq))
            h = rotaciona_para_direita(h);
        if (chave == h->chave && h->dir == NULL)
        {
            free(h); // nó folha: remove diretamente
            return NULL;
        }
        if (ehPreto(h->dir) && ehPreto(h->dir->esq))
            h = move_vermelho_para_direita(h);
        if (chave == h->chave)
        {
            p_no x = minimo(h->dir);                // encontra sucessor
            h->chave = x->chave;                    // substitui chave
            h->dir = remover_rec(h->dir, x->chave); // remove sucessor
        }
        else
            h->dir = remover_rec(h->dir, chave);
    }
    return balancear(h); // rebalanceia após remoção
}

// Função principal de remoção
p_no remover(p_no raiz, int chave)
{
    if (!ehVermelho(raiz->esq) && !ehVermelho(raiz->dir))
        raiz->cor = VERMELHO; // prepara a raiz se necessário
    raiz = remover_rec(raiz, chave);
    if (raiz)
        raiz->cor = PRETO; // raiz sempre preta
    return raiz;
}

// Objetivo Um: Implementação de métodos que calculem a altura (total) e a altura negra de uma BST rubro-negra.
int altura_total(p_no raiz)
{
    if (raiz == NULL)
        return -1;

    int altura_esq = altura_total(raiz->esq);
    int altura_dir = altura_total(raiz->dir);
    return 1 + (altura_esq > altura_dir ? altura_esq : altura_dir);
}

int altura_negra(p_no raiz)
{
    if (raiz == NULL)
        return 0;

    int altura_esq = altura_negra(raiz->esq);
    int altura_dir = altura_negra(raiz->dir);
    int altura_sub = altura_esq > altura_dir ? altura_esq : altura_dir;
    return (raiz->cor == PRETO ? 1 : 0) + altura_sub;
}

// Objetivo dois: Desenvolvimento de um método que retorne a quantidade de nós pretos de uma árvore rubro-negra.
int total_nos_pretos(p_no raiz) {
    if (raiz == NULL)
        return 0;

    int total_esq = total_nos_pretos(raiz->esq);
    int total_dir = total_nos_pretos(raiz->dir);
    
    return (raiz->cor == PRETO ? 1 : 0) + total_esq + total_dir;
}

int main()
{
    p_no raiz = NULL;

    // Inserção
    int elementos[] = {29, 8, 20, 3, 9, 21, 02, 13};
    int n = sizeof(elementos) / sizeof(elementos[0]);

    printf("Inserindo elementos: ");
    for (int i = 0; i < n; i++)
    {
        printf("%d ", elementos[i]);
        raiz = inserir(raiz, elementos[i]);
    }

    // Remoção
    int para_remover[] = {9, 13};
    int m = sizeof(para_remover) / sizeof(para_remover[0]);

    printf("\nRemovendo elementos: ");
    for (int i = 0; i < m; i++)
    {
        printf("%d ", para_remover[i]);
        raiz = remover(raiz, para_remover[i]);
    }

    // Objetivo 01
    printf("\n\nAltura total da árvore: %d", altura_total(raiz));
    printf("\nAltura negra da árvore: %d\n", altura_negra(raiz));

    // Objetivo 02
    printf("\n\nTotal de nós pretos: %d", total_nos_pretos(raiz));

    return 0;
}