#include <stdio.h>

int main(int argc, char **argv)
{
	FILE *fp0, *fp1;
	int cnt;

	if (argc < 3) {
		puts("usage>makefont source.txt font.s");
		return 1;
	}
	fp0 = fopen(argv[1], "rb");
	fp1 = fopen(argv[2], "wb");
	if (fp0 == NULL) {
		puts("can't open input file.");
		return 2;
	}
	if (fp1 == NULL) {
		puts("can't open output file.");
		return 3;
	}

	cnt = 0;

	fputs(".code32\n", fp1);
	fputs(".data\n", fp1);
	fputs(".global _hankaku\n", fp1);
	fputs("_hankaku:\n", fp1);

	do {
		char s[12];
		int i;
		int md;
		if (fgets(s, 12, fp0) != NULL && 
			(s[0] == ' ' || s[0] == '*' || s[0] == '.')) {
			i  = (s[0] == '*') << 7;
			i |= (s[1] == '*') << 6;
			i |= (s[2] == '*') << 5;
			i |= (s[3] == '*') << 4;
			i |= (s[4] == '*') << 3;
			i |= (s[5] == '*') << 2;
			i |= (s[6] == '*') << 1;
			i |= (s[7] == '*')     ;
			/* fputc(i, fp1); */
			md = cnt % 8;
			switch (md) {
			case 0:
				fprintf(fp1, ".byte 0x%02x, ",  i);
				break;
			case 7:
				fprintf(fp1, "0x%02x\n", i);
				break;
			default:
				fprintf(fp1, "0x%02x, ", i);
			}
			cnt++;
		}
	} while (!feof(fp0));
	fclose(fp0);
	fclose(fp1);
	return 0;
}
