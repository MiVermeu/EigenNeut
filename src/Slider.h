#pragma once

#include <vector>

class Slider {
  private:
  // Slider attributes.
  double min = 0;
  double max = 0;
  double& val;
  Eigen::Vector2d pos;
  double width = 20;
  double height = 0;
  bool active = false;

  // Drawn objects.
  sf::CircleShape slidercirc;
  sf::RectangleShape sliderline;

  public:
  Slider(double& variable): val(variable), pos(0,0), slidercirc(20,20), sliderline(sf::Vector2f(width,2)) {
    // Slider button.
    slidercirc.setFillColor(sf::Color::White);
    slidercirc.setOrigin(slidercirc.getRadius(), slidercirc.getRadius());
    // Slider line.
    sliderline.setOrigin(sliderline.getLocalBounds().width/2, sliderline.getLocalBounds().height/2);
  }

  void setLimits(double newmin, double newmax) {
    min = newmin;
    max = newmax;
    val = min;
  }

  void setPosition(double newx, double newy) {
    pos = Eigen::Vector2d(newx, newy);
    sliderline.setPosition(newx, newy);
    slidercirc.setPosition(newx - width/2, newy);
  }

  void setSize(double newwidth, double newheight) {
    width = newwidth;
    height = newheight;
    sliderline.setSize(sf::Vector2f(newwidth, 2));
    sliderline.setOrigin(sliderline.getLocalBounds().width/2, sliderline.getLocalBounds().height/2);

  }

  void draw(sf::RenderWindow& window) {
    window.draw(slidercirc);
    window.draw(sliderline);
  }

  bool overlap(Eigen::Vector2d pos) {
    const Eigen::Vector2d sliderpos(slidercirc.getPosition().x, slidercirc.getPosition().y);
    return (pos - sliderpos).norm() < 20;
  }

  bool setActive(Eigen::Vector2d mouse_pos) {
    if(overlap(mouse_pos)) active = true;
    return active;
  }

  void unsetActive() {
    active = false;
  }

  bool drag(Eigen::Vector2d mouse_pos) {
    if(!active) return false;
    const double minx = sliderline.getPosition().x - width/2;
    const double maxx = sliderline.getPosition().x + width/2;
    const double newx = std::max(std::min(maxx, mouse_pos.x()), minx);
    slidercirc.setPosition(newx, slidercirc.getPosition().y);
    val = (newx-minx)/(maxx-minx) * (max-min) + min;
    return true;
  }

  // void update() {
  //   const double minx = sliderline.getPosition().x - width/2;
  //   const double maxx = sliderline.getPosition().x + width/2;
  //   const double newx = (val-min)/(max-min) * (maxx-minx) + minx;
  //   slidercirc.setPosition(newx, slidercirc.getPosition().y);
  // }

  double getVal() const {
    return val;
  }
}; // class Slider