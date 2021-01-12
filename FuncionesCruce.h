#ifndef TRASLAPEFECHAS_FUNCIONESCRUCE_H
#define TRASLAPEFECHAS_FUNCIONESCRUCE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <map>
#include <algorithm>
#include <queue>
#include <functional>

using namespace std;

class Date {
public:
    int day;
    int month;
    int year;

    Date (int new_day, int new_month, int new_year) {
        day = new_day;
        month = new_month;
        year = new_year;
    }

    bool operator < (Date comp_date) {
        if (month < comp_date.month)
            return true;
        else if (month > comp_date.month)
            return false;
        else {
            if (day < comp_date.day || day == comp_date.day)
                return true;
            else
                return false;
        }
    }

    bool operator > (Date comp_date) {
        if (month > comp_date.month)
            return true;
        else if (month < comp_date.month)
            return false;
        else {
            if (day > comp_date.day || day == comp_date.day)
                return true;
            else
                return false;
        }
    }

    bool operator == (Date comp_date) {
        if (year == comp_date.year && month == comp_date.month && day == comp_date.day)
            return true;
        else
            return false;
    }
};

/* Main */
vector<string> read_line (ifstream &, vector<int> *);
void read_all (ifstream &, map<string, vector<tuple<Date, Date, float, vector<string>>>> &, vector<int>, int, int);
void write_output (string, map<string, vector<tuple<Date, Date, float, vector<string>>>>, pair<vector<string>, vector<string>>);
void insert_data (map<string, vector<tuple<Date, Date, float, vector<string>>>> &, vector<string>, int, int, vector<int>);
vector<tuple<Date, Date, float, vector<string>>> process_dates (vector<tuple<Date, Date, float, vector<string>>>&);

/* Auxiliary */
pair<int, int> get_input ();
Date string_to_date (string);
int get_num_day (int);
bool compare_ranges_by_contribution (tuple<Date, Date, float, vector<string>>, tuple<Date, Date, float, vector<string>>);
bool compare_ranges_by_start_date (tuple<Date, Date, float, vector<string>>, tuple<Date, Date, float, vector<string>>);
pair<vector<string>, vector<string>> get_value_fields (vector<string>, vector<int>);
vector<int> check_indexes (vector<string>);
vector<tuple<Date, Date, float, vector<string>>> cross_date (vector<tuple<Date, Date, float, vector<string>>> &, tuple<Date, Date, float, vector<string>>);

/* Debugging */
void print_ranges (map<string, vector<tuple<Date, Date, float, vector<string>>>>);
#endif //TRASLAPEFECHAS_FUNCIONESCRUCE_H
