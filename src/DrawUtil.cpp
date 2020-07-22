#include "DrawUtil.h"
#include <iostream>

namespace DrawUtil {

void Line(sf::RenderWindow& window, const sf::Vector2f& a, const sf::Vector2f& b, const double width) {
  glLineWidth(width);
  std::vector<sf::Vertex> vbuff = {a,b};
  window.draw(vbuff.data(), vbuff.size(), sf::PrimitiveType::Lines);
}

sf::Vector2f normalized(const sf::Vector2f a) {
  return a/sqrt(a.x*a.x + a.y*a.y);
}

sf::Vector2f normal(const sf::Vector2f& a, const sf::Vector2f& b) {
  return normalized(sf::Vector2f(-(b-a).y, (b-a).x));
}

sf::Vector2f miter(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Vector2f& c) {
  const sf::Vector2f tanl = normalized(normalized(c-b) + normalized(b-a));
  return normal(sf::Vector2f(0,0), tanl);
}

TernaryGraph::TernaryGraph(sf::RenderWindow& window):
        triangle(100,3), tcentre(0,0), triangleR(0),
        window(window),width(window.getSize().x), height(window.getSize().y),
        oldWindowSize(window.getSize()), centre(pos.x+width*0.5, pos.y+height*0.5) {
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

  // Function to convert point-to-point vertex arrays into triangle strips with thickness.
  std::vector<sf::Vertex> TernaryGraph::TriStrip(const std::vector<sf::Vertex>& drawing, const double thickness){
    std::vector<sf::Vertex> result(drawing.size()*2);
    for(int vi=1; vi<drawing.size()-1; ++vi) {
      // Three vertices form an angle around which to find miter points.
      const sf::Vector2f ppos(drawing[vi-1].position);
      const sf::Vector2f cpos(drawing[vi].position);
      const sf::Vector2f npos(drawing[vi+1].position);
      // Push miters into new vector.
      const sf::Vector2f mit = miter(ppos, cpos, npos)*(float)thickness/2.f;
      result[2*vi] = cpos+mit;
      result[2*vi+1] = cpos-mit;
    }
    // First and last points.
    const sf::Vector2f norm = normal(drawing[0].position, drawing[1].position)*(float)thickness/2.f;
    result[0] = drawing[0].position+norm;
    result[1] = drawing[0].position-norm;
    const sf::Vector2f lnorm = normal(drawing[drawing.size()-2].position,
                                      drawing[drawing.size()-1].position)*(float)thickness/2.f;
    result[result.size()-2] = drawing[drawing.size()-1].position+lnorm;
    result[result.size()-1] = drawing[drawing.size()-1].position-lnorm;
    return result;
  } // TernaryGraph::TriStrip

// Function to transform a 3D vector into a 2D location on the ternary plot.
sf::Vector2f TernaryGraph::TriPoint(float e, float mu, float tau) {
  return left + e*(top-left) + mu*(right-left);
} // TernaryGraph::TriPoint()
// Function to transform from 2D ternary point to 3D probability vector.
sf::Vector3f TernaryGraph::InvTriPoint(const sf::Vertex& a) {
  const Eigen::Vector2f avec(a.position.x, a.position.y);
  const Eigen::Vector2f lvec(left.x, left.y);
  const Eigen::Vector2f tvec(top.x, top.y);
  const Eigen::Vector2f rvec(right.x, right.y);
  const double ecomp = (avec-lvec).dot(tvec-lvec) / (tvec-lvec).squaredNorm();
  const double mucomp = (avec-tvec).dot(rvec-tvec) / (rvec-tvec).squaredNorm();
  return sf::Vector3f(ecomp, mucomp, 1-ecomp-mucomp);
}

// Draw everything in class.
void TernaryGraph::draw() {
  window.draw(triangle);

  // Draw divider lines.
  for(int i = 0; i <= numDiv; ++i) {
    const sf::Vector2f a = left + (float)i*(top-left)/(float)numDiv
                           - sf::Vector2f(triangleR*0.1, 0); // Offset for ticks.
    const sf::Vector2f b = right + (float)i*(top-right)/(float)numDiv;
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

  // Draw all added drawings and their highlights.
  for(int di = 0; di < drawings.size(); ++di) {
    const std::vector<sf::Vertex>& drawing = drawings[di];
    window.draw(drawing.data(), std::min((size_t)t*10,drawing.size()), sf::PrimitiveType::TriangleStrip);
  }
  for(std::vector<sf::Vertex>& highlight : highlights) {
    const int start = int(t*2)%highlight.size();
    const int numvtx = std::min(10, (int)(highlight.size() - start));
    window.draw(highlight.data()+start, numvtx, sf::PrimitiveType::TriangleStrip);
  }

} // TernaryGraph::draw()

void TernaryGraph::addDrawing(const std::vector<Eigen::Vector3d>& vec) {
    probs.push_back(vec);
    updateWindow();
}

// Update all relevant parameters in case of a window size change.
void TernaryGraph::updateWindow() {
  // Scale by comparing old and new window size.
  const sf::Vector2f scale((float)window.getSize().x/oldWindowSize.x,
                           (float)window.getSize().y/oldWindowSize.y);
  width *= scale.x; height *= scale.y;
  pos.x *= scale.x; pos.y *= scale.y;
  centre = sf::Vector2f(pos) + sf::Vector2f(width*0.5, height*0.5);
  oldWindowSize = window.getSize();

  // Triangle parameters.
  triangleR = std::min(width/sqrt(3), height/1.5) * 0.75;
  sideL = triangleR*sqrt(3);
  centre.x = pos.x + width - sideL*0.5/0.75; // Right-align graph in window section.
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
  nulabelsprite[0].setScale(sideL/5000, sideL/5000);
  nulabelsprite[1].setScale(sideL/5000, sideL/5000);
  nulabelsprite[2].setScale(sideL/5000, sideL/5000);
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
  highlights.resize(probs.size());
  for(int vi = 0; vi < probs.size(); ++vi) {
    drawings[vi].resize(probs[vi].size());
    for(int di = 0; di < probs[vi].size(); ++di) {
      drawings[vi][di] = TriPoint(probs[vi][di]);
    }
    // Convert VertexArrays to arrays that can be plotted smoothly with triangle strips.
    highlights[vi] = TriStrip(drawings[vi], 10);
    drawings[vi] = TriStrip(drawings[vi], 6);
    // Add a splash of colour.
    for(int i = 0; i < drawings[vi].size(); ++i) {
      // Normalise colours so there's always one out of rgb at 255.
      const double cmax = std::max(std::max(probs[vi][i/2](0), probs[vi][i/2](1)), probs[vi][i/2](2));
      drawings[vi][i].color = sf::Color(255*probs[vi][i/2](2)/cmax,
                                        255*probs[vi][i/2](0)/cmax,
                                        255*probs[vi][i/2](1)/cmax);
    }
    // Make sure the highlight is drawn in white.
    for(int i = 0; i < highlights[vi].size(); ++i) {
      highlights[vi][i].color = sf::Color::White;
    }
  }
}

} // namespace DrawUtil