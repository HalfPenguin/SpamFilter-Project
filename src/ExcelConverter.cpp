#include "ExcelConverter.h"
#include <xlsxio_read.h>
#include <fstream>
#include <iostream>

void ExcelConverter::convert(const std::string& xlsxPath, const std::string& csvPath) {
    xlsxioreader reader = xlsxioread_open(xlsxPath.c_str());
    if (!reader) {
        std::cerr << "Error: Cannot open XLSX file: " << xlsxPath << "\n";
        return;
    }

    // Open first sheet (NULL = first sheet)
    xlsxioreadersheet sheet = xlsxioread_sheet_open(reader, NULL, XLSXIOREAD_SKIP_EMPTY_ROWS);
    if (!sheet) {
        std::cerr << "Error: Could not open first sheet of file: " << xlsxPath << "\n";
        xlsxioread_close(reader);
        return;
    }

    std::ofstream out(csvPath);
    if (!out.is_open()) {
        std::cerr << "Error: Cannot open CSV for writing: " << csvPath << "\n";
        xlsxioread_sheet_close(sheet);
        xlsxioread_close(reader);
        return;
    }

    // Read each row
    while (xlsxioread_sheet_next_row(sheet)) {
        std::string row;
        bool firstCell = true;
        char* value;

        // Read each cell in the row
        while ((value = xlsxioread_sheet_next_cell(sheet)) != NULL) {
            if (!firstCell)
                row += ",";
            row += value;
            free(value);
            firstCell = false;
        }

        // Skip empty or whitespace-only rows
        if (row.find_first_not_of(" ,\t\r\n") == std::string::npos)
            continue;

        out << row << "\n";
    }

    xlsxioread_sheet_close(sheet);
    xlsxioread_close(reader);
}
