#ifndef EXCELCONVERTER_H
#define EXCELCONVERTER_H

#include <string>

class ExcelConverter {
public:
    // Convert XLSX → CSV using xlsxio
    void convert(const std::string& xlsxPath, const std::string& csvPath);
};

#endif
