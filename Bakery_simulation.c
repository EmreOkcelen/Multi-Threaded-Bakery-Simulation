#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUM_CHEFS 6 // Number of chefs

// Data structure representing ingredient information
typedef struct {
    char ingredient1[10];
    char ingredient2[10];
} Ingredients;

// Semaphores
sem_t ingredient_semaphore;
sem_t chef_semaphore[NUM_CHEFS];
sem_t delivery_semaphore;

int ingredients_available = 0; // Variable indicating if ingredients are available
int more_ingredients = 1; // This variable will be set to 0 when the supplier finishes their job

Ingredients ingredient_storage; // Struct to hold ingredients

// Each chef requires a different pair of ingredients; when matched correctly, the corresponding chef is activated
char chef_requirements[NUM_CHEFS][2] = {{'F', 'M'}, // Chefs' requirements
                                        {'E', 'C'}, {'C', 'F'}, {'M', 'C'},
                                        {'E', 'F'}, {'M', 'E'}};

// Supplier thread function
void *supplier_function(void *arg) {
    char *file_name = (char *)arg;
    FILE *ingredient_file = fopen(file_name, "r");
    if (ingredient_file == NULL) {
        fprintf(stderr, "Error: Could not open ingredient file.\n");
        exit(1);
    }

    char ingredient1, ingredient2;
    while (fscanf(ingredient_file, " %c %c", &ingredient1, &ingredient2) == 2) {
        sem_wait(&ingredient_semaphore);
        ingredient_storage.ingredient1[0] = '\0';
        ingredient_storage.ingredient2[0] = '\0';

        // Replace ingredient codes with names (e.g., F = Flour)
        switch (ingredient1) {
            case 'F':
                strcpy(ingredient_storage.ingredient1, "Flour");
                break;
            case 'M':
                strcpy(ingredient_storage.ingredient1, "Milk");
                break;
            case 'E':
                strcpy(ingredient_storage.ingredient1, "Egg");
                break;
            case 'C':
                strcpy(ingredient_storage.ingredient1, "Cocoa");
                break;
            default:
                strcpy(ingredient_storage.ingredient1, "Unknown");
        }
        switch (ingredient2) {
            case 'F':
                strcpy(ingredient_storage.ingredient2, "Flour");
                break;
            case 'M':
                strcpy(ingredient_storage.ingredient2, "Milk");
                break;
            case 'E':
                strcpy(ingredient_storage.ingredient2, "Egg");
                break;
            case 'C':
                strcpy(ingredient_storage.ingredient2, "Cocoa");
                break;
            default:
                strcpy(ingredient_storage.ingredient2, "Unknown");
        }

        sleep(5); // Wait time for supplier to deliver ingredients
        ingredients_available = 1; // Ingredients are available
        printf("Supplier delivered %s and %s.\n", ingredient_storage.ingredient1, ingredient_storage.ingredient2);

        // Wake up the appropriate chef threads
        for (int i = 0; i < NUM_CHEFS; i++) {
            if ((chef_requirements[i][0] == ingredient1 && chef_requirements[i][1] == ingredient2) ||
                (chef_requirements[i][0] == ingredient2 && chef_requirements[i][1] == ingredient1)) {
                sem_post(&chef_semaphore[i]); // Signal the appropriate chef semaphore
            }
        }

        sem_post(&ingredient_semaphore); // Signal the ingredient semaphore
        sem_wait(&delivery_semaphore); // Wait for cake delivery
    }

    fclose(ingredient_file); // Close the ingredient file

    // Supplier finished their job, notify the chefs
    sem_wait(&ingredient_semaphore); // Wait for the ingredient semaphore
    more_ingredients = 0; // No more ingredients available
    sem_post(&ingredient_semaphore); // Signal the ingredient semaphore
    for (int i = 0; i < NUM_CHEFS; i++) {
        sem_post(&chef_semaphore[i]); // Notify all chefs
    }
    return NULL;
}

