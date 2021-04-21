#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

typedef struct
{
    int size;
    void *elements;
    char type;
} Array;

void errorMessage();
void freeDictionary(Dictionary *dictionary);
void freeElement(Element *element);
Element *newElement(const char *key, char type, void *value);
double *copyNumber(const double *number);
Bool *copyBool(const Bool *value);
char *copyString(const char *s);
Array *newArray(void *elements, int size, char type);
double *copyNumberArray(int size, double value[size]);
Bool *copyBoolArray(int size, Bool value[size]);
char **copyStringArray(int size, char *value[size]);
Dictionary *copyDictionary(const Dictionary *dictionary);
Dictionary **copyDictionaryArray(int size, Dictionary *value[size]);
void addElement(Dictionary *dictionary, Element *newp);
int setValue(Dictionary *dictionary, const char *key, const void *value, char type);
int setArray(Dictionary *dictionary, const char *key, int size, void *value, char type);
char *concat(char *s1, char *s2);
int isNumber(char *str);
char **split(char *str, int *size);
int keyExists(const char *key, const Dictionary *dictionary);

// Muestra un mensaje de error cuando no hay memoria disponible y aborta la ejecución
void errorMessage()
{
    fprintf(stderr, "Error: No hay memoria...\n");
    exit(1);
}

// Creates a new empty dictionary
Dictionary *newDictionary()
{
    Dictionary *d;
    if ((d = (Dictionary *) malloc(sizeof(Dictionary))) == NULL)
        errorMessage();

    d->first = NULL;
    return d;
}

// Hace free a un elemento de un diccionario
void freeElement(Element *element)
{
    if (element->type == 'n' || element->type == 'b' || element->type == 's')
        free(element->value);
    else if (element->type == 'd')
        freeDictionary(element->value);
    else if (element->type == 'a')
    {
        int arrayType = ((Array *) element->value)->type;

        // Si es un arreglo numérico o booleano simplemente se le hace free al arreglo de elementos
        if (arrayType == 'n' || arrayType == 'b')
            free(((Array *) element->value)->elements);
        // En cambio si es un arreglo de strings o diccionarios se le hace free a cada uno de los elementos
        // Porque fueron creados con memoria dinámica
        else if (arrayType == 's')
        {
            int i;
            for(i = 0; i < ((Array *) element->value)->size; i++)
                    free(((char **) ((Array *) element->value)->elements)[i]);
        }
        else if (arrayType == 'd')
        {
            int i;
            for(i = 0; i < ((Array *) element->value)->size; i++)
                freeDictionary(((Dictionary **) ((Array *) element->value)->elements)[i]);
        }

        free(element->value); // Se le hace free a la estructura Array
    }
    free(element);
}

// Releases the memory of the given dictionary
void freeDictionary(Dictionary *dictionary)
{
    if (!dictionary)
        return;

    Element *aux;
    for(; dictionary->first; dictionary->first = aux)
    {
        aux = dictionary->first->next;
        freeElement(dictionary->first);
    }

    free(dictionary);
}

// Removes the given key. Returns 1 if it was able to do it otherwise returns 0
int removeElement(Dictionary *dictionary, const char *key)
{
    if(!dictionary)
        return 0;

    Element *aux, *prev = NULL;

    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if (!strcmp(aux->key, key))
        {
            if(!prev)
                dictionary->first = aux->next;
            else
                prev->next = aux->next;
            freeElement(aux);
            return 1;
        }
        prev = aux;
    }

    return 0;
}

// Crea un nuevo elemento de clave key, tipo type y valor value
Element *newElement(const char *key, char type, void *value)
{
    Element *newp;
    if ((newp = (Element *) malloc(sizeof(Element))) == NULL)  // Crea un nuevo elemento en el diccionario
        errorMessage();

    strcpy(newp->key, key);
    newp->type = type;
    newp->value = value;
    newp->next = NULL;
    return newp;
}

// Retorna una nueva dirección de memoria que contendrá una copia de number
double *copyNumber(const double *number)
{
    double *copy;
    if ((copy = (double *) malloc(sizeof(double))) == NULL) // Asigna el espacio de memoria donde se guardará el valor
        errorMessage();

    *copy = *number;
    return copy;
}

