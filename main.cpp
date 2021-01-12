#include "FuncionesCruce.cpp"
#include <assert.h>

/* Map of all SKUs with their date ranges */
map<string, vector<tuple<Date, Date, float, vector<string>>>> ranges;
string input_filename = "prueba.csv";
string output_filename = "output.csv";

int main () {
    ifstream input (input_filename);
    pair<int, int> date = get_input ();

    /* Read first line and get indexes for fields in use */
	auto headers = read_line (input, nullptr);
	auto indexes = check_indexes (headers);

	/* Fill the ranges map with all ranges of all SKUs */
	read_all (input, ranges, indexes, date.first, date.second);

	auto both_headers = get_value_fields(headers, indexes);
	/* Write the data into a new csv file */
	write_output (output_filename, ranges, both_headers);
	return 0;
}
