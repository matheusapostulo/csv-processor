#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "../includes/csv-processor.hpp"
#include <iostream>
#include <sstream>

TEST_CASE("processCsv should return just the selectedColumns", "[test-1]" ) {
    // Storing the cout buffer
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Tests variables
    const char csv[] = "header1,header2,header3,header4\n1,2,3,4\n5,6,7,8\n9,10,11,12";
    const char selectedColumns[] = "header1,header3,header4";
    const char rowFilterDefinitions[] = "header1>1\nheader3<10";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    // Restoring the cout buffer
    std::cout.rdbuf(oldCout);

    // Checking if the output is correct
    REQUIRE(buffer.str() == "header1,header3,header4\n");
}

TEST_CASE("processCsvFile should return the specified result", "[test-2]") {
    // Storing the cout buffer
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Tests variables
    const char csvFilePath[] = "../data.csv";
    const char selectedColumns[] = "col1,col3,col4,col7";
    const char rowFilterDefinitions[] = "col1>l1c1\ncol3>l1c3";

    // Calling the shared object function
    processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

    // Restoring the cout buffer
    std::cout.rdbuf(oldCout);

    // Checking if the output is correct
    REQUIRE(buffer.str() == "col1,col3,col4,col7\nl2c1,l2c3,l2c4,l2c7\nl3c1,l3c3,l3c4,l3c7\n");
}

TEST_CASE("processCsv should allow columns to use quote marks as part of their name", "[test-3]" ) {
    // Storing the cout buffer
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Tests variables
    const char csv[] = "hea\"der1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "hea\"der1,header3";
    const char rowFilterDefinitions[] = "hea\"der1>1\nheader3<8";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    // Restoring the cout buffer
    std::cout.rdbuf(oldCout);

    // Checking if the output is correct
    REQUIRE(buffer.str() == "hea\"der1,header3\n4,6\n");
}

TEST_CASE("processCsv has to return all columns if selectedColumns is empty", "[test-4]" ) {
    // Storing the cout buffer
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "";
    const char rowFilterDefinitions[] = "header1>1\nheader3<8";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    // Restoring the cout buffer
    std::cout.rdbuf(oldCout);

    // Checking if the output is correct
    REQUIRE(buffer.str() == "header1,header2,header3\n4,5,6\n");
}


TEST_CASE("processCsv should apply a header filter even if it is not present in selectedColumns", "[test-5]" ) {
    // Storing the cout buffer
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header3";
    const char rowFilterDefinitions[] = "header1=4\nheader2>3";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    // Restoring the cout buffer
    std::cout.rdbuf(oldCout);

    // Checking if the output is correct
    REQUIRE(buffer.str() == "header1,header3\n4,6\n");
}

TEST_CASE("processCsv should compare the values of the columns by lexicographic order", "[test-6]" ) {
    // Storing the cout buffer
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Tests variables
    const char csv[] = "col1,col2,col3\na,b,2\nb,1,4\nc,2,6";
    const char selectedColumns[] = "col1,col3";
    const char rowFilterDefinitions[] = "col1>a\ncol3<a";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    // Restoring the cout buffer
    std::cout.rdbuf(oldCout);

    // Checking if the output is correct
    REQUIRE(buffer.str() == "col1,col3\nb,4\nc,6\n");
}

TEST_CASE("processCsv should allow the columns of the selected column string to be in arbitrary order", "[test-7]" ) {
    // Storing the cout buffer
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header3,header1";
    const char rowFilterDefinitions[] = "header1=4\nheader2>3";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    // Restoring the cout buffer
    std::cout.rdbuf(oldCout);

    // Checking if the output is correct
    REQUIRE(buffer.str() == "header1,header3\n4,6\n");
}

TEST_CASE("processCsv should allow the rowFilterDefinitions filters to be in arbitrary order", "[test-8]" ) {
    // Storing the cout buffer
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header3,header1,header2";
    const char rowFilterDefinitions[] = "header2>3\nheader1=4";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions);

    // Restoring the cout buffer
    std::cout.rdbuf(oldCout);

    // Checking if the output is correct
    REQUIRE(buffer.str() == "header1,header2,header3\n4,5,6\n");
}

TEST_CASE("processCsv shouldn't allow non-existent columns in selectedColumns", "[test-9]" ) {
    // Redirect cerr buffer
    std::stringstream errStream;
    std::streambuf* oldCerr = std::cerr.rdbuf(errStream.rdbuf());

    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header2,header4";
    const char rowFilterDefinitions[] = "header2>3\nheader1=4";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions); 

    // Restore cerr
    std::cerr.rdbuf(oldCerr);

    // Checking if the output is correct
    REQUIRE(errStream.str() == "Header 'header4' not found in CSV file/string\n");
}

TEST_CASE("processCsv shouldn't allow non-existent columns in rowFilterDefinitions", "[test-10]" ) {
    // Redirect cerr buffer
    std::stringstream errStream;
    std::streambuf* oldCerr = std::cerr.rdbuf(errStream.rdbuf());
    
    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header3";
    const char rowFilterDefinitions[] = "header2>3\nheader5>4";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions); 

    // Restore cerr
    std::cerr.rdbuf(oldCerr);

    // Checking if the output is correct
    REQUIRE(errStream.str() == "Header 'header5' not found in CSV file/string\n");
}

