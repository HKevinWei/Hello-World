#define _CRT_SECURE_NO_WARNINGS

#include <cstring>
#include <cctype>
using namespace std;

const int MAX_WORD_LENGTH = 20;
const int MAX_DOCUMENT_LENGTH = 200;

// Convert word to lower case, returning true if the word is non-empty
// and contains only letters.
bool standardizeWord(char s[])
{
	if (s[0] == '\0')  // Is word empty?
		return false;
	for (int k = 0; s[k] != '\0'; k++)
	{
		if (isalpha(s[k]))
			s[k] = tolower(s[k]);
		else // Any non-alphabetic character means the word is bad.
			return false;
	}
	return true;  // Everything was OK
}

bool isPairSame(const char aw1[], const char aw2[], const char bw1[], const char bw2[])
{
	return (strcmp(aw1, bw1) == 0 && strcmp(aw2, bw2) == 0);
}

int standardizeRules(int distance[],
	char word1[][MAX_WORD_LENGTH + 1],
	char word2[][MAX_WORD_LENGTH + 1],
	int nRules)
{
	if (nRules < 0)
		return 0;
	int c = 0;
	while (c < nRules)
	{
		bool eliminateThisRule = false;
		if (distance[c] <= 0 || !standardizeWord(word1[c]) ||
			!standardizeWord(word2[c]))
			eliminateThisRule = true;
		else
		{
			// Does an earlier rule have the same words?  (There will
			// be at most one, since we will have previously eliminated
			// others.)
			for (int c2 = 0; c2 < c; c2++)
			{
				if (isPairSame(word1[c2], word2[c2], word1[c], word2[c]) ||
					isPairSame(word1[c2], word2[c2], word2[c], word1[c]))
				{
					// Words match, so retain in position c2 the rule
					// with the greater distance.
					if (distance[c2] < distance[c])
						distance[c2] = distance[c];
					eliminateThisRule = true;
					break;
				}
			}
		}
		if (eliminateThisRule)
		{
			// Copy the last rule into this one.  Don't increment c,
			// so that we examine that rule on the next iteration.
			nRules--;
			distance[c] = distance[nRules];
			strcpy(word1[c], word1[nRules]);
			strcpy(word2[c], word2[nRules]);
		}
		else // go on to the next rule
			c++;
	}
	return nRules;
}

int determineQuality(const int distance[],
	const char word1[][MAX_WORD_LENGTH + 1],
	const char word2[][MAX_WORD_LENGTH + 1],
	int nRules,
	const char document[])
{
	// Get document words

	// There can't be more than this many words.  (Worst case is a
	// document like "a a a a a ..."
	const int MAX_DOC_WORDS = MAX_DOCUMENT_LENGTH / 2;

	// We'll store the document words here.  If a document word is more
	// than MAX_WORD_LENGTH letters long, we'll store only the first
	// MAX_WORD_LENGTH+1 letters; since the long word can't possibly
	// match a rule word (which is limited to MAX_WORD_LENGTH
	// characters), we'll store only enough to ensure we don't match.
	char docWord[MAX_DOC_WORDS][MAX_WORD_LENGTH + 1 + 1];

	// Visit each character of the document, transferring letters into
	// the docWord array.
	int nDocWords = 0;
	int docWordPos = 0;
	for (int pos = 0; document[pos] != '\0'; pos++)
	{
		if (isalpha(document[pos]))
		{
			// Append letter to the end of the current docWord
			if (docWordPos < MAX_WORD_LENGTH + 1)
			{
				docWord[nDocWords][docWordPos] = tolower(document[pos]);
				docWordPos++;
			}
		}
		else if (document[pos] == ' ')
		{
			// If a word was started, mark the end and prepare for the next 
			if (docWordPos > 0)
			{
				docWord[nDocWords][docWordPos] = '\0';
				nDocWords++;
				docWordPos = 0;
			}
		}
		// Non-letter, non-blank characters aren't transferred and don't
		// end a word, so do nothing with them.
	}
	// If the document didn't end with a blank, end the last word
	if (docWordPos > 0)
	{
		docWord[nDocWords][docWordPos] = '\0';
		nDocWords++;
	}

	// Count matching rules

	int nMatches = 0;

	// For each rule
	for (int c = 0; c < nRules; c++)
	{
		// For each occurrence of the first word
		for (int pos1 = 1; pos1 < nDocWords; pos1++)
		{
			if (strcmp(docWord[pos1], word1[c]) != 0)
				continue;

			// Find the second word within the distance
			int pos2 = pos1 - distance[c];
			if (pos2 < 0)  // Don't start before the first word of the doc.
				pos2 = 0;
			int end = pos1 + distance[c];
			if (end >= nDocWords)  // Don't end after the last word of the doc.
				end = nDocWords - 1;
			for (; pos2 <= end &&
				(pos2 == pos1 || strcmp(docWord[pos2], word2[c]) != 0);
				pos2++)
				;
			if (pos2 <= end)  // found
			{
				nMatches++;
				break;  // Don't find any more matches for this rule.
			}
		}
	}
	return nMatches;
}