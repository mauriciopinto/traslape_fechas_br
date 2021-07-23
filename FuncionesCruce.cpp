#include "FuncionesCruce.h"

using namespace std;

vector<string> read_line (ifstream &input, vector<int> *indexes) {
	string line;
	vector<string> fields;

	getline (input, line);

	char *c_line = (char *) line.c_str ();
	char *token = strtok (c_line, ";");
	
	while (token != NULL) {
		fields.emplace_back(token);
		token = strtok (NULL, ";");
	}
	return fields;
}

vector<int> check_indexes (vector<string> fields) {
    vector<int> indexes;
    for (int i = 0; i < fields.size (); i++) {
        for_each (fields[i].begin(), fields[i].end(), [](char & c) {
            c = toupper(c);
        });
        if (fields[i] == "SKU" || fields[i] == "APORTE" || fields[i] == "FECHA_INICIO" || fields[i] == "FECHA_FIN")
            indexes.push_back(i);
    }
    return indexes;
}

void read_all (ifstream &input, map<string, vector<tuple<Date, Date, float, vector<string>>>> &ranges, vector<int> indexes, int month, int year) {
    vector<string> current_fields;
    vector<string> value_fields;
    while (input.peek() != EOF) {
        current_fields = read_line(input, &indexes);
        insert_data (ranges, current_fields, month, year, indexes);
    }
    //print_ranges(ranges);
}

pair<vector<string>, vector<string>> get_value_fields (vector<string> fields, vector<int> indexes) {
    vector<string> value_fields;
    vector<string> other_fields;
    pair<vector<string>, vector<string>> both_fields;

    for (int i = 0; i < fields.size (); i++) {
        if (find (indexes.begin(), indexes.end(), i) != indexes.end())
            value_fields.push_back (fields[i]);
        else
            other_fields.push_back (fields[i]);
    }
    both_fields = {value_fields, other_fields};
    return both_fields;
}

void insert_data (map<string, vector<tuple<Date, Date, float, vector<string>>>> &ranges, vector<string> fields, int month, int year, vector<int> indexes) {
    auto both_fields = get_value_fields(fields, indexes);
    auto value_fields = both_fields.first;
    auto other_fields = both_fields.second;

    string sku = value_fields[0];
    float aporte = stof (value_fields[1]);
    string fecha_inicio_str = value_fields[2];
    string fecha_fin_str = value_fields[3];

    Date fecha_inicio = string_to_date (fecha_inicio_str);
    Date fecha_fin = string_to_date (fecha_fin_str);

    /* Convertirla al mes deseado */
    /* Ignorar si se encuentra totalmente fuera del rango */
    if (fecha_inicio.year != year && fecha_fin.year != year)
        return;
    if (fecha_inicio.year == year && fecha_fin.year == year) {
        if (fecha_inicio.month < month && fecha_fin.month < month)
            return;
        if (fecha_inicio.month > month && fecha_fin.month > month) {
            return;
        }
    }
    /* Convertirla al mes deseado */
    if (fecha_inicio.month != month && fecha_fin.month == month) {
        fecha_inicio.month = month;
        fecha_inicio.day = 1;
        fecha_inicio.year = year;
    }
    else if (fecha_inicio.month == month && fecha_fin.month != month) {
        fecha_fin.month = month;
        fecha_fin.day = get_num_day (month);
        fecha_fin.year = year;
    }
    else if (fecha_inicio.month != month && fecha_fin.month != month) {
        fecha_inicio.month = month;
        fecha_inicio.day = 1;
        fecha_fin.month = month;
        fecha_fin.day = get_num_day (month);
        fecha_inicio.year = fecha_fin.year = year;
    }
    if (fecha_inicio.year != year && fecha_fin.year != year)
        return;
    if (fecha_inicio.year == year && fecha_fin.year == year) {
        if (fecha_inicio.month < month && fecha_fin.month < month)
            return;
        if (fecha_inicio.month > month && fecha_fin.month > month) {
            cout << "here" << endl;
            return;
        }
    }
    /**/
    tuple<Date, Date, float, vector<string>> new_entry = { fecha_inicio, fecha_fin, aporte, other_fields };
    if (ranges.find(sku) == ranges.end ()) {
        vector<tuple<Date, Date, float, vector<string>>> new_ranges;
        ranges.insert(pair<string, vector<tuple<Date, Date, float, vector<string>>>> (sku, new_ranges));
    }
    ranges.find(sku)->second.push_back(new_entry);
    auto new_ranges = process_dates(ranges.find(sku)->second);
    ranges.find(sku)->second = new_ranges;
}

