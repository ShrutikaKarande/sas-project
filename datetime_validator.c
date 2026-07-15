#include <stdio.h>
#include <string.h>
#include <stdalign.h>
#include <stdlib.h>
#include <ctype.h>


typedef struct HashNode {
    char *value;
    struct HashNode *next;
} HashNode;
 
typedef struct {
    HashNode **buckets;
    size_t bucket_count;
    size_t entry_count;
} HashSet;

static int get_two_digit(const char *str);
static int is_digit_char(char c);
static int hashset_init(HashSet *set, size_t bucket_count);
static int hashset_contains(HashSet *set, const char *value);   
static int hashset_insert(HashSet *set, const char *value);
static int hashset_resize(HashSet *set, size_t new_bucket_count);
static int hashset_delete(HashSet *set, const char *value);


static int get_two_digit(const char *str)
{
    return ((str[0] - '0') * 10) + (str[1] - '0');
}
static int is_digit_char(char c)
{
    return (c >= '0' && c <= '9');
}

static int hashset_init(HashSet *set, size_t bucket_count)
{
    set->bucket_count = bucket_count;
    set->entry_count = 0;
    set->buckets = (HashNode **)calloc(bucket_count, sizeof(HashNode *));
    if (set->buckets == NULL)
        return -1; // Memory allocation failed
    return 0; // Success
}
static int hashset_contains(HashSet *set, const char *value)
{
    size_t hash = 5381;
    for (const char *ptr = value; *ptr != '\0'; ptr++)
        hash = ((hash << 5) + hash) + (unsigned char)(*ptr); // hash * 33 + c

    size_t index = hash % set->bucket_count;

    HashNode *current = set->buckets[index];
    while (current != NULL) {
        if (strcmp(current->value, value) == 0)
            return 1; // Found
        current = current->next;
    }
    return 0; // Not found
}
static int hashset_resize(HashSet *set, size_t new_bucket_count)
{
    HashNode **new_buckets = (HashNode **)calloc(new_bucket_count, sizeof(HashNode *));
    if (new_buckets == NULL)
        return -1; // Memory allocation failed

    for (size_t i = 0; i < set->bucket_count; i++)
    {
        HashNode *current = set->buckets[i];
        while (current != NULL)
        {
            HashNode *next_node = current->next;

            size_t hash = 5381;
            for (const char *ptr = current->value; *ptr != '\0'; ptr++)
                hash = ((hash << 5) + hash) + (unsigned char)(*ptr); // hash * 33 + c

            size_t new_index = hash % new_bucket_count;
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;

            current = next_node;
        }
    }

    free(set->buckets);
    set->buckets = new_buckets;
    set->bucket_count = new_bucket_count;
    return 0; // Success
}
static int hashset_insert(HashSet *set, const char *value)
{
    if (set->entry_count + 1 > (set->bucket_count * 3) / 4)
    {
        if (hashset_resize(set, set->bucket_count * 2) != 0)
        {
            printf("Error resizing hash set\n");
            return -1; // Resize failed
        }
            
    }

    if (hashset_contains(set, value))
        return 0; // Already exists

    size_t hash = 5381;
    for (const char *ptr = value; *ptr != '\0'; ptr++)
        hash = ((hash << 5) + hash) + (unsigned char)(*ptr); // hash * 33 + c

    size_t index = hash % set->bucket_count;

    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (new_node == NULL)
        return -1; // Memory allocation failed

    new_node->value = strdup(value);
    new_node->next = set->buckets[index];
    set->buckets[index] = new_node;
    set->entry_count++;
    return 1; // Successfully inserted
}

static int hashset_delete(HashSet *set, const char *value)
{
    for (size_t i = 0; i < set->bucket_count; i++)
    {
        HashNode *current = set->buckets[i];
        HashNode *prev = NULL;

        while (current != NULL)
        {
            if (value == NULL || strcmp(current->value, value) == 0)
            {
                if (prev == NULL)
                    set->buckets[i] = current->next;
                else
                    prev->next = current->next;

                free(current->value);
                free(current);
                set->entry_count--;
                if (value != NULL)
                    return 1; // Successfully deleted specific value
            }
            else
            {
                prev = current;
            }
            current = current->next;
        }
    }
    return 0; // Value not found or all nodes deleted
}

