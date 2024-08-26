#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>
#include <unordered_map>
#include <stdexcept> 
#include <fstream>
#include <regex>
#include "../includes/csv-processor.hpp"

// Struct to store the header column name and its index
struct HeaderColumn
{
    std::string name;
    int index;
};

// Struct to store the filter definition
struct Filter
{
    int columnIndex;
    std::string comparator;
    std::string value;
};

// Preprocess the filters based on the header columns and the rowFilterDefinitions and store them in a vector of Filters
std::vector<Filter> preprocessFilters(const std::vector<std::string>& headerColumns, const std::string& rowFilterDefinitions){
    // Checking if the rowFilterDefinitions is empty
    if(rowFilterDefinitions.empty()){
        throw std::runtime_error("Invalid filter: There is no filter, rowFilterDefinitions is empty");
    }
    
    std::vector<Filter> filters;
    std::istringstream filterStream(rowFilterDefinitions);
    std::string filterDefinition;

    while (std::getline(filterStream, filterDefinition, '\n')) {
        // Using a regex to find the headerColumnName, the comparator and the value
        // If we don't find any valid comparator, we throw an error
        std::regex re(R"(([^<>=!=>=<=]+)([><=!=]{1,2})([^<>=!=>=<=]+))"); // Allows anything before and after the comparator
        std::smatch match;
        if (!std::regex_match(filterDefinition, match, re)) {
            throw std::runtime_error("Invalid filter: '" + filterDefinition + "'");
        }
        std::string headerColumnName = match[1];
        std::string comparator = match[2];
        std::string value = match[3];
        
        // Finding the index of the headerColumnName in the headerColumns and storing the filter in the filters vector
        auto it = std::find(headerColumns.begin(), headerColumns.end(), headerColumnName);
        if(it != headerColumns.end()){
            int columnIndex = std::distance(headerColumns.begin(), it);
            filters.push_back({columnIndex, comparator, value});
        } else {
            throw std::runtime_error("Header '"+headerColumnName+"' not found in CSV file/string");
        } 
    }

    return filters;
}

// Check if the row satisfies the filters. 
// Receives a row and a vector of filters, so it iterates over the filters and checks if the row satisfies them
bool satisfiesFilters(const std::vector<std::string>& row, const std::vector<Filter>& filters) {
    // Map to store whether a header satisfies any of its filters
    std::unordered_map<int, bool> headerSatisfied;

    // Iterating over the filters and checking if the row satisfies them
    for (const auto& filter : filters) {
        int columnIndex = filter.columnIndex;
        std::string comparator = filter.comparator;
        std::string value = filter.value;

        // Obtaining the field of the row based on the columnIndex
        const std::string& field = row[columnIndex];

        // Lexicographical comparison using std::strcmp
        int comparison = std::strcmp(field.c_str(), value.c_str());

        bool satisfied = false;
        
        // Checking if the comparison satisfies the filter
        if (comparator == ">") {
            satisfied = (comparison > 0);
        } else if (comparator == "<") {
            satisfied = (comparison < 0);
        } else if (comparator == "=") {
            satisfied = (comparison == 0);
        } else if(comparator == "!="){
            satisfied = (comparison != 0);
        } else if(comparator == ">="){
            satisfied = (comparison >= 0);
        } else if(comparator == "<="){
            satisfied = (comparison <= 0);
        } else { // Checking again if the comparator is valid
            throw std::runtime_error("Invalid filter: '" + field + comparator + value + "'");
        }

        // If the header satisfies the filter, we store as true in the headerSatisfied map
        if(satisfied){
            headerSatisfied[columnIndex] = true;
        }
    }

    // Checking if all headers satisfy the filters
    for (const auto& filter : filters) {
        int columnIndex = filter.columnIndex;
        // If any header doesn't satisfy the filter, we return false
        if(!headerSatisfied[columnIndex]){
            return false;
        }
    }

    return true;
}

