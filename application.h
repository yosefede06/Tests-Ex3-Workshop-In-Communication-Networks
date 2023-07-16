int kv_open(char *servername, void **kv_handle); /*Connect to server*/
int kv_set(void *kv_handle, const char *key, const char *value);
int kv_get(void *kv_handle, const char *key, char **value);
void kv_release(char *value);/* Called after get() on value pointer */
int kv_close(void *kv_handle); /* Destroys the QP */