#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <codecvt>
#include <cstdlib>
#include <regex>

int main() {
    //this program tags dialogue with text tags for translation support and creates a translation csv file and populates it with tags and empty strings, where translations need to be
    //written manually.

    //this section is needed to set the locale to utf-8 so that characters from languages like cryllic are encoded properly
    const std::locale empty_locale = std::locale::empty();
    typedef std::codecvt_utf8<wchar_t> converter_type;
    const converter_type* converter = new converter_type;
    const std::locale utf8_locale = std::locale(empty_locale, converter);

    std::string sourceFileName, translationFileName, updatedSourceFileName;
    std::cout << "Enter the location of the source file (name of story script text file followed by .txt): ";
    std::cin >> sourceFileName;
    std::cout << "Enter the location of the translation (.csv) file to create: ";
    std::cin >> translationFileName;
    std::cout << "Enter the location of the updated source file with tags (.txt) to create: ";
    std::cin >> updatedSourceFileName;
    
    //I'm not sure if wide intput and wide output fstreams and wide strings are necessary to keep the utf-8 encoding, but the .imbue(utf8_locale) line is necessary.
    std::wifstream sourceFile(sourceFileName);
    //Turn the text buffer stream into a utf-8 encoded string.
    sourceFile.imbue(utf8_locale);
    if (!sourceFile.is_open()) 
    {
        std::cerr << "Error: Source file not found or could not be opened." << std::endl;
        /*FILE* outFile = fopen(sourceFileName, "w+,ccs=UTF-8");*/
        return 1;
    }

    std::wofstream translationFile(translationFileName);
    translationFile.imbue(utf8_locale);
    if (!translationFile.is_open()) 
    {
        std::cerr << "Error: translation file could not be created or could not be opened." << std::endl;
        return 1;
    }

    std::wofstream updatedSourceFile(updatedSourceFileName);
    updatedSourceFile.imbue(utf8_locale);
    if (!updatedSourceFile.is_open()) 
    {
        std::cerr << "Error: updated source file could not created or could not be opened." << std::endl;
        return 1;
    }

    int textCount = 1;
    std::wstring sourceLine;
    const std::wstring CSV_QUOTATIONS = L"\"\"\"";
    const std::size_t TEXT_START_POS = 0;
    while (std::getline(sourceFile, sourceLine)) {
        std::wstring updatedLine = sourceLine;
        //if the start of a translatable dialogue text is found,add the necessary text tag at the beginning of the line in case there is a character name before the quotation mark.
        std::size_t dialogueStartPos = sourceLine.find(L"\"");
        if (dialogueStartPos != std::wstring::npos)
        {

            std::wstring tagText = L"text";
            tagText.append(std::to_wstring(textCount));
            //updated script file line contents
            std::wstring updatedLineTagText = tagText;
            updatedLineTagText.append(L":");
            //check whether there are tabs or single spaces before the start of a line with dialogue and insert the tag after the tab or spaces
            std::size_t updatedLineStartPos = sourceLine.find_first_not_of(L' ');
            std::size_t firstTabPos = sourceLine.find_first_not_of(L'\t');
            if (firstTabPos > updatedLineStartPos) 
            {
                updatedLineStartPos = firstTabPos;
            }
            updatedLine.insert(updatedLineStartPos, updatedLineTagText);
            updatedLine.shrink_to_fit();
            //create translation file line contents and write to file
            std::size_t dialogueLength = sourceLine.length() - dialogueStartPos;
            std::wstring translationTextLine = tagText;
            translationTextLine.append(L";");
            translationTextLine.append(CSV_QUOTATIONS);
            translationTextLine.append(sourceLine.substr(dialogueStartPos, dialogueLength));
            translationTextLine.append(CSV_QUOTATIONS);
            translationTextLine.append(L"\n");
            translationFile << translationTextLine;
            textCount++;

        }
        updatedSourceFile << updatedLine << std::endl;
    }


    sourceFile.close();
    translationFile.close();
    updatedSourceFile.close();

	return 0;
}