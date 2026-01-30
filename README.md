# Sistema de Gerenciamento de Biblioteca em C

Este projeto é um sistema robusto de gerenciamento de biblioteca desenvolvido em linguagem C, focado em **eficiência de memória** e **persistência de dados**.

O objetivo foi aplicar conceitos fundamentais de Engenharia de Software, como manipulação direta de memória, estruturas de dados complexas e algoritmos de ordenação, sem depender de bibliotecas externas de alto nível.

## Funcionalidades Principais

- **Gestão de Acervo e Usuários:** Cadastro com validação de unicidade (ID/Matrícula).
- **Sistema de Empréstimos:** Controle de fluxo de empréstimo, devolução e renovação automática de datas.
- **Relatórios Gerenciais:**
    - Listagem de atrasos.
    - Ranking de popularidade (Livros mais emprestados).
- **Persistência de Dados:** O sistema salva e recupera automaticamente todos os registros em arquivo (`.txt`), mantendo o estado entre execuções.
- **Backup Automático:** Rotina de segurança que gera arquivos `.bak` a cada operação crítica de escrita.

## Destaques Técnicos

Abaixo, alguns desafios técnicos implementados neste projeto:

### 1. Otimização de Memória com Ponteiros
Para garantir performance e evitar cópias desnecessárias de dados na *Stack*, todas as funções de manipulação recebem a estrutura principal por referência (ponteiros).

### 2. Estruturas de Dados Aninhadas
O sistema utiliza `structs` complexas para organizar o banco de dados em memória, centralizando a gestão do estado da aplicação.

### 3. Algoritmos "Vanilla" (Feitos à mão)
- **Ordenação:** Implementação manual do algoritmo *Bubble Sort* para gerar o relatório de livros mais populares.
- **Lógica Temporal:** Implementação própria da lógica de cálculo de dias e anos bissextos.

## Como executar

É necessário um compilador C (como GCC).

```bash
# Compile o projeto
gcc Sis_biblioteca.c -o biblioteca





# Execute
./biblioteca
