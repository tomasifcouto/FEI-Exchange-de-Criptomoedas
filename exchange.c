#include "exchange.h"

User users[MAX_USERS];
int num_users = 0;

void load_users(User *users, int *num_users) {
    FILE *file = fopen("users.dat", "rb");
    if (file == NULL) {
        // Criar usuário padrão se o arquivo não existir
        strcpy(users[0].cpf, "12345678900");
        strcpy(users[0].password, "123456");
        users[0].balance_brl = 0.0;
        users[0].balance_btc = 0.0;
        users[0].balance_eth = 0.0;
        users[0].balance_xrp = 0.0;
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

void save_transaction(Transaction *transaction) {
    FILE *file = fopen("transactions.txt", "a");
    if (file == NULL) {
        printf("Erro ao salvar transação.\n");
        return;
    }
    
    char date_str[26];
    struct tm *tm_info = localtime(&transaction->timestamp);
    strftime(date_str, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    fprintf(file, "Data: %s\n", date_str);
    fprintf(file, "CPF: %s\n", transaction->cpf);
    fprintf(file, "Tipo: %s\n", transaction->type);
    if (strcmp(transaction->type, "deposit") != 0) {
        fprintf(file, "Criptomoeda: %s\n", transaction->crypto);
    }
    fprintf(file, "Valor: %.2f\n", transaction->amount);
    fprintf(file, "Taxa: %.2f\n", transaction->fee);
    fprintf(file, "------------------------\n");
    
    fclose(file);
}

void view_statement(char *cpf) {
    FILE *file = fopen("transactions.txt", "r");
    if (file == NULL) {
        printf("Nenhuma transação encontrada.\n");
        return;
    }

    char line[MAX_LINE];
    int printing = 0;

    printf("\n=== Extrato de Operações ===\n");
    while (fgets(line, MAX_LINE, file) != NULL) {
        if (strstr(line, "CPF: ") != NULL) {
            if (strstr(line, cpf) != NULL) {
                printing = 1;
                printf("%s", line);
            } else {
                printing = 0;
            }
        } else if (printing) {
            printf("%s", line);
        }
    }

    fclose(file);
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

    save_transaction(&transaction);
    save_users(users, num_users);
    
    printf("Depósito de R$ %.2f realizado com sucesso.\n", amount);
}

int main() {
    load_users(users, &num_users);
    User current_user;
    int user_index = -1;
    char cpf[CPF_LENGTH];
    char password[PASSWORD_LENGTH];
    
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
    
    do {
        printf("\n=== Menu Principal ===\n");
        printf("1. Consultar saldo\n");
        printf("2. Consultar extrato\n");
        printf("3. Depositar reais\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        scanf("%d", &option);
        
        switch (option) {
            case 1:
                check_balance(&current_user);
                break;
            case 2:
                view_statement(current_user.cpf);
                break;
            case 3:
                printf("Digite o valor para depósito: R$ ");
                scanf("%lf", &amount);
                deposit_brl(&current_user, amount);
                users[user_index] = current_user;
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