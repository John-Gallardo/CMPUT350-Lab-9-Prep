#ifndef CAMERA_H
#define CAMERA_H

#include "vector_util.h"
#include <SFML/System.hpp>

// NOTE: same orientation for world and screen -> x & y axis goes the same way I assume
class Camera {
public:
    Camera(const sf::Vector2f& windowSize, const sf::Vector2f& viewCenterWorld,
           float worldUnitsPerPixel = 1.f)
        : mWindowSize{windowSize},
          mViewCenterWorld{viewCenterWorld},
          mWorldUnitsPerPixel{worldUnitsPerPixel} {}


    sf::Vector2f windowToWorld(const sf::Vector2f& pWindow) {
        // NOTE: formula is from worldToWindow, but rearranged for pWorld
        // I didn't test this but I think it makes sense to account for offsets
        return (pWindow - mWindowSize / 2.0f) * mWorldUnitsPerPixel + mViewCenterWorld;
    };

    sf::Vector2f worldToWindow(const sf::Vector2f& pWorld) {
        // subtract viewport center to remove offset, then divide by mWorldUnitsPerPixel to convert.
        // we add mWindowSize / 2.0f so that we are relative to the CENTER of the window, not (0, 0)
        // if that doesn't make sense, center of the window in windowPercent is (0, 0) without, but with the offset (0.5, 0.5) which makes sense
        return (pWorld - mViewCenterWorld) / mWorldUnitsPerPixel + mWindowSize / 2.0f;
    };

    sf::Vector2f windowPercentToWorld(const sf::Vector2f& pWindowPercent) {
        // convert window percent to window coordinates and then convert to world coordinates
        sf::Vector2f windowCoordinates{pWindowPercent * mWindowSize};
        return windowToWorld(windowCoordinates);
    };

    sf::Vector2f worldToWindowPercent(const sf::Vector2f& pWorld) {
        // convert to window coordinates then divide by mWindowSize for window percent
        sf::Vector2f windowCoordinates{worldToWindow(pWorld)};
        return windowCoordinates / mWindowSize;
    };

    void translateWorld(const sf::Vector2f& deltaWorld) {
        // we move mViewCenterWorld by delta world
        mViewCenterWorld += deltaWorld;
    };

    void translateWindow(const sf::Vector2f& deltaScreen) {
        // translate deltaScreen into world coordinates then move mViewCenterWorld by that
        sf::Vector2f deltaWorld{windowToWorld(deltaScreen)};
        mViewCenterWorld += deltaWorld;
    };

    void translateWindowPercent(const sf::Vector2f& deltaScreenPercent) {
        // convert to percentage to screen pixels and then move by that amount
        sf::Vector2f deltaScreen{deltaScreenPercent * mWindowSize};
        translateWindow(deltaScreen);
    };

    void setCamera(const sf::Vector2f& pWorld, const sf::Vector2f& pWindow) {
        // TODO: I am pretty sure this is wrong
        // 1. convert window to world coordinates
        sf::Vector2f worldCoordinates{windowToWorld(pWindow)};
        // 2. find translation: worldCoordinates - pWorld gives the vector from pWorld to worldCoordinates, our offset
        sf::Vector2f offset{worldCoordinates - pWorld};
        // 3. move camera to pWorld + this offset (ignore y)
        mViewCenterWorld.x = pWorld.x + offset.x;
    };

    void setCameraPercent(const sf::Vector2f& pWorld, const sf::Vector2f& pWindowPercent) {
        // same as above but convert pWindowPercent to screen pixels first
        sf::Vector2f pWindow{pWindowPercent * mWindowSize};
        setCamera(pWorld, pWindow);
    };

    void setCameraCenter(const sf::Vector2f& pWorld) {
        // set mViewCenterWorld to pWorld to set new center
        mViewCenterWorld = pWorld;
    };

    sf::Vector2f getCameraCenter() const {
        // return mViewCenterWorld
        return mViewCenterWorld;
    };

private:
    sf::Vector2f mWindowSize;       // size of the window in screen pixels (i.e. window coords)
    sf::Vector2f mViewCenterWorld;  // world coords point corresponding to the center of our window.
    float mWorldUnitsPerPixel;      // distance in world coords covered by one pixel
};

#endif  // CAMERA_H
