#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_SENSORS 100
#define MAX_LINE_LENGTH 256
#define MAX_STRING_VALUE 16

typedef struct {
    time_t timestamp;
    char* value;
} SensorReading;

typedef struct {
    char* sensor_id;
    int data_type;
    SensorReading* readings;
    int count;
    int capacity;
} SensorData;

void process_file(const char* filename);
void write_sensor_data(SensorData* sensor);
int determine_data_type(const char* value);
int compare_timestamps(const void* a, const void* b);
void free_sensor_data(SensorData* sensors, int sensor_count);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo_de_entrada>\n", argv[0]);
        return 1;
    }

    process_file(argv[1]);
    return 0;
}

void process_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }

    SensorData sensors[MAX_SENSORS];
    int sensor_count = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        char* timestamp_str = strtok(line, " ");
        char* sensor_id = strtok(NULL, " ");
        char* value = strtok(NULL, " \n");

        if (!timestamp_str || !sensor_id || !value) continue;

        time_t timestamp = (time_t)atol(timestamp_str);
        int data_type = determine_data_type(value);

        int sensor_index = -1;
        for (int i = 0; i < sensor_count; i++) {
            if (strcmp(sensors[i].sensor_id, sensor_id) == 0) {
                sensor_index = i;
                break;
            }
        }

        if (sensor_index == -1) {
            if (sensor_count >= MAX_SENSORS) {
                fprintf(stderr, "Limite máximo de sensores atingido\n");
                continue;
            }
            sensor_index = sensor_count++;
            sensors[sensor_index].sensor_id = strdup(sensor_id);
            sensors[sensor_index].data_type = data_type;
            sensors[sensor_index].count = 0;
            sensors[sensor_index].capacity = 10;
            sensors[sensor_index].readings = malloc(sensors[sensor_index].capacity * sizeof(SensorReading));
        }

        if (sensors[sensor_index].count >= sensors[sensor_index].capacity) {
            sensors[sensor_index].capacity *= 2;
            sensors[sensor_index].readings = realloc(sensors[sensor_index].readings, 
                sensors[sensor_index].capacity * sizeof(SensorReading));
        }

        sensors[sensor_index].readings[sensors[sensor_index].count].timestamp = timestamp;
        sensors[sensor_index].readings[sensors[sensor_index].count].value = strdup(value);
        sensors[sensor_index].count++;
    }

    fclose(file);

    for (int i = 0; i < sensor_count; i++) {
        qsort(sensors[i].readings, sensors[i].count, sizeof(SensorReading), compare_timestamps);
        write_sensor_data(&sensors[i]);
    }

    free_sensor_data(sensors, sensor_count);
}

int determine_data_type(const char* value) {
    if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
        return 1;
    }
    
    char* endptr;
    long int_val = strtol(value, &endptr, 10);
    if (*endptr == '\0') {
        return 0;
    }
    
    double float_val = strtod(value, &endptr);
    if (*endptr == '\0') {
        return 2;
    }
    
    return 3;
}

int compare_timestamps(const void* a, const void* b) {
    const SensorReading* ra = (const SensorReading*)a;
    const SensorReading* rb = (const SensorReading*)b;
    if (ra->timestamp < rb->timestamp) return -1;
    if (ra->timestamp > rb->timestamp) return 1;
    return 0;
}

void write_sensor_data(SensorData* sensor) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.dat", sensor->sensor_id);
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Erro ao criar arquivo de sensor");
        return;
    }
    int i = sensor->count;
    do {
        fprintf(file, "%ld %s\n", sensor->readings[i-1].timestamp, sensor->readings[i-1].value);
        i--;
    } while (i != 0);
    
    fclose(file);
}

void free_sensor_data(SensorData* sensors, int sensor_count) {
    for (int i = 0; i < sensor_count; i++) {
        free(sensors[i].sensor_id);
        for (int j = 0; j < sensors[i].count; j++) {
            free(sensors[i].readings[j].value);
        }
        free(sensors[i].readings);
    }
}