void processCsv(const char csv[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    // Taking the first line of the csvData (headers columns line)
    std::istringstream streamCsv(csv);
    std::string headerColumnsLine;
    std::getline(streamCsv, headerColumnsLine);

    // Spliting the headerColumnsLine by commas into a vector of strings
    std::istringstream headerColumnsStream(headerColumnsLine);
    std::vector<std::string> headerColumns;
    std::string column;
    while (std::getline(headerColumnsStream, column, ',')) {
        headerColumns.push_back(column);
    }

    // We'll store the header columns name and its index just if it's in the selectedColumns
    // If selectedColumns is empty, we'll store all headers
    std::vector<HeaderColumn> headerColumnsToSelect; // Array to store the header columns name and its index
    if (selectedColumns[0] == '\0') { // selectedColumns is empty
        for(int i = 0; i < headerColumns.size(); i++){
            headerColumnsToSelect.push_back({headerColumns[i], i});
        }
    } else {
        // Creating an unordered_map to store the header name and its index.
        // It will be used to find the index of the selectedColumns with complexity O(1)
        std::unordered_map<std::string, int> headerColumnIndexMap;
        for (int i = 0; i < headerColumns.size(); ++i) {
            headerColumnIndexMap[headerColumns[i]] = i;
        }

        std::istringstream selectedColumnsStream(selectedColumns);
        std::string column;
        // We'll iterate over the selectedColumns with complexity O(n) where n is the number of selectedColumns
        // The complexity time of this block is O(n) * O(1) = O(n)
        while(std::getline(selectedColumnsStream, column, ',')){
            // Checking if the column is in the headerColumnIndexMap
            auto it = headerColumnIndexMap.find(column); // find in an unordered_map has complexity O(1)
            if (it != headerColumnIndexMap.end()) {
                headerColumnsToSelect.push_back({it->first, it->second});
            } else {
                std::cerr << "Header '" << column << "' not found in CSV file/string" << std::endl;
                return;
            }
        }
    }

    // Before any processing, we need to sort the headerColumnsToSelect by the index
    // Therefore, we'll have the selected columns in the correct order
    // The std::sort has complexity O(n log n) where n is the number of selected columns
    std::sort(headerColumnsToSelect.begin(), headerColumnsToSelect.end(), [](const HeaderColumn& a, const HeaderColumn& b) {
        return a.index < b.index;
    }); 

    // Storing the selected columns in a buffer output to print them at the end
    std::stringstream bufferHeaderColumnsToSelect;
    for (int i = 0; i < headerColumnsToSelect.size(); ++i) {
        bufferHeaderColumnsToSelect << headerColumnsToSelect[i].name;
        if (i < headerColumnsToSelect.size() - 1) bufferHeaderColumnsToSelect << ",";
    }
    bufferHeaderColumnsToSelect << "\n";

    // Preprocessing the rowFilterDefinitions based on all columns. 
    // It's throw a error if a filter has a non-existent column or there's an invalid filter
    std::vector<Filter> filters;
    try {
        filters = preprocessFilters(headerColumns, rowFilterDefinitions);
    } catch(const std::runtime_error& e){
        std::cerr << e.what() << std::endl;
        return;
    }

    // Processing the rows based on the selected columns and the filters
    std::string line;
    std::stringstream bufferDataOutput; // Buffer to store the data output
    while (std::getline(streamCsv, line)) {
        std::istringstream lineStream(line);
        std::string field;
        std::vector<std::string> row;

        while (std::getline(lineStream, field, ',')) {
            row.push_back(field);
        }
        
        // Checking if the row satisfies the filters
        try {
            if(satisfiesFilters(row, filters)) {
            // Storing valid lines in an output buffer to print them at the end
            for (int i = 0; i < headerColumnsToSelect.size(); ++i) {
                bufferDataOutput << row[headerColumnsToSelect[i].index];
                if (i < headerColumnsToSelect.size() - 1) bufferDataOutput << ",";
            }
                bufferDataOutput << "\n";
            }
        } catch(const std::runtime_error& e) {
            std::cerr << e.what() << '\n';
            return;
        }
    }

    // Showing all buffers content in the console
    std::cout << bufferHeaderColumnsToSelect.str() << bufferDataOutput.str();
    
}

void processCsvFile(const char csvFilePath[], const char selectedColumns[], const char rowFilterDefinitions[]) {
    // Reading the CSV file
    std::ifstream file(csvFilePath);
    if (!file.is_open()) {
        std::cerr << "Error opening CSV file" << std::endl;
        return;
    }

    // We'll store the csv data in a string
    std::string csvString;
    std::getline(file, csvString); // Getting the first line

    // Adding the first newline in the string
    csvString += "\n";

    // Concatenating with the others lines
    std::string line;
    while (std::getline(file, line)) {
        csvString += line;
        csvString += "\n";
    }

    // Converting the std::string to const char*
    const char* csv = csvString.c_str();

    // Calling the processCsv function with the new csv
    processCsv(csv,selectedColumns, rowFilterDefinitions);

    file.close();
}
