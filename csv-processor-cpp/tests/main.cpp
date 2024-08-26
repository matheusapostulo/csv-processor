#include "catch.hpp"
#include "../includes/csv-processor.hpp"
#include <iostream>
#include <sstream>

int main(){
    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header2,header3";
    const char rowFilterDefinitions[] = "header1=4\nheader1=7\nheader2>3\nheader3>4";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    return 0;
}
