#include <tvision/PXPictureValidator.h>
#include <tvision/tobjstrm.h>

static inline char uppercase(char c) { return (('a' <= c) && (c <= 'z')) ? c + 'A' - 'a' : c; }

bool isNumber(char ch) { return bool(('0' <= ch) && (ch <= '9')); }

bool isLetter(char ch)
{
    ch &= 0xdf;
    return bool(('A' <= ch) && (ch <= 'Z'));
}

bool isSpecial(char ch, const char* special)
{
    if (memchr(special, ch, strlen(special)) != 0)
        return true;
    else
        return false;
}

/*
  This helper function will be used for a persistant TInputLine mask.
  It will be moved to TINPUTLI.CPP when needed.
*/
uchar numChar(char ch, const char* s)
{
    int count;
    uchar n;

    for (count = strlen(s), n = 0; count; count--, s++)
        if (*s == ch)
            n++;
    return n;
}

bool isComplete(TPicResult result)
{
    return bool((result == prComplete) || (result == prAmbiguous));
}

bool isIncomplete(TPicResult result)
{
    return bool((result == prIncomplete) || (result == prIncompNoFill));
}

const char* const TPXPictureValidator::name = "TPXPictureValidator";

const char* TPXPictureValidator::errorMsg = "Error in picture format.\n %s";

__link(RValidator);

TStreamableClass RPXPictureValidator(
    TPXPictureValidator::name, TPXPictureValidator::build, __DELTA(TPXPictureValidator));

TPXPictureValidator::TPXPictureValidator(TStringView aPic, bool autoFill)
    : TValidator()
{
    pic = newStr(aPic);
    if (autoFill)
        options |= voFill;

    char s[] = "";
    if (picture(s, false) != prEmpty)
        status = vsSyntax;
}

#ifndef NO_STREAMABLE

TPXPictureValidator::TPXPictureValidator(StreamableInit s) noexcept
    : TValidator(s)
{
}

void TPXPictureValidator::write(opstream& os)
{
    TValidator::write(os);
    os.writeString(pic);
}

void* TPXPictureValidator::read(ipstream& is)
{
    TValidator::read(is);
    pic = is.readString();
    index = jndex = 0;

    return this;
}

TStreamable* TPXPictureValidator::build() { return new TPXPictureValidator(streamableInit); }

#endif

TPXPictureValidator::~TPXPictureValidator() { delete[] pic; }

void TPXPictureValidator::error() { messageBox(mfError | mfOKButton, errorMsg, pic); }

bool TPXPictureValidator::isValidInput(char* s, bool suppressFill)
{
    bool doFill = bool(((options & voFill) != 0) && !suppressFill);

    return bool((pic == 0) || (picture((char*)s, doFill) != prError));
}

bool TPXPictureValidator::isValid(const char* s)
{
    char str[256];

    strcpy(str, s);
    return bool((pic == 0) || (picture(str, false) == prComplete));
}

// Consume input
void TPXPictureValidator::consume(char ch, char* input)
{
    input[jndex] = ch;
    index++;
    jndex++;
}

// Skip a character or a picture group

void TPXPictureValidator::toGroupEnd(int& i, int termCh)
{
    int brkLevel, brcLevel;

    brkLevel = 0;
    brcLevel = 0;
    do {
        if (i == termCh)
            return;
        else
            switch (pic[i]) {
            case '[':
                brkLevel++;
                break;
            case ']':
                brkLevel--;
                break;
            case '{':
                brcLevel++;
                break;
            case '}':
                brcLevel--;
                break;
            case ';':
                i++;
                break;
            }
        i++;
    } while (!((brkLevel == 0) && (brcLevel == 0)));
}

// Find the a comma separator
bool TPXPictureValidator::skipToComma(int termCh)
{
    do {
        toGroupEnd(index, termCh);
    } while (!((index == termCh) || (pic[index] == ',')));

    if (pic[index] == ',')
        index++;
    return bool(index < termCh);
}

// Calclate the end of a group
int TPXPictureValidator::calcTerm(int termCh)
{
    int k = index;
    toGroupEnd(k, termCh);
    return k;
}

// The next group is repeated X times }
TPicResult TPXPictureValidator::iteration(char* input, int inTerm)
{
    int itr, k, l;
    TPicResult rslt;
    int termCh;

    itr = 0;
    rslt = prError;

    index++; // Skip '*'

    // Retrieve number

    while (isNumber(pic[index])) {
        itr = itr * 10 + (pic[index] - '0');
        index++;
    }

    k = index;
    termCh = calcTerm(inTerm);

    // If Itr is 0 allow any number, otherwise enforce the number
    if (itr != 0) {
        for (l = 1; l <= itr; l++) {
            index = k;
            rslt = process(input, termCh);
            if (!isComplete(rslt)) {
                // Empty means incomplete since all are required
                if (rslt == prEmpty)
                    rslt = prIncomplete;

                return rslt;
            }
        }
    } else {
        do {
            index = k;
            rslt = process(input, termCh);
        } while (rslt == prComplete);

        if ((rslt == prEmpty) || (rslt == prError)) {
            index++;
            rslt = prAmbiguous;
        }
    }
    index = termCh;

    return rslt;
}

// Process a picture group
TPicResult TPXPictureValidator::group(char* input, int inTerm)
{

    TPicResult rslt;
    int termCh;

    termCh = calcTerm(inTerm);
    index++;
    rslt = process(input, termCh - 1);

    if (!isIncomplete(rslt))
        index = termCh;

    return rslt;
}

