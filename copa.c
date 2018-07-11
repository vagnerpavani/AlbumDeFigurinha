#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXTENSION ".cup"
#define MAXFIGS 700    // trocar pelo numero correto de figurinhas que o album vai possuir
#define PAGES 50       // trocar pelo numero correto de paginas do album
#define FIGSPAGE 10    // trocar pelo numero correto de figurinhas por pagina
#define LIMPAR "clear" // "clear" para o linux , "cls" para o windows -> usado em system(LIMPAR);

// usar wb+ para criar arquivos, mas as funçoes devem retornar ponteiros RB+ para ediçao!!!!

typedef struct stickers
{
    unsigned int numero;
    unsigned int quantidade;
    char nome[20];
} figura;

typedef struct users
{
    char usuario[20];
    char password[10];
} user;

int ArqVazio(FILE *fp)
{
    int gravando = 0;
    figura vazia;
    vazia.nome[0] = '\0';
    vazia.quantidade = 0;
    int i;
    for (i = 1; i <= MAXFIGS; i++)
    {
        vazia.numero = i;
        gravando += fwrite(&vazia, sizeof(figura), 1, fp);
    }
    if (gravando == MAXFIGS)
    {
        printf("usuario criado com sucesso\n");
        return 1;
    }
    else
    {
        printf("falha ao criar usuario %d", gravando);
        return 0;
    }
}
FILE *cadastro()
{
    FILE *fp = NULL;
    int grav;
    char ext[] = EXTENSION;
    user novo;
    printf("Digite o nome do usuario ate 20 caracteres\n");
    scanf("%s", &novo.usuario[0]);

    if (strlen(novo.usuario) <= 20)
    {
        strcat(novo.usuario, ext);
        fp = fopen(novo.usuario, "rb");

        if (fp)
        {
            printf("esse usuario ja existe\n");
            fclose(fp);
            return NULL;
        }
        else
        {
            fp = fopen(novo.usuario, "wb+");
            fclose(fp);
            fp = fopen(novo.usuario, "rb+");
            printf("Digite a senha ate 10 caracteres\n");
            scanf(" %s", &novo.password[0]);
            rewind(fp);
            grav = fwrite(&novo, sizeof(user), 1, fp);
            ArqVazio(fp);
            return fp;
        }
    }
    else
    {
        printf("Digite um nome de usuario de ate 20 caracteres\n");
    }
    return NULL; // retornou NULL a gente sabe que ocorreu um erro na hora de criar o usuario;
}
void GravFig(int unsigned NumFig, int QuantFig, FILE *fp)
{
    int FigPos;
    figura grav;
    FigPos = NumFig - 1;
    fseek(fp, sizeof(user), SEEK_SET);
    if (NumFig == 1)
    {
        fread(&grav, sizeof(figura), 1, fp);
        grav.quantidade = grav.quantidade + QuantFig;
        fseek(fp, sizeof(user), SEEK_SET);
        fwrite(&grav, sizeof(figura), 1, fp);
    }
    else
    {
        fseek(fp, FigPos * sizeof(figura), SEEK_CUR);
        fread(&grav, sizeof(figura), 1, fp);
        grav.quantidade = grav.quantidade + QuantFig;
        fseek(fp, sizeof(user), SEEK_SET);
        fseek(fp, FigPos * sizeof(figura), SEEK_CUR);
        fwrite(&grav, sizeof(figura), 1, fp);
    }
}
FILE *login()
//funçao de login: retorna o ponteiro para o arquivo que o usuario abriu, dessa forma podemos identificar se o usuario ja esta logado por exemplo
{ //colocar a extensao que vai ser utilizada pelos arquivos binarios

    user usuario;
    user userLido;
    FILE *fp = NULL;
    printf("\nLogin:\n");
    printf("Digite o nome de usuario:\n");
    scanf("%s", &usuario.usuario[0]);
    if (strlen(usuario.usuario) > 20)
    {
        printf("o nome de usuario vai até 20 caracteres\n");
        return NULL;
    }
    else
    {
        strcat(usuario.usuario, EXTENSION);
        fp = fopen(usuario.usuario, "rb");
        if (fp)
        {

            fp = fopen(usuario.usuario, "rb+");
            printf("Digite a senha:\n");
            scanf(" %s", &usuario.password[0]);
            rewind(fp);
            fread(&userLido, sizeof(user), 1, fp);
            if (strcmp(userLido.password, usuario.password) == 0)
            {
                printf("Senha digitada com sucesso\n");
                return fp;
            }
            else
            {
                printf("Senha incorreta\n");
                return NULL;
            }
        }
        else
        {
            printf("Usuario nao encontrado \n");
            return NULL;
        }
    }
}
void ImprimirPag(FILE *fp, unsigned int page)
{
    printf("Pagina: %d\n", page);
    figura leitura;
    page = page - 1;
    fseek(fp, sizeof(user), SEEK_SET); //pula a senha com o cursor
    if (page == 0)
    {
        int i;
        for (i = 0; i < FIGSPAGE; i++)
        {
            fread(&leitura, sizeof(figura), 1, fp);
            printf(" %d %s %d\n", leitura.numero, leitura.nome, leitura.quantidade);
        }
    }
    else
    {
        int i;
        fseek(fp, FIGSPAGE * sizeof(figura) * page, SEEK_CUR);
        for (i = 0; i < FIGSPAGE; i++)
        {
            fread(&leitura, sizeof(figura), 1, fp);
            printf(" %d %s %d\n", leitura.numero, leitura.nome, leitura.quantidade);
        }
    }
}
int CmpArqv(FILE *fp1, FILE *fp2)
{
    user logados[2];
    rewind(fp1);
    rewind(fp2);
    fread(&logados[0], sizeof(user), 1, fp1);
    fread(&logados[1], sizeof(user), 1, fp2);

    if (strcmp(logados[0].usuario, logados[1].usuario) == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
void compra(FILE* fp){
	char escolha;
	int num, i;
	printf ("deseja comprar um pacote?\ns para sim / n para Nao  "); //adicionar probabilidade de vir figurinhas novas
	scanf ("%c", &escolha);
	if (escolha=='s'){
		printf("As figurinhas que vieram no pacote foram: ");
		srand(time(NULL));
		for (i = 0; i < 5; i++) {
           /* gerando valores aleat�rios entre 1 e o num da ultima figurinha */
           num = rand() % MAXFIGS;
		   printf("%d ", num);
           GravFig(num,  1, fp);
     	}
	}
}
int PrintRepetidas(FILE *fp)
{
    int i, contador = 0;
    figura leitura;
    fseek(fp, sizeof(user), SEEK_SET);
    printf("Figuras repetidas:\n");
    for (i = 0; i < MAXFIGS; i++)
    {
        fread(&leitura, sizeof(figura), 1, fp);
        if (leitura.quantidade > 1)
        {
            contador++;
            printf("%3d ", leitura.numero);
        }
        if (contador == 10)
        {
            printf("\n");
            contador = 0;
        }
    }
    printf("\n");
}
int CheckRepetidas(FILE *fp, int NumFig)
{
    figura lida;
    int FigPos = NumFig - 1;
    fseek(fp, sizeof(user), SEEK_SET);
    if (NumFig == 1)
    {
        fread(&lida, sizeof(figura), 1, fp);
    }
    else
    {
        fseek(fp, FigPos * sizeof(figura), SEEK_CUR);
        fread(&lida, sizeof(figura), 1, fp);
    }
    if (lida.quantidade > 1)
        return 1;
    else
        return 0;
}
void troca(FILE *fp1)
{
    FILE *fp2 = NULL;
    int resp = 1;
    int escolha1, escolha2;
    printf("Troca de Figurinhas!\n");
    printf("solicite o login do outro usuario:\n");
    while (fp2 == NULL)
    {
        fp2 = login();
        if (fp2 != NULL)
        {
            if (CmpArqv(fp1, fp2) == 0)
            {
                printf("Os dois usuarios logados sao iguais\n");
                printf("Logue com outro usuario\n");
                fp2 = NULL;
            }
        }
        else
        {
            printf("tente novamente com outro usuario\n");
        }
    }
    while (resp == 1)
    {
        printf("usuario 1: escolha uma figura do usuario 2\n");
        PrintRepetidas(fp2);
        scanf("%d", &escolha1);
        if (CheckRepetidas(fp2, escolha1) == 0)
        {
            printf("o usuario nao possui essa figura repetida\n");
            printf("escolha outra figura\n");
        }
        else 
        {
            resp = 0;
        }
    }
    resp = 1;
    while (resp == 1)
    {
        printf("usuario 2: escolha uma figura do usuario 1\n");
        PrintRepetidas(fp1);
        scanf("%d", &escolha2);
        if (CheckRepetidas(fp1 , escolha2) == 0)
        {
            printf("o usuario nao possui essa figura repetida\n");
            printf("escolha outra figura\n");
        }
        else
        {
            resp = 0;
        }
    }
    GravFig(escolha1,1,fp1);
    GravFig(escolha2 , -1 , fp1);
    GravFig(escolha2 , 1 , fp2);
    GravFig(escolha1 , -1 , fp2);
}
void main()
{
    int numerofig;
    int quantifig;
    int page;
    int add;
    FILE *user = NULL;
    user = login();

    printf("digite o numero da figura que deseja adicionar:\n");
    scanf("%d", &numerofig);
    printf("digite a quantidade que deseja adicionar:\n");
    scanf("%d", &quantifig);
    GravFig(numerofig, quantifig, user);
    printf("digite a pagina que deseja visualizar \n");
    scanf ("%d", &page);
    ImprimirPag(user,page);
    troca(user);
    printf("digite a pagina que deseja visualizar \n");
    scanf ("%d", &page);
    ImprimirPag(user,page);    
    
    //printf("digite a pagina que deseja visualizar\n");
    //scanf("%d",&page);
    //ImprimirPag(user, page);
    PrintRepetidas(user);
}