// Retorna una nueva dirección de memoria que contendrá una copia de value
Bool *copyBool(const Bool *value)
{
    Bool *copy;
    if ((copy = (Bool *) malloc(sizeof(Bool))) == NULL) // Asigna el espacio de memoria donde se guardará el valor
        errorMessage();

    *copy = *value;
    return copy;
}

// Retorna una copia del string s
char *copyString(const char *s)
{
    char *str;
    if ((str = (char *) malloc(sizeof(char) * (strlen(s) + 1))) == NULL) // Asigna el espacio suficiente para guardar el string
        errorMessage();
    strcpy(str, s);
    return str;
}

// Crea un nuevo arreglo de la estructura Array de tipo type cuyos elementos serán los de elements
Array *newArray(void *elements, int size, char type)
{
    Array *newp;

    if ((newp = (Array *) malloc(sizeof(Array))) == NULL) // Asigna el espacio de memoria donde se guardará el arreglo
        errorMessage();

    newp->elements = elements;
    newp->type = type;
    newp->size = size;
    return newp;
}

// Crea una copia del arreglo numérico
double *copyNumberArray(int size, double value[size])
{
    double *arrayElements;

    if ((arrayElements = (double *) malloc(sizeof(double) * size)) == NULL) // Asigna el espacio de memoria para guardar los elementos
        errorMessage();

    int i;
    for(i = 0; i < size; i++)
        arrayElements[i] = value[i]; // Copia los elementos

    return arrayElements;
}

// Crea una copia del arreglo booleano
Bool *copyBoolArray(int size, Bool value[size])
{
    Bool *arrayElements;

    if ((arrayElements = (Bool *) malloc(sizeof(Bool) * size)) == NULL) // Asigna el espacio de memoria para guardar los elementos
        errorMessage();

    int i;
    for(i = 0; i < size; i++)
        arrayElements[i] = value[i]; // Copia los elementos

    return arrayElements;
}

// Crea una copia del arreglo de strings
char **copyStringArray(int size, char *value[size])
{
    char **arrayElements;

    if ((arrayElements = (char **) malloc(sizeof(char *) * size)) == NULL) // Asigna el espacio de memoria para guardar el arreglo
        errorMessage();

    int i;
    for(i = 0; i < size; i++) // Copia los elementos
        arrayElements[i] = copyString(value[i]);

    return arrayElements;
}

// Crea una copia del arreglo de diccionarios
Dictionary **copyDictionaryArray(int size, Dictionary *value[size])
{
    Dictionary **arrayElements;

    if ((arrayElements = (Dictionary **) malloc(sizeof(Dictionary *) * size)) == NULL) // Asigna el espacio de memoria para guardar el arreglo
        errorMessage();

    int i;
    for(i = 0; i < size; i++) // Copia los elementos
        arrayElements[i] = copyDictionary(value[i]);

    return arrayElements;
}