TPicResult TPXPictureValidator::checkComplete(TPicResult rslt, int termCh)
{
    int j = index;
    bool status = true;

    if (isIncomplete(rslt)) {
        // Skip optional pieces
        while (status)
            switch (pic[j]) {
            case '[':
                toGroupEnd(j, termCh);
                break;
            case '*':
                if (!isNumber(pic[j + 1]))
                    j++;
                toGroupEnd(j, termCh);
                break;

            default:
                status = false;
            }

        if (j == termCh)
            rslt = prAmbiguous;
    }

    return rslt;
}

TPicResult TPXPictureValidator::scan(char* input, int termCh)
{
    char ch;
    TPicResult rslt, rScan;

    rScan = prError;
    rslt = prEmpty;

    while ((index != termCh) && (pic[index] != ',')) {
        if (jndex >= (int)strlen(input))
            return checkComplete(rslt, termCh);

        ch = input[jndex];
        switch (pic[index]) {
        case '#':
            if (!isNumber(ch))
                return prError;
            else
                consume(ch, input);
            break;
        case '?':
            if (!isLetter(ch))
                return prError;
            else
                consume(ch, input);
            break;
        case '&':
            if (!isLetter(ch))
                return prError;
            else
                consume(uppercase(ch), input);
            break;
        case '!':
            consume(uppercase(ch), input);
            break;
        case '@':
            consume(ch, input);
            break;
        case '*':

            rslt = iteration(input, termCh);
            if (!isComplete(rslt))
                return rslt;

            if (rslt == prError)
                rslt = prAmbiguous;
            break;

        case '{':

            rslt = group(input, termCh);
            if (!isComplete(rslt))
                return rslt;

            break;
        case '[':

            rslt = group(input, termCh);
            if (isIncomplete(rslt))
                return rslt;
            if (rslt == prError)
                rslt = prAmbiguous;

            break;

        default:

            if (pic[index] == ';')
                index++;
            if (uppercase(pic[index]) != uppercase(ch)) {
                if (ch == ' ')
                    ch = pic[index];
                else
                    return rScan;
            }

            consume(pic[index], input);
        }

        if (rslt == prAmbiguous)
            rslt = prIncompNoFill;
        else
            rslt = prIncomplete;
    }

    if (rslt == prIncompNoFill)
        return prAmbiguous;
    else
        return prComplete;
}

TPicResult TPXPictureValidator::process(char* input, int termCh)
{

    TPicResult rslt, rProcess;
    bool incomp;
    int oldI, oldJ, incompJ = 0, incompI = 0;

    incomp = false;
    oldI = index;
    oldJ = jndex;
    do {
        rslt = scan(input, termCh);

        // Only accept completes if they make it farther in the input
        //   stream from the last incomplete

        if ((rslt == prComplete) && incomp && (jndex < incompJ)) {
            rslt = prIncomplete;
            jndex = incompJ;
        }

        if ((rslt == prError) || (rslt == prIncomplete)) {
            rProcess = rslt;

            if (!incomp && (rslt == prIncomplete)) {
                incomp = true;
                incompI = index;
                incompJ = jndex;
            }
            index = oldI;
            jndex = oldJ;
            if (!skipToComma(termCh)) {
                if (incomp) {
                    rProcess = prIncomplete;
                    index = incompI;
                    jndex = incompJ;
                }
                return rProcess;
            }
            oldI = index;
        }
    } while (!((rslt != prError) && (rslt != prIncomplete)));

    if ((rslt == prComplete) && incomp)
        return prAmbiguous;
    else
        return rslt;
}

bool TPXPictureValidator::syntaxCheck()
{

    int i, len;
    int brkLevel, brcLevel;

    if (!pic || (strlen(pic) == 0))
        return false;

    if (pic[strlen(pic) - 1] == ';')
        return false;

    i = 0;
    brkLevel = 0;
    brcLevel = 0;

    len = strlen(pic);
    while (i < len) {
        switch (pic[i]) {
        case '[':
            brkLevel++;
            break;
        case ']':
            brkLevel--;
            break;
        case '{':
            brcLevel++;
            break;
        case '}':
            brcLevel--;
            break;
        case ';':
            i++;
            break;
        }
        i++;
    }

    return bool((brkLevel == 0) && (brcLevel == 0));
}

TPicResult TPXPictureValidator::picture(char* input, bool autoFill)
{

    bool reprocess;
    TPicResult rslt;

    if (!syntaxCheck())
        return prSyntax;

    if (!input || strlen(input) == 0)
        return prEmpty;

    jndex = 0;
    index = 0;

    rslt = process(input, strlen(pic));

    if ((rslt != prError) && (jndex < (int)strlen(input)))
        rslt = prError;

    if ((rslt == prIncomplete) && autoFill) {
        reprocess = false;

        while ((index < (int)strlen(pic)) && !isSpecial(pic[index], "#?&!@*{}[],")) {
            if (pic[index] == ';')
                index++;
            int end = strlen(input);
            input[end] = pic[index];
            input[end + 1] = 0;
            index++;
            reprocess = true;
        }

        jndex = 0;
        index = 0;
        if (reprocess)
            rslt = process(input, strlen(pic));
    }

    if (rslt == prAmbiguous)
        return prComplete;
    else if (rslt == prIncompNoFill)
        return prIncomplete;
    else
        return rslt;
}
