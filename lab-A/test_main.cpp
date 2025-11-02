#define _CRT_SECURE_NO_WARNINGS
#include <gtest/gtest.h>

extern "C" {
    int is_latin_letter(char c);
    char* read_text_file(const char* filename);
    char** split_to_words(char* text, int* word_count);
    int compare_by_length_then_alpha(const void* a, const void* b);
    void print_words_longer_than(char** words, int word_count, int min_length);
    void print_words_with_exact_length(char** words, int word_count, int exact_length);
}

TEST(LatinLetter, ValidValue) {
    EXPECT_TRUE(is_latin_letter('a'));
    EXPECT_TRUE(is_latin_letter('Z'));
    EXPECT_FALSE(is_latin_letter('1'));
    EXPECT_FALSE(is_latin_letter(' '));
}

//read_text_file tests

TEST(ReadTextFileTest, NullFilename) {
    char* result = read_text_file(NULL);
    EXPECT_EQ(result, nullptr);
}

TEST(ReadTextFileTest, NonExistentFile) {
    char* result = read_text_file("nonexistent.txt");
    EXPECT_EQ(result, nullptr);
}

TEST(ReadTextFileTest, EmptyFile) {
    FILE* temp = fopen("empty_test.txt", "w");
    fclose(temp);

    char* result = read_text_file("empty_test.txt");
    ASSERT_NE(result, nullptr);
    EXPECT_STREQ(result, "");
    free(result);

    remove("empty_test.txt");
}

TEST(ReadTextFileTest, FileWithOnlyWhitespace) {
    FILE* temp = fopen("whitespace_test.txt", "w");
    fprintf(temp, "   \n\t\r   ");
    fclose(temp);

    char* result = read_text_file("whitespace_test.txt");
    ASSERT_NE(result, nullptr);
    EXPECT_STRNE(result, nullptr);
    free(result);

    remove("whitespace_test.txt");
}

//split_to_words tests

TEST(SplitToWordsTest, NullInput) {
    int word_count = 0;
    char** result = split_to_words(nullptr, &word_count);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(word_count, 0);
}

TEST(SplitToWordsTest, NullWordCount) {
    char text[] = "hello world";
    char** result = split_to_words(text, nullptr);
    EXPECT_EQ(result, nullptr);
}

TEST(SplitToWordsTest, EmptyString) {
    int word_count = 0;
    char text[] = "";
    char** result = split_to_words(text, &word_count);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(word_count, 0);
}

TEST(SplitToWordsTest, OnlyWhitespace) {
    int word_count = 0;
    char text[] = "   \t\n\r  ";
    char** result = split_to_words(text, &word_count);
    EXPECT_EQ(result, nullptr);
    EXPECT_EQ(word_count, 0);
}

TEST(SplitToWordsTest, SingleWord) {
    int word_count = 0;
    char text[] = "hello";
    printf("Before split_to_words: text = '%s'\n", text);

    char** result = split_to_words(text, &word_count);

    printf("After split_to_words: word_count = %d\n", word_count);
    if (result != nullptr) {
        for (int i = 0; i < word_count; i++) {
            printf("word[%d] = '%s'\n", i, result[i]);
        }
    }

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(word_count, 1);
    if (result != nullptr && word_count > 0) {
        EXPECT_STREQ(result[0], "hello");
    }

    if (result != nullptr) {
        for (int i = 0; i < word_count; i++) free(result[i]);
        free(result);
    }
}

TEST(SplitToWordsTest, MultipleWords) {
    int word_count = 0;
    char text[] = "hello world test";
    char** result = split_to_words(text, &word_count);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(word_count, 3);
    EXPECT_STREQ(result[0], "hello");
    EXPECT_STREQ(result[1], "world");
    EXPECT_STREQ(result[2], "test");

    for (int i = 0; i < word_count; i++) free(result[i]);
    free(result);
}

TEST(SplitToWordsTest, WordsWithHyphensAndApostrophes) {
    int word_count = 0;
    char text[] = "it's state-of-the-art test-word";
    char** result = split_to_words(text, &word_count);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(word_count, 3);
    EXPECT_STREQ(result[0], "it's");
    EXPECT_STREQ(result[1], "state-of-the-art");
    EXPECT_STREQ(result[2], "test-word");

    for (int i = 0; i < word_count; i++) free(result[i]);
    free(result);
}

TEST(SplitToWordsTest, MixedLettersAndNumbers) {
    int word_count = 0;
    char text[] = "hello123 world456 test789";
    char** result = split_to_words(text, &word_count);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(word_count, 3);
    EXPECT_STREQ(result[0], "hello");
    EXPECT_STREQ(result[1], "world");
    EXPECT_STREQ(result[2], "test");

    for (int i = 0; i < word_count; i++) free(result[i]);
    free(result);
}

TEST(SplitToWordsTest, VeryLongWords) {
    int word_count = 0;
    char long_text[] = "supercalifragilisticexpialidocious antidisestablishmentarianism";
    char** result = split_to_words(long_text, &word_count);

    ASSERT_NE(result, nullptr);
    EXPECT_EQ(word_count, 2);

    for (int i = 0; i < word_count; i++) free(result[i]);
    free(result);
}

//compare_by_length_then_alpha tests

TEST(CompareTest, DifferentLengths) {
    const char* word1 = "cat";
    const char* word2 = "elephant";
    const char* words[] = { word1, word2 };

    int result = compare_by_length_then_alpha(&words[0], &words[1]);
    EXPECT_LT(result, 0);

    result = compare_by_length_then_alpha(&words[1], &words[0]);
    EXPECT_GT(result, 0);
}

TEST(CompareTest, SameLengthDifferentAlpha) {
    const char* word1 = "apple";
    const char* word2 = "banana";
    const char* words[] = { word1, word2 };

    int result = compare_by_length_then_alpha(&words[0], &words[1]);
    EXPECT_LT(result, 0);
}

TEST(CompareTest, SameWord) {
    const char* word1 = "test";
    const char* word2 = "test";
    const char* words[] = { word1, word2 };

    int result = compare_by_length_then_alpha(&words[0], &words[1]);
    EXPECT_EQ(result, 0);
}

//print_words_longer_than tests

TEST(PrintWordsTest, NullInputForLongerThan) {
    testing::internal::CaptureStdout();
    print_words_longer_than(nullptr, 5, 3);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("No words to display") != std::string::npos);
}

TEST(PrintWordsTest, InvalidMinLength) {
    const char* words[] = { "test", "hello" };
    testing::internal::CaptureStdout();
    print_words_longer_than((char**)words, 2, 0);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Enter positive minimum word length") != std::string::npos);
}

TEST(PrintWordsTest, InvalidExactLength) {
    const char* words[] = { "test", "hello" };
    testing::internal::CaptureStdout();
    print_words_with_exact_length((char**)words, 2, -1);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(output.find("Enter positive word length") != std::string::npos);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}