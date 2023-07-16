#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h" // Assuming the application's header file is named "application.h"
#include <unistd.h>
#define RESET   "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */

// ------------------------- Helper Functions -------------------------

int compareStrings(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}

size_t getMemoryUsage() {
    FILE* statmFile = fopen("/proc/self/statm", "r");
    if (statmFile == NULL) {
        // Error handling
        return 0;
    }

    size_t pageSize = getpagesize();
    size_t residentSetSize;
    if (fscanf(statmFile, "%*s %zu", &residentSetSize) != 1) {
        // Error handling
        fclose(statmFile);
        return 0;
    }

    fclose(statmFile);
    return residentSetSize * pageSize;
}

// ------------------------- Test Cases -------------------------

// Test Case 1: Verify successful connection to the server
void testConnection(char * serverName, void ** kv_handle) {
    int result = kv_open(serverName, kv_handle);
    if (!result && *kv_handle != NULL) {
        printf(GREEN "Test Case 1: Successful connection.\n" RESET);
    } else {
        printf(RED "Test Case 1: Connection failed.\n" RESET);
    }
}

// Test Case 2: Perform a SET request and verify successful storage
void testSetAndGet(void *kv_handle) {
    int result;
    if (kv_handle != NULL) {
        char *key = "key1";
        char *value = "value1";
        result = kv_set(kv_handle, key, value);
        if (result == 0) {
            printf(GREEN "Test Case 2: SET request successful.\n" RESET);
            char *retrievedValue = NULL;
            result = kv_get(kv_handle, key, &retrievedValue);
            if (result == 0 && compareStrings(retrievedValue, value)) {
                printf(GREEN "Test Case 2: GET request successful.\n" RESET);
            } else {
                printf(RED "Test Case 2: GET request failed.\n" RESET);
            }
            kv_release(retrievedValue);
        } else {
            printf(RED "Test Case 2: SET request failed.\n" RESET);
        }
    } else {
        printf(RED "Test Case 2: Connection failed.\n" RESET);
    }
}

// Test Case 3: Perform a GET request for a non-existent key
void testGetNonExistentKey(void *kv_handle) {
    int result;
    if (kv_handle != NULL) {
        const char *key = "nonexistent_key";
        char *value = NULL;
        result = kv_get(kv_handle, key, &value);
        if (result == 0 && compareStrings(value, "")) {
            printf(GREEN "Test Case 3: GET request for non-existent key successful.\n" RESET);
        } else {
            printf(RED "Test Case 3: GET request for non-existent key failed.\n" RESET);
        }
    } else {
        printf(RED "Test Case 3: Connection failed.\n" RESET);
    }
}

// Test Case 4: Release value memory after GET request
void testReleaseValueMemory(void *kv_handle) {
    int result;
    if (kv_handle != NULL) {
        const char *key = "key1";
        char *value = NULL;
        result = kv_get(kv_handle, key, &value);
        if (result == 0 && value != NULL) {
            printf(GREEN "Test Case 4: GET request successful.\n" RESET);
            kv_release(value);
            printf(GREEN "Test Case 4: Value memory released.\n" RESET);
        } else {
            printf(RED "Test Case 4: GET request failed.\n" RESET);
        }
    } else {
        printf(RED "Test Case 4: Connection failed.\n" RESET);
    }
}

// Test Case 5: Perform multiple SET requests and verify storage
void testMultipleSetAndGet(void *kv_handle) {
    int result;
    if (kv_handle != NULL) {
        const char *key1 = "key1";
        const char *value1 = "value1";
        const char *key2 = "key2";
        const char *value2 = "value2";

        result = kv_set(kv_handle, key1, value1);
        if (result == 0) {
            printf(GREEN "Test Case 5: SET request 1 successful.\n" RESET);
        } else {
            printf(RED "Test Case 5: SET request 1 failed.\n" RESET);
        }
        result = kv_set(kv_handle, key2, value2);
        if (result == 0) {
            printf(GREEN "Test Case 5: SET request 2 successful.\n" RESET);
        } else {
            printf(RED "Test Case 5: SET request 2 failed.\n" RESET);
        }

        char *retrievedValue1 = NULL;
        result = kv_get(kv_handle, key1, &retrievedValue1);
        if (result == 0 && compareStrings(retrievedValue1, value1)) {
            printf(GREEN "Test Case 5: GET request 1 successful.\n" RESET);
        } else {
            printf(RED "Test Case 5: GET request 1 failed.\n" RESET);
            printf("Expected Value: %s, Gotten: %s\n", value1, retrievedValue1);
        }
        kv_release(retrievedValue1);

        char *retrievedValue2 = NULL;
//        sleep(1);
        result = kv_get(kv_handle, key2, &retrievedValue2);
        if (result == 0 && compareStrings(retrievedValue2, value2)) {
            printf(GREEN "Test Case 5: GET request 2 successful.\n" RESET);
        } else {
            printf(RED "Test Case 5: GET request 2 failed.\n" RESET);
        }
        kv_release(retrievedValue2);
    } else {
        printf(RED "Test Case 5: Connection failed.\n" RESET);
    }
}

