#include <iostream>
#include <string>

using namespace std;

char translateNote(int octave, char noteLetter, char accidentalSign);

const char END_OF_BEAT = '/';
const int DEFAULT_OCTAVE = 4;

//*************************************
//  isTuneWellFormed
//*************************************

bool isTuneWellFormed(string tune)
{
	// An empty tune is well-formed.

	if (tune.size() == 0)
		return true;

	// A non-empty tune must end with an end-of-beat marker.

	if (tune[tune.size() - 1] != END_OF_BEAT)
		return false;

	// At this point, we know the tune ends properly, so in the following
	// loop, we do not have to worry about, say, there not being a character
	// following a digit.

	// Each iteration of the loop recognizes one beat.

	int k = 0;
	while (k != tune.size())
	{
		// Each iteration of the loop recognizes one note

		while (tune[k] != END_OF_BEAT)
		{
			// Verify note letter

			char note = tune[k];
			if (note != 'A' && note != 'B' && note != 'C' && note != 'D' &&
				note != 'E' && note != 'F' && note != 'G')
				return false;
			k++;

			// Verify possible accidental

			if (tune[k] == '#' || tune[k] == 'b')
				k++;

			// Verify possible octave

			if (isdigit(tune[k]))
				k++;
		}

		// Advance past end of beat.

		k++;
	}

	// We get here if we got through the tune without a problem.

	return true;
}

//*************************************
//  translateTune
//*************************************

int translateTune(string tune, string& instructions, int& badBeat)
{
	// Define return values

	const int RET_OK = 0;
	const int RET_NOT_WELL_FORMED = 1;
	const int RET_UNPLAYABLE_NOTE = 2;

	// A tune that is not well-formed is not translatable.

	if (!isTuneWellFormed(tune))
		return RET_NOT_WELL_FORMED;

	// We will build the translation in the string named result, and
	// modify the instructions parameter only if the entire translation
	// succeeds.

	string result;

	// Each iteration of the loop translates one beat.

	int k = 0;
	for (int beatNumber = 1; k != tune.size(); beatNumber++)
	{
		// A beat with no note translates to a space

		if (tune[k] == END_OF_BEAT)
		{
			result += ' ';
			k++;
			continue;
		}

		// The beat has at least one note if we get here.  Each iteration
		// of the loop processes one note

		int noteCount = 0;

		while (tune[k] != END_OF_BEAT)
		{
			noteCount++;

			// Record note letter

			char noteLetter = tune[k];
			k++;

			// Record accidental sign, if any

			char accidentalSign = ' ';
			if (tune[k] == '#' || tune[k] == 'b')
			{
				accidentalSign = tune[k];
				k++;
			}

			// Record octave number, if any

			int octave = DEFAULT_OCTAVE;
			if (isdigit(tune[k]))
			{
				octave = tune[k] - '0';
				k++;
			}

			// Attempt to translate the note

			char translatedNote = translateNote(octave, noteLetter, accidentalSign);

			// An unplayable note makes the tune unplayable

			if (translatedNote == ' ')
			{
				badBeat = beatNumber;
				return RET_UNPLAYABLE_NOTE;
			}

			// If there's another note in this beat, it's a chord

			if (tune[k] != END_OF_BEAT  &&  noteCount == 1)
				result += '[';

			result += translatedNote;
		}

		// If beat had a chord, close it off

		if (noteCount > 1)
			result += ']';

		// Advance past end of beat marker

		k++;
	}

	// We've successfully translated the entire tune, so set instructions.

	instructions = result;

	return RET_OK;
}

//*************************************
//  translateNote
//*************************************

// Given an octave number, a note letter, and an accidental sign, return
// the character that the indicated note translates to if it is playable.
// Return a space character if it is not playable.
//
// First parameter:  the octave number (the integer 4 is the number of the
//      octave containing middle C, for example).
// Second parameter:  an upper case note letter, 'A' through 'G'
// Third parameter:  '#', 'b', or ' ' (meaning no accidental sign)
//
// Examples:  translateNote(4, 'A', ' ') returns 'Q'
//            translateNote(4, 'A', '#') returns '%'
//            translateNote(4, 'H', ' ') returns ' '

char translateNote(int octave, char noteLetter, char accidentalSign)
{
	// This check is here solely to report a common CS 31 student error.
	if (octave > 9)
	{
		cerr << "********** translateNote was called with first argument = "
			<< octave << endl;
	}

	int note;
	switch (noteLetter)
	{
	case 'C':  note = 0; break;
	case 'D':  note = 2; break;
	case 'E':  note = 4; break;
	case 'F':  note = 5; break;
	case 'G':  note = 7; break;
	case 'A':  note = 9; break;
	case 'B':  note = 11; break;
	default:   return ' ';
	}
	switch (accidentalSign)
	{
	case '#':  note++; break;
	case 'b':  note--; break;
	case ' ':  break;
	default:   return ' ';
	}
	int sequenceNumber = 12 * (octave - 2) + note;
	string keymap = "Z1X2CV3B4N5M,6.7/A8S9D0FG!H@JK#L$Q%WE^R&TY*U(I)OP";
	if (sequenceNumber < 0 || sequenceNumber >= keymap.size())
		return ' ';
	return keymap[sequenceNumber];
}