TEST_CASE("processCsv shouldn't allow invalid filters", "[test-11]" ) {
    // Redirect cerr buffer
    std::stringstream errStream;
    std::streambuf* oldCerr = std::cerr.rdbuf(errStream.rdbuf());
    
    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header3";
    const char rowFilterDefinitions[] = "header1#2\nheader3<8";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions); 

    // Restore cerr
    std::cerr.rdbuf(oldCerr);

    // Checking if the output is correct
    REQUIRE(errStream.str() == "Invalid filter: 'header1#2'\n");
}

TEST_CASE("processCsv shouldn't allow empty filters", "[test-12]" ) {
    // Redirect cerr buffer
    std::stringstream errStream;
    std::streambuf* oldCerr = std::cerr.rdbuf(errStream.rdbuf());
    
    // Tests variables
    const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
    const char selectedColumns[] = "header1,header3";
    const char rowFilterDefinitions[] = "";

    // Calling the shared object function
    processCsv(csv, selectedColumns, rowFilterDefinitions); 

    // Restore cerr
    std::cerr.rdbuf(oldCerr);

    // Checking if the output is correct
    REQUIRE(errStream.str() == "Invalid filter: There is no filter, rowFilterDefinitions is empty\n");
}

TEST_CASE("processCsv should accept !=, >= and <= and as valid operators in rowFilterDefinitions", "[test-13]" ) {
    SECTION("Testing the != operator"){
        // Storing the cout buffer
        std::stringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        // Tests variables
        const char csvFilePath[] = "../data.csv";
        const char selectedColumns[] = "col3,col1";
        const char rowFilterDefinitions[] = "col1!=l1c1\ncol3!=l2c3";

        // Calling the shared object function
        processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

        // Restoring the cout buffer
        std::cout.rdbuf(oldCout);

        // Checking if the output is correct
        REQUIRE(buffer.str() == "col1,col3\nl3c1,l3c3\n");
    }

    SECTION("Testing the >= operator"){
        std::stringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        // Tests variables
        const char csvFilePath[] = "../data.csv";
        const char selectedColumns[] = "col1,col3";
        const char rowFilterDefinitions[] = "col1>=l1c1\ncol3>=l1c3";

        // Calling the shared object function
        processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

        // Restoring the cout buffer
        std::cout.rdbuf(oldCout);

        // Checking if the output is correct
        REQUIRE(buffer.str() == "col1,col3\nl1c1,l1c3\nl1c1,l1c3\nl2c1,l2c3\nl3c1,l3c3\n");
    }

    SECTION("Testing the <= operator"){
        // Storing the cout buffer
        std::stringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        // Tests variables
        const char csvFilePath[] = "../data.csv";
        const char selectedColumns[] = "col1,col3";
        const char rowFilterDefinitions[] = "col1<=l2c1\ncol3<=l3c3";

        // Calling the shared object function
        processCsvFile(csvFilePath, selectedColumns, rowFilterDefinitions);

        // Restoring the cout buffer
        std::cout.rdbuf(oldCout);

        // Checking if the output is correct
        REQUIRE(buffer.str() == "col1,col3\nl1c1,l1c3\nl1c1,l1c3\nl2c1,l2c3\n");
    }

    SECTION("Testing all new operators"){
        // Storing the cout buffer
        std::stringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        // Tests variables
        const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
        const char selectedColumns[] = "header1,header2,header3";
        const char rowFilterDefinitions[] = "header1!=2\nheader2>=5\nheader3<=6";

        // Calling the shared object function
        processCsv(csv, selectedColumns, rowFilterDefinitions);

        // Restoring the cout buffer
        std::cout.rdbuf(oldCout);

        // Checking if the output is correct
        REQUIRE(buffer.str() == "header1,header2,header3\n4,5,6\n");
    }
}

TEST_CASE("processCsv should allow more than one filter for the same column", "[test-14]" ) {
    SECTION("Readme Test case"){
        // Storing the cout buffer
        std::stringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        // Tests variables
        const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
        const char selectedColumns[] = "header1,header2,header3";
        const char rowFilterDefinitions[] = "header1=1\nheader1=4\nheader2>3\nheader3>4";

        // Calling the shared object function
        processCsv(csv, selectedColumns, rowFilterDefinitions);

        // Restoring the cout buffer
        std::cout.rdbuf(oldCout);

        // Checking if the output is correct
        REQUIRE(buffer.str() == "header1,header2,header3\n4,5,6\n");
    }

    SECTION("Additional test case "){
        // Storing the cout buffer
        std::stringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        // Tests variables
        const char csv[] = "header1,header2,header3\n1,2,3\n4,5,6\n7,8,9";
        const char selectedColumns[] = "header1,header2,header3";
        const char rowFilterDefinitions[] = "header1=4\nheader1=7\nheader2>3\nheader3>4";

        // Calling the shared object function
        processCsv(csv, selectedColumns, rowFilterDefinitions);

        // Restoring the cout buffer
        std::cout.rdbuf(oldCout);

        // Checking if the output is correct
        REQUIRE(buffer.str() == "header1,header2,header3\n4,5,6\n7,8,9\n");
    } 
}


