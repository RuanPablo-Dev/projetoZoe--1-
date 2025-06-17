#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_SENSORS 10
#define READINGS_PER_SENSOR 2000
#define MAX_STRING_LENGTH 16

typedef struct {
    char* name;
    char* data_type;
} SensorInfo;

time_t convert_to_timestamp(int day, int month, int year, int hour, int min, int sec);
time_t random_timestamp_between(time_t start, time_t end);
void generate_random_value(const char* data_type, char* buffer);

int main(int argc, char* argv[]) {
    if (argc < 8 || (argc - 7) % 2 != 0) {
        printf("Uso: %s <inicio_dia> <inicio_mes> <inicio_ano> <fim_dia> <fim_mes> <fim_ano> <sensor1> <tipo1> ... <sensorN> <tipoN>\n", argv[0]);
        printf("Tipos: CONJ_Z=int, BINARIO=bool, CONJ_Q=float, TEXTO=string\n");
        return 1;
    }

    int start_day = atoi(argv[1]);
    int start_month = atoi(argv[2]);
    int start_year = atoi(argv[3]);
    int end_day = atoi(argv[4]);
    int end_month = atoi(argv[5]);
    int end_year = atoi(argv[6]);

    time_t start_time = convert_to_timestamp(start_day, start_month, start_year, 0, 0, 0);
    time_t end_time = convert_to_timestamp(end_day, end_month, end_year, 23, 59, 59);

    if (start_time == -1 || end_time == -1 || start_time > end_time) {
        printf("Datas inválidas\n");
        return 1;
    }

    int sensor_count = (argc - 7) / 2;
    if (sensor_count > MAX_SENSORS) {
        printf("Máximo de %d sensores suportado\n", MAX_SENSORS);
        return 1;
    }

    SensorInfo sensors[MAX_SENSORS];
    for (int i = 0; i < sensor_count; i++) {
        sensors[i].name = argv[7 + i*2];
        sensors[i].data_type = argv[8 + i*2];

        if (
            strcmp(sensors[i].data_type, "CONJ_Z") != 0 &&
            strcmp(sensors[i].data_type, "BINARIO") != 0 &&
            strcmp(sensors[i].data_type, "CONJ_Q") != 0 &&
            strcmp(sensors[i].data_type, "TEXTO") != 0
        ) {
            printf("Tipo de dado inválido para sensor %s\n", sensors[i].name);
            return 1;
        }
    }

    FILE* output = fopen("dadosGerados.txt", "w");
    if (!output) {
        perror("Erro ao criar arquivo de teste");
        return 1;
    }

    srand(time(NULL));
    for (int i = 0; i < sensor_count; i++) {
        for (int j = 0; j < READINGS_PER_SENSOR; j++) {
            time_t timestamp = random_timestamp_between(start_time, end_time);
            char value[MAX_STRING_LENGTH + 1];
            generate_random_value(sensors[i].data_type, value);

            fprintf(output, "%ld %s %s\n", (long)timestamp, sensors[i].name, value);
        }
    }

    fclose(output);
    printf("Arquivo de teste gerado com sucesso: dadosGerados.txt\n");
    printf("Total de leituras: %d\n", sensor_count * READINGS_PER_SENSOR);
    return 0;
}

time_t convert_to_timestamp(int day, int month, int year, int hour, int min, int sec) {
    struct tm tm = {0};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_isdst = -1;

    time_t timestamp = mktime(&tm);
    if (timestamp == -1) {
        fprintf(stderr, "Data inválida: %02d/%02d/%04d\n", day, month, year);
    }
    return timestamp;
}

time_t random_timestamp_between(time_t start, time_t end) {
    double range = difftime(end, start);
    double random_seconds = (double)rand() / (double)RAND_MAX * range;
    return start + (time_t)random_seconds;
}

void generate_random_value(const char* data_type, char* buffer) {
    if (strcmp(data_type, "CONJ_Z") == 0) {
        snprintf(buffer, MAX_STRING_LENGTH, "%d", rand() % 1000);
    } else if (strcmp(data_type, "BINARIO") == 0) {
        snprintf(buffer, MAX_STRING_LENGTH, "%s", rand() % 2 ? "true" : "false");
    } else if (strcmp(data_type, "CONJ_Q") == 0) {
        snprintf(buffer, MAX_STRING_LENGTH, "%.2f", (float)rand() / (float)RAND_MAX * 1000.0f);
    } else if (strcmp(data_type, "TEXTO") == 0) {
        const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        int len = 5 + rand() % (MAX_STRING_LENGTH - 5);
        for (int i = 0; i < len; i++) {
            buffer[i] = chars[rand() % strlen(chars)];
        }
        buffer[len] = '\0';
    } else {
        snprintf(buffer, MAX_STRING_LENGTH, "Tipo inválido");
    }
}
