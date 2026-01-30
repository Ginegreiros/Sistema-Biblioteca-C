#include <stdio.h> 
#include <stdlib.h> // para limpar a tela com system("cls") e funções de conversão
#include <locale.h> // para usar acentuação
#include <stdbool.h> // para usar o tipo bool
#include <string.h> 
#include <time.h> // para manipulação de datas e horas do sistema

// >>>> CONSTANTES GLOBAIS <<<<<

#define MAX_LIVROS 300
#define MAX_USUARIOS 1500
#define MAX_EMPRESTIMOS 2000

// >>>>>> ESTUTURAS DE DADOS <<<<<<

// estrutura para cadastro de livros
struct CadastroLivro
{ 
    int codigo;
    char titulo[100];
    char autor[80];
    char editora[60];
    int ano_publicacao; 
    int numero_livrosDisponiveis; 
    int total_emprestimos; // contador de quantas vezes o livro foi emprestado para métrica de popularidade
    bool disponivel; // para verificar se há exemplares disponíveis para empréstimo sem precisar contar
};

// estrutura para cadastro de usuários
struct CadastroUsuario
{
    char nomeCompleto[100];
    int matricula;
    char curso[50];
    char telefone[15];
    char dataCadastro[20];
};

// estrutura para controle de empréstimos
struct SisEmprestimo
{
    int cod_emprestimo;
    int matricula_usuario;    // chave estrangeira para Usuario
    int codigo_livro;         // chave estrangeira para Livro
    char data_emprestimo[20]; 
    char data_devolucao[20];  // Data prevista (Data atual + 7 dias)
    bool ativo;               // true = Livro ainda não devolvido, false = Livro devolvido
};

// Struct auxiliar para cálculos de data (evita manipulação direta de strings)
typedef struct {
    int dia, mes, ano;
} Data;

// função para obter o número de dias em um mês específico
int diasNoMes(int mes, int ano) {
    if (mes == 2) {
        // Verifica se é ano bissexto
        if ((ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0)) {
            return 29;
        } else {
            return 28;
        }
    } else if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        return 30;
    } else {
        return 31;
    }

}

// >>>>>>>>>> ESTRUTURA PRINCIPAL <<<<<<<<<<<
//Centraliza todos os dados do sistema para facilitar a passagem por referência
struct Biblioteca
{
    struct CadastroLivro livros[MAX_LIVROS];
    struct CadastroUsuario usuarios[MAX_USUARIOS];
    struct SisEmprestimo emprestimos[MAX_EMPRESTIMOS];
    int total_livros;
    int total_usuarios;
    int total_emprestimos;
};

// >>>>>>>> PROTÓTIPOS <<<<<<<<


// Funções de Cadastro
void CadastroLivro(struct Biblioteca *biblioteca);
void CadastroUsuario(struct Biblioteca *biblioteca);

// Funções de Pesquisa
void Pesquisar_livro(struct Biblioteca *biblioteca);
void Pesquisar_usuario(struct Biblioteca *biblioteca);

// Regras de Negócio (Empréstimos, Devoluções, Relatórios, etc)
void RegistrarEmprestimo(struct Biblioteca *biblioteca);
void DevolverLivro(struct Biblioteca *biblioteca);
void RenovarEmprestimo(struct Biblioteca *biblioteca);
void ListarEmprestimosAtivos(struct Biblioteca *biblioteca);

//Relatórios
void RelatorioAtrasos(struct Biblioteca *biblioteca);
void RelatorioLivrosPopulares(struct Biblioteca *biblioteca);

// persistência e utilitários
void SalvarDados(struct Biblioteca *biblioteca);
void CarregarDados(struct Biblioteca *biblioteca);
void AdicionarDias(Data *data, int dias);
void ObterDataAtual(Data *data);
int diasNoMes(int mes, int ano);
void Sair(struct Biblioteca *biblioteca);


//==============================================
// função principal
//==============================================

