/************************************************************************
 *
 * File: Odds.H
 *
 * Main include file for the Odds Calculator Program.
 *
 ************************************************************************/
#define APPNAME "Poker Odds Calculator"
#define VERSION "v1.01"
#define SYNTAX  "Syntax:\n\n\tOdds <Game> {<Outfile>} {-c} {-b<bet>} {-s}\n\nwhere:\n\n\t"\
    "<Game> is the name of the game DLL file\n\n"\
    "\t<Outfile> is an optional output file.\n\n"\
    "\t-c - Forces computation over all possible hands.\n\n"\
    "\t-b<bet> - Sets the wager (default=$1).\n\n"\
    "\t-s - Bypasses high speed algorithm.\n\n"

typedef unsigned char   CARD;

    /* Error messages */

#define BADDLL   "Error! - Cannot load game \"%s\"\n"
#define BADOUTPUT   "Error! - Cannot write to output file \"%s\"\n"
#define DLLNOTSUPPORTED "Warning - Game file does not support high speed probability calculations.\n"

    /* Error codes */

#define NO_ERROR    0
#define ERROR_NO_ERROR  0
#define ERROR_NODLL     1
#define ERROR_NOT_SUPPORTED     2
#define ERROR_BAD_SYNTAX            3
#define ERROR_BAD_OUTPUT            4