int is_valid_datetime(const char *line, size_t len)
{
    
    /*YYYY-MM-DDThh:mm:ssZ or YYYY-MM-DDThh:mm:ss+05:30 */
    if(len != 20 && len != 25) // Minimum length for "YYYY-MM-DDThh:mm:ssZ" is 20, maximum for "YYYY-MM-DDThh:mm:ss+05:30" is 25
        return 0;

    /* --- YYYY (positions 0-3) --- */
    if (!is_digit_char(line[0]) || !is_digit_char(line[1]) ||
        !is_digit_char(line[2]) || !is_digit_char(line[3]))
        return 0;
    
    if (line[4] != '-')
        return 0;

    /* --- MM (positions 5-6) --- */
    if (!is_digit_char(line[5]) || !is_digit_char(line[6]))
        return 0;

    int mm = get_two_digit(&line[5]);
    if (mm < 1 || mm > 12)
        return 0;   

    if(line[7] != '-')
        return 0;
    
    /* --- DD (positions 8-9) --- */
    if (!is_digit_char(line[8]) || !is_digit_char(line[9]))
        return 0;
    
    int dd = get_two_digit(&line[8]);
    if (dd < 1 || dd > 31)  
        return 0;
    
    if(line[10] != 'T')
        return 0;


    /* --- hh (positions 11-12) --- */
    if (!is_digit_char(line[11]) || !is_digit_char(line[12]))
        return 0;
    
    int hh = get_two_digit(&line[11]);
    if(hh>23) return 0;
    
    if(line[13] != ':')
        return 0;

    /* --- mm (positions 14-15) --- */
    if (!is_digit_char(line[14]) || !is_digit_char(line[15]))
        return 0;
    
    int mi = get_two_digit(&line[14]);
    if(mi>59) return 0;

    if (line[16] != ':')
        return 0;

    /* --- ss (positions 17-18) --- */
    if (!is_digit_char(line[17]) || !is_digit_char(line[18]))
        return 0;

    int ss = get_two_digit(&line[17]);
    if (ss > 59)
        return 0;
    
    /* --- TZD (position 19 onward) --- */
    if (line[19] == 'Z') {
        /* Must be exactly 20 characters total - nothing may follow Z. */
        return len == 20;
    }
    
    else if (line[19] == '+' || line[19] == '-') {
        /* Must be exactly 25 characters total - nothing may follow the timezone. */
        if (len != 25)
            return 0;

        /* --- TZD hh:mm (positions 20-24) --- */
        if (!is_digit_char(line[20]) || !is_digit_char(line[21]))
            return 0;
        
        int tz_hh = get_two_digit(&line[20]);
        if (tz_hh > 23)
            return 0;

        if (line[22] != ':')
            return 0;

        if (!is_digit_char(line[23]) || !is_digit_char(line[24]))
            return 0;
        
        int tz_mm = get_two_digit(&line[23]);
        if (tz_mm > 59)
            return 0;

        return 1; // Valid timezone offset
    }

    return 1; // Return 1 if valid, 0 if not valid
}
int main(int argc, char **argv)
{
    const char *input_path = (argc > 1) ? argv[1] : "input.txt";
    const char *output_path = (argc > 2) ? argv[2] : "output.txt";
    printf("Input file: %s\n", input_path);
    printf("Output file: %s\n", output_path);
    int valid_lines = 0, total_lines = 0;
    FILE *fp = fopen(input_path, "r");
    if(fp == NULL)
    {
        printf("Error opening file\n");
        return 1;
    }
    FILE *out = fopen(output_path, "w");
    if(out == NULL)
    {
        printf("Error opening output file\n");
        fclose(fp);
        return 1;
    }

    HashSet seen_datetime;
    int ret =hashset_init(&seen_datetime, 1024);
    if (ret != 0) {
        printf("Error initializing hash set\n");
        fclose(fp);
        fclose(out);
        return 1;
    }

    while(!feof(fp))
    {
        char line[256];
        if(fgets(line, sizeof(line), fp) != NULL)
        {
            //printf("%s", line);
            line[strcspn(line, "\n")] = '\0';
            total_lines++;

            int line_len = strlen(line);
 
            if (line_len == 0)
                continue; /* blank lines are never valid */
    
            if (!is_valid_datetime(line, (size_t)line_len))
            {
                printf("Invalid line: %s\n", line);
                continue;
            }
    
            //valid_lines++;
            if(hashset_insert(&seen_datetime, line)){
                valid_lines++;
                fputs(line, out);
                fputc('\n', out);
            }
    
            //printf("%s", line);
        }
        //printf("Valid lines: %d\n", valid_lines);
    }

    hashset_delete(&seen_datetime, NULL); // Free the hash set memory
    fclose(fp);
    fclose(out);
    return 0;
}