int main()
{
    setlocale(LC_ALL, "Portuguese"); // configura a acentuação para português

    // "static" permite que a struct seja alocada na memória estática (heap/data segment)
    // ao invés da pilha (stack), prevenindo Stack Overflow se os arrays forem grandes.
    static struct Biblioteca biblioteca = {.total_livros = 0, .total_usuarios = 0, .total_emprestimos = 0};

    // Recupera os dados salvos em arquivo, se existirem
    CarregarDados(&biblioteca);

    // declaração da variável para a opção do menu
    int opcao;

    // loop do menu principal
    // garante que o menu será exibido até o usuário escolher sair
    do
    {

        system("clear || cls"); // limpa a tela (funciona em Windows ,Linux e MacOS)

        // exibição do menu principal
        printf("================== Sistema de Biblioteca =================\n");
        printf("\n--------------------- Menu Principal ---------------------\n");

        printf("\n1. Cadastro de Livros\n");
        printf("2. Cadastro de Usuários\n");
        printf("3. Sistema de Empréstimos\n");
        printf("4. Devolver Livros\n");
        printf("5. Pesquisar Livro\n");
        printf("6. Pesquisar Usuário\n");
        printf("7. Relatório de atrasos\n");
        printf("8. Listar Empréstimos Ativos\n");
        printf("9. Renovar Empréstimo\n");
        printf("10. Relatório de Livros Populares\n");
        printf("11. Sair\n");
        printf("\nEscolha uma opção: ");

        scanf("%d", &opcao);
        getchar(); // Limpar o buffer do teclado

        // case para chamar as funções conforme a opção escolhida
        switch (opcao)
        {

        case 1:
            CadastroLivro(&biblioteca);

            break;
        case 2:
            CadastroUsuario(&biblioteca);

            break;
        case 3:
            RegistrarEmprestimo(&biblioteca);

            break;
        case 4:
            DevolverLivro(&biblioteca);

            break;

        case 5:
            Pesquisar_livro(&biblioteca);

            break;

        case 6:
            Pesquisar_usuario(&biblioteca);

            break;

        case 7:
            RelatorioAtrasos(&biblioteca);

            break;

        case 8:
            ListarEmprestimosAtivos(&biblioteca);

            break;

        case 9:
            RenovarEmprestimo(&biblioteca);

            break;

        case 10: 
            RelatorioLivrosPopulares(&biblioteca);

            break;

        case 11:
            Sair(&biblioteca);

            break;

        default:
            printf("Opção inválida. Tente novamente.\n");

            break;
        }

        if (opcao != 11) {
            printf("\nPressione ENTER para continuar...");
            getchar(); // espera o usuário pressionar Enter antes de continuar
        }

    } while (opcao != 11); // o menu continua até o usuário escolher a opção 11 para sair

    return 0;
}

//==============================================
// implementação das funções
//==============================================


// ----------------- CADASTROS -------------------

void CadastroLivro(struct Biblioteca *biblioteca)
{

    // Verifica se o limite máximo de livros foi atingido
    if (biblioteca->total_livros >= MAX_LIVROS)
    {
        printf("Limite máximo de livros cadastrados atingidos.\n");
        return;
    }

    // Ponteiro auxiliar para simplificar o acesso ao índice atual
    struct CadastroLivro *cadastro = &biblioteca->livros[biblioteca->total_livros]; // Aponta para o próximo slot disponível no array de livros

    printf("\n============= Cadastro de livro ==========\n");

    printf("Qual é o código do livro? ");
    scanf("%d", &cadastro->codigo);
    getchar();


    // Validação de unicidade evita códigos duplicados
    for (int i = 0; i < biblioteca->total_livros; i++)
    {

        if (biblioteca->livros[i].codigo == cadastro->codigo)
        {
            printf("Código já cadastrado no sistema.\n");
            return;
        }
    }


    printf("Qual é o titulo do livro? ");
    fgets(cadastro->titulo, sizeof(cadastro->titulo), stdin);
    cadastro->titulo[strcspn(cadastro->titulo, "\n")] = 0; // remove o \n final

    printf("Quem é o autor do livro? ");
    fgets(cadastro->autor, sizeof(cadastro->autor), stdin);
    cadastro->autor[strcspn(cadastro->autor, "\n")] = 0;

    printf("Qual é a editora do livro? ");
    fgets(cadastro->editora, sizeof(cadastro->editora), stdin);
    cadastro->editora[strcspn(cadastro->editora, "\n")] = 0;

    printf("Qual é o ano de publicação do livro? ");
    scanf("%d", &cadastro->ano_publicacao);
    getchar();

    printf("Quantos exemplares do livro estão disponíveis? ");
    scanf("%d", &cadastro->numero_livrosDisponiveis);
    getchar();

    cadastro->total_emprestimos = 0; // inicializa o contador de empréstimos

    biblioteca->total_livros++; // Incrementa a contagem

    // Defini a disponibilidade inicial com base no número de exemplares disponíveis
    cadastro->disponivel = (cadastro->numero_livrosDisponiveis > 0) ? true : false;

    SalvarDados(biblioteca); // Backup automático após o cadastro

    printf("\nLivro cadastrado com sucesso!\n\n\n");
}