// Crea una copia de un diccionario
// En esta función no se hace uso de las funciones setNumber, setString, etc. con el fin de evitar tener que
// Iterar tantas veces sobre el nuevo diccionario, pues ya se tiene un puntero hacia el último elemento que se va agregando.
Dictionary *copyDictionary(const Dictionary *dictionary)
{
    if (!dictionary)
        return NULL;

    Dictionary *d = newDictionary();
    Element *auxOriginal, *auxCopy, *newp;
    // auxOriginal es un auxiliar usado con los elementos del diccionario original
    // auxCopy es un auxiliar usado con los elementos del nuevo diccionario

    auxCopy = NULL;
    for(auxOriginal = dictionary->first; auxOriginal; auxOriginal = auxOriginal->next)
    {
        int size;
        void *value;
        switch (auxOriginal->type)
        {
            case 'n':
                newp = newElement(auxOriginal->key, auxOriginal->type, copyNumber(auxOriginal->value));
                break;
            case 'b':
                newp = newElement(auxOriginal->key, auxOriginal->type, copyBool(auxOriginal->value));
                break;
            case 's':
                newp = newElement(auxOriginal->key, auxOriginal->type, copyString(auxOriginal->value));
                break;
            case 'd':
                newp = newElement(auxOriginal->key, auxOriginal->type, copyDictionary(auxOriginal->value));
                break;
            case 'a':
                size = ((Array *) auxOriginal->value)->size;
                value = ((Array *) auxOriginal->value)->elements;
                switch (((Array *) auxOriginal->value)->type)
                {
                    case 'n':
                        newp = newElement(auxOriginal->key, 'a', newArray(copyNumberArray(size, value), size, 'n'));
                        break;
                    case 'b':
                        newp = newElement(auxOriginal->key, 'a', newArray(copyBoolArray(size, value), size, 'b'));
                        break;
                    case 's':
                        newp = newElement(auxOriginal->key, 'a', newArray(copyStringArray(size, value), size, 's'));
                        break;
                    case 'd':
                        newp = newElement(auxOriginal->key, 'a', newArray(copyDictionaryArray(size, value), size, 'd'));
                        break;
                }
                break;
        }
        if (!auxCopy)
        {
            d->first = newp;
            auxCopy = d->first;
        }
        else
        {
            auxCopy->next = newp;
            auxCopy = auxCopy->next;
        }
    }

    return d;
}

// Conecta el nuevo elemento al final de la lista de elementos del diccionario
void addElement(Dictionary *dictionary, Element *newp)
{
    if(!dictionary->first)
        dictionary->first = newp;
    else
    {
        Element *aux;
        for(aux = dictionary->first; aux->next; aux = aux->next);
        aux->next = newp;
    }
}

// Generalizacion de las funciones setNumber, setBool, setString, setDictionary
int setValue(Dictionary *dictionary, const char *key, const void *value, char type)
{
    if (!dictionary)
        return 0;

    Element *newp;

    switch (type)
    {
        case 'n':
            newp = newElement(key, type, copyNumber(value));
            break;
        case 'b':
            newp = newElement(key, type, copyBool(value));
            break;
        case 's':
            newp = newElement(key, type, copyString(value));
            break;
        case 'd':
            newp = newElement(key, type, copyDictionary(value));
            break;
    }

    removeElement(dictionary, key); // Si la clave ya existe, se elimina, si no existe, no pasa nada
    addElement(dictionary, newp);
    return 1;
}

// Sets a number for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setNumber(Dictionary *dictionary, const char *key, double value)
{
    return setValue(dictionary, key, &value, 'n');
}

// Saves the number associated to the corresponding key in result.
// Returns 1 if it was able to get it otherwise returns 0
int getNumber(const Dictionary *dictionary, const char *key, double *result)
{
    if (!dictionary)
        return 0;

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if(!strcmp(aux->key, key))
        {
            if(aux->type == 'n')
            {
                *result = *( (double *) aux->value);
                return 1;
            }
            return 0;
        }
    }

    return 0;
}

// Sets a boolean for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setBool(Dictionary *dictionary, const char *key, Bool value)
{
    return setValue(dictionary, key, &value, 'b');
}

// Saves the boolean associated to the corresponding key in result.
// Returns 1 if it was able to get it otherwise returns 0
int getBool(const Dictionary *dictionary, const char *key, Bool *result)
{
    if (!dictionary)
        return 0;

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if(!strcmp(aux->key, key))
        {
            if(aux->type == 'b')
            {
                *result = *( (Bool *) aux->value);
                return 1;
            }
            return 0;
        }
    }

    return 0;
}

// Sets a string for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setString(Dictionary *dictionary, const char *key, const char *value)
{
    return setValue(dictionary, key, value, 's');
}

// Returns the string associated to the corresponding key, otherwise returns NULL
char *getString(const Dictionary *dictionary, const char *key)
{
    if (!dictionary)
        return NULL;

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if(!strcmp(aux->key, key))
        {
            if(aux->type == 's')
                return copyString(aux->value); // Es necesario una copia del string

            return NULL;
        }
    }

    return NULL;
}

