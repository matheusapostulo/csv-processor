#ifndef CSV_PROCESSOR_H
#define CSV_PROCESSOR_H

#ifdef __cplusplus
extern "C" {
#endif

void processCsv(const char* csv, const char* selectedColumns, const char* rowFilterDefinitions);
void processCsvFile(const char* csvFilePath, const char* selectedColumns, const char* rowFilterDefinitions);

#ifdef __cplusplus
}
#endif

#endif
