/** @file InputParser_C.h
 *  @brief A very handy string tokenizer for C this basically a swiss army knife kind code segment that can be used wherever we get strings
 *         and we want to seperate them. This code is used in many of my projects including and not limited to
 *         https://github.com/AmmarkoV/RoboVision
 *         https://github.com/AmmarkoV/FlashySlideshows
 *         https://github.com/AmmarkoV/RGBDAcquisition
 *         https://github.com/AmmarkoV/AmmarServer
 * Basic usage is the following
 *
 *  struct InputParserC * ipc=0;
 *  ipc = InputParser_Create(512,5);     //We want to separte lines of Max 512 different strings seperated using 5 delimiters
 *  InputParser_SetDelimeter(ipc,0,' '); //We want to seperate spaces
 *
 *  char word[512];
 *  int numberOfWords = InputParser_SeperateWords(ipc,"zero,one,2,three(four)",1);
 *  for (int i=0; i<numberOfWords; i++)
 *    {
 *       InputParser_GetWord(ipc,i,word,512);
 *    }
 *  InputParser_Destroy(ipc);
 *
 *  @author Ammar Qammaz (AmmarkoV)
 */

#ifndef _INPUTPARSER_C_H_
#define _INPUTPARSER_C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/** @brief Controls the use of scanf for getting back floats ,
           scanf without field width limits can crash with huge input data on libc versions older than 2.13-25.
           so this is better turned of and atof is used to do the float conversion*/
#define USE_SCANF 0

/**
 * @brief A struct that contains the token list
 */
struct tokens
{
  /* START MEANS THE FIRST CHARACTER TO READ..!
     LENGTH MEANS total characters to be read
     i.e.   a _ S _ A _ M _ P _ L _ E
            0   1   2   3   4   5   6
     this token starts from 0 and has 7 characters length*/
  unsigned int token_start;
  unsigned int length;
};

/**
 * @brief Guard Byte to make sure that there is no overflow
 */
struct guard_byte
{
  unsigned int checksum;
};

/**
 * @brief The structure that holds all of the Input Parsing Context
 */
struct InputParserC
{
    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    struct guard_byte guardbyte1;
    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    unsigned int str_length;
    unsigned char local_allocation;
    char * str; /* String to process */

    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    struct guard_byte guardbyte2;
    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    unsigned short cur_container_count;
    unsigned short max_container_count;
    char *container_start;
    char *container_end;

    unsigned short cur_delimeter_count;
    unsigned short max_delimeter_count;
    char *delimeters;

    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    struct guard_byte guardbyte3;
    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

    /* RESULT */
    unsigned int tokens_max;
    unsigned int tokens_count;
    struct tokens* tokenlist;

    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
    struct guard_byte guardbyte4;
    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
};


/**
 * @brief Get Back a String containing the version of the InputParser
 * @retval Pointer to a C String that contains the version of InputParser
 */
char * InputParserC_Version();

/**
 * @brief Clear any non-character characters from string
 * @ingroup InputParser
 * @param String Input/Output
 * @param Length of String
 * @retval 1 if success , 0 if not
 */
int InputParser_ClearNonCharacters(char * inpt , unsigned int length);

/**
 * @brief Trim Starting Characters preceeding string
 * @ingroup InputParser
 * @param String Input/Output
 * @param Length of String
 * @param Character to Trim
 * @retval 1 if success , 0 if not
 */
int InputParser_TrimCharactersStart(char * inpt , unsigned int length,char what2trim);


/**
 * @brief Trim Starting Characters after string
 * @ingroup InputParser
 * @param String Input/Output
 * @param Length of String
 * @param Character to Trim
 * @retval 1 if success , 0 if not
 */
int InputParser_TrimCharactersEnd(char * inpt , unsigned int length,char what2trim);


/**
 * @brief This call will remove all characters specified from the string
 * @ingroup InputParser
 * @param String Input/Output
 * @param Length of String
 * @param Character to Trim
 * @retval 1 if success , 0 if not
 */
int InputParser_TrimCharacters(char * inpt , unsigned int length,char what2trim);


