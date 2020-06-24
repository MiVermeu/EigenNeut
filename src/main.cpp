#define NOGDI
#define GL_SILENCE_DEPRECATION

#include <iostream>
#include <fstream>
#include <random>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "DrawUtil.h"
#include "NeutOsc.h"

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
  
  // Create ternary graph class instance.
  DrawUtil::TernaryGraph tgraph(window);
  neutosc::Oscillator osc;

  int numsteps = 300;
  std::vector<std::vector<NuPath>> nu(3, std::vector<NuPath>(numsteps));
  // If save file exists, try loading it.
  bool loading = false;
  std::ifstream ifile("savedata.dat");
  std::ofstream ofile;
  if(ifile.good()) {
    loading = true;
  } else {
    ofile.open("savedata.dat");
  }
  for(int step = 0; step < numsteps; ++step) {
    std::cout << step << '\n';
    for(int i = 0; i < 23000; i+=10) {
      for(int nui = 0; nui < 3; ++nui) {
        Eigen::Vector3d prob;
        if(loading) {
          ifile >> prob(0) >> prob(1) >> prob(2);
        } else {
          prob = osc.trans(nui , 0.7, i, 1, step*3.1416*2/numsteps);
          ofile << prob(0) << ' ' << prob(1) << ' ' << prob(2) << ' ';
        }
        nu[nui][step].push_back(prob);
      } // Loop over input neutrinos.
    } // Loop over neutrino path.
  } // Loop over variations of paths.

  //Main Loop
  long long patht = 0;
  int neut = 0;
  bool go = false;
  bool animate = false;
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
          if(go) {
            animate = !animate;
          }
          go = true;
        } else if (keycode == sf::Keyboard::Right) {
          neut = (neut+1)%3;
        } else if (keycode == sf::Keyboard::Left) {
          neut -= 1;
          if(neut<0) neut += 3;
        } else if (keycode == sf::Keyboard::Up) {
          patht += 1;
        } else if (keycode == sf::Keyboard::Down) {
          patht -= 1;
        }
        // else if (keycode == sf::Keyboard::C) {
        //   capture = true;
        // }
      } else if (event.type == sf::Event::Resized) {
        const sf::FloatRect visibleArea(0, 0, (float)event.size.width, (float)event.size.height);
        window.setView(sf::View(visibleArea));
        tgraph.updateWindow();
      }
    }

    glClear(GL_COLOR_BUFFER_BIT);

    while(patht < 0) patht += numsteps;

    if(go) {
      tgraph.clear();
      tgraph.addDrawing(nu[neut][(int)(patht)%((int)nu[neut].size())]);
    }

    tgraph.draw();

    // Draw dCP circle.
    sf::CircleShape dcpcirc(30,20);
    dcpcirc.setFillColor(sf::Color::Black);
    dcpcirc.setOutlineColor(sf::Color::White);
    dcpcirc.setOutlineThickness(3);
    dcpcirc.setOrigin(dcpcirc.getRadius(), dcpcirc.getRadius());
    dcpcirc.setPosition(50, 50);
    window.draw(dcpcirc);
    // dCP indicator.
    sf::RectangleShape dcpline(sf::Vector2f(40,3));
    dcpline.setFillColor(sf::Color::White);
    dcpline.setOrigin(0, dcpline.getLocalBounds().height/2);
    dcpline.setPosition(50,50);
    dcpline.rotate(-patht*360/numsteps);
    window.draw(dcpline);

    //Flip the screen buffer
    window.display();
    // Advance path time only if program has started and animation is on.
    if(go) {
      if(animate) {
        ++patht;
      }
      ++tgraph.t; // Advance graph time if program has started.
    }
  }

  return 0;
}
