# Low-Latency Protocols Exercise Tests

These tests are designed to verify the functionality of the Low-Latency Protocols exercise.
The exercise involves implementing a single-threaded server-client application using the Verbs API, comparing the Rendezvous protocol to a trivial "eager" case.

## Prerequisites

Before running the tests, ensure that you have the following:

- A C compiler that supports the necessary features.
- The application code and header files for the Low-Latency Protocols exercise.
- A server set up to run the application.

## Running the Tests

1. Clone or download the repository containing the test code.
2. Ensure that you have the necessary application code and header files in the same directory as the test code.
3. Add these rows of code at the beginning of your main code (bw_template.c):

   ```
   // ------------------ IMPORT TESTS ------------------
    #include "tests.c"
    // ------------------ IMPORT TESTS ------------------
   ```

4. Run your main code (for example bw_template.c) running the server which will handle set/get requests
5. For the client side call:
   ```
   run_tests_one_client(servername) or run_tests_multiple_clients(servername)
   ```
   (depending if you want to test 1 client or multiple clients)

Impotant note: 
    **In case you handle requests after accepting socket connections from the server to the new clients and you want to run the run_tests_multiple_clients(servername) test, you need to connect first all X clients in an interval of T seconds, the default is set as 5 seconds.**
    
The tests will be executed, and the results will be displayed on the console.

## Test Cases

The tests cover different aspects of the application's functionality:

1. **Test Connection**: Verifies that the client can successfully connect to the server.

2. **Test Set and Get**: Tests the SET and GET requests to verify successful storage and retrieval of key-value pairs.

3. **Test Get Non-Existent Key**: Tests the GET request for a non-existent key and ensures that it returns NULL.

4. **Test Release Value Memory**: Tests the successful release of memory after a GET request.

5. **Test Multiple Set and Get**: Tests the storage and retrieval of multiple key-value pairs.

6. **Test Large Set Value**: Tests the handling of SET requests with a value larger than or equal to 4KB.

7. **Test Zero Copy Behavior**: Tests the zero-copy behavior of the Rendezvous protocol.

## Notes

- The tests assume that the application functions are correctly implemented according to the exercise description.
- The tests focus on verifying basic functionality and behavior.
- The memory usage test (Test Case 7) uses a custom function `getMemoryUsage()` to measure memory differences before and after the SET request. Ensure that the `getMemoryUsage()` function is implemented correctly for accurate results.

Feel free to modify the tests or add new test cases as needed to cover specific scenarios or requirements of the exercise.