// Generalización de las funciones setNumberArray, setBoolArray, setStringArray y setDictionaryArray
int setArray(Dictionary *dictionary, const char *key, int size, void *value, char type)
{
    if (!dictionary)
        return 0;

    Element *newp;

    switch (type)
    {
        case 'n':
            newp = newElement(key, 'a', newArray(copyNumberArray(size, value), size, 'n'));
            break;
        case 'b':
            newp = newElement(key, 'a', newArray(copyBoolArray(size, value), size, 'b'));
            break;
        case 's':
            newp = newElement(key, 'a', newArray(copyStringArray(size, value), size, 's'));
            break;
        case 'd':
            newp = newElement(key, 'a', newArray(copyDictionaryArray(size, value), size, 'd'));
            break;
    }

    removeElement(dictionary, key); // Si la clave ya existe, se elimina, si no existe, no pasa nada
    addElement(dictionary, newp);
    return 1;
}

// Sets a numeric array for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setNumberArray(Dictionary *dictionary, const char *key, int size, double value[size])
{
    return setArray(dictionary, key, size, value, 'n');
}

// Returns the numeric array associated to the corresponding key, otherwise returns NULL
double *getNumberArray(const Dictionary *dictionary, const char *key, int *sizeResult)
{
    if (!dictionary)
        return NULL;

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if(!strcmp(aux->key, key))
        {
            if(aux->type == 'a' && ((Array *) aux->value)->type == 'n')
            {
                *sizeResult = ((Array *) aux->value)->size;
                return copyNumberArray(*sizeResult, ((Array *) aux->value)->elements);
            }
            return NULL;
        }
    }

    return NULL;
}

// Sets a boolean array for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setBoolArray(Dictionary *dictionary, const char *key, int size, Bool value[size])
{
    return setArray(dictionary, key, size, value, 'b');
}

// Returns the boolean array associated to the corresponding key, otherwise returns NULL
Bool *getBoolArray(const Dictionary *dictionary, const char *key, int *sizeResult)
{
    if (!dictionary)
        return NULL;

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if(!strcmp(aux->key, key))
        {
            if(aux->type == 'a' && ((Array *) aux->value)->type == 'b')
            {
                *sizeResult = ((Array *) aux->value)->size;
                return copyBoolArray(*sizeResult, ((Array *) aux->value)->elements);
            }
            return NULL;
        }
    }

    return NULL;
}

// Sets an array of strings for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setStringArray(Dictionary *dictionary, const char *key, int size, char *value[size])
{
    return setArray(dictionary, key, size, value, 's');
}

// Returns the array of strings associated to the corresponding key, otherwise returns NULL
char **getStringArray(const Dictionary *dictionary, const char *key, int *sizeResult)
{
    if (!dictionary)
        return NULL;

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if(!strcmp(aux->key, key))
        {
            if(aux->type == 'a' && ((Array *) aux->value)->type == 's')
            {
                *sizeResult = ((Array *) aux->value)->size;
                return copyStringArray(*sizeResult, ((Array *) aux->value)->elements);
            }
            return NULL;
        }
    }

    return NULL;
}

// Sets a dictionary for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setDictionary(Dictionary *dictionary, const char *key, Dictionary *value)
{
    return setValue(dictionary, key, value, 'd');
}

// Returns the dictionary associated to the corresponding key, otherwise returns NULL
Dictionary *getDictionary(const Dictionary *dictionary, const char *key)
{
    if (!dictionary)
        return NULL;

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if(!strcmp(aux->key, key))
        {
            if(aux->type == 'd')
                return copyDictionary(aux->value); // Es necesario hacer una copia del diccionario

            return NULL;
        }
    }

    return NULL;
}

// Sets an array of dictionaries for the given key, if the key does not exists it creates it else it overrides the previous value.
// Returns 1 if it was able to do it otherwise returns 0
int setDictionaryArray(Dictionary *dictionary, const char *key, int size, Dictionary *value[size])
{
    return setArray(dictionary, key, size, value, 'd');
}