Date string_to_date (string date) {
    char *token;

    token = strtok ((char *) date.c_str(), "/");
    string month_str = token;
    token = strtok (NULL, "/");
    string day_str = token;
    token = strtok (NULL, "/");
    string year_str = token;

    int day, month, year;
    day = stoi (day_str);
    month = stoi (month_str);
    year = stoi (year_str);

    if (year < 2000)
        year += 2000;

    Date new_date (day, month, year);
    return new_date;
}

void print_ranges (map<string, vector<tuple<Date, Date, float, vector<string>>>> ranges) {
    map<string, vector<tuple<Date, Date, float, vector<string>>>>::iterator it;
    for (it = ranges.begin(); it != ranges.end(); it++) {
        cout << it->first << ":" << endl;
        auto range_vector = it->second;
        for (int i = 0; i < range_vector.size(); i++) {
            cout << get<0> (range_vector[i]).day << "," << get<0> (range_vector[i]).month << "," <<  get<0> (range_vector[i]).year << " ";
            cout << get<1> (range_vector[i]).day << "," << get<1> (range_vector[i]).month << "," <<  get<1> (range_vector[i]).year << " ";
            cout << get<2> (range_vector[i]) << ",";
            for (int j = 0; j < get<3> (range_vector[i]).size(); j++) {
                cout << get<3> (range_vector[i])[i] << ",";
            }
            cout << endl;
        }
    }
}

vector<tuple<Date, Date, float, vector<string>>> process_dates (vector<tuple<Date, Date, float, vector<string>>> &range_vector) {
    vector<tuple<Date, Date, float, vector<string>>> new_ranges;
    priority_queue<tuple<Date, Date, float, vector<string>>, vector<tuple<Date, Date, float, vector<string>>>, function<bool(tuple<Date, Date, float, vector<string>>, tuple<Date, Date, float, vector<string>>)>> ordered_ranges (compare_ranges_by_contribution);
    for (int i = 0; i < range_vector.size(); i++) {
        ordered_ranges.push(range_vector[i]);
    }

    while (!ordered_ranges.empty()) {
        auto current = ordered_ranges.top();
        new_ranges = cross_date(new_ranges, current);
        ordered_ranges.pop();
    }
    return new_ranges;
}

bool compare_ranges_by_contribution (tuple<Date, Date, float, vector<string>> range_1, tuple<Date, Date, float, vector<string>> range_2) {
    if (get<2> (range_1) == get<2> (range_2))
        return get<1> (range_1).day - get<0> (range_1).day < get<1> (range_2).day - get<0> (range_2).day;
    return get<2> (range_1) < get<2> (range_2);
}

bool compare_ranges_by_start_date (tuple<Date, Date, float, vector<string>> range_1, tuple<Date, Date, float, vector<string>> range_2) {
    return get<0> (range_1) < get<0> (range_2);
}

