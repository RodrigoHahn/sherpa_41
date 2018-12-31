#ifndef RENDERER_CANVAS_CPP
#define RENDERER_CANVAS_CPP

#include "renderer/canvas.hpp"

/**
 * Creates blank canvas of a width and height
 * @param width canvas width
 * @param height canvas height
 */
Canvas::Canvas(uint64_t width, uint64_t height)
    : width(width),
      height(height),
      pixels(PxVector(width * height, CSS::ColorValue(255, 255, 255, 0))) {
}

/**
 * Creates a canvas from a root box and a specified frame width/height
 * @param root box to start drawing from
 * @param frame width and height to draw in
 */
Canvas::Canvas(const Layout::BoxPtr & root, const Layout::Rectangle & frame)
    : width(static_cast<uint64_t>(frame.width)),
      height(static_cast<uint64_t>(frame.height)),
      pixels(PxVector(width * height, CSS::ColorValue(255, 255, 255, 0))) {
    auto cmds = Display::Command::createQueue(root);
    while (!cmds.empty()) {
        cmds.front()->acceptRenderer(*this);
        cmds.pop();
    }
}

/**
 * Renders a Rectangle Command
 * @param cmd command to render
 */
void Canvas::render(const Display::RectangleCmd & cmd) {
    const auto rect  = cmd.getRectangle();
    const auto color = cmd.getColor();

    // set rectangle edges, bounded to canvas
    const auto x0 = toPx(rect.origin.x, 0, width);
    const auto y0 = toPx(rect.origin.y, 0, height);
    const auto x1 = toPx(rect.origin.x + rect.width, 0, width);
    const auto y1 = toPx(rect.origin.y + rect.height, 0, height);

    // color rectangle pixels accordingly
    // std::iota here would be needlessly comsumptive and overly cryptic
    for (uint64_t y = y0; y < y1; ++y) {
        for (uint64_t x = x0; x < x1; ++x) {
            pixels[x + y * width] = color;
        }
    }
}

/**
 * Returns vector of RGBA pixels representing the Canvas
 * @return pixels
 */
std::vector<uint8_t> Canvas::getPixels() const {
    std::vector<uint8_t> rawPixels;
    rawPixels.reserve(width * height * 4);
    std::for_each(pixels.begin(),
                  pixels.end(),
                  [&rawPixels](const auto & pixel) {
                      for (auto channel : pixel.channels()) {
                          rawPixels.push_back(channel);
                      }
                  });
    return rawPixels;
}

/**
 * Converts a start location to a pixel position on the canvas
 * @param x location to convert
 * @param min min bound
 * @param max max bound
 * @return converted location, bounded by canvas size
 */
uint64_t Canvas::toPx(double x, uint64_t min, uint64_t max) {
    return std::min(max, std::max(min, static_cast<uint64_t>(x)));
}

#endif