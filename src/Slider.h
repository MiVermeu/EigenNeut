#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <Eigen/Dense>

char sfKeyToChar(const sf::Keyboard::Key& key) {
  switch (key) {
    case sf::Keyboard::Num0: return '0';
    case sf::Keyboard::Num1: return '1';
    case sf::Keyboard::Num2: return '2';
    case sf::Keyboard::Num3: return '3';
    case sf::Keyboard::Num4: return '4';
    case sf::Keyboard::Num5: return '5';
    case sf::Keyboard::Num6: return '6';
    case sf::Keyboard::Num7: return '7';
    case sf::Keyboard::Num8: return '8';
    case sf::Keyboard::Num9: return '9';
    case sf::Keyboard::Period: return '.';
    default: return '?';
  }
}

class Slider {
  private:
  // Slider attributes.
  double min = 0;
  double max = 0;
  double& val;
  bool loop = true; // Make values either loop or increase continuously.
  Eigen::Vector2d pos;
  double width = 20;
  double height = 0;
  bool active = false;
  bool editing = false;
  std::string editingstring;
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
    slidercirc.setRadius(sliderline.getLocalBounds().width*0.04);
    slidercirc.setOrigin(slidercirc.getRadius(), slidercirc.getRadius());
    labelsprite.setScale(width/6000, width/6000);
    text.setCharacterSize(width/20);
  }

  void setSnap(double snapval) {
    snapvals.push_back(snapval);
    sf::RectangleShape snapline(sf::Vector2f(2, 20));
    snapline.setOrigin(snapline.getLocalBounds().width/2, snapline.getLocalBounds().height/2);
    snaplines.push_back(snapline);
  }

  void setLoop(bool newval) { loop = newval; }

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

  void edit(const sf::Keyboard::Key& key) {
    // Toggle editing on this slider.
    if(key == sf::Keyboard::Enter && editing == false) {
      editing = true;
      editingstring = "";
      text.setString("");
    } else if(key == sf::Keyboard::Enter && editing == true) {
      editing = false;
      try {
        val = std::stod(editingstring);
      } catch(...) {
        std::cout << "Not a valid number.\n";
        text.setString(std::to_string(val));
      }
      update();
    }

    // Append number/period to string.
    if(editing) {
      if((key >= sf::Keyboard::Num0 && key <= sf::Keyboard::Num9) || key == sf::Keyboard::Period) {
        editingstring.push_back(sfKeyToChar(key));
      } else if(key == sf::Keyboard::Backspace) {
        editingstring.pop_back();
      }
      text.setString(editingstring);
    }
  }

  void update() {
    const double minx = sliderline.getPosition().x - sliderline.getLocalBounds().width/2;
    const double maxx = sliderline.getPosition().x + sliderline.getLocalBounds().width/2;
    double newx = (val-min)/(max-min) * (maxx-minx) + minx;
    if(val > max) {
      if(loop) {
        val -= std::floor(val/(max-min))*(max-min);
        newx = (val-min)/(max-min) * (maxx-minx) + minx;
      } else {
        newx = maxx;
      }
    }
    slidercirc.setPosition(newx, slidercirc.getPosition().y);
    text.setString(std::to_string(val));
  }

  void animate() {
    val += (max-min)/400;
    update();
  }

  double& getVal() {
    return val;
  }
}; // class Slider