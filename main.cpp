#include <iostream>
#include <fstream>
#include "Color.h"

using namespace std;

void DisplayHelp(){
    cout << "Invalid Arguments! \n";
    cout << "Usage: \'APConvert [Input File] [Output File]\'";
}


int main(int argc, char *argv[]) {
    if(argc < 2){
        DisplayHelp();
        return 1;
    }

    streampos size;
    char* memblock;
    ifstream inputFile (argv[1], ios::in|ios::binary|ios::ate);

    if(!inputFile.is_open()){
        //file doesn't exist or cant open file
        cout << "Error reading file: " << argv[1] << "\n Does it exist?";
        return 1;
    }

    size = inputFile.tellg();
    memblock = new char [size];
    inputFile.seekg(0, ios::beg);
    inputFile.read(memblock, size);
    inputFile.close();

    cout << "Reading Austin Paint 2 file: " << argv[1] << ": \n";
    cout << "Input File Size: " << size << "\n";

    /**Read palette information**/

    cout << "Reading palette information: \n";
    Color *palette[16];
    for(int i = 0; i < (16 * 3); i+=3){
        palette[i / 3] = new Color(((int)memblock[i + 16] & 0xFF),((int)memblock[i + 16 + 1] & 0xFF),((int)memblock[i + 16 + 2] & 0xFF));
    }

    /**Read pixel array
       This was dumb in A.P.E. and its dumb here.**/

    cout << "Reading pixel array:\n";
    int pixelArray[32][32];
    for(int y = 0; y < 32; y++){
        for(int x = 0; x < 16; x++){
            pixelArray[x * 2][y] = (memblock[(y * 16) + x + (16 * 4)] & 0xFF) >> 4 & 0x0F; //Top 4 bits
            pixelArray[(x * 2) + 1][y] = (memblock[(y * 16) + x + (16 * 4)] & 0xFF) & 0x0F; //Bottom 4 bits
        }
    }
    delete[] memblock;

    cout << "Done! \n\n";
    cout << "Writing Bitmap file: " << argv[2] << ": \n";

    FILE *fileToWrite = fopen(argv[2], "w"); //open file in override mode
    fputs("", fileToWrite); //Create new file & Override old

    ofstream file;
    file.open(argv[2], ios::binary);
    if(!file.is_open()){
        cout << "Error opening file: " << argv[2] << " is it write protected?";
        return 0;
    }

    cout << "Writing header: \n";

    /**yes, i manually programmed this in. deal with it**/

    file.write("BM", 2);
    unsigned char headerInfo[52] = {
            0x76, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x76, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
            0x20, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00,
            0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0xC4, 0x0E, 0x00, 0x00,
            0xC4, 0x0E, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00,
            0x10, 0x00, 0x00, 0x00};
    file.write((const char *)(headerInfo), 52);

    /**BMP uses Blue, Green, Red. BMP is dumb**/

    cout << "Writing palette information \n";
    for(int i = 0; i < 16; i++){
        char test = 0xFF;
        unsigned char paletteInformation[4] = {(unsigned char)palette[i]->b, (unsigned char)palette[i]->g, (unsigned char)palette[i]->r, 0xFF};
        file.write((const char *)paletteInformation, 4);
    }
    file.close();

    /**yes I know its stupid to use two different libraries to write to the same file,
     but for some stupid reason casting the pixelarray data to const char* causes a segmentation fault so i give up**/

    fileToWrite = fopen(argv[2], "a");

    /**BMP is sideways just like AP files, Austin was right on accident**/
    cout << "Writing pixel array \n";
    for(int y = 31; y >= 0; y--){
        for(int x = 0; x < 16; x++){
            fputc((pixelArray[(x * 2)][y] << 4 & 0xFF) + (pixelArray[(x * 2) + 1][y] & 0xFF), fileToWrite);
        }
    }
    cout << "Done!";

    return 0;
}
