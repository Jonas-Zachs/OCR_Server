#include <iostream>
#include <memory>
#include <vector>

#include "httplib.h"
#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>

static std::string process_image(const cv::Mat& img, const std::string& language) {
    // Reject empty input.
    if (img.empty()) {
        return "Error: Image is empty.\n";
    }

    // Convert to grayscale to simplify OCR input.
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    // Create and initialize a Tesseract instance for the requested language.
    std::unique_ptr<tesseract::TessBaseAPI> api(new tesseract::TessBaseAPI());
    if (api->Init(nullptr, language.c_str())) {
        return "Error: Tesseract initialization failed (language=" + language + ")\n";
    }

    // Let Tesseract decide the page layout automatically.
    api->SetPageSegMode(tesseract::PSM_AUTO);

    // Pass the grayscale image directly to Tesseract.
    // Format: data pointer, width, height, bytes per pixel, bytes per line.
    api->SetImage(gray.data, gray.cols, gray.rows, 1, static_cast<int>(gray.step));

    // Run OCR and fetch the resulting UTF-8 text.
    char* outText = api->GetUTF8Text();
    if (outText) {
        std::string text = outText;

        // Tesseract allocates the returned buffer, so it must be released manually.
        delete[] outText;
        api->End();

        return text;
    }

    api->End();
    return "Error: Tesseract returned no text.\n";
}

int main() {
    httplib::Server svr;

    // Accept raw image bytes via POST /upload.
    // The request body must contain a valid image file format, e.g. PNG or JPEG.
    svr.Post("/upload", [](const httplib::Request& req, httplib::Response& res) {
        if (req.body.empty()) {
            res.status = 400;
            res.set_content("Error: Empty request body.\n", "text/plain");
            return;
        }

        // Convert the raw request body into a byte buffer for OpenCV.
        std::vector<uchar> data(req.body.begin(), req.body.end());

        // Decode the image directly from memory instead of writing it to disk first.
        cv::Mat img = cv::imdecode(data, cv::IMREAD_COLOR);
        if (img.empty()) {
            res.status = 400;
            res.set_content("Error: Could not decode image.\n", "text/plain");
            return;
        }

        // Allow the OCR language to be overridden via the X-Language header.
        // Possible values: deu, eng
        std::string language = "deu";
        auto language_from_header = req.get_header_value("X-Language");
        if (!language_from_header.empty()) {
            language = language_from_header;
        }

        // Run OCR and return the extracted text.
        const std::string image_text = process_image(img, language);
        res.set_content(image_text + "\n", "text/plain");
        });

    std::cout << "Server running at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}