void CadastroUsuario(struct Biblioteca *biblioteca)
{

    // Verifica se o limite máximo de usuários foi atingido
    if (biblioteca->total_usuarios >= MAX_USUARIOS)
    {
        printf("Limite máximo de usuários cadastrados atingidos.\n");
        return;
    }

    // Declaração de uma variável local para armazenar os dados do usuário
    // usando ponteiro para modificar diretamente o array na estrutura principal
    struct CadastroUsuario *usuario = &biblioteca->usuarios[biblioteca->total_usuarios]; // Aponta para o próximo slot disponível no array de usuários

    printf("Cadastre um novo usuário\n");

    printf("Qual é a matrícula do usuário? ");
    scanf("%d", &usuario->matricula);
    getchar();

    // Validação de unicidade evita matrículas duplicadas
    for (int i = 0; i < biblioteca->total_usuarios; i++)
    {
        if (biblioteca->usuarios[i].matricula == usuario->matricula)
        {
            printf("Matrícula já cadastrada no sistema.\n");
            return;
        }
    }

    printf("Qual é o nome completo do usuário? ");
    fgets(usuario->nomeCompleto, sizeof(usuario->nomeCompleto), stdin);
    usuario->nomeCompleto[strcspn(usuario->nomeCompleto, "\n")] = 0;

    printf("Qual é o curso do usuário? ");
    fgets(usuario->curso, sizeof(usuario->curso), stdin);
    usuario->curso[strcspn(usuario->curso, "\n")] = 0;

    printf("Qual é o telefone do usuário? ");
    fgets(usuario->telefone, sizeof(usuario->telefone), stdin);
    usuario->telefone[strcspn(usuario->telefone, "\n")] = 0;

    // Registra a data do sistema no momento do cadastro
    time_t agora = time(NULL);
    struct tm *data_cadastro = localtime(&agora);
    strftime(usuario->dataCadastro, sizeof(usuario->dataCadastro), "%d/%m/%Y", data_cadastro);

    biblioteca->total_usuarios++;

    SalvarDados(biblioteca); // salva os dados após o cadastro

    printf("\nUsuário cadastrado com sucesso!\n");
}


// ----------------- PESQUISAS -------------------