// Returns the array of dictionaries associated to the corresponding key, otherwise returns NULL
Dictionary **getDictionaryArray(const Dictionary *dictionary, const char *key, int *sizeResult)
{
    if (!dictionary)
        return NULL;

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next)
    {
        if(!strcmp(aux->key, key))
        {
            if(aux->type == 'a' && ((Array *) aux->value)->type == 'd')
            {
                *sizeResult = ((Array *) aux->value)->size;
                return copyDictionaryArray(*sizeResult, ((Array *) aux->value)->elements);
            }
            return NULL;
        }
    }

    return NULL;
}

// Concatena al final de s1 el contenido de s2, asignando más memoria a s1
// Al llamar a esta función s1 siempre contendrá una dirección reservada con memoria dinámica
char *concat(char *s1, char *s2)
{
    int len = strlen(s1) + strlen(s2) + 1; // Calcula el nuevo tamaño

    if ((s1 = (char *) realloc(s1, sizeof(char) * len)) == NULL) // Asigna el espacio suficiente
        errorMessage();

    strcat(s1, s2); // Concatena s2 al final de s1
    return s1;
}

// Returns the json representation string for the given dictionary. If it can't do it returns NULL
char *jsonFromDictionary(const Dictionary *dictionary)
{
    if (!dictionary)
        return NULL;

    char *json;
    if ((json = (char *) malloc(sizeof(char))) == NULL) // Se reserva el espacio con malloc para así usar la función concat
        errorMessage();
    *json = '\0';

    json = concat(json, "{"); // El json empieza en '{'

    Element *aux;
    for(aux = dictionary->first; aux; aux = aux->next) // Para cada elemento del diccionario
    {
        json = concat(json, "\""); // Empieza en comillas
        json = concat(json, aux->key); // Luego se copia la clave
        json = concat(json, "\":"); // Seguidamente comillas
        switch (aux->type) // Se copia el valor en string dependiendo del tipo
        {
            char num[30];
            char *dict;

            case 'n':
                sprintf(num, "%.3f", *((double *) aux->value));
                json = concat(json, num);
                break;
            case 'b':
                json = concat(json, *((Bool *) aux->value) == true ? "true" : "false");
                break;
            case 's':
                json = concat(json, "\"");
                json = concat(json, (char *) aux->value);
                json = concat(json, "\"");
                break;
            case 'd':
                dict = jsonFromDictionary((Dictionary *) aux->value);
                json = concat(json, dict);
                free(dict);
                break;
            case 'a':
                json = concat(json, "[");
                switch(((Array *) aux->value)->type)
                {
                    int i;

                    case 'n':
                        for(i = 0; i < ((Array *) aux->value)->size; i++)
                        {
                            sprintf(num, "%.3f", ((double *) ((Array *) aux->value)->elements)[i]);
                            json = concat(json, num);
                            if (i != ((Array *) aux->value)->size - 1) // Si no es el último elemento
                                json = concat(json, ",");
                        }
                        break;
                    case 'b':
                        for(i = 0; i < ((Array *) aux->value)->size; i++)
                        {
                            json = concat(json, ((Bool *) ((Array *) aux->value)->elements)[i] == true ? "true" : "false");
                            if (i != ((Array *) aux->value)->size - 1) // Si no es el último elemento
                                json = concat(json, ",");
                        }
                        break;
                    case 's':
                        for(i = 0; i < ((Array *) aux->value)->size; i++)
                        {
                            // Comillas antes y después del string
                            json = concat(json, "\"");
                            json = concat(json, ((char **) ((Array *) aux->value)->elements)[i]);
                            json = concat(json, "\"");
                            if (i != ((Array *) aux->value)->size - 1) // Si no es el último elemento
                                json = concat(json, ",");
                        }
                        break;
                    case 'd':
                        for(i = 0; i < ((Array *) aux->value)->size; i++)
                        {
                            dict = jsonFromDictionary(((Dictionary **) ((Array *) aux->value)->elements)[i]);
                            json = concat(json, dict);
                            free(dict);
                            if (i != ((Array *) aux->value)->size - 1) // Si no es el último elemento
                                json = concat(json, ",");
                        }
                        break;
                }
                json = concat(json, "]");
                break;
        }
        if (aux->next) // Si no es el último elemento del diccionario
            json = concat(json, ",");
    }

    json = concat(json, "}"); // Termina en '}'

    return json;
}