/**
 * @brief This call will setup the default delimiters that are typically used , these are '\n' ',' '=' '(' and ')'
 *        these can be also read by InputParser_GetDelimeter or changed at any time using InputParser_SetDelimeter
 * @ingroup InputParser
 * @param InputParser Context ( needs to be created using InputParser_Create )
 */
void InputParser_DefaultDelimeters(struct InputParserC * ipc);


/**
 * @brief This call will change the value of a delimiter so that it will also act as a tokenization split character
 * @ingroup InputParser
 * @param InputParser Context ( needs to be created using InputParser_Create )
 * @param Number of delimiter we want to set ( has to be less than the max_delimiter_count value we gave in InputParser_Create )
 * @param New Value for the delimiter we want to set
 */
void InputParser_SetDelimeter(struct InputParserC * ipc,int num,char tmp);

/**
 * @brief Get Back the characters that are used as delimiters
 * @ingroup InputParser
 * @param InputParser Context ( needs to be created using InputParser_Create )
 * @param Number of delimiter we want to set ( has to be less than the max_delimiter_count value we gave in InputParser_Create )
 * @retval Character used as delimiter , or Null if incorrect delimter number requested
 */
char InputParser_GetDelimeter(struct InputParserC * ipc,int num);



/**
 * @brief Create an InputParser Structure and Context that can be used to tokenize strings
 * @ingroup InputParser
 * @param  The maximum number of strings/tokens to be returned as output from a SeperateWords command
 * @param  The maximum number of delimters used typically 5 for the defaults '\n' ',' '=' '(' and ')'
 * @retval InputParser Context , or Null if there is an error
 */
struct InputParserC * InputParser_Create(unsigned int max_string_count,unsigned int max_delimiter_count);


/**
 * @brief Destroy an InputParser Structure and Context and safely free all of the memory it might have allocated
 * @ingroup InputParser
 * @param InputParser Context ( needs to be created using InputParser_Create )
 */
void InputParser_Destroy(struct InputParserC * ipc);


/**
 * @brief Perform an internal integrity check
 * @ingroup InputParser
 * @param InputParser Context to be checked ( needs to be created using InputParser_Create )
 * @retval 1=Success,0=Error
 */
unsigned char InputParser_SelfCheck(struct InputParserC * ipc);




/**
 * @brief Retrieve the number of arguments seperated from the last InputParser_SeperateWords call
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @retval Number of arguments tokenized , 0=No arguments/Error
 */
unsigned int InputParser_GetNumberOfArguments(struct InputParserC * ipc);


/**
 * @brief Check if a word ( InputParser_GetWord ) is empty after issuing a InputParser_SeperateWords call
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to check
 * @retval 1=Success,0=Error
 */
unsigned int InputParser_IsEmptyWord(struct InputParserC * ipc,unsigned int num);



/**
 * @brief Checks if word with number num has allocated space in memory ( see InputParser_GetWord )
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to check
 * @retval 1=Success,0=Error
 */
unsigned char CheckWordNumOk(struct InputParserC * ipc,unsigned int num);


/**
 * @brief Get a specific character of a Word ( instead of the whole string that would be returned with InputParser_GetWord )
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to check
 * @param Character of word we want to get back
 * @retval Character we requested or null if there is an error
 */
char InputParser_GetWordChar(struct InputParserC * ipc,unsigned int num,unsigned int pos);

/**
 * @brief Compare two words without case sensitivity and return wether they match or not..!
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to check
 * @param String that we want to compare against
 * @param Length of String that we want to compare against
 * @retval 1=Match,0=Strings are different
 */
unsigned char InputParser_WordCompareNoCase(struct InputParserC * ipc,unsigned int num,const char * word,unsigned int wordsize);

/**
 * @brief Same as InputParser_WordCompareNoCase but supposes word is null terminated and auto-counts its length
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to check
 * @param String that we want to compare against ( that should be null terminated )
 * @retval 1=Match,0=Strings are different
 */
unsigned char InputParser_WordCompareNoCaseAuto(struct InputParserC * ipc,unsigned int num,const char * word);