vector<tuple<Date, Date, float, vector<string>>> cross_date (vector<tuple<Date, Date, float, vector<string>>> &current_ranges, tuple<Date, Date, float, vector<string>> new_range) {
    tuple<Date, Date, float, vector<string>> aux = new_range;
    if (current_ranges.size() == 0) {
        current_ranges.push_back(new_range);
        sort(current_ranges.begin(), current_ranges.end(), compare_ranges_by_start_date);
        return current_ranges;
    }
    for (auto range : current_ranges) {
        /* Si la fecha traslapa por la izquierda */
        if (get<0> (range) < get<0> (aux) && get<1> (range) > get<0> (aux)) {
            if (get<1> (range) == get<1> (aux)) {
                sort(current_ranges.begin(), current_ranges.end(), compare_ranges_by_start_date);
                return current_ranges;
            }
            if (get<1> (range) < get<1> (aux)) {
                if (get<1> (range).day == get_num_day(get<1> (range).month)) {
                    sort(current_ranges.begin(), current_ranges.end(), compare_ranges_by_start_date);
                    return current_ranges;
                }
                Date new_start_date (get<1> (range).day + 1, get<1> (range).month, get<1> (range).year);
                aux = {new_start_date, get<1> (aux), get<2> (aux), get<3> (aux)};
            }
            else if (get<1> (range) > get<1> (aux)) {
                sort(current_ranges.begin(), current_ranges.end(), compare_ranges_by_start_date);
                return current_ranges;
            }
        }
        /* Si la fecha traslapa por la derecha */
        else if (get<0> (aux) < get<0> (range) && get<1> (aux) > get<0> (range)) {
            Date lower_end_date (get<0> (range).day -1, get<0> (range).month, get<0> (range).year);
            current_ranges.push_back({get<0> (aux), lower_end_date, get<2> (aux), get<3> (aux)});

            /* Si se traslapa solo a la derecha */
            if (get<1> (range) > get<1> (aux) || get<1> (range) == get<1> (aux)) {
                sort(current_ranges.begin(), current_ranges.end(), compare_ranges_by_start_date);
                return current_ranges;
            }

            /* Si se traslapa al medio */
            else if (get<1> (range) < get<1> (aux)) {
                Date higher_start_date (get<1> (range).day + 1, get<1> (range).month, get<1> (range).year);
                aux = {higher_start_date, get<1> (aux), get<2> (aux), get<3> (aux)};
                continue;
            }
        }
    }
    current_ranges.push_back(aux);

    sort(current_ranges.begin(), current_ranges.end(), compare_ranges_by_start_date);
    /*for (int i = 0; i < current_ranges.size(); i++) {
        cout << get<0> (current_ranges[i]).day << "," << get<0> (current_ranges[i]).month << "," << get<0> (current_ranges[i]).year << " ";
        cout << get<1> (current_ranges[i]).day << "," << get<1> (current_ranges[i]).month << "," << get<1> (current_ranges[i]).year << " ";
        cout << get<2> (current_ranges[i]) << endl;
    }
    cout << endl;*/
    return current_ranges;
}

pair<int, int> get_input () {
    int month, year;
    cout << "Ingrese el mes que desea filtrar (en formato entero): ";
    cin >> month;
    cout << "Ingrese el año que desea filtrar: ";
    cin >> year;
    pair<int, int> date = {month, year};
    return date;
}

int get_num_day (int month) {
    int day;
    switch (month) {
        case 1:
            day = 31;
            break;

        case 2:
            day = 28;
            break;

        case 3:
            day = 31;
            break;

        case 4:
            day = 30;
            break;

        case 5:
            day = 31;
            break;

        case 6:
            day = 30;
            break;

        case 7:
            day = 31;
            break;

        case 8:
            day = 31;
            break;

        case 9:
            day = 30;
            break;

        case 10:
            day = 31;
            break;

        case 11:
            day = 30;
            break;

        case 12:
            day = 31;
            break;
    }
    return day;
}

void write_output (string filename, map<string, vector<tuple<Date, Date, float, vector<string>>>> ranges, pair<vector<string>, vector<string>> both_headers) {
    ofstream output (filename, ios::trunc);
    map<string, vector<tuple<Date, Date, float, vector<string>>>>::iterator it;

    for (int i = 0; i < both_headers.first.size(); i++) {
        output << both_headers.first[i] << ",";
    }

    for (int i = 0; i < both_headers.second.size(); i++) {
        output << both_headers.second[i];
        if (i == both_headers.second.size() - 1)
            output << endl;
        else
            output << ",";
    }

    for (it = ranges.begin(); it != ranges.end(); it++) {
        auto range_vector = it->second;
        for (int i = 0; i < range_vector.size(); i++) {
            output << it->first << ",";
            output << get<2> (range_vector[i]) << ",";
            output << get<0> (range_vector[i]).day << "/" << get<0> (range_vector[i]).month << "/" <<  get<0> (range_vector[i]).year << ",";
            output << get<1> (range_vector[i]).day << "/" << get<1> (range_vector[i]).month << "/" <<  get<1> (range_vector[i]).year << ",";
            for (int j = 0; j < get<3> (range_vector[i]).size(); j++) {
                output << get<3> (range_vector[i])[j];
                if (j == get<3> (range_vector[i]).size() - 1)
                    output << endl;
                else
                    output << ",";
            }
        }
    }
}