#ifndef DRAWUTIL_H__
#define DRAWUTIL_H__

#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include "NeutOsc.h"

namespace DrawUtil{

void Line(sf::RenderWindow& window, const sf::Vector2f& a, const sf::Vector2f& b, const double width) {
  glLineWidth(width);
  std::vector<sf::Vertex> vbuff = {a,b};
  window.draw(vbuff.data(), vbuff.size(), sf::PrimitiveType::Lines);
}

class TernaryGraph {
  private:
  // Graph parameters
  sf::CircleShape triangle; // Triangle is circle with three points lol.
  sf::Vector2f tcentre;
  double triangleR;
  double sideL;
  sf::Vector2f top;
  sf::Vector2f left;
  sf::Vector2f right;
  const int numDiv = 10;

  // Window parameters.
  sf::RenderWindow& window;
  double width;
  double height;
  sf::Vector2f centre;

  // Transformations.
  sf::Transform rot120;
  sf::Transform rot240;

  // Oscillator and drawings.
  neutosc::Oscillator osc;
  std::vector<std::vector<Eigen::Vector3d>> probs;
  std::vector<std::vector<sf::Vertex>> drawings;

  // Textures aand sprites for labels
  sf::Texture nulabeltex[3];
  sf::Sprite nulabelsprite[3];

  public:
  // Animation time.
  double t = 0;

  TernaryGraph(sf::RenderWindow& window):
        triangle(100,3), tcentre(0,0), triangleR(0),
        window(window),width(window.getSize().x), height(window.getSize().y),
        centre(width*0.5, height*0.5) {
    glLineWidth(10);
    updateWindow();

    // Load textures.
    nulabeltex[0].loadFromFile("../textures/nu_e.png");
    nulabeltex[1].loadFromFile("../textures/nu_mu.png");
    nulabeltex[2].loadFromFile("../textures/nu_tau.png");
    // Assign to sprites and set origin.
    for(int nui = 0; nui < 3; ++nui) {
      nulabelsprite[nui].setTexture(nulabeltex[nui]);
      nulabelsprite[nui].setOrigin(nulabelsprite[nui].getLocalBounds().width/2, nulabelsprite[nui].getLocalBounds().height/2);
    }

    // Rotation transformations.
    rot120.rotate(120, tcentre);
    rot240.rotate(240, tcentre);
  } // TernaryGraph::TernaryGraph()

  // Function to transform a 3D vector into a 2D location on the ternary plot.
  sf::Vector2f TriPoint(float e, float mu, float tau) {
    // Check if three components add up to 1. If not, normalise and warn.
    // const double sum = e+mu+tau;
    // if(sum - 1 > 1e-4) {
    //   // std::cout << "Sum is not 1: " << sum << '\r';
    //   e /= sum;
    //   mu /= sum;
    //   tau /= sum;
    // }

    // Add vectors for the different components together until the right point is reached.
    const sf::Vector2f edir = top-left;
    const sf::Vector2f mudir = right-left;

    return left + e*edir + mu*mudir;
  } // TernaryGraph::TriPoint()

  sf::Vector2f TriPoint(sf::Vector3f a) {
    return TriPoint(a.x,a.y,a.z);
  } // TernaryGraph::TriPoint(sf::Vector3f)

  sf::Vector2f TriPoint(Eigen::Vector3d a) {
    return TriPoint(a(0),a(1),(2));
  } // TernaryGraph::TriPoint(Eigen::Vector3d)

