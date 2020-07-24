#include <iostream>
#include <fstream>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <Eigen/Dense>

#include "DrawUtil.h"
#include "NeutOsc.h"
#include "ControlPanel.h"
#include "Slider.h"

typedef std::vector<Eigen::Vector3d> NuPath;

static const int start_w = 1500;
static const int start_h = 1000;
static const bool start_fullscreen = false;

int main(int argc, char *argv[]) {
  //Get the screen size
  sf::VideoMode screenSize = sf::VideoMode::getDesktopMode();
  screenSize = sf::VideoMode(start_w, start_h, screenSize.bitsPerPixel);
  
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
  
  // Create ternary graph and oscillator class instances.
  DrawUtil::TernaryGraph tgraph(window);
  tgraph.setPosition(window.getSize().x/4, 0);
  tgraph.setSize(window.getSize().x/4.*3, window.getSize().y);
  neutosc::Oscillator osc;
  ControlPanel cp(window, osc.pars());
  cp.setPosition(0,0);
  cp.setSize(600,500);

  // Mouse input variables.
  Eigen::Vector2d mouse_pos(0,0);
  bool mouse_pressed = false;

  //Main Loop
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
          if(cp.isAnimating()) {
            cp.stopAnimate();
          } else {
            cp.animate();
          }
        } else if (keycode == sf::Keyboard::Right) {
          osc.pars().nu = (osc.pars().nu+1)%3;
          redraw = true;
        } else if (keycode == sf::Keyboard::Left) {
          osc.pars().nu -= 1;
          if(osc.pars().nu<0) osc.pars().nu += 3;
          redraw = true;
        } else if(keycode == sf::Keyboard::L) {
          // Export probabilities as function of travel distance (with 10000 steps).
          neutosc::exportData(neutosc::oscillate(osc, osc.pars().L, 10000), osc.pars().L);
        } else if(keycode == sf::Keyboard::E) {
          // Export probabilities as function of energy (with 10000 steps).
          neutosc::exportData(neutosc::oscillate(osc, osc.pars().E, 10000), osc.pars().E);
        } else if(keycode == sf::Keyboard::X) {
          // Export probabilities as function of last active variable (with 10000 steps).
          neutosc::exportData(neutosc::oscillate(osc, cp.lastActiveVar(), 10000), cp.lastActiveVar());
        } else if(keycode == sf::Keyboard::A) {
          // Toggle between neutrinos and antineutrinos.
          osc.pars().anti = !osc.pars().anti;
          osc.update();
          tgraph.setAnti(osc.pars().anti);
          tgraph.updateWindow();
          redraw = true;
        } else if(keycode == sf::Keyboard::M) {
          // Flip mass hierarchy
          osc.pars().Dm31sq *= -1;
          osc.update();
          cp.update();
          redraw = true;
        } else if((keycode >= sf::Keyboard::Num0 && keycode <= sf::Keyboard::Num9) ||
                  keycode == sf::Keyboard::Period || keycode == sf::Keyboard::Enter ||
                  keycode == sf::Keyboard::Backspace) {
          cp.edit(keycode);
          redraw = true;
        }
      } else if (event.type == sf::Event::Resized) {
        const sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));
        tgraph.updateWindow();
        cp.updateWindow();
      } else if (event.type == sf::Event::MouseMoved) {
        mouse_pos = Eigen::Vector2d(event.mouseMove.x, event.mouseMove.y);
      } else if (event.type == sf::Event::MouseButtonPressed) {
        mouse_pos = Eigen::Vector2d(event.mouseButton.x, event.mouseButton.y);
        cp.setActive(mouse_pos);
      } else if (event.type == sf::Event::MouseButtonReleased) {
        mouse_pos = Eigen::Vector2d(event.mouseButton.x, event.mouseButton.y);
        cp.unsetActive();
      }
    }

    // Draw a black background.
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(sf::Color::Black);
    window.draw(background);

    // Handle mouse dragging.
    if(cp.drag(mouse_pos)) {
      redraw = true;
    }
    // Draw control panel.
    cp.draw();

    // If redrawing or animating, regenerate neutrino oscillation probabilities.
    if(redraw || cp.isAnimating()) {
      tgraph.clear();
      osc.update(); // Update internal mixing matrix etc from control panel.
      tgraph.addDrawing(neutosc::oscillate(osc, osc.pars().L, 1500));
      redraw = false;
    }
    tgraph.draw();

    //Flip the screen buffer
    window.display();
  }

  return 0;
}
