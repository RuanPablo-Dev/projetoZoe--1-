# Projeto de Gerenciamento de Sensores Industriais

Este projeto consiste em três programas para gerenciar dados de sensores em uma planta industrial:

## Programas

1. **Organização dos Dados** (`programa1.c`)
   - Processa um arquivo com leituras de sensores misturadas
   - Separa os dados em arquivos por sensor
   - Ordena as leituras por timestamp em cada arquivo

2. **Consulta por Instant(Unix Epoch)** (`programa2.c`)
   - Localiza a leitura mais próxima de um timestamp específico
   - Usa busca binária para eficiência com grandes volumes de dados

3. **Geração de Arquivo de Teste** (`programa3.c`)
   - Cria arquivos de teste com dados aleatórios
   - Permite configurar período, sensores e tipos de dados

## Compilação dos programas

Compilação de cada programa abaixo:
```bash
gcc programa1.c -o programa1.exe
gcc programa2.c -o programa2.exe
gcc programa3.c -o programa3.exe
```

## Execução teste dos programas 

### Programa 3 - Geração de Arquivo de Teste

O comando abaixo serve para gerar um arquivo `.txt` com 2000 dados para cada sensor passado na execução do programa:

```bash
# Parametros: <inicioDia> <inicioMes> <inicioAno> <fimDia> <fimMes> <fimAno> <sensor1> <tipo1> ... <sensorN> <tipoN>

# Tipos: CONJ_Z=int, BINARIO=bool, CONJ_Q=float, TEXTO=string

./programa3.exe 1 5 2025 30 5 2025 sensor1 CONJ_Z sensor2 BINARIO sensor3 CONJ_Q sensor4 TEXTO
```

### Programa 1 - Organização dos Dados

O comando abaixo organiza os dados de um sensor em ordem sequêncial, da data mais antiga para a mais recente:

```bash
# Parametros: <nomeDoArquivoGerado>
./programa1.exe dadosGerados.txt 
```

### Programa 2 - Consulta por Instant(Unix Epoch)

O comando abaixo consulta uma medida no arquivo do sensor passado:

```bash
# Parametros: <sensor> <Instant>
./programa2 sensor1 1747362472
```
Ou se preferir pode escolher uma data específica no link:
https://www.epochconverter.com/