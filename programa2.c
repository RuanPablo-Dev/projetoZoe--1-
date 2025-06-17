#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 256

typedef struct {
    time_t timestamp;
    char value[MAX_LINE_LENGTH];
} SensorReading;

int load_sensor_data(const char* sensor_id, SensorReading** readings);
int find_closest_reading(SensorReading* readings, int count, time_t target);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Uso: %s <sensor_id> <timestamp>\n", argv[0]);
        return 1;
    }

    char* sensor_id = argv[1];
    time_t target_timestamp = (time_t)atol(argv[2]);

    SensorReading* readings = NULL;
    int count = load_sensor_data(sensor_id, &readings);
    if (count == -1) {
        printf("Sensor não encontrado ou erro ao carregar dados\n");
        return 1;
    }

    int closest_index = find_closest_reading(readings, count, target_timestamp);
    if (closest_index != -1) {
        printf("Leitura mais próxima:\n");
        printf("Timestamp: %ld\n", readings[closest_index].timestamp);
        printf("Valor: %s\n", readings[closest_index].value);
    } else {
        printf("Nenhuma leitura encontrada\n");
    }

    free(readings);
    return 0;
}

int load_sensor_data(const char* sensor_id, SensorReading** readings) {
    char filename[256];
    snprintf(filename, sizeof(filename), "%s.dat", sensor_id);

    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;
    }

    int count = 0;
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) count++;

    *readings = malloc(count * sizeof(SensorReading));
    if (!*readings) {
        fclose(file);
        return -1;
    }

    rewind(file);
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        char* timestamp_str = strtok(line, " ");
        char* value = strtok(NULL, "\n");

        (*readings)[i].timestamp = (time_t)atol(timestamp_str);
        strncpy((*readings)[i].value, value, MAX_LINE_LENGTH - 1);
        (*readings)[i].value[MAX_LINE_LENGTH - 1] = '\0';
        i++;
    }

    fclose(file);
    return count;
}

int find_closest_reading(SensorReading* readings, int count, time_t target) {
    if (count == 0) return -1;

    int left = 0;
    int right = count - 1;
    int mid = 0;

    while (left <= right) {
        mid = left + (right - left) / 2;

        if (readings[mid].timestamp == target) {
            return mid;
        } else if (readings[mid].timestamp > target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    int candidates[3];
    int candidate_count = 0;

    if (left < count) candidates[candidate_count++] = left;
    if (right >= 0) candidates[candidate_count++] = right;
    if (mid >= 0 && mid < count && mid != left && mid != right) candidates[candidate_count++] = mid;

    if (candidate_count == 0) return -1;

    int closest = candidates[0];
    time_t min_diff = labs(target - readings[closest].timestamp);

    for (int i = 1; i < candidate_count; i++) {
        time_t diff = labs(target - readings[candidates[i]].timestamp);
        if (diff < min_diff) {
            min_diff = diff;
            closest = candidates[i];
        }
    }

    return closest;
}
