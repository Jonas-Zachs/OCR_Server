# OCR_Server

A lightweight OCR (Optical Character Recognition) HTTP server built with C++, using OpenCV, Tesseract, and cpp-httplib.

The server receives images via HTTP, processes them in memory, and returns extracted text.

---

## Features

- Accepts image uploads via HTTP POST
- In-memory image decoding (no disk usage)
- OCR using Tesseract
- Supports multiple languages (deu, eng)
- Fast and lightweight C++ implementation

---

## GUI Usage (Client)

This server is designed to work together with a Qt-based GUI client (Repository: OCR_Client_GUI).

Workflow:

1. Start the OCR server locally

2. Launch the GUI client application

3. In the GUI:
   - Click Open to select an image file
   - Choose the OCR language (German / English)
   - Click Extract Text
   - The image is automatically sent to the server

4. The extracted text is displayed directly in the GUI

Behind the scenes:

- The GUI sends the image as raw bytes via HTTP POST to:
  http://127.0.0.1:8080/upload

- The selected language is sent via the X-Language header
- The server returns the recognized text as plain text

---

## Response

Success (200):
Returns extracted text as plain text.

Error responses:
- 400 Empty or invalid image
- 500 Server or OCR failure

---

## Requirements

- C++17 or higher
- OpenCV
- Tesseract OCR
- cpp-httplib
- (Dependencies should be installed automatically via vcpkg when building the application for the first time.)
