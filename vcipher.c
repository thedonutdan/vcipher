#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define ISALPHA(x) ((x >= 'A' && x <= 'Z') || (x >= 'a' && x <= 'z'))
#define MAX 1000

/*
Vigenere Cipher Class Project
by Daniel Andrews
*/

FILE* fpin,* fpout,* fplog;

void verify_cmd_args(int argc, char *argv[], char* keyword, char* mode);
void process_file(char* str);
void create_key(char* key, char* str, char* word);
void encode_decode(char* str, char* key, char* mode);
void output_message(char* str);
void frequency_analysis(char* str, int factors[20]);
void sort_factors(int freq[20], int factors[20]);
void keyword_analysis(char* str, char* keyword, int factors[20]);
void output_hist(char* str, char* keyword, int keylength, double hist[26]);
void shift_keyword(char* keyword, int keylength);
void print_message(char* str);

int main(int argc, char *argv[])
{
	char str[MAX], key[MAX], keyword[21], mode[10];
	int factors[20];

	verify_cmd_args(argc, argv, keyword, mode);
	
	if ((fpin = fopen(argv[2], "r")) == NULL) {
		printf("Unable to open %s for input.", argv[2]);
		exit(-1);
	}
	if ((fpout = fopen(argv[3], "w")) == NULL) {
		printf("Unable to open %s for output.", argv[3]);
		exit(-1);
	}

	process_file(str);
	
	if (!strcmp(mode, "encode") || !strcmp(mode, "decode")) {
		create_key(key, str, keyword);
		encode_decode(str, key, mode);
	}
	else if (!strcmp(mode, "decipher"))
	{
		if ((fplog = fopen("decipherlog.txt", "w")) == NULL) {
			printf("Unable to open decipherlog.txt for output.");
			exit(-1);
		}

		frequency_analysis(str, factors);
		keyword_analysis(str, keyword, factors);
		create_key(key, str, keyword);
		encode_decode(str, key, "decode");
		print_message(str);

		fclose(fplog);
	}
	output_message(str);

	fclose(fpin);
	fclose(fpout);

	return 0;
}

void verify_cmd_args(int argc, char* argv[], char* keyword, char* mode)
{
	if (argc > 1)
		strcpy(mode, argv[1]);
	if (argc < 4) {
		printf("Usage: vcipher.exe mode source dest keyword");
		exit(-1);
	}
	else if (strcmp(mode, "encode") && strcmp(mode, "decode") && strcmp(mode, "decipher")) {
		printf("Usage: vcipher.exe mode source dest keyword\n");
		printf("Modes: encode decode decipher");
		exit(-1);
	}
	else if (argc < 5) {
		if (!strcmp(mode, "encode") || (!strcmp(mode, "decode"))) {
			printf("Encoding and decoding functionalities require keyword.");
			exit(-1);
		}
	}
	else {
		if (strlen(argv[4]) < 5 && strlen(argv[4]) > 20) {
			printf("Please use keyword between 5 and 20 characters in length.");
			exit(-1);
		}
		else {
			strcpy(keyword, argv[4]);
			for (int i = 0; keyword[i]; i++) {
				if (!ISALPHA(keyword[i])) {
					printf("Please use alphabetic keyword.");
					exit(-1);
				}
			}
		}
	}

	return;
}

void process_file(char* str)
{
	int i = 0, j;
	char ch;

	//Check that file does not exceed 1000 character limit
	for (j = 1; fgetc(fpin) != EOF; j++);
	if (j > MAX) {
		printf("%d character limit exceeded. Please choose smaller file to process.", MAX);
		exit(-1);
	}
	rewind(fpin);

	while ((ch = fgetc(fpin)) != EOF) {
		if (ISALPHA(ch))
			str[i++] = toupper(ch);
	}
	str[i] = 0;

	return;
}

void create_key(char* key, char* str, char* word)
{
	int i = 0, j = 0;

	for (j = 0; word[j] != 0; j++) {
			word[j] = toupper(word[j]);
	}

	while (str[i] != 0){
		for (j = 0; (word[j] && str[i]); j++, i++) {
			key[i] = word[j];
		}
	}
	key[i] = 0;

	return;
}

void encode_decode(char* str, char* key, char* mode)
{
	if (!strcmp(mode, "encode")) {
		for (int i = 0; str[i]; i++) {
			str[i] = str[i] + (key[i] - 'A');
			if (str[i] > 'Z')
				str[i] = str[i] - 26;
		}
	}
	else if (!strcmp(mode, "decode")) {
		for (int i = 0; str[i]; i++) {
			str[i] = str[i] - (key[i] - 'A');
			if (str[i] < 'A')
				str[i] = str[i] + 26;
		}
	}

	return;
}

