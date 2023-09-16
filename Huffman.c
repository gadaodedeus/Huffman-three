/*  COMENTARIOS PERTINENTES!!!
Meu programa utiliza de uma unica estrutura como elemento da lista e no da arvore que serao utilizados.
Alem disso utilizo um vetor de 256 (2exp8), em que o inidce do vetor representa o caracter e seu valor representa o peso
do caracter na arvore de huffman.
Outra tecnica utilizada que vale ressaltar é a utilizacao de um arquivo auxiliar (aux.huf) que guarda a string em binario
gerada. A opcao de criar um arquivo para isso foi para nao salvar a string binaria na ram, uma vez que isso pode consumir
muita ram.*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

typedef struct node {       //no da arvore
    char simbolo ;
    struct node * filho_esq ;
    struct node * filho_dir ;
    struct node * prox; //Proximo da lista
    int peso ; 
}no;

typedef struct{     //arvore
    no *inicio;
    int qnt;        //quantidade de caracteres inseridos na arvore //numero de folhas
}tree;

typedef struct t{       //elemento da tabela
    int c;
    int bin[16];
    struct t* prox;
    int tam;    //tamanho do binario gerado
}t_elem;

typedef struct{         //tabela de referencia binaria
    t_elem  *inicio, *fim;
    int tam;
}table;

void inicia_list(tree* l);
void add_list(no* n, tree* l);
tree* cria_arvore(int* v);
void cria_tabela(no* n, char* vet, int i);
void inicia_table(table* t);
void add_table(t_elem* e);
void cria_aux(char* arq);
char convert_byte ( char * vet , int n );
void convert ( char ch , char * Vet );

table* tabela_cod;
int flag=0;

int main()
{
    char c[50], buffer_c[20], arq_input[30], arq_output[50];
    FILE *f=malloc(sizeof(FILE)), *p=malloc(sizeof(FILE)); 
    int  qnt_bit = 0, i=0,ascii[255],j, op, cod_c, qnt_bin;;
    tree* root = malloc(sizeof(tree));

    root->inicio = NULL;
    root->qnt=0;
    no* aux = malloc(sizeof(no));

    tabela_cod = malloc(sizeof(table));     
    inicia_table(tabela_cod);


    printf("\n\t\t\tBem vindo ao UNESPZiP!");
    printf("\n\n\t\t1. Comprimir arquivo");
    printf("\n\n\t\t2. Descomprimir arquivo");
    scanf("%d", &op); 

    for(i=0;i<255;i++)
    {
        ascii[i]=0;
        if(i == 10)
            ascii[i] = -1;
    }
    

    ///////////////Comprime///////////////

    if(op == 1)
    { 
        
        printf("\nInforme o nome do arquivo: ");
        scanf("%s", arq_input);

        
        p = fopen(arq_input, "r");
        if(p == NULL)
        {
            printf("ERRO!\n\n");
            return 0;
        }
    

        
        while(i != EOF) //referencia de pesos e letras
        {
            i=getc(p);
            ascii[i]++;
        }
        fclose(p);


        root = cria_arvore(ascii);      //cria a arvore baseado no vetor ascii
        
        if (root == NULL)
        {
            printf("\n\t\tERRO: Arquivo indisponivel!!");
            return 0;
        }
       

        cria_tabela(root->inicio, c, 0);   //cria a tabela de referencia binaria
        cria_aux(arq_input);  

        //criacao do arquivo de saida
        printf("\nInforme o nome do arquivo comprimido: ");
        scanf("%s", arq_output);

        p = fopen("aux.huf", "r");
        if(p == NULL)
        {
            printf("\n\t\tERRO");
            return 0;
        }
            
        f = fopen(arq_output, "w");

        fprintf(f, "%d\n", root->qnt);    //print quantidade de letras
        aux = root->inicio;
        for(i=0;i<255;i++)
        {
            if(ascii[i]>0)
                fprintf(f, "%c %d\n", i, ascii[i]); //printa caracter e peso
        }

        //converte e printa codigo em ascii
         while(!feof(p))              
        {
           if(qnt_bit == 8)
            {
                cod_c = convert_byte(buffer_c, 8);
                putc(cod_c, f);
                qnt_bit = 0;
            }
            buffer_c[qnt_bit] = getc(p);
            qnt_bit++;
        }
       
        cod_c = convert_byte(buffer_c, 8);
        putc(cod_c, f);
        fclose(p);
        fclose(f);
    }



    ///////////////Descomprime///////////////

    else if(op == 2)
    {
        int tam, trash, num_letras_tt=0, k=0;
        printf("\nInforme o nome do arquivo: ");
        scanf("%s", arq_input); 
        p = fopen(arq_input, "r");
        if(p == NULL)
        {
            printf("ERRO!\n\n");
            return 1;
        }

        
        fscanf(p, "%d", &tam);
        while(tam>0)        //input dos carateres e pesos
        {
            trash=getc(p);  // \n
            i=getc(p);
            trash=getc(p);  // ' '
            fscanf(p, "%d", &ascii[i]);
            tam--;
            num_letras_tt+=ascii[i];
        }
    

        

        root = cria_arvore(ascii);          //cria arvore
        
        if (root == NULL)
        {
            printf("\n\t\tERRO: Arquivo indisponivel!!");
            return 0;
        }

        cria_tabela(root->inicio, c, 0);           //cria tabela

        printf("\nInforme o nome do arquivo comprimido: ");
        scanf("%s", arq_output);
        f = fopen(arq_output, "w");
        if(f == NULL)
        {
            printf("ERRO!\n\n");
            return 1;
        }
        
        i=getc(p);
        aux = root->inicio;
        while(num_letras_tt > k)         //enquanto tiver caracteres a serem printados, de acordo com o numero total de
        {                               //letras do arquivo original nao for atingido
            
            j=0;
            i=getc(p);
            convert(i, buffer_c);
            while(j<8 && num_letras_tt > k)              //enquanto nao termina de ler o byte
            {
                if(aux->filho_dir == aux->filho_esq)
                {
                    putc(aux->simbolo, f);      //ao chegar em uma folha printa o char da folha
                    aux=root->inicio;
                    k++;                    //numero de letras printadas
                }
                
                else if(buffer_c[j] == 49)  // 1 -> esquerda
                {
                    aux=aux->filho_esq;
                    j++;  
                }
        

                else if(buffer_c[j] == 48)  // 0 -> direita
                {
                    aux=aux->filho_dir;
                    j++;  
                
                }
                   
            }
        }       
        fclose(p); 
        fclose(f);  
    }

    remove("aux.huf");
        
    
    return 0;
}

void inicia_list(tree *l)   //inicia lista que sera usada para construir a arvore
{
    l->inicio = NULL;
    l->qnt = 0;
}

void add_list(no* n, tree* l)   //adiciona o elemento em ordem, baseado no peso de seu caracter
{
    no *aux = malloc(sizeof(no));
    no *ant = malloc(sizeof(no));
    

    if(!l->inicio)
    {
        l->inicio = n;
    }
    else 
    {
        aux = l->inicio;
        while(n->peso > aux->peso)
        {
            ant = aux;
            aux = aux->prox;
            if(aux == NULL)
                break;
        }

        if(l->inicio == aux)
        {
            if(aux!=NULL)
                n->prox = aux;
            l->inicio = n;
        }
        else
        {
            if(aux!=NULL)
                    n->prox = aux;
            ant->prox = n;

        }
        
    }
    l->qnt++;
}

tree* cria_arvore(int* v)   //cria arvore de huffman
{
   
    int i;
    tree *l = malloc(sizeof(tree));
    inicia_list(l);

    for(i=0;i<255;i++)  //percorre a tabela ascii e cria os nos da arvore
    {
        if(v[i] > 0)
        {
            no *temp = malloc(sizeof(no));
            temp->simbolo = i;
            temp->filho_dir = NULL;
            temp->filho_esq = NULL;
            temp->prox = NULL;
            temp->peso = v[i];
            add_list(temp, l); 
            if(i == 10)      //char '\n' deve ser informado quando esta presente no texto
                flag=1;         //que sera comprimido
        }
            
    }
    i=l->qnt;
    while(i>1)  //cria subarvores de modo que quando i = 1 temos a arvore pronta
    {   
        no *temp = malloc(sizeof(no));
        temp->simbolo = 254;
        
        temp->filho_esq = l->inicio;
        temp->peso = l->inicio->peso;
        l->inicio = l->inicio->prox;
        

        temp->filho_dir = l->inicio;
        temp->peso += l->inicio->peso;
        l->inicio = l->inicio->prox;

        add_list(temp, l);  //qnt++ //adiciona subarvores de volta a lista

        i--;
        l->qnt-=1;  //qnt--
    }

    return l;   //root
}

void cria_tabela(no* n, char* vet, int i)  //cria tabela de referencia
{   
    int j;
    if(n->filho_dir == n->filho_esq)    //ao encontrar uma folha cria-se um elemento da tabela
    {                                   //e ele é inserido na tabela
        t_elem* temp = malloc(sizeof(t_elem));
        for(j=0;j<i;j++)
        {
            temp->bin[j] = vet[j];
        }
        temp->c = n->simbolo;
        temp->tam=i;
        temp->prox = NULL;
        add_table(temp);   
        return;
    }
    
    vet[i] = '1';
    cria_tabela(n->filho_esq, vet, i+1);

    vet[i] = '0';
    cria_tabela(n->filho_dir, vet, i+1);
    
}

void add_table(t_elem* e)   //adiciona elemento no final da tabela
{
    if(tabela_cod->inicio == NULL)
    {
        tabela_cod->inicio = e;
        tabela_cod->fim = e;
    }
    else
    {
        tabela_cod->fim->prox = e;
        tabela_cod->fim = e;
    }
    
    tabela_cod->tam++;
}

void inicia_table(table* t) //inicia tabela
{
    t->inicio = NULL;
    t->fim = NULL;
    t->tam = 0;
}

void cria_aux(char* arq)    //cria arquivo auxiliar para salvar o binario gerado
{   
    int j=0, count=0, c2= 'r'; 
    t_elem* aux = malloc(sizeof(t_elem));
    FILE* f = fopen(arq, "r");
    FILE* p = fopen("aux.huf", "w");
    if(p == NULL)
    {
        printf("ERRO!\n\n");
        return ;
    }
   
    aux = tabela_cod->inicio;

    while(c2 != EOF)
    {   
        c2= getc(f);
        if((!flag && c2 == 10) || c2<0) //caso em que o char '\n' nao faz parte do texto original
            continue;

        while(c2 != aux->c)     //encontra o codigo do char
        {
            aux = aux->prox;
        }

        for(j=0; j<aux->tam; j++)   //printa codigo binario
        {
            fprintf(p,"%c", aux->bin[j]);
            count++;
        }
        
        aux=tabela_cod->inicio;
       
    }
    
    while(count % 8 != 0)   //completa com '0' os bits restantes
    {
        fprintf(p, "0");
        count++;
    }
        
    fclose(p);
    fclose(f);

}

char convert_byte ( char * vet , int n )
{
    char rtno = 0 , i = 0;

    while ( i < n )
    {
        rtno += pow(2, i) * ( vet [(n-1) -i ] - 48) ;
        i ++;
    }
    return rtno ;
}

void convert ( char ch , char * Vet )
{
    int i = 7; // indice auxiliar

    while ( i >=0 )
    {
        if (ch & 1)
            Vet [ i ] = '1';
        else
            Vet [ i ] = '0';
 
        i--;
        ch >>= 1;
    }
}
