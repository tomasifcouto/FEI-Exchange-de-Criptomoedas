#include "exchange.h"
#include <time.h>
#include <ctype.h>

User users[MAX_USERS];
int num_users = 0;
Prices current_prices = {20000.0, 1500.0, 0.5, 0}; // Preços iniciais

// Arquivos para armazenamento
#define USERS_FILE "users.dat"
#define CRYPTOCURRENCIES_FILE "cryptocurrencies.dat"

// Funções de validação
bool is_valid_cpf(const char *cpf) {
    if (strlen(cpf) != 11) return false;
    
    for (int i = 0; i < 11; i++) {
        if (!isdigit(cpf[i])) return false;
    }
    
    return true;
}

bool is_valid_password(const char *password) {
    int len = strlen(password);
    return len >= 6 && len < PASSWORD_LENGTH;
}

bool is_valid_name(const char *name) {
    int len = strlen(name);
    return len > 0 && len < NAME_LENGTH;
}

bool cpf_exists(const User *users, int num_users, const char *cpf) {
    for (int i = 0; i < num_users; i++) {
        if (users[i].active && strcmp(users[i].cpf, cpf) == 0) {
            return true;
        }
    }
    return false;
}

void load_users(User *users, int *num_users) {
    FILE *file = fopen(USERS_FILE, "rb");
    if (file == NULL) {
        // Criar usuário padrão se o arquivo não existir
        strcpy(users[0].cpf, "123456789");
        strcpy(users[0].name, "Admin");
        strcpy(users[0].password, "123456");
        users[0].balance_brl = 0.0;
        users[0].balance_btc = 0.0;
        users[0].balance_eth = 0.0;
        users[0].balance_xrp = 0.0;
        users[0].num_transactions = 0;
        users[0].active = true;
        *num_users = 1;
        return;
    }
    *num_users = fread(users, sizeof(User), MAX_USERS, file);
    fclose(file);
}

void save_users(const User *users, int num_users) {
    FILE *file = fopen(USERS_FILE, "wb");
    if (file == NULL) {
        printf("Erro ao salvar usuários.\n");
        return;
    }
    fwrite(users, sizeof(User), num_users, file);
    fclose(file);
}