void output_message(char* str)
{
	for (int i = 0, j = 1; str[i]; j++) {
		if ((j % 6))
			fputc(str[i++], fpout);
		else if ((j % 60))
			fputc(' ', fpout);
		else
			fputc('\n', fpout);
	}

	return;
}

void print_message(char* str)
{
	putchar('\n');
	fputc('\n', fplog);
	for (int i = 0, j = 1; str[i]; j++) {
		if ((j % 6)) {
			putchar(str[i]);
			fputc(str[i++], fplog);
		}
		else if ((j % 60)) {
			putchar(' ');
			fputc(' ', fplog);
		}
		else {
			putchar('\n');
			fputc('\n', fplog);
		}
	}

	return;
}

void frequency_analysis(char* str, int factors[20])
{
	int i = 0, j = 0, k = 0;
	char temp1[4], temp2[4];
	char tgram[MAX][4];
	int dist[MAX], freqf[20];

	for (i = 0; i < MAX; i++)
		strcpy((char *)(tgram + i), "\0");
	for (i = 0; i < MAX; i++)
		dist[i] = 0;
	for (i = 0; i < 20; i++)
		factors[i] = i + 1;
	for (i = 0; i < 20; i++)
		freqf[i] = 0;
	
	for (i = 0; str[i + 1]; i++) {
		temp1[0] = str[i];
		temp1[1] = str[i + 1];
		temp1[2] = str[i + 2];
		temp1[3] = 0;
		for (j = i + 3; str[j]; j++) {
			temp2[0] = str[j];
			temp2[1] = str[j + 1];
			temp2[2] = str[j + 2];
			temp2[3] = 0;
			if (!strcmp(temp1, temp2)) {
				strcpy((char *)(tgram + k), temp1);
				dist[k++] = (j - i);
				continue;
			}
		}
	}

	--k;
	printf("---------------Factor Frequency Analysis--------------------\n");
	printf("%5s%5s|%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d\n", "Seq", "Dist",
			5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
	printf("----------|-------------------------------------------------\n");
	fprintf(fplog, "---------------Factor Frequency Analysis--------------------\n");
	fprintf(fplog, "%5s%5s|%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d\n", "Seq", "Dist",
		5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);
	fprintf(fplog, "----------|-------------------------------------------------\n");
	for (i = 0; i < k; i++) {
		printf("%5s%5d|", tgram + i, dist[i]);
		fprintf(fplog, "%5s%5d|", tgram + i, dist[i]);
		for (j = 5; j <= 20; j++) {
			if (!(dist[i] % j)) {
				printf("%3c", 'x');
				fprintf(fplog, "%3c", 'x');
			}
			else {
				printf("%3c", ' ');
				fprintf(fplog, "%3c", ' ');
			}
		}
		printf("%c", '\n');
		fprintf(fplog, "%c", '\n');

	}

	for (i = 0; i < k; i++) {
		for (j = 1; j <= 20; j++) {
			if (!(dist[i] % j))
				++freqf[j - 1];
		}
	}

	sort_factors(freqf, factors);
		
	return;
}

void sort_factors(int freqf[20], int factors[20])
{
	int i, j, temp1, temp2;

	for (i = 0; i < 20; i++) {
		temp1 = freqf[i];
		temp2 = factors[i];
		j = i - 1;
		while (j >= 0 && temp1 > freqf[j]) {
			freqf[j + 1] = freqf[j];
			factors[j + 1] = factors[j];
			--j;
		}
		freqf[j + 1] = temp1;
		factors[j + 1] = temp2;
	}
	
	return;
}

void keyword_analysis(char* str, char* keyword, int factors[20])
{
	int i, j, select = 0, keylength, options[5] = {0, 0, 0, 0, 0};
	double hist[26] = {0.0812, 0.0149, 0.0271, 0.0432, 0.1202, 0.023, 0.0203, 0.0592,
						0.0731, 0.001, 0.0069, 0.0398, 0.0261, 0.0695, 0.0768, 0.0182,
						0.0011, 0.0602, 0.0628, 0.1010, 0.0288, 0.0111, 0.0209, 0.0017,
						0.0211, 0.0007};
	char selects[10];
	
	for (i = 1, j = 0; i < 20 && j < 5; i++) {
		if (factors[i] > 4) {
			options[j] = factors[i];
			++j;
		}
	}
	
	printf("\nPossible keyword lengths:\n");
	fprintf(fplog, "\nPossible keyword lengths:\n");
	for (i = 0; i < 5; i++) {
		printf("%d - %d\n", i + 1, options[i]);
		fprintf(fplog, "%d - %d\n", i + 1, options[i]);
	}
	printf("Please select an option to continue(1-5): ");
	while (1) {
		if (fgets(selects, 10, stdin) != NULL) {
			select = atoi(selects);

			if (select >= 1 && select <= 5)
				break;
			else
				printf("Please select an option to continue(1-5): ");
		}
	}
	keylength = options[select - 1];

	for (i = 0; i < 26; i++) {
		hist[i] = (hist[i] * strlen(str)) / keylength;
		if (hist[i] < 1)
			hist[i] = 1;
		hist[i] = round(hist[i]);
	}

	for (i = 0; i < keylength; i++)
		keyword[i] = 'A';
	keyword[i] = 0;

	do {
		output_hist(str, keyword, keylength, hist);
		shift_keyword(keyword, keylength);
		printf("Continue keyword analysis?\n");
		printf("1 - Yes\n");
		printf("2 - No\n");
		printf("Enter selection(1-2): ");
		while (1) {
			if (fgets(selects, 10, stdin) != NULL) {
				select = atoi(selects);
				if (select >= 1 && select <= 2)
					break;
				else
					printf("Enter Selection(1-2): ");
			}
		}
	} while (select == 1);

	printf("\nSelect an option below to continue:\n");
	printf("1 - Decipher text with current keyword\n");
	printf("2 - Try new keyword length\n");
	printf("3 - Exit program\n");
	printf("Enter selection(1-3): ");
	while (1) {
		if (fgets(selects, 10, stdin) != NULL) {
			select = atoi(selects);
			if (select >= 1 && select <= 3)
				break;
			else
				printf("Enter Selection(1-3): ");
		}
	}
	
	if (select == 1)
		return;
	else if (select == 2)
		keyword_analysis(str, keyword, factors);
	else
		exit(1);

	return;
}

void output_hist(char* str, char* keyword, int keylength, double hist[26])
{
	int i, j, k, count;
	
	printf("\nShift each column until it aligns with the expected character frequency (freq).\n");
	printf("Positive to shift up, negative to shift down.\n");
	printf("\n%2s%5s%5s|","#", "Char", "Freq");
	fprintf(fplog, "\n%2s%5s%5s|", "#", "Char", "Freq");
	for (j = 0; j < keylength; j++) {
		printf("%5c", keyword[j]);
		fprintf(fplog, "%5c", keyword[j]);
	}
	printf("\n");
	fprintf(fplog, "\n");
	printf("------------|");
	fprintf(fplog, "------------|");
	for (j = 0; j < keylength; j++) {
		printf("%5s", "-----");
		fprintf(fplog, "%5s", "-----");
	}
	printf("\n");
	fprintf(fplog, "\n");

	for (i = 0; i < 26; i++) {
		printf("%2d%5c%5.0lf|", i, i + 65, hist[i]);
		fprintf(fplog, "%2d%5c%5.0lf|", i, i + 65, hist[i]);
		for (j = 0; j < keylength; j++) {
			for (k = j, count = 0; (k >= 0 && (unsigned)k < strlen(str) - 1); k += keylength) {
				char temp;
				if ((temp = str[k] - (keyword[j] - 'A')) < 'A')
					temp += 26;
				if (temp == i + 65)
					++count;
			}
			printf("%5d", count);
			fprintf(fplog, "%5d", count);
		}
		printf("\n");
		fprintf(fplog, "\n");
	}
	
	return;
}

void shift_keyword(char* keyword, int keylength)
{
	char selects[10];

	for (int i = 0; i < keylength; i++) {
		int shift;
		printf("Shift for column %d: ", i + 1);

		while (1) {
			if (fgets(selects, 10, stdin) != NULL) {
				shift = atoi(selects);
				if (shift >= -26 && shift <= 26) 
					break;
				else
					printf("Shift for column %d: ", i + 1);
			}
		}
		
		keyword[i] = keyword[i] + (char)shift;
		if (keyword[i] > 'Z')
			keyword[i] -= 26;
		if (keyword[i] < 'A')
			keyword[i] += 26;
		printf("%c\n", keyword[i]);
	}

	printf("Keyword: %s\n", keyword);
	fprintf(fplog, "Keyword: %s\n", keyword);

	return;
}