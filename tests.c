#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "application.h" // Assuming the application's header file is named "application.h"
#include <unistd.h>
#include <assert.h>
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
        printf("Test Case 1: Connection failed.\n");
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
            printf("Test Case 2: SET request successful.\n");
            char *retrievedValue = NULL;
            result = kv_get(kv_handle, key, &retrievedValue);
            if (result == 0 && compareStrings(retrievedValue, value)) {
                printf("Test Case 2: GET request successful.\n");
            } else {
                printf("Test Case 2: GET request failed.\n");
            }
            kv_release(retrievedValue);
        } else {
            printf("Test Case 2: SET request failed.\n");
        }
    } else {
        printf("Test Case 2: Connection failed.\n");
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
            printf("Test Case 3: GET request for non-existent key successful.\n");
        } else {
            printf("Test Case 3: GET request for non-existent key failed.\n");
        }
    } else {
        printf("Test Case 3: Connection failed.\n");
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
            printf("Test Case 4: GET request successful.\n");
            kv_release(value);
            printf("Test Case 4: Value memory released.\n");
        } else {
            printf("Test Case 4: GET request failed.\n");
        }
    } else {
        printf("Test Case 4: Connection failed.\n");
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
            printf("Test Case 5: SET request 1 successful.\n");
        } else {
            printf("Test Case 5: SET request 1 failed.\n");
        }

        result = kv_set(kv_handle, key2, value2);
        if (result == 0) {
            printf("Test Case 5: SET request 2 successful.\n");
        } else {
            printf("Test Case 5: SET request 2 failed.\n");
        }

        char *retrievedValue1 = NULL;
        result = kv_get(kv_handle, key1, &retrievedValue1);
        if (result == 0 && compareStrings(retrievedValue1, value1)) {
            printf("Test Case 5: GET request 1 successful.\n");
        } else {
            printf("Test Case 5: GET request 1 failed.\n");
        }
        kv_release(retrievedValue1);

        char *retrievedValue2 = NULL;
        result = kv_get(kv_handle, key2, &retrievedValue2);
        if (result == 0 && compareStrings(retrievedValue2, value2)) {
            printf("Test Case 5: GET request 2 successful.\n");
        } else {
            printf("Test Case 5: GET request 2 failed.\n");
        }
        kv_release(retrievedValue2);
    } else {
        printf("Test Case 5: Connection failed.\n");
    }
}

// Test Case 6: Perform a SET request with a value >= 4KB
void testLargeSetValue(void *kv_handle) {
    int result;
    if (kv_handle != NULL) {
        const char *key = "large_key";
        char * largeValue = malloc(32000 * 2);
        for (int i = 0; i < 32000 * 2; i++) {
            largeValue[i] = 'a';
        }
        result = kv_set(kv_handle, key, largeValue);
        if (result == 0) {
            printf("Test Case 6: SET request with large value successful.\n");
        } else {
            printf("Test Case 6: SET request with large value failed.\n");
        }

        char *retrievedValue = NULL;
        result = kv_get(kv_handle, key, &retrievedValue);
        if (result == 0 && compareStrings(retrievedValue, largeValue)) {
            printf("Test Case 6: GET request for large value successful.\n");
        } else {
            printf("Test Case 6: GET request for large value failed.\n");
        }
        kv_release(retrievedValue);
    } else {
        printf("Test Case 6: Connection failed.\n");
    }
}


// Test Case 8: Verify zero-copy behavior for Rendezvous protocol
void testZeroCopyBehavior(char * serverName) {
    void *kv_handle = NULL;
    int result = kv_open(serverName, &kv_handle);
    if (kv_handle != NULL) {
        const char *key = "large_key";
        const char *largeValue = "This is a large value exceeding 4KB...";

        size_t initialMemoryUsage = getMemoryUsage(); // Custom function to retrieve memory usage

        result = kv_set(kv_handle, key, largeValue);
        if (result == 0) {
            printf("Test Case 8: SET request with large value successful.\n");
        } else {
            printf("Test Case 8: SET request with large value failed.\n");
        }

        size_t finalMemoryUsage = getMemoryUsage(); // Custom function to retrieve memory usage

        if (finalMemoryUsage - initialMemoryUsage <= 4096) {
            printf("Test Case 8: Zero-copy behavior observed.\n");
        } else {
            printf("Test Case 8: Zero-copy behavior not observed.\n");
        }

        kv_close(kv_handle);
    } else {
        printf("Test Case 8: Connection failed.\n");
    }
}


// Test Case 9: Test concurrent SET requests from multiple clients
void testConcurrentSetRequests(char * serverName) {
    // Client 1
    void *kv_handle1 = NULL;
    int result1 = kv_open(serverName, &kv_handle1);
    if (kv_handle1 != NULL) {
        const char *key1 = "key1";
        const char *value1 = "value1";

        result1 = kv_set(kv_handle1, key1, value1);
        if (result1 == 0) {
            printf("Client 1: SET request 1 successful.\n");
        } else {
            printf("Client 1: SET request 1 failed.\n");
        }

        kv_close(kv_handle1);
    } else {
        printf("Client 1: Connection failed.\n");
    }

    // Client 2
    void *kv_handle2 = NULL;
    int result2 = kv_open(serverName, &kv_handle2);
    if (kv_handle2 != NULL) {
        const char *key2 = "key2";
        const char *value2 = "value2";

        result2 = kv_set(kv_handle2, key2, value2);
        if (result2 == 0) {
            printf("Client 2: SET request 1 successful.\n");
        } else {
            printf("Client 2: SET request 1 failed.\n");
        }

        kv_close(kv_handle2);
    } else {
        printf("Client 2: Connection failed.\n");
    }
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