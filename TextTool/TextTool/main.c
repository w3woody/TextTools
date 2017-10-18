/*	main.c
 *
 *		Entry point for the text tool compiler, which takes as input a .txt
 *	file and generates a .c file with a single string declaration. I created
 *	this tool because I was tired of hacking up text files to embed in command
 *	line tools.
 *
 *		This tool is designed to be incorporated into Xcode.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static const char *GHelp =
	"texttool\n"                                                              \
	"\n"                                                                      \
	"    A simple tool for embedding text files into a program by translating the\n" \
	"text into C. This takes as its input a text file, and generates as output\n" \
	"a C file which contains a single string declaration.\n"                  \
	"\n"                                                                      \
	"Usage: texttool [-h] [-o filename] [-s string] inputfile\n"              \
	"\n"                                                                      \
	"-h  Prints this help file. This help file will also be printed when any illegal\n" \
	"    parameters are provided.\n"                                          \
	"\n"                                                                      \
	"-o  Output file name. By default this is the input file name with the extension\n" \
	"    replaced with .c and .h\n"                                           \
	"\n"                                                                      \
	"-s  The name of the string constant crated. If not provided, the name of the\n" \
	"    string constant will be the same as the input file name without the\n" \
	"    extension.\n"                                                        \
	"\n"                                                                      \
	"This program takes an input file MyText.txt and generate two files, MyText.c\n" \
	"and MyText.h with a string constant MyText containing the text of the file.\n" \
	"\n"                                                                      \
	"Copyright (C) 2017 by William Woody, all rights reserved.\n";


/************************************************************************/
/*																		*/
/*	Parser Definitions													*/
/*																		*/
/************************************************************************/

static char GOutputFile[FILENAME_MAX];
static char GOutputFileName[FILENAME_MAX];
static char GInputFile[FILENAME_MAX];
static char GStringName[FILENAME_MAX];

/*	PrintHelp
 *
 *		Print a help message
 */

static void PrintHelp()
{
	printf("%s\n",GHelp);
	exit(0);
}

/*	PrintError
 *
 *		print the command line invocation, an error message, and help
 */

static void PrintError(int argc, const char *argv[])
{
	printf("Command line arguments contains an error:\n");
	for (int i = 0; i < argc; ++i) {
		printf("%s ",argv[i]);
	}
	printf("\n\n");
	PrintHelp();
	exit(1);
}

/*	ParseArgs
 *
 *		Parse the input arguments, pulling the parameters for invoking this
 *	app, and storing away the contents
 *
 *		The parameters we accept are:
 *
 *		-h			Help
 *		-c class	Output class name (default is input file name)
 *		-o file		Output file name (default is input file name)
 *		input		Input file name
 */

static void ParseArgs(int argc, const char *argv[])
{
	const char *ptr;

	GOutputFile[0] = 0;
	GInputFile[0] = 0;
	GStringName[0] = 0;

	for (int i = 1; i < argc; ) {
		ptr = argv[i++];
		if (*ptr == '-') {
			if (!strcmp(ptr,"-h")) {
				PrintHelp();
			} else if (!strcmp(ptr,"-s")) {
				if (i >= argc) {
					PrintError(argc,argv);
				}
				if (GStringName[0]) {
					PrintError(argc,argv);
				}
				strncpy(GStringName,argv[i++],sizeof(GStringName)-1);
			} else if (!strcmp(ptr,"-o")) {
				if (i >= argc) {
					PrintError(argc,argv);
				}
				if (GOutputFile[0]) {
					PrintError(argc,argv);
				}
				strncpy(GOutputFile,argv[i++],sizeof(GOutputFile)-1);
			} else {
				PrintError(argc,argv);
			}
		} else {
			if (GInputFile[0]) {
				PrintError(argc,argv);
			}
			strncpy(GInputFile,ptr,sizeof(GInputFile)-1);
		}
	}

	if (GInputFile[0] == 0) {
		printf("Missing input file\n\n");
		PrintHelp();
	}
}

/************************************************************************/
/*																		*/
/*	Generator Files														*/
/*																		*/
/************************************************************************/

/*	WriteHeaderFile
 *
 *		Write the header file. Our header file declares one string.
 */

static int WriteHeaderFile()
{
	char scratch[FILENAME_MAX];

	strncpy(scratch,GOutputFile,sizeof(scratch)-1);
	strncat(scratch,".h",sizeof(scratch) - strlen(scratch) - 1);
	FILE *out = fopen(scratch,"w");
	if (out == NULL) {
		printf("Unable to write to file %s\n\n",scratch);
		return 0;
	}

	/*
	 *	Simple declaration
	 */

	fprintf(out,"/*  %s.h\n",GOutputFileName);
	fprintf(out," *\n");
	fprintf(out," *      Automatically generated header\n");
	fprintf(out," */\n");
	fprintf(out,"\n");
	fprintf(out,"#ifndef %s_h\n",GOutputFileName);
	fprintf(out,"#define %s_h\n",GOutputFileName);
	fprintf(out,"\n");
	fprintf(out,"extern const char %s[];\n",GStringName);
	fprintf(out,"\n");
	fprintf(out,"#endif /* %s_h */\n",GOutputFileName);

	fclose(out);
	return 1;
}

