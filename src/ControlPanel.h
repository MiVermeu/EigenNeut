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
  double width, height;

  // Animating variables.
  bool animating = false;
  int last_active = 3;

  public:
  ControlPanel(sf::RenderWindow& window, neutosc::OscPars& op) {
    Slider th12slider(op.th12, "theta_12.png");
    Slider th23slider(op.th23, "theta_23.png");
    Slider th13slider(op.th13, "theta_13.png");
    Slider dCPslider(op.dCP, "delta_CP.png");
    Slider Dm21sqslider(op.Dm21sq, "Delta_m_21^2.png");
    Slider Dm31sqslider(op.Dm31sq, "Delta_m_31^2.png");
    // Slider Dm32sqslider(op.Dm32sq);

    th12slider.setLimits(0, PI);
    th23slider.setLimits(0, PI);
    th13slider.setLimits(0, PI);
    dCPslider.setLimits(0, 2*PI);
    Dm21sqslider.setLimits(0, 2.e-4);
    Dm31sqslider.setLimits(0, 5.e-3);
    // Dm32sqslider.setLimits(0, 5.e-3);

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
    // Dm32sqslider.setSnap(op.Dm32sq);
    
    sliders.push_back(th12slider);
    sliders.push_back(th23slider);
    sliders.push_back(th13slider);
    sliders.push_back(dCPslider);
    sliders.push_back(Dm21sqslider);
    sliders.push_back(Dm31sqslider);
    // sliders.push_back(Dm32sqslider);

    for(int si = 0; si < sliders.size(); ++si) {
      sliders[si].setSize(500, 20);
      sliders[si].setPosition(window.getSize().x*0.22, window.getSize().y*0.05*(1+si));
    }
  }

  void draw(sf::RenderWindow& window) {
    for(Slider& slider : sliders) {
      slider.draw(window);
    }
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