#ifndef CAMERA_H
#define CAMERA_H

#include "vector_util.h"
#include <SFML/System.hpp>

class Camera {
public:
    Camera(const sf::Vector2f& windowSize, const sf::Vector2f& viewCenterWorld,
           float worldUnitsPerPixel = 1.f)
        : mWindowSize{windowSize},
          mViewCenterWorld{viewCenterWorld},
          mWorldUnitsPerPixel{worldUnitsPerPixel} {}

    // TODO: all of these:

    sf::Vector2f windowToWorld(const sf::Vector2f& pWindow) { return {}; };

    sf::Vector2f worldToWindow(const sf::Vector2f& pWorld) { return {}; };

    sf::Vector2f windowPercentToWorld(const sf::Vector2f& pWindowPercent) { return {}; };

    sf::Vector2f worldToWindowPercent(const sf::Vector2f& pWorld) { return {}; };

    void translateWorld(const sf::Vector2f& deltaWorld) {};

    void translateWindow(const sf::Vector2f& deltaScreen) {};

    void translateWindowPercent(const sf::Vector2f& deltaScreenPercent) {};

    void setCamera(const sf::Vector2f& pWorld, const sf::Vector2f& pWindow) {};

    void setCameraPercent(const sf::Vector2f& pWorld, const sf::Vector2f& pWindowPercent) {};

    void setCameraCenter(const sf::Vector2f& pWorld) {};

    sf::Vector2f getCameraCenter() const { return {}; };

private:
    sf::Vector2f mWindowSize;       // size of the window in screen pixels (i.e. window coords)
    sf::Vector2f mViewCenterWorld;  // world coords point corresponding to the center of our window.
    float mWorldUnitsPerPixel;      // distance in world coords covered by one pixel
};

#endif  // CAMERA_H
