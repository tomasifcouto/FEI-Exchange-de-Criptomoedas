## Participantes do Projeto

[Tomás Iervolino Fernandes Couto RA: 22.224.025-1] 
[Lucas Alves dos Anjos RA: 22.124.008-8]

# FEI-Exchange-de-Criptomoedas
Trabalho de desenvolvimento de algoritmos FEI
# Exchange de Criptomoedas

Este é um programa simples de exchange de criptomoedas que permite realizar operações básicas como consulta de saldo, extrato e operações com criptomoedas.

## Objetivo do Projeto

Criar uma Exchange de Criptomoedas que permita aos usuários:
- Gerenciar carteira de investimentos em reais e criptomoedas (Bitcoin, Ethereum e Ripple)
- Realizar operações de compra e venda com taxas específicas para cada criptomoeda
- Acompanhar cotações que variam aleatoriamente
- Manter histórico das transações

## Compilação

Para compilar o programa, você precisará de um compilador C (como o GCC). Execute o seguinte comando no terminal:

```bash
gcc -o exchange exchange.c
```

## Execução

Após compilar, execute o programa com:

```bash
./exchange
```

## Funcionalidades Implementadas

1. Login com CPF e senha
2. Consulta de saldo em reais e criptomoedas
3. Consulta de extrato de operações
4. Depósito de reais
5. Saque de reais (requer senha)
6. Compra de criptomoedas (requer senha)
7. Venda de criptomoedas (requer senha)
8. Visualização de cotações atualizadas

## Taxas de Operação

### Bitcoin (BTC)
- Taxa de compra: 2%
- Taxa de venda: 3%

### Ethereum (ETH)
- Taxa de compra: 1%
- Taxa de venda: 2%

### Ripple (XRP)
- Taxa de compra: 1%
- Taxa de venda: 1%

## Limites do Sistema

- Máximo de 10 usuários simultâneos
- Cada usuário pode ter até 100 transações armazenadas
- Transações excedentes substituem as mais antigas

## Usuário Padrão

Para teste inicial, um usuário padrão é criado automaticamente com as seguintes credenciais:
- CPF: 123456789
- Senha: 123456

## Armazenamento de Dados

O programa utiliza armazenamento binário para garantir a persistência dos dados:
- `users.dat`: Armazena os dados dos usuários e suas transações
  - Informações da conta (CPF, senha, saldos)
  - Histórico das últimas 100 transações de cada usuário

## Observações

- Todas as transações são registradas com data e hora
- O extrato mostra apenas as transações do usuário logado
- Os saldos são atualizados automaticamente após cada operação
- As cotações são atualizadas automaticamente com variação de ±5%
- Todas as operações que envolvem movimentação de valores requerem confirmação de senha
- Compra e venda de criptomoedas exigem confirmação do usuário após exibição dos detalhes da operação
