#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 10  // Limite de 10 usuários
#define MAX_TRANSACTIONS_PER_USER 100  // Limite de 100 transações por usuário
#define CPF_LENGTH 12
#define PASSWORD_LENGTH 20
#define MAX_LINE 256

// Estrutura para armazenar cotações
typedef struct {
    double btc_price;
    double eth_price;
    double xrp_price;
    time_t last_update;
} Prices;

// Estrutura para armazenar transações
typedef struct {
    char cpf[CPF_LENGTH];
    char type[20];  // "deposit", "buy", "sell", "withdraw"
    char crypto[10]; // "BTC", "ETH", "XRP"
    double amount;
    double fee;
    time_t timestamp;
} Transaction;

// Estrutura para armazenar informações do usuário
typedef struct {
    char cpf[CPF_LENGTH];
    char password[PASSWORD_LENGTH];
    double balance_brl;
    double balance_btc;
    double balance_eth;
    double balance_xrp;
    Transaction transactions[MAX_TRANSACTIONS_PER_USER];
    int num_transactions;
} User;

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
int validate_password(User *user, char *password);

// Funções de operações
void check_balance(User *user);
void view_statement(User *user);
void deposit_brl(User *user, double amount);
int withdraw_brl(User *user, double amount, char *password);
void save_transaction(User *user, Transaction *transaction);

// Funções de criptomoedas
void update_prices(Prices *prices);
void show_prices(Prices *prices);
int buy_crypto(User *user, char *crypto, double amount_brl, char *password, Prices *prices);
int sell_crypto(User *user, char *crypto, double amount_crypto, char *password, Prices *prices);

#endif 