// Retorna 1 si str corresponde a un número válido, de lo contrario retorna 0
int isNumber(char *str)
{
    if (*str == '\0')
        return 0;

    int len = strlen(str);

    int i;
    for(i = 0; i < len; i++)
        if ((str[i] < '0' || str[i] > '9') && str[i] != '.' && str[i] != '-') // Verifica que solo haya numeros o puntos
            return 0;

    // Primera ocurrencia de '-' y '.'
    char *minus = strchr(str, '-'), *dot = strchr(str, '.');

    if (minus && dot) // Si hay tanto menos como punto
    {
        // Verifica que solo haya un menos y que esté al inicio,
        // que haya un solo punto y no esté inmediatamente después del menos ni al final
        if (minus == strrchr(str, '-') && minus == str && dot == strrchr(str, '.') && dot > minus+1 && dot != str+len-1)
            return 1;
        return 0;
    }
    if (minus) // Si hay menos pero no punto
    {
        // Verifica que haya uno solo y no esté en el inicio
        if (minus == strrchr(str, '-') && minus == str)
            return 1;
        return 0;
    }
    if (dot) // Si hay punto pero no menos
    {
        // Verifica que solo haya uno y no esté ni al inicio ni al final
        if(dot == strrchr(str, '.') && dot != str && dot != str+len-1)
            return 1;
        return 0;
    }

    return 1;
}

// Separa en subcadenas los elementos de un diccionario o un arreglo en formato json
char **split(char *str, int *size)
{
    int i, j, len = strlen(str),
        count1 = 0, // Se encarga de los ""
        count2 = 0, // Se encarga de los {}
        count3 = 0; // Se encarga de los []
    *size = 0;
    char **array = NULL;

    for(i = 0, j = 0; i <= len; i++)
    {
        if (str[i] == '"')
            count1 = !count1;
        else if (str[i] == '{')
            count2++;
        else if (str[i] == '}')
            count2--;
        else if (str[i] == '[')
            count3++;
        else if (str[i] == ']')
            count3--;
        else if ((str[i] == ',' && !count1 && !count2 && !count3) || str[i] == '\0')
        {
            str[i] = '\0';
            (*size)++;
            array = (char **) realloc(array, sizeof(char *) * (*size));
            array[*size - 1] = str + j;
            j = i + 1;
        }
    }
    return array;
}

// Retorna 1 si la clave ya existe en el diccionario, de lo contrario retorna 0
int keyExists(const char *key, const Dictionary *dictionary)
{
    if (!dictionary)
        return 0;

    Element *aux;

    for (aux = dictionary->first; aux; aux = aux->next)
        if (!strcmp(aux->key, key))
            return 1;

    return 0;
}

