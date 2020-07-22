#define NOGDI
#define GL_SILENCE_DEPRECATION

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
static const int render_scale = 1;

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

  //Setup OpenGL things
  glHint(GL_POINT_SMOOTH, GL_NICEST);
  glHint(GL_LINE_SMOOTH, GL_NICEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_SMOOTH);
  
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
        } else if(keycode == sf::Keyboard::S) {
          // const std::string filename = "nu.csv";
          // std::ofstream ofile(filename);
          // if(!ofile.is_open()) {
          //   std::cout << "Couldn't open file " << filename << ".\n";
          //   return;
          // }

          // // Header.
          // ofile << "L,e,mu,tau\n";
          // // Record probabilities as function of L.
          // for(double x = 0; x < L; x += step) {
          //   ofile << x << ',' << 
          // }
          // std::cout << "Saving to " << filename << ".\n";
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

    // Need to do this to prevent drawings from remaining on screen after frame.
    glClear(GL_COLOR_BUFFER_BIT);

    // Handle mouse dragging.
    if(cp.drag(mouse_pos)) {
      redraw = true;
    }
    // Draw control panel.
    cp.draw();

    // If sliding, change the neutrino.
    const int numsteps = 1000;
    const double E = 1;
    const double L = 33060.7*E; // PI / (1.267*Dm21sq) * E (Full Dm12sq period.)
    if(redraw || animate) {
      tgraph.clear();
      osc.update(); // Update internal mixing matrix etc.
      // Determine neutrino path on the fly.
      tgraph.addDrawing(osc.trans(neut, E, L, L/numsteps));
      redraw = false;
    }
    tgraph.draw();

    //Flip the screen buffer
    window.display();
    // Animate.
    if(animate) {
      cp.animate();
    } else {
      cp.stopAnimate();
    }
    ++tgraph.t; // Advance graph time for initial drawing animation.
  }

  return 0;
}
