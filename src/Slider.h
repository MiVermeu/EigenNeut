#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <Eigen/Dense>

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
  std::vector<double> snapvals;

  // Drawn objects.
  sf::CircleShape slidercirc;
  sf::RectangleShape sliderline;
  std::vector<sf::RectangleShape> snaplines;
  // Textures and sprites for labels.
  sf::Texture labeltex;
  sf::Sprite labelsprite;
  // Text field.
  sf::Font font;
  sf::Text text;

  public:
  Slider(double& variable, std::string texname): val(variable), pos(-1, -1),
         slidercirc(20,20), sliderline(sf::Vector2f(width,2)) {
    // Slider button.
    slidercirc.setFillColor(sf::Color::White);
    slidercirc.setOrigin(slidercirc.getRadius(), slidercirc.getRadius());
    // Slider line.
    sliderline.setOrigin(sliderline.getGlobalBounds().width/2, sliderline.getGlobalBounds().height/2);

    // Label.
    labeltex.loadFromFile("../textures/"+texname);
    labelsprite.setTexture(labeltex);

    // Text field.
    if(!font.loadFromFile("../textures/Roboto-Regular.ttf")) {
      std::cout << "Couldn't load font.\n";
    }
    text.setString(std::to_string(val));
    text.setFillColor(sf::Color::White);
  }

  void setLimits(double newmin, double newmax) {
    min = newmin;
    max = newmax;
  }

  void setPosition(double newx, double newy) {
    pos = Eigen::Vector2d(newx, newy);
    sliderline.setPosition(newx - width*0.07, newy); // Line is not exactly centred in slider.
    const sf::Vector2u slpos(sliderline.getPosition().x, sliderline.getPosition().y);
    const double slw = sliderline.getLocalBounds().width;
    slidercirc.setPosition(slpos.x - slw/2 + (val-min)/(max-min) * slw, slpos.y);
    labelsprite.setPosition(slpos.x - slw/2*1.15 - labelsprite.getGlobalBounds().width,
                            slpos.y - labelsprite.getGlobalBounds().height/2);
    text.setFont(font);
    text.setPosition(slpos.x + slw/2 * 1.1, slpos.y - text.getGlobalBounds().height);
    for(int si = 0; si < snaplines.size(); ++si) {
      const double minx = slpos.x - slw/2;
      const double maxx = slpos.x + slw/2;
      snaplines[si].setPosition((snapvals[si]-min)/(max-min) * (maxx-minx) + minx, slpos.y);
    }
  }

  void setSize(double newwidth, double newheight) {
    width = newwidth;
    height = newheight;
    sliderline.setSize(sf::Vector2f(newwidth*0.7, 2));
    sliderline.setOrigin(sliderline.getGlobalBounds().width/2, sliderline.getGlobalBounds().height/2);
    slidercirc.setRadius(sliderline.getLocalBounds().width*0.05);
    slidercirc.setOrigin(slidercirc.getRadius(), slidercirc.getRadius());
    labelsprite.setScale(width/1100, width/1100);
    text.setCharacterSize(width/20);
  }

  void setSnap(double snapval) {
    snapvals.push_back(snapval);
    sf::RectangleShape snapline(sf::Vector2f(2, 20));
    snapline.setOrigin(snapline.getLocalBounds().width/2, snapline.getLocalBounds().height/2);
    snaplines.push_back(snapline);
  }

  void draw(sf::RenderWindow& window) {
    window.draw(slidercirc);
    window.draw(sliderline);
    for(int si = 0; si < snapvals.size(); ++si) {
      window.draw(snaplines[si]);
    }
    labelsprite.setTexture(labeltex);
    window.draw(labelsprite);
    text.setFont(font);
    window.draw(text);
  }

  bool setActive(Eigen::Vector2d mouse_pos) {
    const Eigen::Vector2d sliderpos(slidercirc.getPosition().x, slidercirc.getPosition().y);
    if((mouse_pos - sliderpos).norm() < 20) active = true;
    return active;
  }

  void unsetActive() {
    active = false;
  }

  bool drag(Eigen::Vector2d mouse_pos) {
    if(!active) return false;
    const double minx = sliderline.getPosition().x - sliderline.getLocalBounds().width/2;
    const double maxx = sliderline.getPosition().x + sliderline.getLocalBounds().width/2;
    double newx = std::max(std::min(maxx, mouse_pos.x()), minx);
    // Snap.
    for(double sv : snapvals) {
      const double svx = (sv-min)/(max-min) * (maxx-minx) + minx;
      if(std::abs(newx-svx) < 10) newx = svx;
    }
    slidercirc.setPosition(newx, slidercirc.getPosition().y);
    val = (newx-minx)/(maxx-minx) * (max-min) + min;
    text.setString(std::to_string(val));
    return true;
  }

  void update() {
    const double minx = sliderline.getPosition().x - sliderline.getLocalBounds().width/2;
    const double maxx = sliderline.getPosition().x + sliderline.getLocalBounds().width/2;
    if(val > max) val = min; // Loop the slider continuously.
    const double newx = (val-min)/(max-min) * (maxx-minx) + minx;
    slidercirc.setPosition(newx, slidercirc.getPosition().y);
    text.setString(std::to_string(val));
  }

  void animate() {
    val += (max-min)/400;
    update();
  }

  double getVal() {
    return val;
  }
}; // class Slider