/*	WriteCFile
 *
 *		Transscribe our string, escaping certain characters to fit in
 *	our string. We assume the input stream is a byte stream; that way if
 *	it is UTF-8, we encode bytes
 */

static int WriteCFile()
{
	char scratch[FILENAME_MAX];
	char buffer[512];				/* Maximum block size */

	strncpy(scratch,GOutputFile,sizeof(scratch)-1);
	strncat(scratch,".c",sizeof(scratch) - strlen(scratch) - 1);
	FILE *out = fopen(scratch,"w");
	if (out == NULL) {
		printf("Unable to write to file %s\n\n",scratch);
		return 0;
	}

	FILE *in = fopen(GInputFile,"r");
	if (in == NULL) {
		printf("Unable to read from file %s\n\n",GInputFile);
		fclose(out);
		return 0;
	}

	/*
	 *	Top of file
	 */

	fprintf(out,"/*  %s.c\n",GOutputFileName);
	fprintf(out," *\n");
	fprintf(out," *      Automatically generated source file\n");
	fprintf(out," */\n");
	fprintf(out,"\n");
	fprintf(out,"#include \"%s.h\"\n",GOutputFileName);
	fprintf(out,"\n");
	fprintf(out,"const char %s[] = ",GStringName);

	/*
	 *	Now run through the input and transscribe
	 */

	int hasText = 0;
	while (NULL != fgets(buffer, sizeof(buffer), in)) {
		hasText = 1;

		fprintf(out,"\n\t\"");

		for (char *ptr = buffer; *ptr; ++ptr) {
			if (*ptr < 32) {
				switch (*ptr) {
					case 0x07:
						fprintf(out,"\\a");
						break;
					case 0x08:
						fprintf(out,"\\b");
						break;
					case 0x0C:
						fprintf(out,"\\f");
						break;
					case 0x0A:
						fprintf(out,"\\n");
						break;
					case 0x0D:
						fprintf(out,"\\r");
						break;
					case 0x09:
						fprintf(out,"\\t");
						break;
					case 0x0B:
						fprintf(out,"\\v");
						break;
					default:
						fprintf(out,"\\%o",0xFF & *ptr);
						break;
				}
			} else if (*ptr > 127) {
				fprintf(out,"\\%o",0xFF & *ptr);
			} else {
				switch (*ptr) {
					case '\\':
					case '\"':
					case '\'':
					case '?':
						fprintf(out,"\\%c",*ptr);
						break;
					default:
						fprintf(out,"%c",*ptr);
						break;
				}
			}
		}

		fprintf(out,"\"");
	}

	if (!hasText) {
		/* Handle empty (0 byte) fringe case */
		fprintf(out,"\"\"");
	}
	fprintf(out,";\n");

	fclose(in);
	fclose(out);
	return 1;
}

/************************************************************************/
/*																		*/
/*	Main entry point													*/
/*																		*/
/************************************************************************/


/*	main
 *
 *		Kick off the parser
 */

int main(int argc, const char * argv[])
{
	char *x, *y, *w;
	size_t s;

	/*
	 *	Parse the arguments
	 */

	ParseArgs(argc,argv);

	/*
	 *	Now calculate the output file and string names from the input file,
	 *	if one is not provided.
	 */

	if (GOutputFile[0] == 0) {
		/*
		 *	Trim '.'
		 */

		strncpy(GOutputFile,GInputFile,sizeof(GOutputFile)-1);
		y = NULL;
		for (x = GOutputFile; *x; ++x) {
			if (*x == '/') y = NULL;
			if (*x == '.') y = x;
		}

		if (y) *y = 0;		/* trim final .extension from file name */
	}

	/*
	 *	Find the output file name. This is just the name of the file, and
	 *	is used when constructing the #include
	 */

	y = NULL;
	w = GOutputFile;

	for (x = GInputFile; *x; ++x) {
		if (*x == '/') {
			w = x+1;
			y = NULL;
		}
		if (*x == '.') y = x;
	}

	if (y == NULL) {
		strncpy(GOutputFileName,w,sizeof(GOutputFileName)-1);
	} else {
		s = y - w;
		if (s > sizeof(GOutputFileName)-1) s = sizeof(GOutputFileName)-1;
		memmove(GOutputFileName, w, s);
		GOutputFileName[s] = 0;
	}

	/*
	 *	Calculate the string constant name
	 */

	if (GStringName[0] == 0) {
		strncpy(GStringName,GOutputFileName,sizeof(GStringName)-1);
	}

	/*
	 *	Generate the output .h file.
	 */

	if (!WriteHeaderFile()) {
		PrintError(argc, argv);
		return 1;
	}
	if (!WriteCFile()) {
		PrintError(argc, argv);
		return 1;
	}

	/*
	 *	Done.
	 */

	return 0;
}

