#include <stdio.h>

#define SIZE 4

struct hash_item {
	int initial;
	int changed;
};

void generate_hash_map(int *initial_array, struct hash_item *hash_map) {
	int max = initial_array[SIZE - 1];
	for (int i = 0; i < SIZE; i++) {
		max++;
		hash_map[i].initial = initial_array[i];
		hash_map[i].changed = max;
	}
}

void permutation(int *array) {
	int current_permutation[SIZE];
	for (int i = 0; i < SIZE; i++) {
		current_permutation[i] = 0;
	}
	struct hash_item hash_map[SIZE];
	generate_hash_map(array, hash_map);
	printf("HASMAP:\n");
	for (int i = 0; i < SIZE; i++) {
		printf("ITEM %d: KEY = %d, VALUE = %d\n", i, hash_map[i].initial, hash_map[i].changed);
	}
	int cursor = SIZE - 1;
	while (cursor >= 0) {
		if (current_permutation[cursor] == 0) {
			array[cursor] = hash_map[cursor].changed;
			current_permutation[cursor] = 1;
			for (int i = cursor + 1; i < SIZE; i++) {
				array[i] = hash_map[i].initial;
				current_permutation[i] = 0;
			}
			cursor = SIZE - 1;
			printf("CURRENT PERMUTATION:\n");
			for (int i = 0; i < SIZE; i++) {
				printf("%d ", array[i]);
			}
			printf("\n");
		} else {
			cursor--;
		}
	}
}

int main() {
	int initial_array[SIZE] = {0, 1, 2, 3};
	permutation(initial_array);
	return 0;
}
