# Language / Idioma
- [🇧🇷 Português](#sistema-de-gerenciamento-de-biblioteca-em-portugues)
- [🇺🇸 English](#library-management-system-in-english)

---

<a name="sistema-de-gerenciamento-de-biblioteca-em-portugues"></a>
# Sistema de Gerenciamento de Biblioteca em Português
Este projeto é um sistema robusto de gerenciamento de biblioteca desenvolvido em linguagem C, focado em **eficiência de memória** e **persistência de dados**.

O objetivo foi aplicar conceitos fundamentais de Engenharia de Software, como manipulação direta de memória, estruturas de dados complexas e algoritmos de ordenação, sem depender de bibliotecas externas de alto nível.

## Funcionalidades Principais

* **Gestão de Acervo e Usuários:** Cadastro com validação de unicidade (ID/Matrícula).
* **Sistema de Empréstimos:** Controle de fluxo de empréstimo, devolução e renovação automática de datas.
* **Relatórios Gerenciais:**
    * Listagem de atrasos.
    * Ranking de popularidade (Livros mais emprestados).
* **Persistência de Dados:** O sistema salva e recupera automaticamente todos os registros em arquivo (`.txt`), mantendo o estado entre execuções.
* **Backup Automático:** Rotina de segurança que gera arquivos `.bak` a cada operação crítica de escrita.

## Destaques Técnicos

Abaixo, alguns desafios técnicos implementados neste projeto:

### 1. Otimização de Memória com Ponteiros
Para garantir performance e evitar cópias desnecessárias de dados na *Stack*, todas as funções de manipulação recebem a estrutura principal por referência (ponteiros).

### 2. Estruturas de Dados Aninhadas
O sistema utiliza `structs` complexas para organizar o banco de dados em memória, centralizando a gestão do estado da aplicação.

### 3. Algoritmos "Vanilla" (Feitos à mão)
* **Ordenação:** Implementação manual do algoritmo *Bubble Sort* para gerar o relatório de livros mais populares.
* **Lógica Temporal:** Implementação própria da lógica de cálculo de dias e anos bissextos.

## Como executar
```bash
# Compile o projeto
gcc Sis_biblioteca.c -o biblioteca

# Execute o programa
./biblioteca
```
---
<a name="library-management-system-in-english"></a>
# Library Management System in C

This project is a robust library management system developed in C, focused on **memory efficiency** and **data persistence**.

The objective was to apply fundamental Software Engineering concepts, such as direct memory manipulation, complex data structures, and sorting algorithms, without relying on high-level external libraries.

## Key Features

* **Collection and User Management:** Registration with uniqueness validation (ID/Registration number).
* **Loan System:** Control of borrowing flow, returns, and automatic date renewal.
* **Management Reports:**
    * Overdue list.
    * Popularity ranking (Most borrowed books).
* **Data Persistence:** The system automatically saves and retrieves all records in a text file (`.txt`), maintaining state between executions.
* **Automatic Backup:** Security routine that generates `.bak` files during every critical write operation.

## Technical Highlights

Below are some of the technical challenges implemented in this project:

### 1. Memory Optimization with Pointers
To ensure performance and avoid unnecessary data copying on the *Stack*, all manipulation functions receive the main structure by reference (pointers).

### 2. Nested Data Structures
The system uses complex `structs` to organize the database in memory, centralizing the application state management.

### 3. "Vanilla" Algorithms (Handcrafted)
* **Sorting:** Manual implementation of the *Bubble Sort* algorithm to generate the most popular books report.
* **Time Logic:** Custom implementation of date calculation logic, including leap years.

## How to Run

A C compiler (such as GCC) is required.

```bash
# Compile the project
gcc Sis_biblioteca.c -o biblioteca

# Run the program
./biblioteca