// Chef thread function
void *chef_function(void *arg) {
    int chef_id = *(int *)arg; // Get chef ID
    free(arg);

    char ingredient_name1[10], ingredient_name2[10];

    // Convert ingredient codes to names using switch case
    switch (chef_requirements[chef_id][0]) {
        case 'F':
            strcpy(ingredient_name1, "flour");
            break;
        case 'M':
            strcpy(ingredient_name1, "milk");
            break;
        case 'E':
            strcpy(ingredient_name1, "egg");
            break;
        case 'C':
            strcpy(ingredient_name1, "cocoa");
            break;
        default:
            strcpy(ingredient_name1, "unknown");
    }
    switch (chef_requirements[chef_id][1]) {
        case 'F':
            strcpy(ingredient_name2, "flour");
            break;
        case 'M':
            strcpy(ingredient_name2, "milk");
            break;
        case 'E':
            strcpy(ingredient_name2, "egg");
            break;
        case 'C':
            strcpy(ingredient_name2, "cocoa");
            break;
        default:
            strcpy(ingredient_name2, "unknown");
    }

    printf("Chef %d is waiting for %s and %s.\n", chef_id + 1, ingredient_name1, ingredient_name2);

    while (1) {
        // Wait for ingredients to arrive
        sem_wait(&chef_semaphore[chef_id]); // Wait for the chef semaphore

        sem_wait(&ingredient_semaphore); // Wait for the ingredient semaphore
        if (!more_ingredients) { // If no more ingredients
            sem_post(&ingredient_semaphore); // Signal the ingredient semaphore
            break; // Exit the loop
        }

        if (!ingredients_available) { // If ingredients are not available
            sem_post(&ingredient_semaphore); // Signal the ingredient semaphore
            continue; // Continue waiting
        }

        // Get ingredients
        char ingredient1[10], ingredient2[10]; // Character arrays for ingredient names
        strcpy(ingredient1, ingredient_storage.ingredient1); // Copy the first ingredient
        strcpy(ingredient2, ingredient_storage.ingredient2); // Copy the second ingredient
        ingredients_available = 0; // Ingredients are no longer available

        printf("Chef %d received %s\n", chef_id + 1, ingredient1);
        printf("Chef %d received %s\n", chef_id + 1, ingredient2);
        sem_post(&ingredient_semaphore); // Signal the ingredient semaphore

        // Simulate cake preparation
        int preparation_time = (rand() % 5) + 1; // Preparation time
        printf("Chef %d started preparing the cake.\n", chef_id + 1);
        printf("Supplier is waiting for the cake...\n");
        sleep(preparation_time); // Wait for the preparation time
        printf("Chef %d finished preparing the cake.\n", chef_id + 1);

        // Deliver the cake to the supplier
        sem_post(&delivery_semaphore); // Signal the delivery semaphore
        printf("Chef %d delivered the cake to the supplier.\n", chef_id + 1);
        printf("Supplier received the cake and went to sell it.\n");
        // Indicate that the chef is waiting for ingredients again
        printf("Chef %d is waiting for %s and %s.\n", chef_id + 1, ingredient_name1, ingredient_name2);
    }
    sleep(10);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 3 || strcmp(argv[1], "-i") != 0) {
        fprintf(stderr, "Usage: %s -i <ingredient_file>\n", argv[0]);
        return 1;
    }

    char *ingredient_file_name = argv[2]; // Get the ingredient file name

    srand(time(NULL));

    // Initialize semaphores
    sem_init(&ingredient_semaphore, 0, 1);  // Initialize the ingredient semaphore
    sem_init(&delivery_semaphore, 0, 0); // Initialize the delivery semaphore
    for (int i = 0; i < NUM_CHEFS; i++) {
        sem_init(&chef_semaphore[i], 0, 0); // Initialize the chef semaphores
    }

    // Create chef threads
    pthread_t chef_threads[NUM_CHEFS];
    for (int i = 0; i < NUM_CHEFS; i++) {
        int *chef_id = malloc(sizeof(int)); // Allocate memory for chef ID
        *chef_id = i;
        pthread_create(&chef_threads[i], NULL, chef_function, chef_id); // Create chef thread
    }

    // Create supplier thread
    pthread_t supplier_id;
    pthread_create(&supplier_id, NULL, supplier_function, ingredient_file_name); // Create supplier thread

    // Wait for the supplier thread to finish
    pthread_join(supplier_id, NULL);
    printf("All ingredients have been used up! Notifying the chefs...\n");

    // Terminate chef threads
    for (int i = 0; i < NUM_CHEFS; i++) {
        sem_post(&chef_semaphore[i]); // Wake up and terminate chef threads
    }
    for (int i = 0; i < NUM_CHEFS; i++) {
        pthread_join(chef_threads[i], NULL);
    }

    // Clean up semaphores
    sem_destroy(&ingredient_semaphore);
    sem_destroy(&delivery_semaphore);
    for (int i = 0; i < NUM_CHEFS; i++) {
        sem_destroy(&chef_semaphore[i]);
    }
    printf("Work is done!\n");

    return 0;
}