int login(const char *cpf, const char *password, User *users, int num_users, User *current_user) {
    if (!is_valid_cpf(cpf) || !is_valid_password(password)) {
        return 0;
    }

    for (int i = 0; i < num_users; i++) {
        if (users[i].active && strcmp(users[i].cpf, cpf) == 0) {
            if (strcmp(users[i].password, password) == 0) {
                *current_user = users[i];
                return 1;
            }
            return 0;
        }
    }
    return 0;
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

int register_user(User *users, int *num_users, const char *name, const char *cpf, const char *password) {
    if (*num_users >= MAX_USERS) {
        printf("Erro: Limite de usuários atingido.\n");
        return 0;
    }

    if (!is_valid_cpf(cpf)) {
        printf("Erro: CPF inválido.\n");
        return 0;
    }

    if (!is_valid_password(password)) {
        printf("Erro: Senha deve ter pelo menos 6 caracteres.\n");
        return 0;
    }

    if (!is_valid_name(name)) {
        printf("Erro: Nome inválido.\n");
        return 0;
    }

    if (cpf_exists(users, *num_users, cpf)) {
        printf("Erro: CPF já cadastrado.\n");
        return 0;
    }

    User new_user = {0};  // Inicializa com zeros
    strcpy(new_user.cpf, cpf);
    strcpy(new_user.name, name);
    strcpy(new_user.password, password);
    new_user.active = true;
    new_user.balance_brl = 0.0;
    new_user.balance_btc = 0.0;
    new_user.balance_eth = 0.0;
    new_user.balance_xrp = 0.0;
    new_user.num_transactions = 0;

    users[*num_users] = new_user;
    (*num_users)++;
    
    save_users(users, *num_users);
    return 1;
}

int delete_user(User *users, int *num_users, const char *cpf) {
    if (!is_valid_cpf(cpf)) {
        return 0;
    }

    for (int i = 0; i < *num_users; i++) {
        if (strcmp(users[i].cpf, cpf) == 0 && users[i].active) {
            users[i].active = false;  // Exclusão lógica
            save_users(users, *num_users);
            return 1;
        }
    }
    return 0;
}

int register_cryptocurrency(Cryptocurrency *cryptocurrencies, int *num_cryptocurrencies, 
                          const char *name, double initial_price, double buy_fee, double sell_fee) {
    if (*num_cryptocurrencies >= MAX_CRYPTOCURRENCIES) {
        return 0;
    }

    // Validações
    if (strlen(name) == 0 || strlen(name) >= CRYPTO_NAME_LENGTH ||
        initial_price <= 0 || buy_fee < 0 || sell_fee < 0) {
        return 0;
    }

    // Verifica se já existe
    for (int i = 0; i < *num_cryptocurrencies; i++) {
        if (strcmp(cryptocurrencies[i].name, name) == 0 && cryptocurrencies[i].active) {
            return 0;
        }
    }

    Cryptocurrency new_crypto = {0};
    strcpy(new_crypto.name, name);
    new_crypto.current_price = initial_price;
    new_crypto.buy_fee = buy_fee;
    new_crypto.sell_fee = sell_fee;
    new_crypto.active = true;

    cryptocurrencies[*num_cryptocurrencies] = new_crypto;
    (*num_cryptocurrencies)++;

    save_cryptocurrencies(cryptocurrencies, *num_cryptocurrencies);
    return 1;
}

int delete_cryptocurrency(Cryptocurrency *cryptocurrencies, int *num_cryptocurrencies, const char *name) {
    for (int i = 0; i < *num_cryptocurrencies; i++) {
        if (strcmp(cryptocurrencies[i].name, name) == 0 && cryptocurrencies[i].active) {
            cryptocurrencies[i].active = false;  // Exclusão lógica
            save_cryptocurrencies(cryptocurrencies, *num_cryptocurrencies);
            return 1;
        }
    }
    return 0;
}

void save_cryptocurrencies(const Cryptocurrency *cryptocurrencies, int num_cryptocurrencies) {
    FILE *file = fopen(CRYPTOCURRENCIES_FILE, "wb");
    if (file != NULL) {
        fwrite(&num_cryptocurrencies, sizeof(int), 1, file);
        fwrite(cryptocurrencies, sizeof(Cryptocurrency), num_cryptocurrencies, file);
        fclose(file);
    }
}

void load_cryptocurrencies(Cryptocurrency *cryptocurrencies, int *num_cryptocurrencies) {
    FILE *file = fopen(CRYPTOCURRENCIES_FILE, "rb");
    if (file != NULL) {
        fread(num_cryptocurrencies, sizeof(int), 1, file);
        fread(cryptocurrencies, sizeof(Cryptocurrency), *num_cryptocurrencies, file);
        fclose(file);
    } else {
        *num_cryptocurrencies = 0;
    }
}

int main() {
    User users[MAX_USERS];
    Cryptocurrency cryptocurrencies[MAX_CRYPTOCURRENCIES];
    int num_users = 0;
    int num_cryptocurrencies = 0;
    User current_user;
    char option;
    
    // Carrega dados do arquivo
    load_users(users, &num_users);
    load_cryptocurrencies(cryptocurrencies, &num_cryptocurrencies);
    
    while (1) {
        printf("\n=== EXCHANGE DE CRIPTOMOEDAS ===\n");
        printf("1. Login\n");
        printf("2. Cadastrar novo investidor\n");
        printf("3. Sair\n");
        printf("Escolha uma opção: ");
        scanf(" %c", &option);
        
        switch (option) {
            case '1': {
                char cpf[CPF_LENGTH];
                char password[PASSWORD_LENGTH];
                
                printf("\nLogin\n");
                printf("CPF (apenas números): ");
                scanf("%s", cpf);
                printf("Senha: ");
                scanf("%s", password);
                
                if (login(cpf, password, users, num_users, &current_user)) {
                    // Menu do usuário logado
                    while (1) {
                        printf("\n=== MENU PRINCIPAL ===\n");
                        printf("1. Excluir investidor\n");
                        printf("2. Cadastrar criptomoeda\n");
                        printf("3. Excluir criptomoeda\n");
                        printf("4. Verificar saldo\n");
                        printf("5. Depositar\n");
                        printf("6. Sacar\n");
                        printf("7. Comprar criptomoeda\n");
                        printf("8. Vender criptomoeda\n");
                        printf("9. Ver cotações\n");
                        printf("e. Ver extrato\n");
                        printf("0. Sair\n");
                        printf("Escolha uma opção: ");
                        scanf(" %c", &option);
                        
                        switch (option) {
                            case '1': {
                                char cpf_delete[CPF_LENGTH];
                                char confirm;
                                
                                printf("\nExcluir investidor\n");
                                printf("CPF do investidor a ser excluído: ");
                                scanf("%s", cpf_delete);
                                
                                // Encontra e mostra os dados do usuário
                                for (int i = 0; i < num_users; i++) {
                                    if (strcmp(users[i].cpf, cpf_delete) == 0 && users[i].active) {
                                        printf("\nDados do investidor:\n");
                                        printf("Nome: %s\n", users[i].name);
                                        printf("CPF: %s\n", users[i].cpf);
                                        printf("Saldo em BRL: R$ %.2f\n", users[i].balance_brl);
                                        
                                        printf("\nConfirmar exclusão (S/N)? ");
                                        scanf(" %c", &confirm);
                                        
                                        if (toupper(confirm) == 'S') {
                                            if (delete_user(users, &num_users, cpf_delete)) {
                                                printf("Investidor excluído com sucesso!\n");
                                            } else {
                                                printf("Erro ao excluir investidor.\n");
                                            }
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                            case '2': {
                                char name[CRYPTO_NAME_LENGTH];
                                double initial_price, buy_fee, sell_fee;
                                
                                printf("\nCadastro de Criptomoeda\n");
                                printf("Nome: ");
                                scanf("%s", name);
                                printf("Cotação inicial: ");
                                scanf("%lf", &initial_price);
                                printf("Taxa de compra (%%): ");
                                scanf("%lf", &buy_fee);
                                printf("Taxa de venda (%%): ");
                                scanf("%lf", &sell_fee);
                                
                                if (register_cryptocurrency(cryptocurrencies, &num_cryptocurrencies, 
                                                         name, initial_price, buy_fee/100, sell_fee/100)) {
                                    printf("Criptomoeda cadastrada com sucesso!\n");
                                } else {
                                    printf("Erro ao cadastrar criptomoeda.\n");
                                }
                                break;
                            }
                            case '3': {
                                char name[CRYPTO_NAME_LENGTH];
                                char confirm;
                                
                                printf("\nExcluir Criptomoeda\n");
                                printf("Nome da criptomoeda: ");
                                scanf("%s", name);
                                
                                // Encontra e mostra os dados da criptomoeda
                                for (int i = 0; i < num_cryptocurrencies; i++) {
                                    if (strcmp(cryptocurrencies[i].name, name) == 0 && cryptocurrencies[i].active) {
                                        printf("\nDados da criptomoeda:\n");
                                        printf("Nome: %s\n", cryptocurrencies[i].name);
                                        printf("Cotação atual: %.2f\n", cryptocurrencies[i].current_price);
                                        printf("Taxa de compra: %.2f%%\n", cryptocurrencies[i].buy_fee * 100);
                                        printf("Taxa de venda: %.2f%%\n", cryptocurrencies[i].sell_fee * 100);
                                        
                                        printf("\nConfirmar exclusão (S/N)? ");
                                        scanf(" %c", &confirm);
                                        
                                        if (toupper(confirm) == 'S') {
                                            if (delete_cryptocurrency(cryptocurrencies, &num_cryptocurrencies, name)) {
                                                printf("Criptomoeda excluída com sucesso!\n");
                                            } else {
                                                printf("Erro ao excluir criptomoeda.\n");
                                            }
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                            case '4':
                                check_balance(&current_user);
                                break;
                            case '5':
                                printf("Digite o valor para depósito: R$ ");
                                double amount;
                                scanf("%lf", &amount);
                                deposit_brl(&current_user, amount);
                                users[num_users - 1] = current_user;
                                break;
                            case '6':
                                printf("Digite o valor para saque: R$ ");
                                scanf("%lf", &amount);
                                printf("Digite sua senha: ");
                                char confirm_password[PASSWORD_LENGTH];
                                scanf("%s", confirm_password);
                                if (withdraw_brl(&current_user, amount, confirm_password)) {
                                    users[num_users - 1] = current_user;
                                }
                                break;
                            case '7': {
                                show_prices(&current_prices);
                                printf("\nDigite a criptomoeda (BTC/ETH/XRP): ");
                                char crypto[10];
                                scanf("%s", crypto);
                                printf("Digite o valor em reais: R$ ");
                                scanf("%lf", &amount);
                                printf("Digite sua senha: ");
                                char confirm_password[PASSWORD_LENGTH];
                                scanf("%s", confirm_password);
                                if (buy_crypto(&current_user, crypto, amount, confirm_password, &current_prices)) {
                                    users[num_users - 1] = current_user;
                                }
                                break;
                            }
                            case '8': {
                                show_prices(&current_prices);
                                printf("\nDigite a criptomoeda (BTC/ETH/XRP): ");
                                char crypto[10];
                                scanf("%s", crypto);
                                printf("Digite a quantidade de %s: ", crypto);
                                double amount;
                                scanf("%lf", &amount);
                                printf("Digite sua senha: ");
                                char confirm_password[PASSWORD_LENGTH];
                                scanf("%s", confirm_password);
                                if (sell_crypto(&current_user, crypto, amount, confirm_password, &current_prices)) {
                                    users[num_users - 1] = current_user;
                                }
                                break;
                            }
                            case '9':
                                show_prices(&current_prices);
                                break;
                            case 'e':
                            case 'E':
                                view_statement(&current_user);
                                break;
                            case '0':
                                printf("\nLogout realizado com sucesso!\n");
                                goto main_menu;
                            default:
                                printf("\nOpção inválida!\n");
                        }
                    }
                } else {
                    printf("\nCPF ou senha inválidos!\n");
                }
                break;
            }
            case '2': {
                char name[NAME_LENGTH];
                char cpf[CPF_LENGTH];
                char password[PASSWORD_LENGTH];
                
                printf("\nCadastro de Novo Investidor\n");
                printf("Nome: ");
                scanf(" %[^\n]s", name);
                printf("CPF (apenas números): ");
                scanf("%s", cpf);
                printf("Senha: ");
                scanf("%s", password);
                
                if (register_user(users, &num_users, name, cpf, password)) {
                    printf("Investidor cadastrado com sucesso!\n");
                } else {
                    printf("Erro ao cadastrar investidor.\n");
                }
                break;
            }
            case '3':
                printf("\nObrigado por usar nossa exchange!\n");
                return 0;
            default:
                printf("\nOpção inválida!\n");
        }
        main_menu:;
    }
    
    return 0;
} 