void Pesquisar_livro(struct Biblioteca *biblioteca)
{
    if (biblioteca->total_livros == 0)
    {
        printf("Nenhum livro cadastrado no sistema.\n");
        return;
    }

    // declaração da variável para a opção de pesquisa e controle de fluxo
    int opcao;

    // exibição do menu de opções de pesquisa
    printf("\n====================== Pesquisar Livro ======================\n");
    printf("1. Pesquisar por título\n");
    printf("2. Pesquisar por autor\n");
    printf("3. Pesquisar por código\n");
    printf("4. Busca avançada (Autor e ano de publicação)\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    getchar();

    int encontrado = 0;
    char termo[100]; // para armazenar o termo de busca
    int codigoBusca;

    // Lógica unificada de busca para evitar repetição de código

    // pesquisa por título
    if (opcao == 1)
    {
        printf("Digite o título do livro: ");
        fgets(termo, sizeof(termo), stdin);
        termo[strcspn(termo, "\n")] = 0;

        for (int i = 0; i < biblioteca->total_livros; i++)
        {
            // strstr verifica se o termo faz parte da string (busca parcial)
            if (strstr(biblioteca->livros[i].titulo, termo))
            {
                printf("\nCódigo: %d\n", biblioteca->livros[i].codigo);
                printf("Título: %s\n", biblioteca->livros[i].titulo);
                printf("Autor: %s\n", biblioteca->livros[i].autor);
                printf("Editora: %s\n", biblioteca->livros[i].editora);
                printf("Ano de publicação: %d\n", biblioteca->livros[i].ano_publicacao);
                printf("Número de exemplares disponíveis: %d\n", biblioteca->livros[i].numero_livrosDisponiveis);
                printf("Disponível: %s\n", biblioteca->livros[i].disponivel ? "Sim" : "Não");
                encontrado = 1;
            }
        }
    }

    // pesquisa por autor
    else if (opcao == 2)
    {
        printf("Digite o autor: ");
        fgets(termo, sizeof(termo), stdin);
        termo[strcspn(termo, "\n")] = 0;

        for (int i = 0; i < biblioteca->total_livros; i++)
        {
            if (strstr(biblioteca->livros[i].autor, termo))
            {

                printf("\nCódigo: %d\n", biblioteca->livros[i].codigo);
                printf("Título: %s\n", biblioteca->livros[i].titulo);
                printf("Autor: %s\n", biblioteca->livros[i].autor);
                printf("Editora: %s\n", biblioteca->livros[i].editora);
                printf("Ano de publicação: %d\n", biblioteca->livros[i].ano_publicacao);
                printf("Número de exemplares disponíveis: %d\n", biblioteca->livros[i].numero_livrosDisponiveis);
                printf("Disponível: %s\n", biblioteca->livros[i].disponivel ? "Sim" : "Não");
                encontrado = 1;
            }
        }
    }

    // pesquisa por código
    else if (opcao == 3)
    {
        printf("Digite o código: ");
        scanf("%d", &codigoBusca);
        getchar();

        for (int i = 0; i < biblioteca->total_livros; i++)
        {
            if (biblioteca->livros[i].codigo == codigoBusca)
            {
                printf("\nCódigo: %d\n", biblioteca->livros[i].codigo);
                printf("Título: %s\n", biblioteca->livros[i].titulo);
                printf("Autor: %s\n", biblioteca->livros[i].autor);
                printf("Editora: %s\n", biblioteca->livros[i].editora);
                printf("Ano de publicação: %d\n", biblioteca->livros[i].ano_publicacao);
                printf("Número de exemplares disponíveis: %d\n", biblioteca->livros[i].numero_livrosDisponiveis);
                printf("Disponível: %s\n", biblioteca->livros[i].disponivel ? "Sim" : "Não");
                encontrado = 1;
                break; // Se achou pelo código, não precisa continuar procurando
            }
        }
    }
    // Busca avançada (Autor e ano de publicação)
    else if (opcao == 4) {

        char autor[80];
        int ano_publicacao;

        printf("Digite o autor: ");
        fgets(autor, sizeof(autor), stdin);
        autor[strcspn(autor, "\n")] = 0;

        printf("Digite o ano de publicação: ");
        scanf("%d", &ano_publicacao);
        getchar();

        printf("\nBuscando livros de %s publicados em %d...\n", autor, ano_publicacao);

        for (int i = 0; i < biblioteca->total_livros; i++) {

            // verifica se o autor e o ano de publicação correspondem
            if (strstr(biblioteca->livros[i].autor, autor) && biblioteca->livros[i].ano_publicacao == ano_publicacao) {

                printf("\n Resultado encontrado:\n");
                printf("Título: %s\n", biblioteca->livros[i].titulo);
                printf("Código: %d\n", biblioteca->livros[i].codigo);
                printf("Autor: %s\n", biblioteca->livros[i].autor);
                printf("Ano de publicação: %d\n", biblioteca->livros[i].ano_publicacao);
                printf("Editora: %s\n", biblioteca->livros[i].editora);

                encontrado = 1;
            }
        }

        if (!encontrado)
            printf("Nenhum livro encontrado para os critérios especificados.\n");
    }

    else
    {
        printf("Opção inválida.\n");
    }
}

// função para pesquisar usuários
void Pesquisar_usuario(struct Biblioteca *biblioteca)
{

    if (biblioteca->total_usuarios == 0)
    {
        printf("Nenhum usuário cadastrado no sistema.\n");
        return;
    }

    // declaração da variável para a opção de pesquisa e controle de fluxo
    int opcao;

    // exibição do menu de opções de pesquisa
    printf("\n======================== Pesquisar Usuário =======================\n");
    printf("1. Pesquisar por matrícula\n");
    printf("2. Pesquisar por nome\n\n");
    printf("Escolha uma opção: ");
    scanf("%d", &opcao);
    getchar();

    int encontrado = 0;

    // verifica se há usuários cadastrados
    if (biblioteca->total_usuarios == 0)
    {

        printf("Nenhum usuário cadastrado no sistema.\n");
        return;
    }

    // pesquisa por matrícula
    if (opcao == 1)
    {
        int matriculaBusca;
        printf("Digite a matrícula: ");
        scanf("%d", &matriculaBusca);
        getchar();

        for (int i = 0; i < biblioteca->total_usuarios; i++)
        {

            if (biblioteca->usuarios[i].matricula == matriculaBusca)
            {
                printf("\nUsuário encontrado:\n");
                printf("Nome completo: %s\n", biblioteca->usuarios[i].nomeCompleto);
                printf("Matrícula: %d\n", biblioteca->usuarios[i].matricula);
                printf("Curso: %s\n", biblioteca->usuarios[i].curso);
                printf("Telefone: %s\n", biblioteca->usuarios[i].telefone);
                printf("Data de cadastro: %s\n", biblioteca->usuarios[i].dataCadastro);
                encontrado = 1;
                return;
            }
        }

    }

    // pesquisa por nome
    else if (opcao == 2)
    {
        char nomeBusca[100];

        printf("Digite o nome completo: ");
        fgets(nomeBusca, sizeof(nomeBusca), stdin);
        nomeBusca[strcspn(nomeBusca, "\n")] = 0;

        for (int i = 0; i < biblioteca->total_usuarios; i++)
        {
            if (strstr(biblioteca->usuarios[i].nomeCompleto, nomeBusca))
            { 
                printf("\n Nome completo: %s\n", biblioteca->usuarios[i].nomeCompleto);
                printf("Matrícula: %d\n", biblioteca->usuarios[i].matricula);
                printf("Curso: %s\n", biblioteca->usuarios[i].curso);
                printf("Telefone: %s\n", biblioteca->usuarios[i].telefone);
                printf("Data de cadastro: %s\n", biblioteca->usuarios[i].dataCadastro);
                encontrado = 1;
            } 
        }
    

        if (!encontrado)
            printf("Nenhum usuário encontrado.\n");

    }
}

//---------------- ULTILITÁRIOS DE DATA -----------------

void ObterDataAtual(Data *data) {
    time_t agora = time(NULL);
    struct tm *data_atual = localtime(&agora);

    data->dia = data_atual->tm_mday;
    data->mes = data_atual->tm_mon + 1;     // tm_mon é de 0 a 11 então soma 1
    data->ano = data_atual->tm_year + 1900; // tm_year é anos desde 1900
}

// lÓGICA DE CALENDÁRIO PARA SOMAR DIAS (VIRADA DE MÊS/ANO)
void AdicionarDias(Data *data, int dias) {
    data->dia += dias;

    while (data->dia > diasNoMes(data->mes, data->ano)) {
        data->dia -= diasNoMes(data->mes, data->ano);
        data->mes++;

        if (data->mes > 12) {
            data->mes = 1;
            data->ano++;
        }
    }
}

// --------------------------- REGRAS DE NEGÓCIO -------------------

void RegistrarEmprestimo(struct Biblioteca *biblioteca)
{

    // Declaração de uma variável local para armazenar os dados do empréstimo
    // usando ponteiro para modificar diretamente o array na estrutura principal
    struct SisEmprestimo *emprestimo = &biblioteca->emprestimos[biblioteca->total_emprestimos]; // Aponta para o próximo slot disponível no array de empréstimo
    printf("\n======================== Registrar Empréstimo =======================\n");

    printf("Qual é o código do empréstimo? ");
    scanf("%d", &emprestimo->cod_emprestimo);

    printf("Qual é a matrícula do usuário? ");
    scanf("%d", &emprestimo->matricula_usuario);

    printf("Qual é o código do livro? ");
    scanf("%d", &emprestimo->codigo_livro);
    getchar();

    // Busca o livro e verifica disponibilidade
    for (int i = 0; i < biblioteca->total_livros; i++)
    {

        if (biblioteca->livros[i].codigo == emprestimo->codigo_livro)
        {

            if (biblioteca->livros[i].disponivel)
            {

                // decrementa o número de livros disponíveis
                biblioteca->livros[i].numero_livrosDisponiveis--;

                biblioteca->livros[i].total_emprestimos++; // incrementa o contador de empréstimos do livro

                // se não houver mais livros disponíveis, marca como indisponível
                if (biblioteca->livros[i].numero_livrosDisponiveis == 0)
                {
                    biblioteca->livros[i].disponivel = false;
                }

                break;
            }
            else
            {
                printf("Livro não disponível para empréstimo.\n");
                return;
            }
        }
    }

    // Registra as datas de empréstimo e devolução
    Data hoje;
    ObterDataAtual(&hoje);
    Data devolucao = hoje;
    AdicionarDias(&devolucao, 7);

    // formata para string para salvar na struct
    sprintf(emprestimo->data_emprestimo, "%02d/%02d/%04d", hoje.dia, hoje.mes, hoje.ano);

    sprintf(emprestimo->data_devolucao, "%02d/%02d/%04d", devolucao.dia, devolucao.mes, devolucao.ano);

    emprestimo->ativo = true; // marca o empréstimo como ativo
    biblioteca->total_emprestimos++; // incrementa contagem

    SalvarDados(biblioteca); // salva os dados após o registro do empréstimo

    printf("\nEmpréstimo registrado com sucesso!\n");
    printf("Data de empréstimo: %s\n", emprestimo->data_emprestimo);
    printf("Data de devolução: %s\n\n", emprestimo->data_devolucao);

}

void RenovarEmprestimo(struct Biblioteca *biblioteca)
{
    int codigo_emprestimo;
    printf("\n======================== Renovar Empréstimo =======================\n");

    //achar o empréstimo
    printf("Digite o código do empréstimo: ");
    scanf("%d", &codigo_emprestimo);
    getchar();

    // loop para encontrar o empréstimo
    for (int i = 0; i < biblioteca->total_emprestimos; i++)
    {
        // verifica se o código do empréstimo corresponde e se está ativo
        if (biblioteca->emprestimos[i].cod_emprestimo == codigo_emprestimo )
        {
            if (!biblioteca->emprestimos[i].ativo)
            {
                printf("Este empréstimo já foi finalizado e não pode ser renovado.\n");
                return;
            }

            // calcula nova data (hoje + 7 dias)

            Data hoje, nova_devolucao;
            ObterDataAtual(&hoje);
            nova_devolucao = hoje;
            AdicionarDias(&nova_devolucao, 7);

            struct SisEmprestimo *emprestimo = &biblioteca->emprestimos[i];
        
            sprintf(emprestimo->data_devolucao, "%02d/%02d/%04d", nova_devolucao.dia, nova_devolucao.mes, nova_devolucao.ano);

            SalvarDados(biblioteca); // salva os dados após a renovação do empréstimo
        
            printf("\nEmpréstimo renovado com sucesso!\n");
            printf("Nova data de devolução: %s\n\n", emprestimo->data_devolucao);
        
            return;

        }
    }

    printf("Empréstimo não encontrado.\n");
    
}

void DevolverLivro(struct Biblioteca *biblioteca)
{
    int codigo_emprestimo;
    int encontrado = -1; // indica o índice do empréstimo encontrado

    printf("\n======================== Devolver Livro =======================\n");
    printf("Qual é o código do empréstimo? ");
    scanf("%d", &codigo_emprestimo);
    getchar();

    for (int i = 0; i < biblioteca->total_emprestimos; i++)
    {
        if (biblioteca->emprestimos[i].cod_emprestimo == codigo_emprestimo && biblioteca->emprestimos[i].ativo)
        {
            encontrado = i;
            break;
        }
    }

    if (encontrado == -1)
    {
        printf("Empréstimo não encontrado ou já devolvido.\n");
        return;
    }

    // marca como devolvido
    biblioteca->emprestimos[encontrado].ativo = false;

    // Devolve o livro (incrementa o número de livros disponíveis)
    int codigo_livro = biblioteca->emprestimos[encontrado].codigo_livro;
    for (int j = 0; j < biblioteca->total_livros; j++)
    {
        if (biblioteca->livros[j].codigo == codigo_livro)
        {
            biblioteca->livros[j].numero_livrosDisponiveis++;
            biblioteca->livros[j].disponivel = true; // marca como disponível
            break;
        }
    }

    SalvarDados(biblioteca); // salva os dados após a devolução

    printf("Livro devolvido com sucesso!\n");
}

// função para devolver livros
void ListarEmprestimosAtivos(struct Biblioteca *biblioteca)
{
    printf("\n======================== Empréstimos Ativos =========================\n");

    int encontrados = 0;

    for (int i = 0; i < biblioteca->total_emprestimos; i++)
    {
        if (biblioteca->emprestimos[i].ativo)
        {
            printf("\nCódigo do Empréstimo: %d\n", biblioteca->emprestimos[i].cod_emprestimo);
            printf("Matrícula do Usuário: %d\n", biblioteca->emprestimos[i].matricula_usuario);
            printf("Código do Livro: %d\n", biblioteca->emprestimos[i].codigo_livro);
            printf("Data de Empréstimo: %s\n", biblioteca->emprestimos[i].data_emprestimo);
            printf("Data de Devolução: %s\n", biblioteca->emprestimos[i].data_devolucao);
            encontrados++;
        }
    }

    if (encontrados == 0)
    {
        printf("Nenhum empréstimo ativo encontrado.\n");
    }

}

//------------------- Relatórios -------------------

void RelatorioAtrasos(struct Biblioteca *biblioteca)
{
    printf("\n======================== Relatório de Atrasos =======================\n");

    Data hoje;
    ObterDataAtual(&hoje);
    printf ("Data atual: %02d/%02d/%04d\n", hoje.dia, hoje.mes, hoje.ano);

    int atrasos_encontrados = 0;

    for (int i = 0; i < biblioteca->total_emprestimos; i++)
    {
        // verifica se o empréstimo está ativo
        if (biblioteca->emprestimos[i].ativo)
        {
            int dia_dev, mes_dev, ano_dev;
            sscanf(biblioteca->emprestimos[i].data_devolucao, "%02d/%02d/%04d", &dia_dev, &mes_dev, &ano_dev);

            // Lógica de comparação de datas
            bool atrasado = false;

            if (ano_dev < hoje.ano)
            {
                atrasado = true;
            }
            else if (ano_dev == hoje.ano)
            {
                if (mes_dev < hoje.mes)
                {
                    atrasado = true;
                }
                else if (mes_dev == hoje.mes)
                {
                    if (dia_dev < hoje.dia)
                    {
                        atrasado = true;
                    }
                }
            }
            if (atrasado)
            {
                printf("\n--- Empréstimo Atrasado ---\n");
                printf("\nEmpréstimo Código: %d\n", biblioteca->emprestimos[i].cod_emprestimo);
                printf("Matrícula do Usuário: %d\n", biblioteca->emprestimos[i].matricula_usuario);
                printf("Código do Livro: %d\n", biblioteca->emprestimos[i].codigo_livro);
                printf("Data de Empréstimo: %s\n", biblioteca->emprestimos[i].data_emprestimo);
                printf("Data de Devolução: %s\n", biblioteca->emprestimos[i].data_devolucao);
                atrasos_encontrados++;

            }
        }

    }
        if (atrasos_encontrados == 0)
        {
            printf("Nenhum empréstimo atrasado encontrado.\n");
        }
}

void RelatorioLivrosPopulares(struct Biblioteca *biblioteca){

    // verificação inicial
    if (biblioteca->total_livros == 0)
    {
        printf("Sem dados para relatório.\n");
        return;
    }

    // Cópia para não alterar o array original na ordenação
    static struct CadastroLivro copia[MAX_LIVROS];
    for (int i = 0; i < biblioteca->total_livros; i++)
    {
        copia[i] = biblioteca->livros[i];
    }

    // Bubble sort para ordenar os livros por total_emprestimos (maior para menor)
    struct CadastroLivro temp;
    for (int i =0 ; i < biblioteca->total_livros - 1; i++)
    {
        for (int j = 0; j < biblioteca->total_livros - i - 1; j++)
        {
            if (copia[j].total_emprestimos < copia[j+1].total_emprestimos)
            {
                temp = copia[j];
                copia[j] = copia[j+1];
                copia[j+1] = temp;
            }
        }
    }

    printf("\n======================== Relatório de Livros mais emprestados =======================\n");

    int listou = 0;

    for (int i = 0; i < biblioteca->total_livros; i++){
        if (copia[i].total_emprestimos > 0){
            printf("%d° : %s (Total: %d)\n", i+1, copia[i].titulo, copia[i].total_emprestimos);
            listou = 1;
        }

    }

    // se nenhum livro foi emprestado ainda
    if (!listou){
        printf("Nenhum livro foi emprestado ainda.\n");
    }

    printf("=============================================================================\n");

}

// ------------------- PERSISTÊNCIA (SALVAR DADOS) -------------------

void SalvarDados(struct Biblioteca *biblioteca)
{
    FILE *arquivo_livros_txt = NULL, *arquivo_livros_bak = NULL;
    FILE *arquivo_usuarios_txt = NULL, *arquivo_usuarios_bak = NULL;
    FILE *arquivo_emprestimos_txt = NULL, *arquivo_emprestimos_bak = NULL;

    arquivo_livros_txt = fopen("livros.txt", "w");
    arquivo_livros_bak = fopen("livros.bak","w");

    // só continua se os arquivos foram abertos corretamente

    if (arquivo_livros_txt && arquivo_livros_bak )
    {
        for ( int i = 0; i < biblioteca->total_livros; i++)
        {
            
            // Usa o formato pipe-separated para facilitar a leitura com sscanf

            fprintf(arquivo_livros_txt, "%d|%s|%s|%s|%d|%d|%d|%d\n",
                    biblioteca->livros[i].codigo, 
                    biblioteca->livros[i].titulo,
                    biblioteca->livros[i].autor, 
                    biblioteca->livros[i].editora,
                    biblioteca->livros[i].ano_publicacao, biblioteca->livros[i].numero_livrosDisponiveis,
                    biblioteca->livros[i].disponivel ? 1 : 0,
                    biblioteca->livros[i].total_emprestimos );
                    
            // Gera um arquivo de backup (.bak) para segurança dos dados
            fprintf(arquivo_livros_bak,"%d|%s|%s|%s|%d|%d|%d|%d\n",
                    biblioteca->livros[i].codigo, 
                    biblioteca->livros[i].titulo,
                    biblioteca->livros[i].autor, 
                    biblioteca->livros[i].editora,
                    biblioteca->livros[i].ano_publicacao, biblioteca->livros[i].numero_livrosDisponiveis,
                    biblioteca->livros[i].disponivel ? 1 : 0,
                    biblioteca->livros[i].total_emprestimos );  

        }

        fclose(arquivo_livros_txt); // fecha o txt
        fclose(arquivo_livros_bak); // fecha o backup

    }else{
        printf("Erro: Não foi possível abrir os arquivos dos livros.\n");

        if(arquivo_livros_txt) fclose(arquivo_livros_txt);
        if(arquivo_livros_bak) fclose(arquivo_livros_bak);

    }
     
    arquivo_usuarios_txt = fopen("usuarios.txt", "w");
    arquivo_usuarios_bak = fopen("usuarios.bak", "w");

    if (arquivo_usuarios_txt  && arquivo_usuarios_bak )
    {

        for (int i = 0; i < biblioteca->total_usuarios; i++)
        {
            fprintf(arquivo_usuarios_txt, "%s|%d|%s|%s|%s\n",
                    biblioteca->usuarios[i].nomeCompleto, 
                    biblioteca->usuarios[i].matricula,
                    biblioteca->usuarios[i].curso,
                    biblioteca->usuarios[i].telefone,
                    biblioteca->usuarios[i].dataCadastro);

            fprintf(arquivo_usuarios_bak, "%s|%d|%s|%s|%s\n",
                    biblioteca->usuarios[i].nomeCompleto, 
                    biblioteca->usuarios[i].matricula,
                    biblioteca->usuarios[i].curso,
                    biblioteca->usuarios[i].telefone,
                    biblioteca->usuarios[i].dataCadastro);

        }

        fclose(arquivo_usuarios_txt);
        fclose(arquivo_usuarios_bak);

    }else{
        printf("Erro: Não foi possível abrir os arquivos dos usuários.\n");

        if(arquivo_usuarios_txt) fclose(arquivo_usuarios_txt);
        if(arquivo_usuarios_bak) fclose(arquivo_usuarios_bak);

    }

    arquivo_emprestimos_txt = fopen("emprestimos.txt", "w");
    arquivo_emprestimos_bak = fopen("emprestimos.bak", "w");

    if( arquivo_emprestimos_txt && arquivo_emprestimos_bak )
    {

        for(int i = 0; i < biblioteca->total_emprestimos; i++)
        {
            fprintf(arquivo_emprestimos_txt, "%d|%d|%d|%s|%s|%d\n", 
                    biblioteca->emprestimos[i].cod_emprestimo,
                    biblioteca->emprestimos[i].matricula_usuario,
                    biblioteca->emprestimos[i].codigo_livro,
                    biblioteca->emprestimos[i].data_emprestimo,
                    biblioteca->emprestimos[i].data_devolucao,
                    biblioteca->emprestimos[i].ativo ? 1 : 0 );

            fprintf(arquivo_emprestimos_bak, "%d|%d|%d|%s|%s|%d\n", 
                    biblioteca->emprestimos[i].cod_emprestimo,
                    biblioteca->emprestimos[i].matricula_usuario,
                    biblioteca->emprestimos[i].codigo_livro,
                    biblioteca->emprestimos[i].data_emprestimo,
                    biblioteca->emprestimos[i].data_devolucao,
                    biblioteca->emprestimos[i].ativo ? 1 : 0 );

        }

        fclose(arquivo_emprestimos_txt);
        fclose(arquivo_emprestimos_bak);

    }else{
        printf("Erro: Não foi possível abrir os arquivos dos empréstimos.\n");

        if(arquivo_emprestimos_txt) fclose(arquivo_emprestimos_txt);
        if(arquivo_emprestimos_bak) fclose(arquivo_emprestimos_bak);

    }

}

// Função para carregar dados de arquivos
void CarregarDados(struct Biblioteca *biblioteca)
{
    char linha[512];

    FILE *arquivo_livros = fopen("livros.txt", "r");
    if (arquivo_livros)
    {
        while (fgets(linha, sizeof(linha), arquivo_livros) &&biblioteca->total_livros < MAX_LIVROS)
        {
            struct CadastroLivro *livro = &biblioteca->livros[biblioteca->total_livros];
            memset(livro, 0, sizeof(struct CadastroLivro)); 
            int disponivel_int = 0;

            // Lê os dados do arquivo usando sscanf com formato pipe-separated
            int campos = sscanf(linha,"%d|%99[^|]|%79[^|]|%59[^|]|%d|%d|%d|%d",
                       &livro->codigo,
                       livro->titulo,
                       livro->autor,
                       livro->editora,
                       &livro->ano_publicacao,
                       &livro->numero_livrosDisponiveis,
                       &disponivel_int,
                       &livro->total_emprestimos);
                       
            if (campos == 8)
            {

                livro->disponivel = (disponivel_int != 0);
                biblioteca->total_livros++;
            }
        }
        fclose(arquivo_livros);
    }

    FILE *arquivo_usuarios = fopen("usuarios.txt", "r");
    if (arquivo_usuarios)
    {
        while (fgets(linha, sizeof(linha), arquivo_usuarios) && biblioteca->total_usuarios < MAX_USUARIOS)
        {
            struct CadastroUsuario *usuario = &biblioteca->usuarios[biblioteca->total_usuarios];
            memset(usuario, 0, sizeof(struct CadastroUsuario));

            int campos = sscanf(linha,"%99[^|]|%d|%49[^|]|%14[^|]|%19[^\n]",
                       usuario->nomeCompleto,
                       &usuario->matricula,
                       usuario->curso,
                       usuario->telefone,
                       usuario->dataCadastro);

            if (campos == 5)
            {
                biblioteca->total_usuarios++;
            }

        }
        fclose(arquivo_usuarios);
    }

    FILE *arquivo_emprestimos = fopen("emprestimos.txt", "r");
    if (arquivo_emprestimos)
    {
        while (fgets(linha, sizeof(linha), arquivo_emprestimos) && biblioteca->total_emprestimos < MAX_EMPRESTIMOS)
        {
            struct SisEmprestimo *emprestimo = &biblioteca->emprestimos[biblioteca->total_emprestimos];
            memset(emprestimo, 0, sizeof(struct SisEmprestimo));
            int ativo_int = 0;

            int campos = sscanf(linha,"%d|%d|%d|%19[^|]|%19[^|]|%d",
                       &emprestimo->cod_emprestimo,
                       &emprestimo->matricula_usuario,
                       &emprestimo->codigo_livro,
                       emprestimo->data_emprestimo,
                       emprestimo->data_devolucao,
                       &ativo_int);

            if (campos == 6)
            {

                emprestimo->ativo = (ativo_int != 0);
                biblioteca->total_emprestimos++;
            }

        }
        fclose(arquivo_emprestimos);
    }

    printf("Todos os dados foram carregados com sucesso!\n");
}

// -------- ENCERRAMENTO DO SISTEMA ---------

void Sair(struct Biblioteca *biblioteca)
{
    SalvarDados(biblioteca); // Salva os dados antes de sair

    printf("\nSaindo do sistema. Dados salvos com sucesso!\n");
}
