#include <sstream>
#include <tiffio.hxx>
int main() { 
    std::ostringstream tiff_ostream;
    TIFF *tiff = TIFFStreamOpen("memory", &tiff_ostream);
    return 0; 
}
