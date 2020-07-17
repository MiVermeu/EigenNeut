#include "DrawUtil.h"
#include <iostream>

namespace DrawUtil {

void Line(sf::RenderWindow& window, const sf::Vector2f& a, const sf::Vector2f& b, const double width) {
  glLineWidth(width);
  std::vector<sf::Vertex> vbuff = {a,b};
  window.draw(vbuff.data(), vbuff.size(), sf::PrimitiveType::Lines);
}

TernaryGraph::TernaryGraph(sf::RenderWindow& window):
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
sf::Vector2f TernaryGraph::TriPoint(float e, float mu, float tau) {
  return left + e*(top-left) + mu*(right-left);
} // TernaryGraph::TriPoint()

// Draw everything in class.
void TernaryGraph::draw() {
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
  for(int di = 0; di < drawings.size(); ++di) {
    std::vector<sf::Vertex>& drawing = drawings[di];

    glLineWidth(5);
    for(int i = 0; i < drawing.size(); ++i) {
      // Normalise colours so there's always one out of rgb at 255.
      const double cmax = std::max(std::max(probs[di][i](0), probs[di][i](1)), probs[di][i](2));
      drawing[i].color = sf::Color(255*probs[di][i](2)/cmax,
                                   255*probs[di][i](0)/cmax,
                                   255*probs[di][i](1)/cmax);
    }
    window.draw(drawing.data(), /* std::min((size_t)t*10,drawing.size()) */ drawing.size(), sf::PrimitiveType::LineStrip);
  }
  for(std::vector<sf::Vertex>& drawing : drawings) {
    glLineWidth(7);
    // Make sure this line is drawn in white.
    for(int i = 0; i < drawing.size(); ++i) {
      drawing[i].color = sf::Color::White;
    }
    const int start = int(t)%drawing.size();
    const int numvtx = std::min(10, (int)(drawing.size() - start));
    window.draw(drawing.data()+start, numvtx, sf::PrimitiveType::LineStrip);
  }

} // TernaryGraph::draw()

// Update all relevant parameters in case of a window size change.
void TernaryGraph::updateWindow() {
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
}

} // namespace DrawUtil