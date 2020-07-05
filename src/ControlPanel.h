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

  public:
  ControlPanel(sf::RenderWindow& window, neutosc::OscPars& op) {
    Slider th12slider(op.th12, "theta_12.png");
    Slider th23slider(op.th23, "theta_23.png");
    Slider th13slider(op.th13, "theta_13.png");
    Slider Dm21sqslider(op.Dm21sq, "Delta_m_21^2.png");
    Slider Dm31sqslider(op.Dm31sq, "Delta_m_31^2.png");
    // Slider Dm32sqslider(op.Dm32sq);
    Slider dCPslider(op.dCP, "delta_CP.png");

    th12slider.setLimits(0, PI/2);
    th23slider.setLimits(0, PI/2);
    th13slider.setLimits(0, PI/2);
    Dm21sqslider.setLimits(0, 2.e-4);
    Dm31sqslider.setLimits(0, 5.e-3);
    // Dm32sqslider.setLimits(0, 5.e-3);
    dCPslider.setLimits(0, 2*PI);

    th12slider.setSnap(op.th12);
    th23slider.setSnap(op.th23);
    th13slider.setSnap(op.th13);
    Dm21sqslider.setSnap(op.Dm21sq);
    Dm31sqslider.setSnap(op.Dm31sq);
    // Dm32sqslider.setSnap(op.Dm32sq);
    dCPslider.setSnap(op.dCP);
    dCPslider.setSnap(PI);
    
    sliders.push_back(th12slider);
    sliders.push_back(th23slider);
    sliders.push_back(th13slider);
    sliders.push_back(Dm21sqslider);
    sliders.push_back(Dm31sqslider);
    // sliders.push_back(Dm32sqslider);
    sliders.push_back(dCPslider);

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
    for(Slider& slider : sliders) {
      slider.setActive(mouse_pos);
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
}; // class ControlPanel