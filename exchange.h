#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_TRANSACTIONS 1000
#define CPF_LENGTH 12
#define PASSWORD_LENGTH 20
#define MAX_LINE 256

// Estrutura para armazenar informações do usuário
typedef struct {
    char cpf[CPF_LENGTH];
    char password[PASSWORD_LENGTH];
    double balance_brl;
    double balance_btc;
    double balance_eth;
    double balance_xrp;
} User;

// Estrutura para armazenar transações
typedef struct {
    char cpf[CPF_LENGTH];
    char type[20];  // "deposit", "buy", "sell"
    char crypto[10]; // "BTC", "ETH", "XRP"
    double amount;
    double fee;
    time_t timestamp;
} Transaction;

// Taxas de compra e venda
#define BTC_BUY_FEE 0.02
#define BTC_SELL_FEE 0.03
#define ETH_BUY_FEE 0.01
#define ETH_SELL_FEE 0.02
#define XRP_BUY_FEE 0.01
#define XRP_SELL_FEE 0.01

// Funções de autenticação
int login(char *cpf, char *password, User *current_user);
void load_users(User *users, int *num_users);
void save_users(User *users, int num_users);

// Funções de operações
void check_balance(User *user);
void view_statement(char *cpf);
void deposit_brl(User *user, double amount);
void save_transaction(Transaction *transaction);

#endif 