  void draw() {
    window.draw(triangle);

    // Draw divider lines.
    for(int i = 0; i <= numDiv; ++i) {
      const double y = left.y - i*(left.y-top.y)/numDiv;
      const double x = centre.x - (1-(double)i/numDiv)*0.5*sideL;
      const sf::Vector2f a(x-triangleR*0.1,y); // Offset for ticks.
      const sf::Vector2f b(width-x,y);
      Line(window, a, b, 0.5);
      const sf::Vector2f a120 = rot120.transformPoint(a);
      const sf::Vector2f b120 = rot120.transformPoint(b);
      Line(window, a120, b120, 0.5);
      const sf::Vector2f a240 = rot240.transformPoint(a);
      const sf::Vector2f b240 = rot240.transformPoint(b);
      Line(window, a240, b240, 0.5);
    }

    // Draw labels.
    window.draw(nulabelsprite[0]);
    window.draw(nulabelsprite[1]);
    window.draw(nulabelsprite[2]);

    // Draw all added drawings.
    for(std::vector<sf::Vertex>& drawing : drawings) {
      glLineWidth(7);
      // for(int i = 0; i < drawing.size(); ++i) {
      //   drawing[i].color = sf::Color::Blue;
      // }
      const int start = int(t)%drawing.size();
      const int numvtx = std::min(10, (int)(drawing.size() - start));
      window.draw(drawing.data()+start, numvtx, sf::PrimitiveType::LineStrip);
    }

    for(std::vector<sf::Vertex>& drawing : drawings) {
      glLineWidth(1);
      // for(int i = 0; i < drawing.size(); ++i) {
      //   drawing[i].color = sf::Color::Blue;
      // }
      window.draw(drawing.data(), std::min((size_t)t*10,drawing.size()), sf::PrimitiveType::LineStrip);
    }

  } // TernaryGraph::draw()

  // Add a drawing in the form of a vector of 3D positions.
  void addDrawing(std::vector<Eigen::Vector3d>& vec) {
    probs.push_back(vec);
    updateWindow();
  } // void addDrawing

  // Clear all drawings.
  void clear() {
    probs.resize(0);
  } // void clear()

  // Update all relevant parameters in case of a window size change.
  void updateWindow() {
    width = window.getSize().x;
    height = window.getSize().y;
    centre = sf::Vector2f(width*0.5, height*0.5);

    // Triangle parameters.
    triangleR = std::min(width/sqrt(3), height/1.5) * 0.7;
    sideL = triangleR*sqrt(3);
    top = sf::Vector2f(centre.x, centre.y-sideL*sqrt(0.75)*0.5);
    left = sf::Vector2f(centre.x-sideL*0.5, centre.y+sideL*sqrt(0.75)*0.5);
    right = sf::Vector2f(centre.x+sideL*0.5, centre.y+sideL*sqrt(0.75)*0.5);

    triangle.setRadius(triangleR);
    triangle.setOrigin(triangleR, triangleR*1.5/2); // Height of triangle is not R.
    triangle.setPosition(centre);
    triangle.setOutlineColor(sf::Color(255,255,255));
    triangle.setFillColor(sf::Color(0,0,0,0));
    triangle.setOutlineThickness(10);

    tcentre = triangle.getPosition() + sf::Vector2f(0,triangleR*(1 - 1.5/2));

    // Set label scale and locations.
    nulabelsprite[0].setScale(sideL/1000, sideL/1000);
    nulabelsprite[1].setScale(sideL/1000, sideL/1000);
    nulabelsprite[2].setScale(sideL/1000, sideL/1000);
    nulabelsprite[0].setPosition(top - (right-left)*0.1f);
    nulabelsprite[1].setPosition(right - (left-top)*0.1f);
    nulabelsprite[2].setPosition(left - (top-right)*0.1f);

    // Reset rotation transformations.
    rot120 = sf::Transform::Identity;
    rot240 = sf::Transform::Identity;
    rot120.rotate(120, tcentre);
    rot240.rotate(240, tcentre);

    // Determine VertexArrays from probability arrays.
    drawings.resize(probs.size());
    for(int vi = 0; vi < probs.size(); ++vi) {
      drawings[vi].resize(probs[vi].size());
      for(int di = 0; di < probs[vi].size(); ++di) {
        drawings[vi][di] = (TriPoint(probs[vi][di]));
      }
    }
  } // TernaryGraph::updateWindow()

}; // class TernaryGraph

}

#endif