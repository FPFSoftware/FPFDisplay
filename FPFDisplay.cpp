#include "GUIDisplay.hh"
#include "TApplication.h"
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <gdmlfile> [rootfile]\n";
        return 1;
    }
    std::string gdmlFile = argv[1];
    std::string rootFile = (argc > 2) ? argv[2] : "";

    TApplication app("FPFDisplay", &argc, argv);

    GUIDisplay gui;
    try {
        gui.LoadGeometry(gdmlFile, false);
        gui.Initialize("FPF Event Display");
        if (!rootFile.empty()) {
            gui.LoadData(rootFile);
        }
        app.Run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
