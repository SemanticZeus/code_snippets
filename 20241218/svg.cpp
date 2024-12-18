#include "ofMain.h"
#include "svgpp/svgpp.hpp"
#include <fstream>
#include <sstream>
#include <map>

class SvgHandler {
public:
    SvgHandler() = default;

    void onPathMoveTo(float x, float y) {
        path.moveTo(x, y);
    }

    void onPathLineTo(float x, float y) {
        path.lineTo(x, y);
    }

    void onPathClosePath() {
        path.close();
    }

    void setFillColor(float r, float g, float b) {
        path.setFillColor(ofColor(r * 255, g * 255, b * 255));
    }

    void setStrokeColor(float r, float g, float b) {
        path.setStrokeColor(ofColor(r * 255, g * 255, b * 255));
    }

    void setStrokeWidth(float width) {
        path.setStrokeWidth(width);
    }

    ofPath& getPath() {
        return path;
    }

private:
    ofPath path;
};

class ofApp : public ofBaseApp {
public:
    ofPath svgPath;

    void setup() {
        ofBackground(255);
        parseSVG("example.svg");
    }

    void draw() {
        svgPath.draw();
    }

private:
    void parseSVG(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            ofLogError() << "Failed to open SVG file: " << filePath;
            return;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();

        SvgHandler handler;
        try {
            svgpp::document_traversal<> traversal;
            traversal.load_document(buffer.str(), handler);
        } catch (const std::exception& e) {
            ofLogError() << "Error parsing SVG: " << e.what();
        }

        svgPath = handler.getPath();
    }
};

//========================================================================
int main() {
    ofSetupOpenGL(1024, 768, OF_WINDOW);
    ofRunApp(new ofApp());
}