/**
 * @brief Compare two words with case sensitivity and return wether they match or not..!
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to check
 * @param String that we want to compare against ( that should be null terminated )
 * @retval 1=Match,0=Strings are different
 */
unsigned char InputParser_WordCompare(struct InputParserC * ipc,unsigned int num,const char * word,unsigned int wordsize);

/**
 * @brief Same as InputParser_WordCompare but supposes word is null terminated and auto-counts its length
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to check
 * @param String that we want to compare against ( that should be null terminated )
 * @retval 1=Match,0=Strings are different
 */
unsigned char InputParser_WordCompareAuto(struct InputParserC * ipc,unsigned int num,const char * word);

/**
 * @brief Get back a string ( token ) after performing InputParser_SeperateWords
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to get
 * @param Pointer to a C String that will accomodate the output
 * @param Size of Pointer provided , should be big enough for the whole string plus its null terminator
 * @retval 1=Sucess,0=Failure
 */
unsigned int InputParser_GetWord(struct InputParserC * ipc,unsigned int num,char * wheretostore,unsigned int storagesize);

/**
 * @brief Get back an uppercase string ( token ) after performing InputParser_SeperateWords
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to get
 * @param Pointer to a C String that will accomodate the output
 * @param Size of Pointer provided , should be big enough for the whole string plus its null terminator
 * @retval 1=Sucess,0=Failure
 */
unsigned int InputParser_GetUpcaseWord(struct InputParserC * ipc,unsigned int num,char * wheretostore,unsigned int storagesize);

/**
 * @brief Get back a lowercase string ( token ) after performing InputParser_SeperateWords
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to get
 * @param Pointer to a C String that will accomodate the output
 * @param Size of Pointer provided , should be big enough for the whole string plus its null terminator
 * @retval 1=Sucess,0=Failure
 */
unsigned int InputParser_GetLowercaseWord(struct InputParserC * ipc,unsigned int num,char * wheretostore,unsigned int storagesize);


/**
 * @brief Get back a signed integer ( token ) after performing InputParser_SeperateWords
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to get
 * @retval The value in the form  of a number of token with number num , if the token is not a number returns null
 */
signed int InputParser_GetWordInt(struct InputParserC * ipc,unsigned int num);


/**
 * @brief Get back a float ( token ) after performing InputParser_SeperateWords
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to get
 * @retval The value in the form  of a floating precision number of token , if the token is not a number returns NaN
 */
float InputParser_GetWordFloat(struct InputParserC * ipc,unsigned int num);


/**
 * @brief Get back The Length of a token so that we can allocate a proper buffer and then call InputParser_GetWord
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to get
 * @retval The length of the token string size , 0=Failure to access token num
 */
unsigned int InputParser_GetWordLength(struct InputParserC * ipc,unsigned int num);


/**
 * @brief Split input string in tokens, this is the main functionality of this library..!
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to split to tokens
 * @param 1=Keep a copy of the input string, this is safer in case the input string changes from another thread etc. , 0=Zero copy splitting that has more chances of going wrong
 * @retval The number of tokens split,0=Failure
 */
int InputParser_SeperateWords(struct InputParserC * ipc,char * inpt,char keepcopy);

/**
 * @brief Same as InputParser_SeperateWords but accepts const char strings as input
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to split to tokens
 * @param 1=Keep a copy of the input string, this is safer in case the input string changes from another thread etc. , 0=Zero copy splitting that has more chances of going wrong
 * @retval The number of tokens split,0=Failure
 */
int InputParser_SeperateWordsCC(struct InputParserC * ipc,const char * inpt,char keepcopy);

/**
 * @brief Same as InputParser_SeperateWords but accepts unsigned char strings as input
 * @ingroup InputParser
 * @param InputParser Context to be used ( needs to be created using InputParser_Create )
 * @param Word that we want to split to tokens
 * @param 1=Keep a copy of the input string, this is safer in case the input string changes from another thread etc. , 0=Zero copy splitting that has more chances of going wrong
 * @retval The number of tokens split,0=Failure
 */
int InputParser_SeperateWordsUC(struct InputParserC * ipc,unsigned char * inpt,char keepcopy);


#ifdef __cplusplus
}
#endif


#endif

