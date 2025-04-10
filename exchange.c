#include "exchange.h"
#include <time.h>
#include <ctype.h>

User users[MAX_USERS];
int num_users = 0;
Prices current_prices = {20000.0, 1500.0, 0.5, 0}; // Preços iniciais

void load_users(User *users, int *num_users) {
    FILE *file = fopen("users.dat", "rb");
    if (file == NULL) {
        // Criar usuário padrão se o arquivo não existir
        strcpy(users[0].cpf, "123456789");
        strcpy(users[0].password, "123456");
        users[0].balance_brl = 0.0;
        users[0].balance_btc = 0.0;
        users[0].balance_eth = 0.0;
        users[0].balance_xrp = 0.0;
        users[0].num_transactions = 0;
        *num_users = 1;
        return;
    }
    *num_users = fread(users, sizeof(User), MAX_USERS, file);
    fclose(file);
}

void save_users(User *users, int num_users) {
    FILE *file = fopen("users.dat", "wb");
    if (file == NULL) {
        printf("Erro ao salvar usuários.\n");
        return;
    }
    fwrite(users, sizeof(User), num_users, file);
    fclose(file);
}

int login(char *cpf, char *password, User *current_user) {
    for (int i = 0; i < num_users; i++) {
        if (strcmp(users[i].cpf, cpf) == 0 && strcmp(users[i].password, password) == 0) {
            *current_user = users[i];
            return i;
        }
    }
    return -1;
}

void check_balance(User *user) {
    printf("\n=== Saldo da Carteira ===\n");
    printf("CPF: %s\n", user->cpf);
    printf("Saldo em Reais: R$ %.2f\n", user->balance_brl);
    printf("Bitcoin: %.8f BTC\n", user->balance_btc);
    printf("Ethereum: %.8f ETH\n", user->balance_eth);
    printf("Ripple: %.8f XRP\n", user->balance_xrp);
}

void save_transaction(User *user, Transaction *transaction) {
    // Se atingiu o limite, move todas as transações uma posição para trás
    if (user->num_transactions >= MAX_TRANSACTIONS_PER_USER) {
        for (int i = 0; i < MAX_TRANSACTIONS_PER_USER - 1; i++) {
            user->transactions[i] = user->transactions[i + 1];
        }
        user->num_transactions = MAX_TRANSACTIONS_PER_USER - 1;
    }
    
    // Adiciona a nova transação
    user->transactions[user->num_transactions] = *transaction;
    user->num_transactions++;
}

