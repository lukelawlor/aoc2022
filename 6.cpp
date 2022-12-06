/*
 * Advent of Code 2022 Day 6
 *
 * Solution by Luke L <lklawlor1@gmail.com>
 */

#include <iostream>
#include <cstdlib>

int main(int argc, char **argv)
{
	// Check for correct # of command line args
	if (argc != 3)
	{
		std::cerr <<	"error: wrong number of command line args provided\n"	
				"usage: " << argv[0] << " bufferstream marker_len\n";
		return 1;
	}

	// Data buffer stream to read from
	const char *bufstream = argv[1];

	// # of chars that make up a marker
	int marker_len = std::atoi(argv[2]);

	if (marker_len == 0)
	{
		std::cerr << "error: marker_len was 0.\n";
		return 1;
	}

	// Index in bufstream
	int pos = 0;

	// String of last few chars read
	char prev_char[marker_len];

	// Current char being read
	char c;

	// Read first (marker_len) chars into prev_char
	try
	{
		while (true)
		{
			if ((c = bufstream[pos]) == '\0')
				throw "unexpected end of stream. # of chars read was less than marker length";

			prev_char[pos] = c;
			if (++pos == marker_len)
				break;
		}
	}
	catch (const char *err)
	{
		std::cerr << "error: " << err << ".\n";
		return 1;
	}

	// Check for (marker_len) unique chars in a row
	try
	{
		while (true)
		{
		l_while_start:
			if ((c = bufstream[pos++]) == '\0')
				throw "no markers found";

			// Check if chars are all unique
			for (int i = 0; i < marker_len - 1; i++)
			{
				for (int j = i + 1; j < marker_len; j++)
				{
					if (prev_char[i] == prev_char[j])
					{
						// Duplicate char found, append c to end of prev_char
						for (int k = 0; k < marker_len - 1; k++)
							prev_char[k] = prev_char[k + 1];
						prev_char[marker_len - 1] = c;
						goto l_while_start;
					}
				}
			}

			// No duplicate chars found
			std::cout << "first marker after character " << pos - 1 << '\n';
			break;
		}
	}
	catch (const char *err)
	{
		std::cerr << "error: " << err << ".\n";
		return 1;
	}

	return 0;
}
