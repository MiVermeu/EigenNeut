#define NOGDI
#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <fstream>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "DrawUtil.h"
#include "NeutOsc.h"
#include "Slider.h"

typedef std::vector<Eigen::Vector3d> NuPath;

static const int start_w = 1440;
static const int start_h = 1080;
static const bool start_fullscreen = false;
static const int render_scale = 1;

int main(int argc, char *argv[]) {
  //Get the screen size
  sf::VideoMode screenSize = sf::VideoMode::getDesktopMode();
  screenSize = sf::VideoMode(start_w, start_h, screenSize.bitsPerPixel);
  // DrawUtil::render_scale = float(render_scale) * 0.5f;
  
  //GL settings
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 8;
  settings.majorVersion = 1;
  settings.minorVersion = 0;

  //Create the window
  sf::RenderWindow window;
  sf::Uint32 window_style = (start_fullscreen ? sf::Style::Fullscreen : sf::Style::Resize | sf::Style::Close);
  window.create(screenSize, "EigenNeut", window_style, settings);
  window.setFramerateLimit(60);
  window.requestFocus();
  sf::View view = window.getDefaultView();

  //Setup OpenGL things
  glHint(GL_POINT_SMOOTH, GL_NICEST);
  glHint(GL_LINE_SMOOTH, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_SMOOTH);
  
  // Create ternary graph and oscillator class instances.
  DrawUtil::TernaryGraph tgraph(window);
  neutosc::Oscillator osc;

  // Test slider.
  double& sliderval = osc.pars().dCP;
  Slider slider(sliderval);
  slider.setLimits(-3.1416, 3.1416);
  slider.setSize(window.getSize().x*0.4, window.getSize().y*0.01);
  slider.setPosition(window.getSize().x/2, window.getSize().y*0.05);
  // dCP circle.
  sf::CircleShape dcpcirc(30,20);
  dcpcirc.setFillColor(sf::Color::Black);
  dcpcirc.setOutlineColor(sf::Color::White);
  dcpcirc.setOutlineThickness(3);
  dcpcirc.setOrigin(dcpcirc.getRadius(), dcpcirc.getRadius());
  dcpcirc.setPosition(50, 50);
  // dCP indicator.
  sf::RectangleShape dcpline(sf::Vector2f(40,3));
  dcpline.setFillColor(sf::Color::White);
  dcpline.setOrigin(0, dcpline.getLocalBounds().height/2);
  dcpline.setPosition(50,50);

  // Mouse input variables.
  Eigen::Vector2d mouse_pos(0,0);
  bool mouse_pressed = false;

  //Main Loop
  int neut = 0;
  bool animate = false;
  bool redraw = true;
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        break;
      } else if (event.type == sf::Event::KeyPressed) {
        const sf::Keyboard::Key keycode = event.key.code;
        if (keycode == sf::Keyboard::Escape) {
          window.close();
          break;
        } else if (keycode == sf::Keyboard::Space) {
          animate = !animate;
        } else if (keycode == sf::Keyboard::Right) {
          neut = (neut+1)%3;
          redraw = true;
        } else if (keycode == sf::Keyboard::Left) {
          neut -= 1;
          if(neut<0) neut += 3;
          redraw = true;
        } else if (keycode == sf::Keyboard::Up) {
          sliderval += 0.03;
          redraw = true;
        } else if (keycode == sf::Keyboard::Down) {
          sliderval -= 0.03;
          redraw = true;
        }
      } else if (event.type == sf::Event::Resized) {
        const sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));
        slider.setSize(window.getSize().x*0.4, window.getSize().y*0.01);
        slider.setPosition(window.getSize().x/2, window.getSize().y*0.05);
        tgraph.updateWindow();
      } else if (event.type == sf::Event::MouseMoved) {
        mouse_pos = Eigen::Vector2d(event.mouseMove.x, event.mouseMove.y);
      } else if (event.type == sf::Event::MouseButtonPressed) {
        mouse_pos = Eigen::Vector2d(event.mouseButton.x, event.mouseButton.y);
        slider.setActive(mouse_pos);
        // ActivatePoint(renderTexture);
      } else if (event.type == sf::Event::MouseButtonReleased) {
        mouse_pos = Eigen::Vector2d(event.mouseButton.x, event.mouseButton.y);
        slider.unsetActive();
      }
    }

    // Need to do this to prevent drawings from remaining on screen after frame.
    glClear(GL_COLOR_BUFFER_BIT);

    // Handle mouse dragging.
    if(slider.drag(mouse_pos)) {
      redraw = true;
    }
    // Draw the slider.
    slider.draw(window);

    // Draw dCP circle.
    window.draw(dcpcirc);
    dcpline.rotate(-sliderval*180/3.1416);
    window.draw(dcpline);
    dcpline.rotate(sliderval*180/3.1416);

    // If sliding, change the neutrino.
    if(redraw || animate) {
      tgraph.clear();
      osc.update(); // Update internal mixing matrix etc.
      // Determine neutrino path on the fly.
      std::vector<Eigen::Vector3d> prob(23000/10);
      for(int i = 0; i < 23000; i+=10) {
        prob[i/10] = osc.trans(neut , 0.7, i);
      }
      tgraph.addDrawing(prob);
      redraw = false;
    }
    tgraph.draw();

    //Flip the screen buffer
    window.display();
    // Advance animation time.
    if(animate) {
      sliderval += 0.03;
    }
    ++tgraph.t; // Advance graph time for initial drawing animation.
  }

  return 0;
}
