#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_text_file(const char* filename) {
    if (filename == NULL) {
        printf("Error: filename cannot be NULL\n");
        return NULL;
    }

    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: failed to open file '%s'\n", filename);
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        printf("Error: failed to determine file size\n");
        fclose(file);
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size < 0) {
        printf("Error: invalid file size\n");
        fclose(file);
        return NULL;
    }

    if (file_size == 0) {
        printf("File '%s' is empty\n", filename);
        fclose(file);
        char* empty_buffer = malloc(1);
        if (empty_buffer != NULL) {
            empty_buffer[0] = '\0';
        }
        return empty_buffer;
    }

    rewind(file);

    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        printf("Error: failed to allocate memory for file\n");
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != (size_t)file_size) {
        printf("Error: read %zu out of %ld bytes\n", bytes_read, file_size);
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[bytes_read] = '\0';
    fclose(file);

    int has_content = 0;
    for (size_t i = 0; i < bytes_read; i++) {
        if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n' && buffer[i] != '\r') {
            has_content = 1;
            break;
        }
    }

    if (!has_content) {
        printf("Warning: file '%s' contains only whitespace characters\n", filename);
    }

    return buffer;
}

int is_latin_letter(char c) {
    const char* latin = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    return strchr(latin, c) != NULL;
}

char** split_to_words(char* text, int* word_count) {
    if (!text || !word_count) return NULL;

    *word_count = 0;
    if (text[0] == '\0') return NULL;

    char* temp = text;
    while (*temp) {
        if (is_latin_letter(*temp)) {
            (*word_count)++;
            while (*temp && (is_latin_letter(*temp) || *temp == '-' || *temp == '\'')) temp++;
        }
        else {
            temp++;
        }
    }

    if (*word_count == 0) return NULL;

    char** words = malloc(*word_count * sizeof(char*));
    if (words == NULL) {
        return NULL;
    }

    int i = 0;
    char* start = text;

    while (*text) {
        if (is_latin_letter(*text)) {
            start = text;
            while (*text && (is_latin_letter(*text) || *text == '-' || *text == '\'')) {
                text++;
            }

            int len = text - start;
            words[i] = malloc(len + 1);
            if (words[i] == NULL) {
                for (int j = 0; j < i; j++) free(words[j]);
                free(words);
                return NULL;
            }
            strncpy(words[i], start, len);
            words[i][len] = '\0';
            i++;
        }
        else {
            text++;
        }
    }

    return words;
}

int compare_by_length_then_alpha(const void* a, const void* b) {
    const char* word1 = *(const char**)a;
    const char* word2 = *(const char**)b;

    size_t len1 = strlen(word1);
    size_t len2 = strlen(word2);

    if (len1 < len2) return -1;
    if (len1 > len2) return 1;

    return strcmp(word1, word2);
}

void sort_words(char** words, int word_count) {
    if (words == NULL || word_count <= 1) return;
    qsort(words, word_count, sizeof(char*), compare_by_length_then_alpha);
}

char** get_sorted_words_array_from_file(const char* filename, int* word_count) {
    char* text = read_text_file(filename);
    if (text == NULL) {
        *word_count = 0;
        return NULL;
    }

    char** words = split_to_words(text, word_count);
    if (words == NULL) {
        free(text);
        *word_count = 0;
        return NULL;
    }

    sort_words(words, *word_count);

    free(text);

    return words;
}

void print_words_longer_than(char** words, int word_count, int min_length) {
    if (words == NULL || word_count == 0) {
        printf("No words to display\n");
        return;
    }

    if (min_length < 1) {
        printf("Enter positive minimum word length");
        return;
    }

    printf("Words longer than %d characters:\n", min_length);
    int found = 0;

    for (int i = 0; i < word_count; i++) {
        if (strlen(words[i]) > min_length) {
            printf("%d: '%s' (length: %zu)\n", ++found, words[i], strlen(words[i]));
        }
    }

    if (found == 0) {
        printf("No words found longer than %d characters\n", min_length);
    }
    else {
        printf("Total: %d words\n", found);
    }
}

void print_words_with_exact_length(char** words, int word_count, int exact_length) {
    if (words == NULL || word_count == 0) {
        printf("No words to display\n");
        return;
    }

    if (exact_length < 1) {
        printf("Enter positive word length");
        return;
    }

    printf("Words with length %d (alphabetical order):\n", exact_length);
    int found = 0;

    for (int i = 0; i < word_count; i++) {
        if (strlen(words[i]) == exact_length) {
            printf("%d: '%s'\n", ++found, words[i]);
        }
    }

    if (found == 0) {
        printf("No words found with length %d\n", exact_length);
    }
    else {
        printf("Total: %d words\n", found);
    }
}

int original_main() {
    const char* filename = "text.txt";
    int word_count = 0;

    char** sorted_words = get_sorted_words_array_from_file(filename, &word_count);

    if (sorted_words != NULL) {
        printf("Enter minimum word length to display: ");
        int min_length;
        scanf("%d", &min_length);
        print_words_longer_than(sorted_words, word_count, min_length);

        printf("\n");

        printf("Enter exact word length to find: ");
        int exact_length;
        scanf("%d", &exact_length);
        print_words_with_exact_length(sorted_words, word_count, exact_length);

        for (int i = 0; i < word_count; i++) {
            free(sorted_words[i]);
        }
        free(sorted_words);
    }
    else {
        printf("Failed to process file\n");
    }

    return 0;
}