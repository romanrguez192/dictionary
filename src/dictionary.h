typedef struct element
{
    char key[80];
    void *value;
    char type;
    struct element *next;
} Element;

typedef struct dictionary
{
    Element *first;
} Dictionary;

typedef enum {true, false} Bool;

// Creates a new empty dictionary
Dictionary *newDictionary();

// Saves the number associated to the corresponding key in result.
// Returns 1 if it was able to get it otherwise returns 0
int getNumber(const Dictionary *dictionary, const char *key, double *result);

// Saves the boolean associated to the corresponding key in result.
// Returns 1 if it was able to get it otherwise returns 0
int getBool(const Dictionary *dictionary, const char *key, Bool *result);

// Returns the string associated to the corresponding key, otherwise returns NULL
char *getString(const Dictionary *dictionary, const char *key);

// Returns the dictionary associated to the corresponding key, otherwise returns NULL
Dictionary *getDictionary(const Dictionary *dictionary, const char *key);

// Returns the numeric array associated to the corresponding key, otherwise returns NULL
double *getNumberArray(const Dictionary *dictionary, const char *key, int *sizeResult);

// Returns the boolean array associated to the corresponding key, otherwise returns NULL
Bool *getBoolArray(const Dictionary *dictionary, const char *key, int *sizeResult);

// Returns the array of strings associated to the corresponding key, otherwise returns NULL
char **getStringArray(const Dictionary *dictionary, const char *key, int *sizeResult);

// Returns the array of dictionaries associated to the corresponding key, otherwise returns NULL
Dictionary **getDictionaryArray(const Dictionary *dictionary, const char *key, int *sizeResult);

// Removes the given key. Returns 1 if it was able to do it otherwise returns 0
int removeElement(Dictionary *dictionary, const char *key);

// Sets a number for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setNumber(Dictionary *dictionary, const char *key, double value);

// Sets a boolean for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setBool(Dictionary *dictionary, const char *key, Bool value);

// Sets a string for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setString(Dictionary *dictionary, const char *key, const char *value);

// Sets a dictionary for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setDictionary(Dictionary *dictionary, const char *key, Dictionary *value);

// Sets a numeric array for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setNumberArray(Dictionary *dictionary, const char *key, int size, double value[size]);

// Sets a boolean array for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setBoolArray(Dictionary *dictionary, const char *key, int size, Bool value[size]);

// Sets an array of strings for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setStringArray(Dictionary *dictionary, const char *key, int size, char *value[size]);

// Sets an array of dictionaries for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setDictionaryArray(Dictionary *dictionary, const char *key, int size, Dictionary *value[size]);

// Returns a new dictionary created from its json representation. If it can't parse the json returns NULL
Dictionary *dictionaryFromJson(const char *json);

// Returns the json representation string for the given dictionary. If it can't do it returns NULL
char *jsonFromDictionary(const Dictionary *dictionary);

// Releases the memory of the given dictionary
void freeDictionary(Dictionary *dictionary);