// Returns a new dictionary created from its json representation. If it can't parse the json returns NULL
Dictionary *dictionaryFromJson(const char *json)
{
    if (!json || *json != '{') // Verifica la existencia del string y que empience por '{'
        return NULL;

    int len = strlen(json);

    if (json[len - 1] != '}') // Vertifica que el json termine en '}'
        return NULL;

    char jsonCopy[len];

    strcpy(jsonCopy, json+1); // Borra las llaves de inicio y fin
    jsonCopy[len - 2] = '\0';

    Dictionary *d = newDictionary();

    int size, i, j;
    char **jsonArray = split(jsonCopy, &size); // Separa el json

    if (jsonArray[0][0] == '\0') // Si era un diccionario vacio
    {
        free(jsonArray);
        return d;
    }

    char key[80];

    for(i = 0; i < size; i++)
    {
        char *s = jsonArray[i]; // Para facilitar la lectura

        if (*s++ != '"') // Verifica que la clave empiece en comillas
            break;

        for(j = 0; *s != '"' && *s != '\0'; j++) // Copia la clave
            key[j] = *s++;
        key[j] = '\0';

        if(key[0] == '\0') // Verifica que la clave no sea un string vacío
            break;

        if (*s++ == '\0') // Verifica que la clave haya terminado en comillas
            break;

        if (*s++ != ':') // Verifica que haya ':' luego de la clave
            break;

        if (keyExists(key, d)) // Verifica que no haya claves repetidas
            break;

        // En este punto s guarda lo que corresponde al valor

        if (*s == '"') // Si es un string
        {
            int lastChar = strlen(s) - 1;
            if(s[lastChar] != '"')
                break;
            s[lastChar] = '\0';
            setString(d, key, s+1);
        }
        else if (!strcmp(s, "true")) // Si es true
            setBool(d, key, true);
        else if (!strcmp(s, "false")) // Si es false
            setBool(d, key, false);
        else if (isNumber(s)) // Si es un numero
            setNumber(d, key, atof(s));
        else if (*s == '{') // Si es un diccionario
        {
            Dictionary *dAux;
            if ((dAux = dictionaryFromJson(s))) // Si es un diccionario valido
            {
                setDictionary(d, key, dAux);
                freeDictionary(dAux);
            }
            else
                break;
        }
        else if (*s == '[') // Si es un arreglo
        {
            int arraySize, valid = 1, lastChar = strlen(s) - 1;
            if(s[lastChar] != ']')
                break;
            s[lastChar] = '\0';

            char **arrayElements = split(s+1, &arraySize);

            if ((arrayElements[0][0] >= '0' && arrayElements[0][0] <= '9') || arrayElements[0][0] == '-') // Si es un arreglo de números
            {
                double array[arraySize];

                for(j = 0; j < arraySize; j++)
                {
                    if (!isNumber(arrayElements[j]))
                    {
                        valid = 0;
                        break;
                    }

                    array[j] = atof(arrayElements[j]);
                }

                free(arrayElements);

                if (!valid)
                    break;

                setNumberArray(d, key, arraySize, array);
            }
            else if (arrayElements[0][0] == '"') // Si es un arreglo de strings
            {
                char *array[arraySize];

                for(j = 0; j < arraySize; j++)
                {
                    lastChar = strlen(arrayElements[j]) - 1;
                    if(arrayElements[j][0] != '"' || arrayElements[j][lastChar] != '"')
                    {
                        valid = 0;
                        break;
                    }

                    arrayElements[j][lastChar] = '\0';
                    // Guarda el string sin las comillas del inicio y del final
                    array[j] = arrayElements[j] + 1;
                }

                free(arrayElements);

                if (!valid)
                    break;

                setStringArray(d, key, arraySize, array);
            }
            else if (arrayElements[0][0] == 't' || arrayElements[0][0] == 'f') // Si es un arreglo booleano
            {
                Bool array[arraySize];

                for(j = 0; j < arraySize; j++)
                {
                    int isTrue =  !strcmp(arrayElements[j], "true"),
                        isFalse = !strcmp(arrayElements[j], "false");

                    if (!isTrue && !isFalse)
                    {
                        valid = 0;
                        break;
                    }

                    array[j] = isTrue ? true : false;
                }

                free(arrayElements);

                if (!valid)
                    break;

                setBoolArray(d, key, arraySize, array);
            }
            else if (arrayElements[0][0] == '{') // Si es un arreglo de diccionarios
            {
                Dictionary *array[arraySize];

                for(j = 0; j < arraySize; j++)
                {
                    if (!(array[j] = dictionaryFromJson(arrayElements[j]))) // Si no es un diccionario valido
                    {
                        valid = 0;
                        break;
                    }
                }

                free(arrayElements);

                if (!valid) // Si se encontró un elemento inválido, debe hacérsele free
                {           // a todos los diccionarios anteriores creados
                    int k;
                    for(k = 0; k < j; k++)
                        freeDictionary(array[k]);

                    break;
                }

                setDictionaryArray(d, key, arraySize, array);

                for(j = 0; j < arraySize; j++)
                    freeDictionary(array[j]);
            }
            else
                break;

        }
        else
            break;

        // Si se llega a este punto quiere decir que no hubo errores en el json y que ya se añadieron todos los elementos del diccionario
        if (i == size - 1)
        {
            free(jsonArray);
            return d;
        }
    }

    // Si se llega a este punto quiere decir que hubo algún error en el json
    free(jsonArray);
    freeDictionary(d);
    return NULL;
}