// Test Case 6: Perform a SET request with a value >= 4KB
void testLargeSetValue(void *kv_handle) {
    int result;
    if (kv_handle != NULL) {
        const char *key = "large_key";
        size_t message_size = 32000 * 2;
        char * largeValue = calloc(message_size, sizeof(char));
        memset(largeValue, 'a', message_size);
        result = kv_set(kv_handle, key, largeValue);
        if (result == 0) {
            printf(GREEN "Test Case 6: SET request with large value successful.\n" RESET);
        } else {
            printf(RED "Test Case 6: SET request with large value failed.\n" RESET);
        }

        char *retrievedValue = NULL;
        result = kv_get(kv_handle, key, &retrievedValue);
        if (result == 0 && compareStrings(retrievedValue, largeValue)) {
            printf(GREEN "Test Case 6: GET request for large value successful.\n" RESET);
        } else {
            printf(RED "Test Case 6: GET request for large value failed.\n" RESET);
        }
        kv_release(retrievedValue);
    } else {
        printf(RED "Test Case 6: Connection failed.\n" RESET);
    }
}



// Test Case 7: Verify zero-copy behavior for Rendezvous protocol
void testZeroCopyBehavior(void *kv_handle ) {
    int result;
    if (kv_handle != NULL) {
        const char *key = "large_key2";
        size_t message_size = 32000 * 2;
        char * largeValue = calloc(message_size, sizeof(char));
        memset(largeValue, 'a', message_size);
        size_t initialMemoryUsage = getMemoryUsage();
        result = kv_set(kv_handle, key, largeValue);
        if (result == 0) {
            printf(GREEN "Test Case 8: SET request with large value successful.\n" RESET);
        } else {
            printf(RED "Test Case 8: SET request with large value failed.\n" RESET);
        }
        size_t finalMemoryUsage = getMemoryUsage();
        if (finalMemoryUsage - initialMemoryUsage == 0) {
            printf(GREEN "Test Case 8: Zero-copy behavior observed.\n" RESET);
        } else {
            printf(RED "Test Case 8: Zero-copy behavior not observed.\n" RESET);
        }
    } else {
        printf(RED "Test Case 8: Connection failed.\n" RESET);
    }
}



/**
 * NOT PART OF THE API
 * @param kv_handle
 */
void run_tests_helper(void * kv_handle) {
    testSetAndGet(kv_handle);
    testGetNonExistentKey(kv_handle);
    testReleaseValueMemory(kv_handle);
    testMultipleSetAndGet(kv_handle);
    testLargeSetValue(kv_handle);
    testZeroCopyBehavior(kv_handle);
}

void run_tests_one_client(char * servername) {
    void *kv_handle;
    testConnection(servername, &kv_handle);
    run_tests_helper(kv_handle);
}

/**
 * TO RUN THIS TEST CONNECT X CLIENTS IN T SECONDS
 * @param servername
 */
void run_tests_multiple_clients(char * servername) {
    void *kv_handle;
    testConnection(servername, &kv_handle);
    sleep(5);
    run_tests_helper(kv_handle);
    sleep(1);
    run_tests_helper(kv_handle);
    sleep(1);
    run_tests_helper(kv_handle);
    sleep(1);
    run_tests_helper(kv_handle);
    sleep(1);
    run_tests_helper(kv_handle);
    sleep(1);
    run_tests_helper(kv_handle);
}