void view_statement(User *user) {
    printf("\n=== Extrato de Operações ===\n");
    
    for (int i = 0; i < user->num_transactions; i++) {
        Transaction *t = &user->transactions[i];
        char date_str[26];
        struct tm *tm_info = localtime(&t->timestamp);
        strftime(date_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
        
        printf("\nData: %s\n", date_str);
        printf("Tipo: %s\n", t->type);
        if (strcmp(t->type, "deposit") != 0 && strcmp(t->type, "withdraw") != 0) {
            printf("Criptomoeda: %s\n", t->crypto);
        }
        printf("Valor: R$ %.2f\n", t->amount);
        if (t->fee > 0) {
            printf("Taxa: R$ %.2f\n", t->fee);
        }
        printf("------------------------\n");
    }
}

void deposit_brl(User *user, double amount) {
    if (amount <= 0) {
        printf("Valor inválido para depósito.\n");
        return;
    }

    user->balance_brl += amount;

    Transaction transaction = {
        .timestamp = time(NULL),
        .amount = amount,
        .fee = 0
    };
    strcpy(transaction.cpf, user->cpf);
    strcpy(transaction.type, "deposit");
    strcpy(transaction.crypto, "");

    save_transaction(user, &transaction);
    save_users(users, num_users);
    
    printf("Depósito de R$ %.2f realizado com sucesso.\n", amount);
}

int validate_password(User *user, char *password) {
    return strcmp(user->password, password) == 0;
}

void update_prices(Prices *prices) {
    srand(time(NULL));
    double variation;
    
    // Atualiza Bitcoin (±5%)
    variation = (double)(rand() % 1000) / 10000.0; // 0 a 0.05
    if (rand() % 2) variation = -variation;
    prices->btc_price *= (1.0 + variation);
    
    // Atualiza Ethereum (±5%)
    variation = (double)(rand() % 1000) / 10000.0;
    if (rand() % 2) variation = -variation;
    prices->eth_price *= (1.0 + variation);
    
    // Atualiza Ripple (±5%)
    variation = (double)(rand() % 1000) / 10000.0;
    if (rand() % 2) variation = -variation;
    prices->xrp_price *= (1.0 + variation);
    
    prices->last_update = time(NULL);
}

void show_prices(Prices *prices) {
    printf("\n=== Cotações Atuais ===\n");
    printf("Bitcoin (BTC): R$ %.2f\n", prices->btc_price);
    printf("Ethereum (ETH): R$ %.2f\n", prices->eth_price);
    printf("Ripple (XRP): R$ %.2f\n", prices->xrp_price);
    
    char date_str[26];
    struct tm *tm_info = localtime(&prices->last_update);
    strftime(date_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    printf("Última atualização: %s\n", date_str);
}

int withdraw_brl(User *user, double amount, char *password) {
    if (!validate_password(user, password)) {
        printf("Senha inválida!\n");
        return 0;
    }
    
    if (amount <= 0 || amount > user->balance_brl) {
        printf("Valor inválido para saque!\n");
        return 0;
    }
    
    user->balance_brl -= amount;
    
    Transaction transaction = {
        .timestamp = time(NULL),
        .amount = amount,
        .fee = 0
    };
    strcpy(transaction.cpf, user->cpf);
    strcpy(transaction.type, "withdraw");
    strcpy(transaction.crypto, "");
    
    save_transaction(user, &transaction);
    save_users(users, num_users);
    
    printf("Saque de R$ %.2f realizado com sucesso.\n", amount);
    return 1;
}

int buy_crypto(User *user, char *crypto, double amount_brl, char *password, Prices *prices) {
    if (!validate_password(user, password)) {
        printf("Senha inválida!\n");
        return 0;
    }
    
    if (amount_brl <= 0 || amount_brl > user->balance_brl) {
        printf("Valor inválido para compra!\n");
        return 0;
    }
    
    double fee_rate, crypto_price, amount_crypto;
    if (strcmp(crypto, "BTC") == 0) {
        fee_rate = BTC_BUY_FEE;
        crypto_price = prices->btc_price;
    } else if (strcmp(crypto, "ETH") == 0) {
        fee_rate = ETH_BUY_FEE;
        crypto_price = prices->eth_price;
    } else if (strcmp(crypto, "XRP") == 0) {
        fee_rate = XRP_BUY_FEE;
        crypto_price = prices->xrp_price;
    } else {
        printf("Criptomoeda inválida!\n");
        return 0;
    }
    
    double fee = amount_brl * fee_rate;
    amount_crypto = (amount_brl - fee) / crypto_price;
    
    printf("\n=== Confirmação de Compra ===\n");
    printf("Criptomoeda: %s\n", crypto);
    printf("Valor em reais: R$ %.2f\n", amount_brl);
    printf("Taxa cobrada (%.1f%%): R$ %.2f\n", fee_rate * 100, fee);
    printf("Quantidade de %s: %.8f\n", crypto, amount_crypto);
    printf("Preço unitário: R$ %.2f\n", crypto_price);
    
    char confirm;
    printf("\nConfirmar compra? (S/N): ");
    scanf(" %c", &confirm);
    
    if (toupper(confirm) != 'S') {
        printf("Operação cancelada.\n");
        return 0;
    }
    
    user->balance_brl -= amount_brl;
    if (strcmp(crypto, "BTC") == 0) {
        user->balance_btc += amount_crypto;
    } else if (strcmp(crypto, "ETH") == 0) {
        user->balance_eth += amount_crypto;
    } else {
        user->balance_xrp += amount_crypto;
    }
    
    Transaction transaction = {
        .timestamp = time(NULL),
        .amount = amount_brl,
        .fee = fee
    };
    strcpy(transaction.cpf, user->cpf);
    strcpy(transaction.type, "buy");
    strcpy(transaction.crypto, crypto);
    
    save_transaction(user, &transaction);
    save_users(users, num_users);
    
    printf("Compra realizada com sucesso!\n");
    return 1;
}

int sell_crypto(User *user, char *crypto, double amount_crypto, char *password, Prices *prices) {
    if (!validate_password(user, password)) {
        printf("Senha inválida!\n");
        return 0;
    }
    
    double available_crypto, crypto_price, fee_rate;
    if (strcmp(crypto, "BTC") == 0) {
        available_crypto = user->balance_btc;
        crypto_price = prices->btc_price;
        fee_rate = BTC_SELL_FEE;
    } else if (strcmp(crypto, "ETH") == 0) {
        available_crypto = user->balance_eth;
        crypto_price = prices->eth_price;
        fee_rate = ETH_SELL_FEE;
    } else if (strcmp(crypto, "XRP") == 0) {
        available_crypto = user->balance_xrp;
        crypto_price = prices->xrp_price;
        fee_rate = XRP_SELL_FEE;
    } else {
        printf("Criptomoeda inválida!\n");
        return 0;
    }
    
    if (amount_crypto <= 0 || amount_crypto > available_crypto) {
        printf("Quantidade inválida para venda!\n");
        return 0;
    }
    
    double amount_brl = amount_crypto * crypto_price;
    double fee = amount_brl * fee_rate;
    double final_amount = amount_brl - fee;
    
    printf("\n=== Confirmação de Venda ===\n");
    printf("Criptomoeda: %s\n", crypto);
    printf("Quantidade: %.8f %s\n", amount_crypto, crypto);
    printf("Valor em reais: R$ %.2f\n", amount_brl);
    printf("Taxa cobrada (%.1f%%): R$ %.2f\n", fee_rate * 100, fee);
    printf("Valor final: R$ %.2f\n", final_amount);
    printf("Preço unitário: R$ %.2f\n", crypto_price);
    
    char confirm;
    printf("\nConfirmar venda? (S/N): ");
    scanf(" %c", &confirm);
    
    if (toupper(confirm) != 'S') {
        printf("Operação cancelada.\n");
        return 0;
    }
    
    if (strcmp(crypto, "BTC") == 0) {
        user->balance_btc -= amount_crypto;
    } else if (strcmp(crypto, "ETH") == 0) {
        user->balance_eth -= amount_crypto;
    } else {
        user->balance_xrp -= amount_crypto;
    }
    user->balance_brl += final_amount;
    
    Transaction transaction = {
        .timestamp = time(NULL),
        .amount = amount_brl,
        .fee = fee
    };
    strcpy(transaction.cpf, user->cpf);
    strcpy(transaction.type, "sell");
    strcpy(transaction.crypto, crypto);
    
    save_transaction(user, &transaction);
    save_users(users, num_users);
    
    printf("Venda realizada com sucesso!\n");
    return 1;
}

int main() {
    load_users(users, &num_users);
    User current_user;
    int user_index = -1;
    char cpf[CPF_LENGTH];
    char password[PASSWORD_LENGTH];
    char confirm_password[PASSWORD_LENGTH];
    
    printf("=== Exchange de Criptomoedas ===\n");
    printf("Login\n");
    printf("CPF: ");
    scanf("%s", cpf);
    printf("Senha: ");
    scanf("%s", password);
    
    user_index = login(cpf, password, &current_user);
    
    if (user_index == -1) {
        printf("CPF ou senha inválidos.\n");
        return 1;
    }
    
    int option;
    double amount;
    char crypto[10];
    
    do {
        update_prices(&current_prices);
        
        printf("\n=== Menu Principal ===\n");
        printf("1. Consultar saldo\n");
        printf("2. Consultar extrato\n");
        printf("3. Depositar reais\n");
        printf("4. Sacar reais\n");
        printf("5. Comprar criptomoeda\n");
        printf("6. Vender criptomoeda\n");
        printf("7. Ver cotações\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &option);
        
        switch (option) {
            case 1:
                check_balance(&current_user);
                break;
            case 2:
                view_statement(&current_user);
                break;
            case 3:
                printf("Digite o valor para depósito: R$ ");
                scanf("%lf", &amount);
                deposit_brl(&current_user, amount);
                users[user_index] = current_user;
                break;
            case 4:
                printf("Digite o valor para saque: R$ ");
                scanf("%lf", &amount);
                printf("Digite sua senha: ");
                scanf("%s", confirm_password);
                if (withdraw_brl(&current_user, amount, confirm_password)) {
                    users[user_index] = current_user;
                }
                break;
            case 5:
                show_prices(&current_prices);
                printf("\nDigite a criptomoeda (BTC/ETH/XRP): ");
                scanf("%s", crypto);
                printf("Digite o valor em reais: R$ ");
                scanf("%lf", &amount);
                printf("Digite sua senha: ");
                scanf("%s", confirm_password);
                if (buy_crypto(&current_user, crypto, amount, confirm_password, &current_prices)) {
                    users[user_index] = current_user;
                }
                break;
            case 6:
                show_prices(&current_prices);
                printf("\nDigite a criptomoeda (BTC/ETH/XRP): ");
                scanf("%s", crypto);
                printf("Digite a quantidade de %s: ", crypto);
                scanf("%lf", &amount);
                printf("Digite sua senha: ");
                scanf("%s", confirm_password);
                if (sell_crypto(&current_user, crypto, amount, confirm_password, &current_prices)) {
                    users[user_index] = current_user;
                }
                break;
            case 7:
                show_prices(&current_prices);
                break;
            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (option != 0);
    
    return 0;
} 