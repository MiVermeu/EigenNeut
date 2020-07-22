#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <Eigen/Dense>

#include "NeutOsc.h"
#include "Slider.h"

#define PI 3.14159265358979323846

class ControlPanel {
  private:
  std::vector<Slider> sliders;

  // Animating variables.
  bool animating = false;
  int last_active = 3;

  // Window, position and size.
  sf::RenderWindow& window;
  sf::Vector2i pos;
  sf::Vector2u size;
  sf::Vector2u oldWindowSize;

  public:
  ControlPanel(sf::RenderWindow& window, neutosc::OscPars& op):
    window(window) {
    Slider th12slider(op.th12, "theta_12.png");
    Slider th23slider(op.th23, "theta_23.png");
    Slider th13slider(op.th13, "theta_13.png");
    Slider dCPslider(op.dCP, "delta_CP.png");
    Slider Dm21sqslider(op.Dm21sq, "Delta_m_21^2.png");
    Slider Dm31sqslider(op.Dm31sq, "Delta_m_31^2.png");
    Slider rhoslider(op.rho, "rho.png");

    th12slider.setLimits(0, PI);
    th23slider.setLimits(0, PI);
    th13slider.setLimits(0, PI);
    dCPslider.setLimits(0, 2*PI);
    Dm21sqslider.setLimits(0, 2.e-4);
    Dm31sqslider.setLimits(-5.e-3, 5.e-3);
    rhoslider.setLimits(0, 5000);

    th12slider.setSnap(op.th12);
    th12slider.setSnap(PI/2);
    th23slider.setSnap(op.th23);
    th23slider.setSnap(PI/2);
    th13slider.setSnap(op.th13);
    th13slider.setSnap(PI/2);
    dCPslider.setSnap(op.dCP);
    dCPslider.setSnap(PI);
    Dm21sqslider.setSnap(op.Dm21sq);
    Dm31sqslider.setSnap(op.Dm31sq);
    Dm31sqslider.setSnap(0);
    Dm31sqslider.setSnap(-op.Dm31sq);
    rhoslider.setSnap(2700);

    rhoslider.setLoop(false);
    
    sliders.push_back(th12slider);
    sliders.push_back(th23slider);
    sliders.push_back(th13slider);
    sliders.push_back(dCPslider);
    sliders.push_back(Dm21sqslider);
    sliders.push_back(Dm31sqslider);
    sliders.push_back(rhoslider);

    updateWindow();
  }

  void draw() {
    for(Slider& slider : sliders) {
      slider.draw(window);
    }
  }

  void updateWindow() {
    // Scale by comparing old and new window size.
    const sf::Vector2f scale((float)window.getSize().x/oldWindowSize.x,
                            (float)window.getSize().y/oldWindowSize.y);
    size.x *= scale.x; size.y *= scale.y;
    pos.x *= scale.x; pos.y *= scale.y;
    oldWindowSize = window.getSize();

    for(int si = 0; si < sliders.size(); ++si) {
      sliders[si].setSize(size.x * 0.8, size.y / sliders.size());
      sliders[si].setPosition(pos.x + size.x/2 * 1.1, pos.y + (1+si)* size.y / (sliders.size()+1));
    }
  }

  void setPosition(const int x, const int y) {
    pos.x = x; pos.y = y;
    updateWindow();
  }
  void setSize(const unsigned w, const unsigned h) {
    size.x = w; size.y = h;
    updateWindow();
  }

  void setActive(Eigen::Vector2d mouse_pos) {
    for(int si = 0; si < sliders.size(); ++si) {
      if(sliders[si].setActive(mouse_pos)) {
        // Only switch animating slider if not animating already.
        if(!animating) last_active = si;
        break;
      }
    }
  }

  void unsetActive() {
    for(Slider& slider : sliders) {
      slider.unsetActive();
    }
  }

  bool drag(Eigen::Vector2d mouse_pos) {
    bool isdragging = false;
    for(Slider& slider : sliders) {
      isdragging |= slider.drag(mouse_pos);
    }
    return isdragging;
  }

  void edit(const sf::Keyboard::Key& key) {
    sliders[last_active].edit(key);
  }

  void update() {
    for(Slider& slider : sliders) {
      slider.update();
    }
  }

  void animate() {
    sliders[last_active].animate();
    animating = true;
  }
  void stopAnimate() {
    animating = false;
  }
}; // class ControlPanel