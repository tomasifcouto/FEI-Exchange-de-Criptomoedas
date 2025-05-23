#ifndef EXCHANGE_H
#define EXCHANGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_USERS 100
#define MAX_CRYPTOCURRENCIES 50
#define MAX_TRANSACTIONS_PER_USER 100
#define CPF_LENGTH 12
#define PASSWORD_LENGTH 20
#define NAME_LENGTH 100
#define CRYPTO_NAME_LENGTH 50
#define MAX_LINE 256

// Estrutura para armazenar transações
typedef struct {
    char cpf[CPF_LENGTH];
    char type[20];
    char crypto[10];  // "BTC", "ETH", "XRP"
    double amount;
    double fee;
    time_t timestamp;
} Transaction;

// Estrutura para armazenar informações do usuário
typedef struct {
    char cpf[CPF_LENGTH];
    char name[NAME_LENGTH];
    char password[PASSWORD_LENGTH];
    double balance_brl;
    double balance_btc;
    double balance_eth;
    double balance_xrp;
    bool active;
    Transaction transactions[MAX_TRANSACTIONS_PER_USER];
    int num_transactions;
} User;

// Estrutura para armazenar informações de criptomoeda
typedef struct {
    char name[CRYPTO_NAME_LENGTH];
    double current_price;
    double buy_fee;
    double sell_fee;
    bool active;
} Cryptocurrency;

// Estrutura para armazenar cotações
typedef struct {
    double btc_price;
    double eth_price;
    double xrp_price;
    time_t last_update;
} Prices;

// Taxas de compra e venda
#define BTC_BUY_FEE 0.02
#define BTC_SELL_FEE 0.03
#define ETH_BUY_FEE 0.01
#define ETH_SELL_FEE 0.02
#define XRP_BUY_FEE 0.01
#define XRP_SELL_FEE 0.01

// Funções de autenticação e gerenciamento de usuários
int login(const char *cpf, const char *password, User *users, int num_users, User *current_user);
int register_user(User *users, int *num_users, const char *name, const char *cpf, const char *password);
int delete_user(User *users, int *num_users, const char *cpf);
void save_users(const User *users, int num_users);
void load_users(User *users, int *num_users);

// Funções de gerenciamento de criptomoedas
int register_cryptocurrency(Cryptocurrency *cryptocurrencies, int *num_cryptocurrencies, 
                          const char *name, double initial_price, double buy_fee, double sell_fee);
int delete_cryptocurrency(Cryptocurrency *cryptocurrencies, int *num_cryptocurrencies, const char *name);
void save_cryptocurrencies(const Cryptocurrency *cryptocurrencies, int num_cryptocurrencies);
void load_cryptocurrencies(Cryptocurrency *cryptocurrencies, int *num_cryptocurrencies);

// Funções auxiliares
bool is_valid_cpf(const char *cpf);
bool cpf_exists(const User *users, int num_users, const char *cpf);
bool is_valid_password(const char *password);
bool is_valid_name(const char *name);

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